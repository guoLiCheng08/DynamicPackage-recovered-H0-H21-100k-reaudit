#ifndef DYNAMIC_TORQUE_H
#define DYNAMIC_TORQUE_H

#include "dynamic_math.h"
#include "dynamic_recovered.h"

#ifdef __cplusplus
extern "C" {
#endif

/* RWheelTorque：清零输出后执行 output += -1 * input。 */
int dp_rwheel_torque(DpVec3 *out_torque, const DpVec3 *wheel_group_torque);
/* 原 ELF ABI：out、wheel_group_torque。 */
int RWheelTorque(DpVec3 *out_torque, const DpVec3 *wheel_group_torque);

/* MagTorque：out = magnetic_moment × magnetic_field。 */
int dp_mag_torque(DpVec3 *out_torque, const DpVec3 *magnetic_moment,
                  const DpVec3 *magnetic_field_body);
/* 原 ELF ABI：out、magnetic_moment、magnetic_field_body。 */
int MagTorque(DpVec3 *out_torque, const DpVec3 *magnetic_moment,
              const DpVec3 *magnetic_field_body);

/* GravityGradientTorque：3*GM_Earth/|r|^3 * (u × (J*u))，u=attitude*r/|r|。 */
int dp_gravity_gradient_torque(DpVec3 *out_torque, const DpVec3 *position_gci,
                               const DpMatrix *attitude_3x3,
                               const DpMatrix *inertia_3x3);
/* 原 ELF ABI：out、position、inertia、attitude；返回内部 vector_scale 状态。 */
int GravityGradientTorque(DpVec3 *out_torque, const DpVec3 *position_gci,
                          const DpMatrix *inertia_3x3,
                          const DpMatrix *attitude_3x3);

#ifdef __cplusplus
}
#endif
#endif /* DYNAMIC_TORQUE_H */
