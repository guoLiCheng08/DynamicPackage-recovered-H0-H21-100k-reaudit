#include <stdio.h>

#include "dynamic_core_bridge.h"
#include "dynamic_flex.h"

int main(void)
{
    DpCoreDefaultModel model;
    double effective_data[9] = {0.0};
    double delta_data[3] = {
        2.7341254189096375e-08,
        -3.6380063456012022e-08,
        1.0912338781943976e-07
    };
    double rhs_data[3] = {0.0};
    DpMatrix effective = {3, 3, 3, 0, effective_data};
    DpVector delta = {3, 0, delta_data};
    DpVector rhs = {3, 0, rhs_data};

    if (dp_core_default_model_init(&model) != 0 ||
        dp_flex_effective_inertia_3x3(&effective,
            &(DpFlexRigidMatrices){model.coupling, model.base_inertia}) != 0 ||
        blas_gemv(&effective, &delta, &rhs, 1.0, 0.0) != 0) return 1;
    printf("remaining SADA alpha-delta RHS = % .17g % .17g % .17g\n",
           rhs_data[0], rhs_data[1], rhs_data[2]);
    return 0;
}
