#include <stdio.h>

#include "dynamic_core_bridge.h"
#include "dynamic_flex.h"

static void print_vec(const char *label, const DpVec3 *value)
{
    printf("%-25s % .17g % .17g % .17g\n", label, value->x, value->y, value->z);
}

int main(void)
{
    DpCoreDefaultModel model;
    DpMatrix effective = {3, 3, 3, 0, (double[9]){0.0}};
    const char *names[6] = {"m3_e8", "m3_448", "m3_4a8", "m3_508", "m3_568", "m9"};
    DpMatrix matrices[6];
    DpVec3 command = {0.0, 0.01, -0.01};
    DpVec3 acceleration = {0.0, 1.7453292519943296e-05, -8.726646259971648e-06};
    DpVec3 omega = {0.0010000000474974513, -0.0020000000949949026, 0.003000000026077032};
    DpVector command_view = {3, 0, &command.x};
    DpVector acceleration_view = {3, 0, &acceleration.x};
    unsigned index;

    if (dp_core_default_model_init(&model) != 0 ||
        dp_flex_effective_inertia_3x3(&effective, &(DpFlexRigidMatrices){model.coupling, model.base_inertia}) != 0) return 1;
    matrices[0] = (DpMatrix){3, 3, 3, 0, model.m3_e8};
    matrices[1] = (DpMatrix){3, 3, 3, 0, model.m3_448};
    matrices[2] = (DpMatrix){3, 3, 3, 0, model.m3_4a8};
    matrices[3] = (DpMatrix){3, 3, 3, 0, model.m3_508};
    matrices[4] = (DpMatrix){3, 3, 3, 0, model.m3_568};
    matrices[5] = (DpMatrix){3, 3, 3, 0, model.m9};

    for (index = 0u; index < 6u; ++index) {
        DpVec3 mapped_command = {0.0, 0.0, 0.0};
        DpVec3 mapped_acceleration = {0.0, 0.0, 0.0};
        DpVec3 cross_command = {0.0, 0.0, 0.0};
        DpVec3 cross_acceleration = {0.0, 0.0, 0.0};
        DpVec3 solved = {0.0, 0.0, 0.0};
        DpVector mc_view = {3, 0, &mapped_command.x};
        DpVector ma_view = {3, 0, &mapped_acceleration.x};
        char label[80];
        if (blas_gemv(&matrices[index], &command_view, &mc_view, 1.0, 0.0) != 0 ||
            blas_gemv(&matrices[index], &acceleration_view, &ma_view, 1.0, 0.0) != 0 ||
            vector3_cross(&(DpVector){3, 0, &omega.x}, &mc_view,
                          &(DpVector){3, 0, &cross_command.x}) != 0 ||
            vector3_cross(&(DpVector){3, 0, &omega.x}, &ma_view,
                          &(DpVector){3, 0, &cross_acceleration.x}) != 0) return 1;
        if (dp_flex_solve_rigid_acceleration(&solved, &effective, &mapped_command) != 0) return 1;
        (void)snprintf(label, sizeof(label), "Jinv*%s*cmd", names[index]);
        print_vec(label, &solved);
        if (dp_flex_solve_rigid_acceleration(&solved, &effective, &mapped_acceleration) != 0) return 1;
        (void)snprintf(label, sizeof(label), "Jinv*%s*acc", names[index]);
        print_vec(label, &solved);
        if (dp_flex_solve_rigid_acceleration(&solved, &effective, &cross_command) != 0) return 1;
        (void)snprintf(label, sizeof(label), "Jinv*(w×%s*cmd)", names[index]);
        print_vec(label, &solved);
        if (dp_flex_solve_rigid_acceleration(&solved, &effective, &cross_acceleration) != 0) return 1;
        (void)snprintf(label, sizeof(label), "Jinv*(w×%s*acc)", names[index]);
        print_vec(label, &solved);
    }
    return 0;
}
