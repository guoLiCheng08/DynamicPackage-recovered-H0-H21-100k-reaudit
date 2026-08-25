/* DynamicPackage 反作用飞轮恢复实现。 */
#include "dynamic_devices.h"
#include "dynamic_sensors.h"

#include <math.h>

static double clamp_torque_as_acceleration(double acceleration,
                                           const DpReactionWheelRecovered *wheel)
{
    const double torque = acceleration * wheel->inertia;
    if (torque > wheel->torque_limit) return wheel->torque_limit / wheel->inertia;
    if (torque < -wheel->torque_limit) return -wheel->torque_limit / wheel->inertia;
    return acceleration;
}

void dp_set_wheel_acc(DpReactionWheelRecovered wheels[DP_WHEEL_COUNT],
                      const double torque_command[DP_WHEEL_COUNT],
                      DpGaussianSampler gaussian_sampler, void *opaque)
{
    unsigned index;
    double candidate[DP_WHEEL_COUNT];

    /* SetWheelAcc 第一循环：命令扭矩除以单轮惯量。 */
    for (index = 0; index < DP_WHEEL_COUNT; ++index) {
        candidate[index] = torque_command[index] / wheels[index].inertia;
    }

    /* 第二循环：可选高斯扰动，汇编参数为 sigma / inertia。 */
    for (index = 0; index < DP_WHEEL_COUNT; ++index) {
        if (wheels[index].add_gaussian_noise_flag == 1u && candidate[index] != 0.0) {
            const double sigma = wheels[index].acceleration_noise_sigma / wheels[index].inertia;
            candidate[index] += gaussian_sampler != NULL ?
                gaussian_sampler(sigma, opaque) : dp_ran_gaussian_recovered(sigma);
        }
    }

    /*
     * 余下分支以当前转速、转速上限和转矩上限确定实际加速度。
     * 加速到超出转速上限的方向被禁止；反向命令允许脱离饱和区。
     */
    for (index = 0; index < DP_WHEEL_COUNT; ++index) {
        DpReactionWheelRecovered *wheel = &wheels[index];
        double actual = clamp_torque_as_acceleration(candidate[index], wheel);
        /* 原实现仅对正向角速度上限走特殊分支：omega == limit 时不写
         * acceleration（保留旧值）；omega > limit 时才阻断继续正向加速。
         * 原 ELF 对正负超限均阻断继续朝超限方向的加速；但精确等于正向
         * 上限时仍不写 acceleration（保留旧值），该精确相等语义必须保留。 */
        if (wheel->omega == wheel->omega_limit) {
            continue;
        }
        if ((wheel->omega > wheel->omega_limit && actual > 0.0) ||
            (wheel->omega < -wheel->omega_limit && actual < 0.0)) {
            actual = 0.0;
        }
        wheel->acceleration = actual;
    }
}

double dp_get_wheel_speed(const DpReactionWheelRecovered wheels[DP_WHEEL_COUNT], unsigned index)
{
    const DpReactionWheelRecovered *wheel;

    if (wheels == NULL || index >= DP_WHEEL_COUNT) {
        return 0.0;
    }
    wheel = &wheels[index];
    if (wheel->add_gaussian_noise_flag == 1u) {
        return wheel->omega + dp_ran_gaussian_recovered(wheel->telemetry_noise_sigma);
    }
    return wheel->omega;
}

void wheel_matrix_calc(DpMatrix *out, const DpVector *col0, const DpVector *col1,
                       const DpVector *col2, const DpVector *col3)
{
    (void)matrix_set_col(out, 0, col0);
    (void)matrix_set_col(out, 1, col1);
    (void)matrix_set_col(out, 2, col2);
    (void)matrix_set_col(out, 3, col3);
}

void dp_update_wheel(DpReactionWheelRecovered wheels[DP_WHEEL_COUNT], double step_time)
{
    unsigned index;
    for (index = 0; index < DP_WHEEL_COUNT; ++index) {
        DpReactionWheelRecovered *wheel = &wheels[index];
        /* UpdateWheel 的写入顺序：torque, omega, angular_momentum。 */
        wheel->torque = wheel->acceleration * wheel->inertia;
        wheel->omega += wheel->acceleration * step_time;
        wheel->angular_momentum = wheel->omega * wheel->inertia;
    }
}

