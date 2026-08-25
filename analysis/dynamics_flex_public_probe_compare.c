#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "dynamic_satellite_globals.h"

static uint64_t bits(double x) { uint64_t b; memcpy(&b, &x, sizeof(b)); return b; }
int main(void) {
    double a0d[3] = {0.1, -0.2, 0.3};
    double a1d[3] = {1.0, 2.0, -1.0};
    double a2d[10] = {0};
    double a3d[10] = {0.25, -0.5};
    double a4d[10] = {0};
    double a5d[10] = {0.75, 0.125};
    DpVector a0 = {3,0,a0d}, a1={3,0,a1d}, a2={10,0,a2d}, a3={10,0,a3d}, a4={10,0,a4d}, a5={10,0,a5d};
    static const uint64_t original_j_bits[9] = {
        UINT64_C(0x40924a0000000000), UINT64_C(0x4088700000000000), UINT64_C(0x3ff7ae147ae147ae),
        UINT64_C(0x40886fffffffffff), UINT64_C(0x4094b9ffffffffff), UINT64_C(0xc031b33333333333),
        UINT64_C(0x3ff7ae147ae147ae), UINT64_C(0xc031b33333333333), UINT64_C(0x40a02e0000000000)
    };
    Update_sat_inertia_xw(0);
    memcpy(J_c_B_mem, original_j_bits, sizeof(original_j_bits));
    SADA.current_angle[0] = 0.0;
    SADA.current_angle[1] = 3.14159265358979323846264338327950288;
    SADA.angular_velocity[0] = 3.14159265358979323846264338327950288;
    SADA.angular_velocity[1] = 0.0;
    SADA.angular_acceleration[0] = 0.0;
    SADA.angular_acceleration[1] = 0.0;
    dynamics_flex(&a0, &a1, &a2, &a3, &a4, &a5,
                  &L_c_B, &H_w_B, &J_c_B, &J_c_B_inv);
    printf("a0=%016llx,%016llx,%016llx\n",(unsigned long long)bits(a0d[0]),(unsigned long long)bits(a0d[1]),(unsigned long long)bits(a0d[2]));
    printf("a2=%016llx,%016llx,%016llx\n",(unsigned long long)bits(a2d[0]),(unsigned long long)bits(a2d[1]),(unsigned long long)bits(a2d[2]));
    printf("a4=%016llx,%016llx\n",(unsigned long long)bits(a4d[0]),(unsigned long long)bits(a4d[1]));
    return 0;
}
