#include "dynamic_core_bridge.h"

#include "dynamic_flex.h"
#include "dynamic_recovered.h"
#include "dynamic_time.h"

#include <string.h>

static const uint64_t dp_sada_acceleration_reaction_bits[9] = {
    UINT64_C(0x0000000000000000), UINT64_C(0xc038235bb217ffc6), UINT64_C(0x3ff198d5cc359b79),
    UINT64_C(0x0000000000000000), UINT64_C(0xc0376b4eaf527e12), UINT64_C(0xbffa611795412e9d),
    UINT64_C(0x0000000000000000), UINT64_C(0xbfa702987a31042b), UINT64_C(0xc098d53ecdc86e9f)
};

static void dp_core_bridge_rhs(double time, const double state[DP_STATE_DIM],
                               double derivative[DP_STATE_DIM], void *opaque)
{
    const DpDynamicsContext *context = opaque;
    (void)time;
    if (dp_differential_equation_33(derivative, state, context) != 0) {
        memset(derivative, 0, DP_STATE_DIM * sizeof(double));
    }
}

int dp_core_default_model_init(DpCoreDefaultModel *model)
{
    DpSatInertiaFlag0Outputs output;

    if (model == NULL) {
        return -1;
    }
    memset(model, 0, sizeof(*model));
    output.inertia_3x3 = model->inertia;
    output.inverse_inertia_3x3 = model->inverse;
    output.matrix_3x3_e8 = model->m3_e8;
    output.matrix_3x3_448 = model->m3_448;
    output.matrix_3x3_4a8 = model->m3_4a8;
    output.matrix_3x3_508 = model->m3_508;
    output.matrix_3x3_568 = model->m3_568;
    output.matrix_3x10_688 = model->m6;
    output.coupling_3x10_898 = model->coupling_data;
    output.matrix_3x3_9a0 = model->m9;
    output.modal_matrix_a_10x10 = model->modal_a_data;
    output.modal_matrix_d_10x10 = model->modal_d_data;
    if (dp_load_sat_inertia_flag0_static(&output) != 0) {
        return -1;
    }
    model->coupling = (DpMatrix){3, 10, 10, 0, model->coupling_data};
    model->base_inertia = (DpMatrix){3, 3, 3, 0, model->inertia};
    model->modal_a = (DpMatrix){10, 10, 10, 0, model->modal_a_data};
    model->modal_d = (DpMatrix){10, 10, 10, 0, model->modal_d_data};
    model->sada_command_momentum_map = (DpMatrix){3, 3, 3, 0, model->m9};
    memcpy(model->sada_acceleration_reaction_data, dp_sada_acceleration_reaction_bits,
           sizeof(model->sada_acceleration_reaction_data));
    model->sada_acceleration_reaction_map_3x3 =
        (DpMatrix){3, 3, 3, 0, model->sada_acceleration_reaction_data};
    model->sada_modal_pre_map_3x3 = (DpMatrix){3, 3, 3, 0, model->m3_4a8};
    model->sada_modal_acceleration_map_3xn = (DpMatrix){3, 10, 10, 0, model->m6};
    return 0;
}

int dp_core_default_rk4_step_with_terms(DpCoreDefaultModel *model,
                                        double state[DP_STATE_DIM], double *time, double step,
                                        const DpVec3 *angular_momentum,
                                        const DpVec3 *minus_term)
{
    DpVec3 zero = {0.0, 0.0, 0.0};
    DpDynamicsContext context;

    if (model == NULL || state == NULL || time == NULL) {
        return -1;
    }
    context.flex.rigid_matrices.coupling_3xn = model->coupling;
    context.flex.rigid_matrices.base_inertia_3x3 = model->base_inertia;
    context.flex.modal_matrix_a_nxn = &model->modal_a;
    context.flex.modal_matrix_d_nxn = &model->modal_d;
    context.flex.angular_momentum = angular_momentum == NULL ? &zero : angular_momentum;
    context.flex.minus_term = minus_term;
    context.flex.rigid_map_3x3 = NULL;
    context.flex.rigid_aux = NULL;
    context.flex.sada_drive = &model->sada_drive;
    context.flex.sada_command_momentum_map_3x3 = &model->sada_command_momentum_map;
    context.flex.sada_acceleration_reaction_map_3x3 = &model->sada_acceleration_reaction_map_3x3;
    context.flex.sada_modal_pre_map_3x3 = &model->sada_modal_pre_map_3x3;
    context.flex.sada_modal_acceleration_map_3xn = &model->sada_modal_acceleration_map_3xn;

    /* CoreDynamic: TimeAdd(step_time) 在 RK4_Intergrator 之前。 */
    TimeAdd(step);
    dp_rk4_integrator_33(state, time, step, dp_core_bridge_rhs, &context);
    return 0;
}

int dp_core_default_rk4_step(DpCoreDefaultModel *model,
                             double state[DP_STATE_DIM], double *time, double step)
{
    return dp_core_default_rk4_step_with_terms(model, state, time, step, NULL, NULL);
}
