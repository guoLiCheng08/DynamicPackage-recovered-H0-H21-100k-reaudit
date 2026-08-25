/* DynamicPackage 轨道动力学恢复内核。 */
#ifndef DYNAMIC_ORBIT_H
#define DYNAMIC_ORBIT_H

#include "dynamic_recovered.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double gravitational_parameter;
    double reference_radius;
    double j2;
    double j3;
    double j4;
    double j5;
    double j6;
    double spacecraft_mass;
} DpOrbitGravityModel;

/* 从原 ELF 的 GM_Earth、R_Earth 与 J2..J6 常量对象直接提取。 */
extern const DpOrbitGravityModel DP_ORBIT_EARTH_MODEL;
/* 原 ELF ABI：唯一 double 参数位于 xmm0，直接更新全局 SpacecraftMass。 */
void SetSpacecraftMass(double mass);
/* 原 ELF ABI：rdi 为输入 DpVector；复制到全局 F_I_external。 */
void UpdateExternalForce(const DpVector *external_force_gci_3);

/* 原符号 M2E：E 初值为 mean_anomaly，最多 10000 次 Newton 更新，|delta| < 1e-10 时停止。 */
double M2E(double mean_anomaly, double eccentricity);

/* 原符号 Eccentric2Mean：E、e、sin(E) → M，保持调用方提供的正弦值。 */
double Eccentric2Mean(double eccentric_anomaly, double eccentricity,
                      double sine_eccentric_anomaly);

/* 原符号 Eccentric2True：E、e、cos(E)、sin(E)、sqrt(1-e²) → ν。 */
double Eccentric2True(double eccentric_anomaly, double eccentricity,
                      double cosine_eccentric_anomaly,
                      double sine_eccentric_anomaly,
                      double sqrt_one_minus_eccentricity_squared);

/* 原符号 Elements2PosVel_M：位置输出、速度输出、a/e/i/RAAN/ω/M。 */
void Elements2PosVel_M(double position_gci[3], double velocity_gci[3],
                       double semi_major_axis, double eccentricity,
                       double inclination, double raan,
                       double argument_of_periapsis, double mean_anomaly);

/* 原符号 Elements2PosVel_nu：位置输出、速度输出、a/e/i/RAAN/ω/ν。 */
void Elements2PosVel_nu(double position_gci[3], double velocity_gci[3],
                        double semi_major_axis, double eccentricity,
                        double inclination, double raan,
                        double argument_of_periapsis, double true_anomaly);

/* 原符号 PosVel2Elements_M：a/e/i/RAAN/ω/ν/M/u 输出，位置和速度输入。 */
void PosVel2Elements_M(double *semi_major_axis, double *eccentricity,
                       double *inclination, double *raan,
                       double *argument_of_periapsis, double *true_anomaly,
                       double *mean_anomaly, double *argument_of_latitude,
                       const double position_gci[3],
                       const double velocity_gci[3]);

/*
 * 计算 r_dot = v 与 v_dot。内部轴对称摄动是对 orbit_dynamic 常数访问的
 * 高层可校准表达；最终是否按位等价必须以原 ELF 多步金标验证。
 */
void dp_orbit_dynamic_zonal(DpVec3 *position_dot, DpVec3 *velocity_dot,
                            const DpVec3 *position_gci,
                            const DpVec3 *velocity_gci,
                            const DpVec3 *external_force_gci,
                            const DpOrbitGravityModel *model);

#ifdef __cplusplus
}
#endif
#endif /* DYNAMIC_ORBIT_H */
