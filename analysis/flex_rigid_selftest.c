#include <math.h>
#include <stdio.h>

#include "dynamic_flex.h"

static int close_enough(double actual, double expected)
{
    return fabs(actual - expected) <= 1e-12;
}

int main(void)
{
    double coupling_data[6] = {
        1.0, 2.0,
        0.0, 1.0,
        2.0, 0.0,
    };
    double inertia_data[9] = {
        10.0, 0.0, 0.0,
        0.0, 11.0, 0.0,
        0.0, 0.0, 12.0,
    };
    double effective_data[9] = {0.0};
    DpFlexRigidMatrices matrices;
    DpMatrix effective = {3, 3, 3, 0, effective_data};
    DpVec3 t0 = {4.0, 1.0, -2.0};
    DpVec3 t1 = {1.0, 2.0, 3.0};
    DpVec3 t2 = {-1.0, 0.0, 1.0};
    DpVec3 t3 = {0.0, 1.0, 2.0};
    DpVec3 omega = {1.0, 2.0, 3.0};
    DpVec3 momentum = {4.0, 5.0, 6.0};
    DpVec3 rhs = {0.0, 0.0, 0.0};
    DpVec3 acceleration = {0.0, 0.0, 0.0};

    matrices.coupling_3xn = (DpMatrix){3, 2, 2, 0, coupling_data};
    matrices.base_inertia_3x3 = (DpMatrix){3, 3, 3, 0, inertia_data};

    if (dp_flex_effective_inertia_3x3(&effective, &matrices) != 0) return 1;
    /* I - C*C^T = diag(5,10,8) with off-diagonal [-2,-2; -2,0; -2,0]. */
    if (!close_enough(effective_data[0], 5.0) ||
        !close_enough(effective_data[1], -2.0) ||
        !close_enough(effective_data[2], -2.0) ||
        !close_enough(effective_data[3], -2.0) ||
        !close_enough(effective_data[4], 10.0) ||
        !close_enough(effective_data[5], 0.0) ||
        !close_enough(effective_data[6], -2.0) ||
        !close_enough(effective_data[7], 0.0) ||
        !close_enough(effective_data[8], 8.0)) return 2;

    if (dp_flex_compose_rigid_rhs(&rhs, &t0, &t1, &t2, &t3, &omega, &momentum) != 0) return 3;
    /* four terms sum to (4,4,4), omega×H=(-3,6,-3), hence rhs=(7,-2,7). */
    if (!close_enough(rhs.x, 7.0) || !close_enough(rhs.y, -2.0) || !close_enough(rhs.z, 7.0)) return 4;

    if (dp_flex_solve_rigid_acceleration(&acceleration, &effective, &rhs) != 0) return 5;
    /* Verify by forward multiplication rather than embedding a second inverse implementation. */
    if (!close_enough(effective_data[0] * acceleration.x + effective_data[1] * acceleration.y + effective_data[2] * acceleration.z, rhs.x) ||
        !close_enough(effective_data[3] * acceleration.x + effective_data[4] * acceleration.y + effective_data[5] * acceleration.z, rhs.y) ||
        !close_enough(effective_data[6] * acceleration.x + effective_data[7] * acceleration.y + effective_data[8] * acceleration.z, rhs.z)) return 6;

    puts("flex rigid block self-test: PASS");
    return 0;
}