int dp_update_wheel_group(DpReactionWheelRecovered wheels[DP_WHEEL_COUNT], double step_time,
                          DpVector *group_angular_momentum_3,
                          DpVector *group_torque_3,
                          const DpMatrix *wheel_mapping_3x4)
{
    double wheel_torque_data[DP_WHEEL_COUNT] = {0.0};
    DpVector wheel_torque;
    unsigned index;

    if (wheels == NULL || group_angular_momentum_3 == NULL || group_torque_3 == NULL ||
        wheel_mapping_3x4 == NULL || group_angular_momentum_3->data == NULL ||
        group_torque_3->data == NULL || wheel_mapping_3x4->data == NULL ||
        group_angular_momentum_3->count != 3 || group_torque_3->count != 3 ||
        wheel_mapping_3x4->rows != 3 || wheel_mapping_3x4->cols != 4 ||
        wheel_mapping_3x4->row_stride < 4) {
        return -1;
    }
    wheel_torque.count = DP_WHEEL_COUNT;
    wheel_torque.reserved_04 = 0;
    wheel_torque.data = wheel_torque_data;
    for (index = 0u; index < DP_WHEEL_COUNT; ++index) {
        DpReactionWheelRecovered *wheel = &wheels[index];
        wheel->torque = wheel->acceleration * wheel->inertia;
        wheel->omega += wheel->acceleration * step_time;
        wheel->angular_momentum = wheel->omega * wheel->inertia;
        wheel_torque.data[index] = wheel->torque;
    }
    if (blas_gemv(wheel_mapping_3x4, &wheel_torque, group_torque_3, 1.0, 0.0) != 0) {
        return -1;
    }
    /* 原 UpdateWheel 不检查 vector_axpy 返回值；step_time==0 时保持群组角动量。 */
    (void)vector_axpy(group_torque_3, step_time, group_angular_momentum_3);
    return 0;
}

void dp_set_thruster_work_status(DpThrusterRecovered *thruster, uint32_t work_status)
{
    if (thruster != NULL) {
        thruster->work_status = work_status;
    }
}

int dp_update_thruster(DpThrusterRecovered *thruster)
{
    int32_t index;

    if (thruster == NULL || thruster->lever_arm.data == NULL ||
        thruster->force_input.data == NULL || thruster->force_output.data == NULL ||
        thruster->torque_output.data == NULL || thruster->lever_arm.count != 3 ||
        thruster->force_input.count != 3 || thruster->force_output.count != 3 ||
        thruster->torque_output.count != 3) {
        return -1;
    }
    if (thruster->work_status != 1u) {
        /* 原 UpdateThruster 先清零 +0x60，再清零 +0x88。 */
        for (index = 0; index < 3; ++index) {
            thruster->force_output.data[index] = 0.0;
        }
        for (index = 0; index < 3; ++index) {
            thruster->torque_output.data[index] = 0.0;
        }
        return 0;
    }
    if (vector_axpby(&thruster->force_input, thruster->force_scale,
                     &thruster->force_output, 0.0) != 0 ||
        vector3_cross(&thruster->lever_arm, &thruster->force_output,
                      &thruster->torque_output) != 0) {
        return -1;
    }
    return 0;
}

void dp_set_mtq_moment(DpMtqRecovered mtq[6], const double commanded_moment[6])
{
    unsigned index;

    if (mtq == NULL || commanded_moment == NULL) {
        return;
    }
    for (index = 0u; index < 6u; ++index) {
        const double command = commanded_moment[index];
        const double limit = mtq[index].moment_limit;
        const double magnitude = fabs(command);

        if (limit > magnitude) {
            mtq[index].actual_moment = command;
        } else if (command > 0.0) {
            mtq[index].actual_moment = limit;
        } else if (0.0 > command) {
            mtq[index].actual_moment = -limit;
        } else {
            /* 原零命令路径执行 limit * 0.0；保留其 +0 结果。 */
            mtq[index].actual_moment = limit * 0.0;
        }
    }
}

