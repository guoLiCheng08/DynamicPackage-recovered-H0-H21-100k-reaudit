#include "dynamic_torque.h"

#include <stddef.h>

static DpVector dp_vec3_view(DpVec3 *value)
{
    DpVector view;
    view.count = 3;
    view.reserved_04 = 0;
    view.data = &value->x;
    return view;
}

int dp_rwheel_torque(DpVec3 *out_torque, const DpVec3 *wheel_group_torque)
{
    DpVector out;
    DpVector input;

    if (out_torque == NULL || wheel_group_torque == NULL) return -1;
    out = dp_vec3_view(out_torque);
    input = dp_vec3_view((DpVec3 *)wheel_group_torque);
    out_torque->x = 0.0;
    out_torque->y = 0.0;
    out_torque->z = 0.0;
    return vector_axpy(&input, -1.0, &out);
}

int dp_mag_torque(DpVec3 *out_torque, const DpVec3 *magnetic_moment,
                  const DpVec3 *magnetic_field_body)
{
    DpVector out;
    DpVector moment;
    DpVector field;

    if (out_torque == NULL || magnetic_moment == NULL || magnetic_field_body == NULL) return -1;
    out = dp_vec3_view(out_torque);
    moment = dp_vec3_view((DpVec3 *)magnetic_moment);
    field = dp_vec3_view((DpVec3 *)magnetic_field_body);
    return vector3_cross(&moment, &field, &out);
}

int dp_gravity_gradient_torque(DpVec3 *out_torque, const DpVec3 *position_gci,
                               const DpMatrix *attitude_3x3,
                               const DpMatrix *inertia_3x3)
{
    DpVec3 unit_position;
    DpVec3 inertia_times_unit = {0.0, 0.0, 0.0};
    DpVector position;
    DpVector unit;
    DpVector inertia_unit;
    DpVector out;
    const double radius = position_gci == NULL ? 0.0 :
                          vector_nrm2(&(DpVector){3, 0, (double *)&position_gci->x});
    double scale;

    if (out_torque == NULL || position_gci == NULL || attitude_3x3 == NULL ||
        inertia_3x3 == NULL || radius == 0.0) return -1;
    position = dp_vec3_view((DpVec3 *)position_gci);
    unit = dp_vec3_view(&unit_position);
    inertia_unit = dp_vec3_view(&inertia_times_unit);
    out = dp_vec3_view(out_torque);
    if (blas_gemv(attitude_3x3, &position, &unit, 1.0 / radius, 0.0) != 0 ||
        blas_gemv(inertia_3x3, &unit, &inertia_unit, 1.0, 0.0) != 0 ||
        vector3_cross(&unit, &inertia_unit, &out) != 0) return -1;
    {
        double radius_cubed = radius;
        radius_cubed *= radius;
        radius_cubed *= radius;
        /* GravityGradientTorque 读取原 ELF ffa0：GM_Earth = 3.986004415e14。 */
        scale = 3.0 * 3.986004415e14;
        scale /= radius_cubed;
    }
    return vector_scale(&out, scale);
}

/* 原 ELF ABI：rdi=输出，rsi=位置，rdx=惯量，rcx=姿态。 */
int GravityGradientTorque(DpVec3 *out_torque, const DpVec3 *position_gci,
                          const DpMatrix *inertia_3x3,
                          const DpMatrix *attitude_3x3)
{
    return dp_gravity_gradient_torque(out_torque, position_gci,
                                      attitude_3x3, inertia_3x3);
}

/* 原 ELF ABI：rdi=输出，rsi=磁矩，rdx=磁场；内部叉乘 ABI 为 moment、field、out。 */
int MagTorque(DpVec3 *out_torque, const DpVec3 *magnetic_moment,
              const DpVec3 *magnetic_field_body)
{
    return dp_mag_torque(out_torque, magnetic_moment, magnetic_field_body);
}

/* 原 ELF ABI：rdi=输出，rsi=轮组力矩；先清零再输出 -input。 */
int RWheelTorque(DpVec3 *out_torque, const DpVec3 *wheel_group_torque)
{
    return dp_rwheel_torque(out_torque, wheel_group_torque);
}
