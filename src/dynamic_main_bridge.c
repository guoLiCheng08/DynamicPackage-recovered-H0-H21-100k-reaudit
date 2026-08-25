#include "dynamic_main_bridge.h"

#include "dynamic_orbit.h"
#include "dynamic_time.h"
#include "dynamic_torque.h"

#include <string.h>

static int dp_dyn_main_fill_telemetry_propagation(DpTelemetrySourceSnapshot *source,
                                                   const DpDeviceMeasureRecovered *devices,
                                                   const DpCoreEnvironmentOutputs *environment)
{
    double position_ecef_data[3] = {0.0};
    double velocity_ecef_data[3] = {0.0};
    DpVector position_gci = {3, 0, (double *)devices->gps.position_gci};
    DpVector velocity_gci = {3, 0, (double *)devices->gps.velocity_gci};
    DpVector position_ecef = {3, 0, position_ecef_data};
    DpVector velocity_ecef = {3, 0, velocity_ecef_data};
    DpCalendarTime calendar;

    memcpy(&calendar, environment->device_environment.time_values, sizeof(calendar));
    memcpy(source->propagated_prefix, environment->device_environment.time_values,
           sizeof(source->propagated_prefix));
    GCI2ECEF(&position_ecef, &position_gci, &calendar);
    GCI2ECEF(&velocity_ecef, &velocity_gci, &calendar);
    source->position_ecef.x = position_ecef_data[0];
    source->position_ecef.y = position_ecef_data[1];
    source->position_ecef.z = position_ecef_data[2];
    source->velocity_ecef.x = velocity_ecef_data[0];
    source->velocity_ecef.y = velocity_ecef_data[1];
    source->velocity_ecef.z = velocity_ecef_data[2];
    PosVel2Elements_M(&source->orbit_elements[0], &source->orbit_elements[1],
                      &source->orbit_elements[2], &source->orbit_elements[3],
                      &source->orbit_elements[4], &source->orbit_elements[5],
                      &source->orbit_elements[6], &source->orbit_elements[7],
                      devices->gps.position_gci, devices->gps.velocity_gci);
    return 0;
}

static void dp_dyn_main_fill_actuator_telemetry(DpTelemetrySourceSnapshot *source,
                                                const DpDeviceControlContext *device_control)
{
    unsigned index;

    if (device_control->wheels != NULL) {
        for (index = 0u; index < DP_WHEEL_COUNT; ++index) {
            source->rw_omega[index] = dp_get_wheel_speed(device_control->wheels, index);
        }
    }
    if (device_control->sada != NULL) {
        source->sada_raw[0] = device_control->sada->current_angle[0];
        source->sada_raw[1] = device_control->sada->current_angle[1];
        source->sada_raw[2] = device_control->sada->angular_velocity[0];
        source->sada_raw[3] = device_control->sada->angular_velocity[1];
    }
}

