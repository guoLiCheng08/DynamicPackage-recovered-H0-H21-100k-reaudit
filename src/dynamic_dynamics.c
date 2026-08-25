#include "dynamic_dynamics.h"

#include <stddef.h>
#include <string.h>
#ifdef DP_DIAGNOSTIC_FLEX_RHS_TRACE
#include <stdio.h>

/* 每个微分调用 6 doubles／48 bytes：body-rate[3] 后接刚柔函数使用的
 * local H_total[3]。仅供离线 H0 原 ELF交叉输入差分，默认不编译。 */
static void dp_dynamics_h_trace_binary(const DpVec3 *body_rate,
                                       const double coupled_momentum[3])
{
    double record[6];
    FILE *trace;

    if (body_rate == NULL || coupled_momentum == NULL) {
        return;
    }
    memcpy(&record[0], body_rate, 3u * sizeof(record[0]));
    memcpy(&record[3], coupled_momentum, 3u * sizeof(record[0]));
    trace = fopen("/tmp/dp_dynamics_h_trace.bin", "ab");
    if (trace == NULL) {
        return;
    }
    (void)fwrite(record, sizeof(record), 1u, trace);
    (void)fclose(trace);
}
#endif
#ifdef DP_DIAGNOSTIC_H_TRACE
#include <inttypes.h>
#include <stdio.h>
#endif

/* 原 dynamics：六个 descriptor 参数依次为加速度输出、外部加项、角速度、
 * 附加动量、惯量和逆惯量。局部项先形成 -omega×(J·omega+附加动量)，再将
 * 第二参数累加到该局部项；第一参数接收局部和经逆惯量投影的结果，第二参数不写回。 */
void dynamics(DpVector *angular_acceleration_out, const DpVector *external_term,
              const DpVector *body_rate, const DpVector *additional_momentum,
              const DpMatrix *inertia, const DpMatrix *inverse_inertia)
{
    double total_momentum_data[3] = {0.0, 0.0, 0.0};
    double gyroscopic_term_data[3] = {0.0, 0.0, 0.0};
    DpVector total_momentum = {3, 0, total_momentum_data};
    DpVector gyroscopic_term = {3, 0, gyroscopic_term_data};

    (void)blas_gemv(inertia, body_rate, &total_momentum, 1.0, 0.0);
    (void)vector_add(&total_momentum, additional_momentum);
    (void)vector3_cross(body_rate, &total_momentum, &gyroscopic_term);
    (void)vector_scale(&gyroscopic_term, -1.0);
    (void)vector_axpy(external_term, 1.0, &gyroscopic_term);
    (void)blas_gemv(inverse_inertia, &gyroscopic_term, angular_acceleration_out,
                    1.0, 0.0);
}

