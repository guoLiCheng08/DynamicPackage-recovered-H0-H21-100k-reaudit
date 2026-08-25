#include "dynamic_core_environment.h"

#include <string.h>

static void dp_core_environment_rhs(double time, const double state[DP_STATE_DIM],
                                    double derivative[DP_STATE_DIM], void *opaque)
{
    const DpDynamicsContext *context = opaque;
    (void)time;
    if (dp_differential_equation_33(derivative, state, context) != 0) {
        memset(derivative, 0, DP_STATE_DIM * sizeof(double));
    }
}

static void dp_core_environment_build_context(DpDynamicsContext *context,
                                              DpCoreDefaultModel *model,
                                              const DpVec3 *angular_momentum,
                                              const DpVec3 *minus_term,
                                              DpVec3 *zero)
{
    context->flex.rigid_matrices.coupling_3xn = model->coupling;
    context->flex.rigid_matrices.base_inertia_3x3 = model->base_inertia;
    context->flex.modal_matrix_a_nxn = &model->modal_a;
    context->flex.modal_matrix_d_nxn = &model->modal_d;
    context->flex.angular_momentum = angular_momentum == NULL ? zero : angular_momentum;
    context->flex.minus_term = minus_term;
    context->flex.rigid_map_3x3 = NULL;
    context->flex.rigid_aux = NULL;
    context->flex.sada_drive = &model->sada_drive;
    context->flex.sada_command_momentum_map_3x3 = &model->sada_command_momentum_map;
    context->flex.sada_acceleration_reaction_map_3x3 = &model->sada_acceleration_reaction_map_3x3;
    context->flex.sada_modal_pre_map_3x3 = &model->sada_modal_pre_map_3x3;
    context->flex.sada_modal_acceleration_map_3xn = &model->sada_modal_acceleration_map_3xn;
}

int dp_core_environment_from_states(DpCoreEnvironmentOutputs *out,
                                    const double pre_integration_state[DP_STATE_DIM],
                                    const double post_integration_state[DP_STATE_DIM],
                                    const DpCalendarTime *calendar_time)
{
    double pre_position_data[3];
    double pre_velocity_data[3];
    double q_xyz_data[3];
    double attitude_data[9] = {0.0};
    double sun_gci_data[3] = {0.0};
    double magnetic_body_data[3] = {0.0};
    double sun_body_data[3] = {0.0};
    DpVector pre_position = {3, 0, pre_position_data};
    DpVector pre_velocity = {3, 0, pre_velocity_data};
    DpQuatAbi attitude;
    DpMatrix attitude_matrix = {3, 3, 3, 0, attitude_data};
    DpVector magnetic_gci = {3, 0, &out->magnetic_gci.x};
    DpVector sun_gci = {3, 0, sun_gci_data};
    DpVector magnetic_body = {3, 0, magnetic_body_data};
    DpVector sun_body = {3, 0, sun_body_data};
    double julian_date;

    if (out == NULL || pre_integration_state == NULL || post_integration_state == NULL ||
        calendar_time == NULL) {
        return -1;
    }
    pre_position_data[0] = pre_integration_state[7];
    pre_position_data[1] = pre_integration_state[8];
    pre_position_data[2] = pre_integration_state[9];
    pre_velocity_data[0] = pre_integration_state[10];
    pre_velocity_data[1] = pre_integration_state[11];
    pre_velocity_data[2] = pre_integration_state[12];
    q_xyz_data[0] = post_integration_state[1];
    q_xyz_data[1] = post_integration_state[2];
    q_xyz_data[2] = post_integration_state[3];
    attitude.w = post_integration_state[0];
    attitude.xyz.count = 3;
    attitude.xyz.reserved_04 = 0;
    attitude.xyz.data = q_xyz_data;

    (void)dp_calc_inertial_magnetic_vector(&magnetic_gci, &pre_position, &pre_velocity,
                                            calendar_time);
    quat_att_mat(&attitude_matrix, &attitude);
    (void)blas_gemv(&attitude_matrix, &magnetic_gci, &magnetic_body, 1.0, 0.0);

    julian_date = Calc_JD(calendar_time->year, calendar_time->month,
                          calendar_time->day, calendar_time->hour,
                          calendar_time->minute, calendar_time->second);
    dp_sun_vector(julian_date, &sun_gci);
    (void)blas_gemv(&attitude_matrix, &sun_gci, &sun_body, 1.0, 0.0);

    out->device_environment.sun_body.x = sun_body_data[0];
    out->device_environment.sun_body.y = sun_body_data[1];
    out->device_environment.sun_body.z = sun_body_data[2];
    out->device_environment.sun_gci.x = sun_gci_data[0];
    out->device_environment.sun_gci.y = sun_gci_data[1];
    out->device_environment.sun_gci.z = sun_gci_data[2];
    out->device_environment.magnetic_body.x = magnetic_body_data[0];
    out->device_environment.magnetic_body.y = magnetic_body_data[1];
    out->device_environment.magnetic_body.z = magnetic_body_data[2];
    memcpy(out->device_environment.time_values, calendar_time,
           sizeof(out->device_environment.time_values));
    return 0;
}

int dp_core_default_rk4_step_with_terms_and_environment(
    DpCoreDefaultModel *model, double state[DP_STATE_DIM], double *time, double step,
    const DpVec3 *angular_momentum, const DpVec3 *minus_term,
    DpCoreEnvironmentOutputs *out_environment)
{
    double pre_integration_state[DP_STATE_DIM];
    DpCalendarTime calendar_time;
    DpVec3 zero = {0.0, 0.0, 0.0};
    DpDynamicsContext context;

    if (model == NULL || state == NULL || time == NULL || out_environment == NULL) {
        return -1;
    }
    memcpy(pre_integration_state, state, sizeof(pre_integration_state));
    dp_core_environment_build_context(&context, model, angular_momentum, minus_term, &zero);
    TimeAdd(step);
    TimeArrayGet((double *)&calendar_time);
    if (dp_core_environment_from_states(out_environment, pre_integration_state, state,
                                        &calendar_time) != 0) {
        return -1;
    }
    dp_rk4_integrator_33(state, time, step, dp_core_environment_rhs, &context);
    return dp_core_environment_from_states(out_environment, pre_integration_state, state,
                                           &calendar_time);
}
