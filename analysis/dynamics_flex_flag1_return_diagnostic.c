#include <stdio.h>
#include "dynamic_satellite_globals.h"

int main(void)
{
    double acc_data[10] = {0.0};
    double pos_data[10] = {0.25, -0.5};
    double vel_out_data[10] = {0.0};
    double vel_data[10] = {0.75, 0.125};
    DpVec3 alpha = {0.0, 0.0, 0.0};
    DpVec3 rate = {1.0, 2.0, -1.0};
    DpVec3 h = {0.0, 0.0, 0.0};
    DpVec3 l = {0.0, 0.0, 0.0};
    DpVector acc = {10, 0, acc_data};
    DpVector pos = {10, 0, pos_data};
    DpVector vel_out = {10, 0, vel_out_data};
    DpVector vel = {10, 0, vel_data};
    DpFlexDynamicsConfig config;
    Update_sat_inertia_xw(1);
    if (dp_global_default_flex_config_get(&config) != 0) return 2;
    config.rigid_matrices.base_inertia_3x3 = J_c_B;
    config.angular_momentum = &h;
    config.minus_term = &l;
    printf("return=%d\n", dp_flex_dynamics_step(&alpha, &acc, &vel_out, &rate, &pos, &vel, &config));
    return 0;
}