int dp_update_mag_moment(DpVector *group_moment_out_3, DpVector *channel_moment_6,
                         const DpMatrix *mapping_3x6,
                         const DpMtqRecovered mtq[6])
{
    unsigned index;

    if (group_moment_out_3 == NULL || channel_moment_6 == NULL || mapping_3x6 == NULL ||
        mtq == NULL || group_moment_out_3->data == NULL || channel_moment_6->data == NULL ||
        mapping_3x6->data == NULL || group_moment_out_3->count != 3 ||
        channel_moment_6->count != 6 || mapping_3x6->rows != 3 ||
        mapping_3x6->cols != 6 || mapping_3x6->row_stride < 6) {
        return -1;
    }
    for (index = 0u; index < 6u; ++index) {
        channel_moment_6->data[index] = mtq[index].actual_moment;
    }
    return blas_gemv(mapping_3x6, channel_moment_6, group_moment_out_3, 1.0, 0.0);
}

void dp_set_sada(DpSadaRecovered *sada, uint32_t command_flag,
                 const double command_angle[2])
{
    if (sada == NULL || command_angle == NULL) {
        return;
    }
    sada->command_flag = command_flag;
    sada->command_angle[0] = command_angle[0];
    sada->command_angle[1] = command_angle[1];
}

void dp_get_sada_angle(const DpSadaRecovered *sada, double out_angle[2])
{
    if (sada == NULL || out_angle == NULL) {
        return;
    }
    out_angle[0] = sada->current_angle[0];
    out_angle[1] = sada->current_angle[1];
}

void dp_drive_sada_once(DpSadaRecovered *sada, double step_time)
{
    unsigned index;

    if (sada == NULL) {
        return;
    }
    for (index = 0u; index < 2u; ++index) {
        double target = sada->command_limit[index];
        double increment_limit;
        double increment;

        if (sada->command_angle[index] <= target) {
            target = fmax(-target, sada->command_angle[index]);
        }
        increment_limit = sada->acceleration_limit[index] * step_time;
        increment = target - sada->angular_velocity[index];
        if (increment <= increment_limit) {
            increment = fmax(-increment_limit, increment);
        } else {
            increment = increment_limit;
        }
        sada->angular_velocity[index] += increment;
        sada->angular_acceleration[index] = increment / step_time;
        sada->current_angle[index] += sada->angular_velocity[index] * step_time;
    }
}

int dp_update_device_control(const DpDeviceControlCommand *command, double step_time,
                             DpDeviceControlContext *context)
{
    if (command == NULL || context == NULL || context->wheels == NULL ||
        context->wheel_group_angular_momentum_3 == NULL ||
        context->wheel_group_torque_3 == NULL || context->wheel_mapping_3x4 == NULL ||
        context->mtq == NULL || context->mtq_group_moment_3 == NULL ||
        context->mtq_channel_moment_6 == NULL || context->mtq_mapping_3x6 == NULL ||
        context->thruster == NULL || context->sada == NULL) {
        return -1;
    }

    /* 0x3848..0x3855：先按命令帧的 +0x74 执行惯量更新。 */
    if (context->inertia_update != NULL) {
        context->inertia_update(command->inertia_update_flag, context->inertia_update_opaque);
    }
    /* 0x3856..0x386b：SetWheelAcc，随后 UpdateWheel。 */
    dp_set_wheel_acc(context->wheels, command->wheel_torque_command,
                     context->gaussian_sampler, context->gaussian_opaque);
    if (dp_update_wheel_group(context->wheels, step_time,
                              context->wheel_group_angular_momentum_3,
                              context->wheel_group_torque_3,
                              context->wheel_mapping_3x4) != 0) {
        return -1;
    }
    /* 0x386c..0x3879：SetMomentMTQ，随后 UpdateMagMoment。 */
    dp_set_mtq_moment(context->mtq, command->mtq_moment_command);
    if (dp_update_mag_moment(context->mtq_group_moment_3,
                             context->mtq_channel_moment_6,
                             context->mtq_mapping_3x6, context->mtq) != 0) {
        return -1;
    }
    /* 0x387a..0x3888：推进器开关及更新。 */
    dp_set_thruster_work_status(context->thruster, command->thruster_work_status);
    if (dp_update_thruster(context->thruster) != 0) {
        return -1;
    }
    /* 0x38c1..0x38d7：SetSADA，随后 drive_SADA_once。 */
    dp_set_sada(context->sada, command->sada_command_flag, command->sada_command_angle);
    dp_drive_sada_once(context->sada, step_time);
    return 0;
}