int dp_dyn_main_recovered_step(DpDynMainRecoveredContext *context,
                               DpState *state,
                               const DpDeviceControlCommand *command,
                               const DpDynMainCoreTerms *core_terms,
                               DpMainTelemetryFrame *out_telemetry,
                               DpIpcSharedFrame *optional_ipc_frame,
                               const DpDynMainIpcControl *ipc_control)
{
    DpCoreEnvironmentOutputs environment;
    DpTelemetrySourceSnapshot source;
    DpVec3 injected_wheel_h = core_terms != NULL ? core_terms->wheel_angular_momentum : (DpVec3){0.0, 0.0, 0.0};
    DpVec3 injected_minus_torque = core_terms != NULL ? core_terms->minus_total_torque : (DpVec3){0.0, 0.0, 0.0};
    const double device_control_step = context != NULL && context->device_control_step_time > 0.0
        ? context->device_control_step_time : (context == NULL ? 0.0 : context->step_time);

    if (context == NULL || state == NULL || command == NULL || core_terms == NULL ||
        out_telemetry == NULL || context->core_model == NULL || context->devices == NULL ||
        context->device_control == NULL || context->integration_time == NULL) {
        return -1;
    }
    if (dp_update_device_control(command, device_control_step, context->device_control) != 0) {
        return -1;
    }
    if (context->device_control->sada != NULL) {
        context->core_model->sada_drive.command_angle[0] =
            context->device_control->sada->command_angle[0];
        context->core_model->sada_drive.command_angle[1] =
            context->device_control->sada->command_angle[1];
        context->core_model->sada_drive.angular_acceleration[0] =
            context->device_control->sada->angular_acceleration[0];
        context->core_model->sada_drive.angular_acceleration[1] =
            context->device_control->sada->angular_acceleration[1];
    }
    /* 原 dyn_main 在 UpdateDeviceControl 后将 WheelGroup 的 H_w 直接写入核心输入，
     * 并以其群组力矩形成 SatTorque 的反号注入项。调用方额外提供的项按分量累加，
     * 以保持此前外部环境／测试力矩扩展接口。 */
    if (context->device_control->wheel_group_angular_momentum_3 != NULL) {
        injected_wheel_h.x += context->device_control->wheel_group_angular_momentum_3->data[0];
        injected_wheel_h.y += context->device_control->wheel_group_angular_momentum_3->data[1];
        injected_wheel_h.z += context->device_control->wheel_group_angular_momentum_3->data[2];
    }
    if (context->device_control->wheel_group_torque_3 != NULL) {
        injected_minus_torque.x += context->device_control->wheel_group_torque_3->data[0];
        injected_minus_torque.y += context->device_control->wheel_group_torque_3->data[1];
        injected_minus_torque.z += context->device_control->wheel_group_torque_3->data[2];
    }
    if (context->prior_magnetic_body_valid != 0u &&
        context->device_control->mtq_group_moment_3 != NULL) {
        DpVec3 magnetic_moment = {
            context->device_control->mtq_group_moment_3->data[0],
            context->device_control->mtq_group_moment_3->data[1],
            context->device_control->mtq_group_moment_3->data[2]
        };
        DpVec3 magnetic_torque;
        if (dp_mag_torque(&magnetic_torque, &magnetic_moment,
                          &context->prior_magnetic_body) != 0) {
            return -1;
        }
        /* SatTorque 将 MagTorque 直接累加；CoreDynamic 接口接收其反号。 */
        injected_minus_torque.x -= magnetic_torque.x;
        injected_minus_torque.y -= magnetic_torque.y;
        injected_minus_torque.z -= magnetic_torque.z;
    }
    if (dp_core_default_rk4_step_with_terms_and_environment(
            context->core_model, (double *)state, context->integration_time, context->step_time,
            &injected_wheel_h, &injected_minus_torque,
            &environment) != 0) {
        return -1;
    }
    context->prior_magnetic_body = environment.device_environment.magnetic_body;
    context->prior_magnetic_body_valid = 1u;
    dp_update_device_measure_recovered(context->devices, state,
                                       &environment.device_environment, context->step_time,
                                       context->gaussian2_sampler, context->gaussian2_opaque);
    if (dp_telemetry_source_from_devices(&source, context->devices) != 0 ||
        dp_dyn_main_fill_telemetry_propagation(&source, context->devices, &environment) != 0) {
        return -1;
    }
    dp_dyn_main_fill_actuator_telemetry(&source, context->device_control);
    dp_update_main_out_pack(out_telemetry, &source);
    if (optional_ipc_frame != NULL) {
        if (ipc_control == NULL) return -1;
        dp_send_dyn_tele(optional_ipc_frame, out_telemetry, ipc_control->drc_data_20,
                         ipc_control->drc_data_24, ipc_control->drc_data_40,
                         ipc_control->drc_data_44, ipc_control->drc_data_8c);
    }
    return 0;
}
