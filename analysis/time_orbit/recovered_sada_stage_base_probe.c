#include <stdio.h>
#include <string.h>

#include "dynamic_core_bridge.h"
#include "dynamic_recovered.h"

#define GOLD_DIR "analysis/time_orbit/"

static int read_blob(const char *name, void *out, size_t bytes)
{
    char path[256];
    FILE *file;
    (void)snprintf(path, sizeof(path), "%s%s", GOLD_DIR, name);
    file = fopen(path, "rb");
    if (file == NULL) return -1;
    if (fread(out, 1u, bytes, file) != bytes || fclose(file) != 0) return -1;
    return 0;
}

static void advance_state(double out[DP_STATE_DIM], const double state[DP_STATE_DIM],
                          const double derivative[DP_STATE_DIM], double scale)
{
    unsigned index;
    for (index = 0u; index < DP_STATE_DIM; ++index) out[index] = state[index] + scale * derivative[index];
}

int main(void)
{
    DpCoreDefaultModel model;
    DpDynamicsContext context;
    DpState initial;
    double stage_state[DP_STATE_DIM];
    double stage_input[4][DP_STATE_DIM];
    double derivative[4][DP_STATE_DIM];
    DpVec3 zero = {0.0, 0.0, 0.0};
    FILE *file;
    unsigned stage;

    if (read_blob("gold_sada_position_step1_pre_y.bin", &initial, sizeof(initial)) != 0 ||
        dp_core_default_model_init(&model) != 0) return 1;
    memset(&context, 0, sizeof(context));
    memset(derivative, 0, sizeof(derivative));
    context.flex.rigid_matrices.coupling_3xn = model.coupling;
    context.flex.rigid_matrices.base_inertia_3x3 = model.base_inertia;
    context.flex.modal_matrix_a_nxn = &model.modal_a;
    context.flex.modal_matrix_d_nxn = &model.modal_d;
    context.flex.angular_momentum = &zero;
    context.flex.rigid_aux = NULL;
    model.sada_drive.command_angle[0] = 0.01;
    model.sada_drive.command_angle[1] = -0.01;
    model.sada_drive.angular_acceleration[0] = 1.7453292519943296e-05;
    model.sada_drive.angular_acceleration[1] = -8.726646259971648e-06;
    context.flex.sada_drive = &model.sada_drive;
    context.flex.sada_command_momentum_map_3x3 = &model.sada_command_momentum_map;
    context.flex.sada_acceleration_reaction_map_3x3 = &model.sada_acceleration_reaction_map_3x3;
    context.flex.sada_modal_pre_map_3x3 = &model.sada_modal_pre_map_3x3;
    context.flex.sada_modal_acceleration_map_3xn = &model.sada_modal_acceleration_map_3xn;

    memcpy(stage_input[0], &initial, sizeof(stage_input[0]));
    if (dp_differential_equation_33(derivative[0], stage_input[0], &context) != 0) return 1;
    printf("stage1 direct alpha=%.17g %.17g %.17g modal0=%.17g map=%p\n",
           derivative[0][4], derivative[0][5], derivative[0][6], derivative[0][13],
           (const void *)context.flex.sada_modal_acceleration_map_3xn);
    advance_state(stage_state, stage_input[0], derivative[0], 0.05);
    memcpy(stage_input[1], stage_state, sizeof(stage_input[1]));
    if (dp_differential_equation_33(derivative[1], stage_input[1], &context) != 0) return 1;
    advance_state(stage_state, stage_input[0], derivative[1], 0.05);
    memcpy(stage_input[2], stage_state, sizeof(stage_input[2]));
    if (dp_differential_equation_33(derivative[2], stage_input[2], &context) != 0) return 1;
    advance_state(stage_state, stage_input[0], derivative[2], 0.1);
    memcpy(stage_input[3], stage_state, sizeof(stage_input[3]));
    if (dp_differential_equation_33(derivative[3], stage_input[3], &context) != 0) return 1;

    file = fopen("/tmp/e10_recovered_sada_with_momentum_stages.bin", "wb");
    if (file == NULL) return 1;
    for (stage = 0u; stage < 4u; ++stage) {
        if (fwrite(&derivative[stage][4], sizeof(double), 3u, file) != 3u ||
            fwrite(&derivative[stage][13], sizeof(double), 10u, file) != 10u) {
            (void)fclose(file);
            return 1;
        }
    }
    if (fclose(file) != 0) return 1;
    file = fopen("/tmp/e10_recovered_sada_stage_y.bin", "wb");
    if (file == NULL || fwrite(stage_input, sizeof(stage_input), 1u, file) != 1u ||
        fclose(file) != 0) return 1;
    file = fopen("/tmp/e10_recovered_sada_stage_dydt.bin", "wb");
    if (file == NULL || fwrite(derivative, sizeof(derivative), 1u, file) != 1u ||
        fclose(file) != 0) return 1;
    return 0;
}
