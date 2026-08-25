#include <stdio.h>
#include <string.h>

#include "dynamic_core_bridge.h"
#include "dynamic_recovered.h"

static int read_blob(const char *name, void *out, size_t bytes)
{
    char path[256];
    FILE *file;
    (void)snprintf(path, sizeof(path), "analysis/time_orbit/%s", name);
    file = fopen(path, "rb");
    if (file == NULL) return -1;
    if (fread(out, 1u, bytes, file) != bytes || fclose(file) != 0) return -1;
    return 0;
}

int main(void)
{
    DpCoreDefaultModel model;
    DpDynamicsContext context;
    DpState state;
    DpVec3 zero = {0.0, 0.0, 0.0};
    double input_data[3] = {0.0, 0.01, -0.01};
    double output_data[3] = {0.0, 0.0, 0.0};
    DpVector input = {3, 0, input_data};
    DpVector output = {3, 0, output_data};
    double derivative[DP_STATE_DIM];

    if (read_blob("gold_sada_position_step1_pre_y.bin", &state, sizeof(state)) != 0 ||
        dp_core_default_model_init(&model) != 0) return 1;
    if (blas_gemv(&model.sada_command_momentum_map, &input, &output, 1.0, 0.0) != 0) return 1;
    printf("mapped momentum = %.17g %.17g %.17g\n", output_data[0], output_data[1], output_data[2]);
    input_data[0] = 0.0;
    input_data[1] = 1.7453292519943296e-05;
    input_data[2] = -8.726646259971648e-06;
    output_data[0] = output_data[1] = output_data[2] = 0.0;
    if (blas_gemv(&model.sada_command_momentum_map, &input, &output, 1.0, 0.0) != 0) return 1;
    printf("mapped reaction embedded = %.17g %.17g %.17g\n", output_data[0], output_data[1], output_data[2]);
    input_data[0] = 1.7453292519943296e-05;
    input_data[1] = -8.726646259971648e-06;
    input_data[2] = 0.0;
    output_data[0] = output_data[1] = output_data[2] = 0.0;
    if (blas_gemv(&model.sada_command_momentum_map, &input, &output, 1.0, 0.0) != 0) return 1;
    printf("mapped reaction leading = %.17g %.17g %.17g\n", output_data[0], output_data[1], output_data[2]);
    memset(&context, 0, sizeof(context));
    context.flex.rigid_matrices.coupling_3xn = model.coupling;
    context.flex.rigid_matrices.base_inertia_3x3 = model.base_inertia;
    context.flex.modal_matrix_a_nxn = &model.modal_a;
    context.flex.modal_matrix_d_nxn = &model.modal_d;
    context.flex.angular_momentum = &zero;
    model.sada_drive.command_angle[0] = 0.01;
    model.sada_drive.command_angle[1] = -0.01;
    model.sada_drive.angular_acceleration[0] = 1.7453292519943296e-05;
    model.sada_drive.angular_acceleration[1] = -8.726646259971648e-06;
    context.flex.sada_drive = &model.sada_drive;
    context.flex.sada_command_momentum_map_3x3 = &model.sada_command_momentum_map;
    {
        double effective_data[9] = {0.0};
        double reaction_data[3] = {0.0, 0.0, 0.0};
        double momentum_data[3] = {0.0, 0.0, 0.0};
        DpMatrix effective = {3, 3, 3, 0, effective_data};
        DpVec3 rhs;
        DpVec3 total_momentum;
        double *state_data = (double *)&state;
        DpVec3 rate = {state_data[4], state_data[5], state_data[6]};
        DpVec3 reaction;
        DpVector reaction_view = {3, 0, reaction_data};
        DpVector momentum_view = {3, 0, momentum_data};
        input_data[0] = 0.0; input_data[1] = 0.01; input_data[2] = -0.01;
        if (dp_flex_effective_inertia_3x3(&effective,
                &(DpFlexRigidMatrices){model.coupling, model.base_inertia}) != 0 ||
            blas_gemv(&model.base_inertia, &(DpVector){3, 0, &rate.x},
                      &momentum_view, 1.0, 0.0) != 0 ||
            blas_gemv(&model.sada_command_momentum_map, &input, &reaction_view, 1.0, 0.0) != 0) return 1;
        momentum_data[0] += reaction_data[0];
        momentum_data[1] += reaction_data[1];
        momentum_data[2] += reaction_data[2];
        total_momentum.x = momentum_data[0]; total_momentum.y = momentum_data[1]; total_momentum.z = momentum_data[2];
        input_data[0] = 0.0; input_data[1] = 1.7453292519943296e-05; input_data[2] = -8.726646259971648e-06;
        reaction_data[0] = reaction_data[1] = reaction_data[2] = 0.0;
        if (dp_flex_compose_rigid_rhs_coupled(&rhs, &rate, &total_momentum, NULL,
                &model.coupling, &model.modal_a, &model.modal_d,
                &(DpVector){10, 0, &state_data[13]},
                &(DpVector){10, 0, &state_data[23]}) != 0 ||
            blas_gemv(&model.sada_command_momentum_map, &input, &reaction_view, 1.0, 0.0) != 0) return 1;
        reaction.x = reaction_data[0]; reaction.y = reaction_data[1]; reaction.z = reaction_data[2];
        rhs.x -= reaction.x; rhs.y -= reaction.y; rhs.z -= reaction.z;
        printf("recovered final rhs = %.17g %.17g %.17g\n", rhs.x, rhs.y, rhs.z);
    }
    if (dp_differential_equation_33(derivative, (const double *)&state, &context) != 0) return 1;
    printf("derivative alpha = %.17g %.17g %.17g\n", derivative[4], derivative[5], derivative[6]);
    return 0;
}