int dp_differential_equation_33(double derivative[DP_STATE_DIM],
                                const double state[DP_STATE_DIM],
                                const DpDynamicsContext *context)
{
    double quaternion_xyz[3];
    double quaternion_dot_xyz[3] = {0.0, 0.0, 0.0};
    double modal_acceleration_data[DP_FLEX_MAX_MODE_DIM] = {0.0};
    double modal_velocity_output_data[DP_FLEX_MAX_MODE_DIM] = {0.0};
    double position_data[3];
    double acceleration_data[3] = {0.0, 0.0, 0.0};
    DpQuatAbi quaternion = {0};
    DpQuatAbi quaternion_dot = {0};
    DpVector angular_rate = {0};
    DpVector modal_position = {0};
    DpVector modal_velocity = {0};
    DpVector modal_acceleration = {0};
    DpVector modal_velocity_output = {0};
    DpVector position = {0};
    DpVector acceleration = {0};
    double coupled_momentum_data[3] = {0.0, 0.0, 0.0};
    double coupling_momentum_data[3] = {0.0, 0.0, 0.0};
    double sada_command_input_data[3] = {0.0, 0.0, 0.0};
    double sada_command_pre_data[3] = {0.0, 0.0, 0.0};
    double sada_command_momentum_data[3] = {0.0, 0.0, 0.0};
    DpVector body_rate_view = {0};
    DpVector coupled_momentum = {0};
    DpVector coupling_momentum = {0};
    DpVector actuator_momentum = {0};
    DpVector sada_command_input = {0};
    DpVector sada_command_pre = {0};
    DpVector sada_command_momentum = {0};
    DpFlexDynamicsConfig flex_config = {0};
    DpVec3 body_rate = {0.0, 0.0, 0.0};
    DpVec3 angular_acceleration = {0.0, 0.0, 0.0};
    const int mode_count = context == NULL ? 0 : context->flex.rigid_matrices.coupling_3xn.cols;

    if (derivative == NULL || state == NULL || context == NULL ||
        mode_count <= 0 || mode_count > DP_FLEX_MAX_MODE_DIM) {
        return -1;
    }

    quaternion_xyz[0] = state[1];
    quaternion_xyz[1] = state[2];
    quaternion_xyz[2] = state[3];
    quaternion.w = state[0];
    quaternion.xyz.count = 3;
    quaternion.xyz.reserved_04 = 0;
    quaternion.xyz.data = quaternion_xyz;
    quat_reunit(&quaternion);

    angular_rate.count = 3;
    angular_rate.reserved_04 = 0;
    angular_rate.data = (double *)&state[4];
    quaternion_dot.w = 0.0;
    quaternion_dot.xyz.count = 3;
    quaternion_dot.xyz.reserved_04 = 0;
    quaternion_dot.xyz.data = quaternion_dot_xyz;
    quat_diff(&quaternion_dot, &quaternion, &angular_rate);

    body_rate.x = state[4];
    body_rate.y = state[5];
    body_rate.z = state[6];
    body_rate_view.count = 3;
    body_rate_view.reserved_04 = 0;
    body_rate_view.data = &body_rate.x;
    coupled_momentum.count = 3;
    coupled_momentum.reserved_04 = 0;
    coupled_momentum.data = coupled_momentum_data;
    coupling_momentum.count = 3;
    coupling_momentum.reserved_04 = 0;
    coupling_momentum.data = coupling_momentum_data;
    modal_position.count = mode_count;
    modal_position.reserved_04 = 0;
    modal_position.data = (double *)&state[13];
    modal_velocity.count = mode_count;
    modal_velocity.reserved_04 = 0;
    modal_velocity.data = (double *)&state[13 + mode_count];
    modal_acceleration.count = mode_count;
    modal_acceleration.reserved_04 = 0;
    modal_acceleration.data = modal_acceleration_data;
    modal_velocity_output.count = mode_count;
    modal_velocity_output.reserved_04 = 0;
    modal_velocity_output.data = modal_velocity_output_data;

    /* 原 dynamics_flex 0xebbd..0xedf7 在陀螺叉乘前按严格加法顺序组装
     * H_total：J·omega，随后 H_w，随后 SADA 命令投影，最后 C·eta。 */
    if (context->flex.angular_momentum == NULL ||
        blas_gemv(&context->flex.rigid_matrices.base_inertia_3x3, &body_rate_view,
                  &coupled_momentum, 1.0, 0.0) != 0) {
        return -1;
    }
    actuator_momentum.count = 3;
    actuator_momentum.reserved_04 = 0;
    actuator_momentum.data = (double *)&context->flex.angular_momentum->x;
    if (vector_add(&coupled_momentum, &actuator_momentum) != 0) {
        return -1;
    }
    sada_command_input.count = 3;
    sada_command_input.reserved_04 = 0;
    sada_command_input.data = sada_command_input_data;
    sada_command_pre.count = 3;
    sada_command_pre.reserved_04 = 0;
    sada_command_pre.data = sada_command_pre_data;
    sada_command_momentum.count = 3;
    sada_command_momentum.reserved_04 = 0;
    sada_command_momentum.data = sada_command_momentum_data;
    if (context->flex.sada_drive != NULL &&
        context->flex.sada_modal_pre_map_3x3 != NULL &&
        context->flex.sada_command_momentum_map_3x3 != NULL) {
        /* 原 dynamics_flex 0xec41..0xeccf：命令角 [0,s0,s1] 先经 Sat+0x4a8，
         * 再经 Sat+0x9a0；二者结果才并入陀螺叉乘前的总角动量。 */
        sada_command_input_data[1] = context->flex.sada_drive->command_angle[0];
        sada_command_input_data[2] = context->flex.sada_drive->command_angle[1];
        if (blas_gemv(context->flex.sada_modal_pre_map_3x3,
                      &sada_command_input, &sada_command_pre, 1.0, 0.0) != 0 ||
            blas_gemv(context->flex.sada_command_momentum_map_3x3,
                      &sada_command_pre, &sada_command_momentum, 1.0, 0.0) != 0 ||
            vector_add(&coupled_momentum, &sada_command_momentum) != 0) {
            return -1;
        }
    }
    if (blas_gemv(&context->flex.rigid_matrices.coupling_3xn, &modal_position,
                  &coupling_momentum, 1.0, 0.0) != 0 ||
        vector_add(&coupled_momentum, &coupling_momentum) != 0) {
        return -1;
    }
#ifdef DP_DIAGNOSTIC_H_TRACE
    {
        uint64_t h_bits[3];
        memcpy(&h_bits[0], &coupled_momentum.data[0], sizeof(h_bits[0]));
        memcpy(&h_bits[1], &coupled_momentum.data[1], sizeof(h_bits[1]));
        memcpy(&h_bits[2], &coupled_momentum.data[2], sizeof(h_bits[2]));
        (void)fprintf(stderr, "dp_h_total=%016" PRIx64 ",%016" PRIx64 ",%016" PRIx64 "\\n",
                      h_bits[0], h_bits[1], h_bits[2]);
    }
#endif
#ifdef DP_DIAGNOSTIC_FLEX_RHS_TRACE
    dp_dynamics_h_trace_binary(&body_rate, coupled_momentum.data);
#endif
    flex_config = context->flex;
    flex_config.angular_momentum = (const DpVec3 *)coupled_momentum.data;
    if (dp_flex_dynamics_step(&angular_acceleration, &modal_acceleration,
                              &modal_velocity_output, &body_rate, &modal_position,
                              &modal_velocity, &flex_config) != 0) {
        return -1;
    }

    position_data[0] = state[7];
    position_data[1] = state[8];
    position_data[2] = state[9];
    position.count = 3;
    position.reserved_04 = 0;
    position.data = position_data;
    acceleration.count = 3;
    acceleration.reserved_04 = 0;
    acceleration.data = acceleration_data;
    orbit_dynamic(&acceleration, &position);

    memset(derivative, 0, DP_STATE_DIM * sizeof(double));
    derivative[0] = quaternion_dot.w;
    derivative[1] = quaternion_dot.xyz.data[0];
    derivative[2] = quaternion_dot.xyz.data[1];
    derivative[3] = quaternion_dot.xyz.data[2];
    derivative[4] = angular_acceleration.x;
    derivative[5] = angular_acceleration.y;
    derivative[6] = angular_acceleration.z;
    derivative[7] = state[10];
    derivative[8] = state[11];
    derivative[9] = state[12];
    derivative[10] = acceleration.data[0];
    derivative[11] = acceleration.data[1];
    derivative[12] = acceleration.data[2];
    memcpy(&derivative[13], modal_acceleration.data, (size_t)mode_count * sizeof(double));
    memcpy(&derivative[13 + mode_count], modal_velocity_output.data,
           (size_t)mode_count * sizeof(double));
    return 0;
}
