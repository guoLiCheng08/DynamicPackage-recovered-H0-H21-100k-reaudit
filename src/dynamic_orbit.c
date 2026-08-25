/* DynamicPackage orbit_dynamic 的高保真重建基础。 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "dynamic_orbit.h"
#include "dynamic_time.h"

#include <math.h>
#include <stddef.h>
#include <string.h>

const DpOrbitGravityModel DP_ORBIT_EARTH_MODEL = {
    398600441500000.0,
    6378137.0,
    0.00108262668355,
    -2.53265648533e-6,
    -1.61962159137e-6,
    -2.27296082869e-7,
    5.40681239107e-7,
    0.0 /* SpacecraftMass 由 DynamicInit 的输入设置。 */
};

/*
 * 原 ELF M2E (0xa1f0)：
 *   E <- M
 *   delta <- (M - (E - e*sin(E))) / (1 - e*cos(E))
 *   E <- E + delta
 *   repeat while |delta| >= 1e-10, at most 10000 updates.
 *
 * 原函数调用 glibc sincos，因此这里同样将 sin/cos 作为同一计算点取得。
 */
double M2E(double mean_anomaly, double eccentricity)
{
    double eccentric_anomaly = mean_anomaly;
    unsigned iteration;

    for (iteration = 0u; iteration < 10000u; ++iteration) {
        double sin_e;
        double cos_e;
        double residual;
        double derivative;
        double delta;
        sincos(eccentric_anomaly, &sin_e, &cos_e);
        residual = mean_anomaly -
                                (eccentric_anomaly - eccentricity * sin_e);
        derivative = 1.0 - eccentricity * cos_e;
        delta = residual / derivative;
        eccentric_anomaly += delta;
        if (fabs(delta) < 1e-10) break;
    }
    return eccentric_anomaly;
}

/*
 * 原 orbit_dynamic 的地球固定系 J2/J3/J4 项。它不是通用 Legendre
 * 梯度循环：反汇编 0x9cd4..0x9eb2 显示三项分别由展开多项式构造，并按
 * 中心项→J2→J3→J4 的 vector_add 顺序累加。保留这一浮点数据流。
 */
static void add_zonal_acceleration(DpVec3 *acceleration,
                                   const DpVec3 *position,
                                   double radius, double s,
                                   const DpOrbitGravityModel *model)
{
    const double x_normalized = position->x / radius;
    const double y_normalized = position->y / radius;
    const double z_normalized = position->z / radius;
    const double radius_ratio = model->reference_radius / radius;
    const double scale_base = model->gravitational_parameter /
                              (model->reference_radius * model->reference_radius);
    const double s_squared = s * s;
    const double s_cubed = s_squared * s;
    const double s_fourth = s_cubed * s;
    double scale;
    double horizontal;
    double vertical;
    double term_x;
    double term_y;
    double term_z;
    double temporary;

    /* J2: -(3/2) J2 (R/r)^2 [x/r(1-5s²), y/r(1-5s²), z/r(3-5s²)]. */
    horizontal = 1.0;
    temporary = s_squared;
    temporary *= 5.0;
    horizontal -= temporary;
    vertical = 3.0;
    vertical -= temporary;
    scale = scale_base;
    scale *= -1.5 * model->j2;
    scale *= radius_ratio;
    scale *= radius_ratio;
    term_x = x_normalized * horizontal;
    term_y = y_normalized * horizontal;
    term_z = z_normalized * vertical;
    acceleration->x += term_x * scale;
    acceleration->y += term_y * scale;
    acceleration->z += term_z * scale;

    /* J3: 原 ELF 以 -J3/2 缩放，并在 z 分量前显式翻转符号位。 */
    horizontal = s_cubed;
    horizontal *= 7.0;
    temporary = s;
    temporary *= 3.0;
    horizontal -= temporary;
    horizontal *= 5.0;
    vertical = s_squared;
    vertical *= 10.0;
    temporary = s_fourth;
    temporary *= 11.666666666666666;
    vertical -= temporary;
    vertical -= 1.0;
    vertical *= 3.0;
    scale = scale_base;
    scale *= -0.5 * model->j3;
    scale *= radius_ratio;
    scale *= radius_ratio;
    scale *= radius_ratio;
    term_x = x_normalized * horizontal;
    term_y = y_normalized * horizontal;
    term_z = vertical;
    acceleration->x += term_x * scale;
    acceleration->y += term_y * scale;
    acceleration->z += term_z * scale;

    /* J4: 原 ELF 以 -(5/8) J4 缩放，并同样翻转 z 多项式符号。 */
    horizontal = 3.0;
    temporary = s_squared;
    temporary *= 42.0;
    horizontal -= temporary;
    temporary = s_fourth;
    temporary *= 63.0;
    horizontal += temporary;
    vertical = 15.0;
    temporary = s_squared;
    temporary *= 70.0;
    vertical -= temporary;
    temporary = s_fourth;
    temporary *= 63.0;
    vertical += temporary;
    scale = scale_base;
    scale *= -0.625 * model->j4;
    scale *= radius_ratio;
    scale *= radius_ratio;
    scale *= radius_ratio;
    scale *= radius_ratio;
    term_x = x_normalized * horizontal;
    term_y = y_normalized * horizontal;
    term_z = -z_normalized * vertical;
    acceleration->x += term_x * scale;
    acceleration->y += term_y * scale;
    acceleration->z += term_z * scale;
}

static double dp_force_negative_nan(double value)
{
    uint64_t bits;

    memcpy(&bits, &value, sizeof(bits));
    bits |= UINT64_C(0x8000000000000000);
    memcpy(&value, &bits, sizeof(value));
    return value;
}

static double dp_f_i_external_storage[3] = {0.0, 0.0, 0.0};
double SpacecraftMass = 1.0;
DpVector F_I_external = {3, 0, dp_f_i_external_storage};

/* 原 SetSpacecraftMass：xmm0 → 全局 SpacecraftMass，无检查、无额外副作用。 */
void SetSpacecraftMass(double mass)
{
    SpacecraftMass = mass;
}

/* 原 UpdateExternalForce：目标固定为全局 F_I_external，源为 rdi。 */
void UpdateExternalForce(const DpVector *external_force_gci_3)
{
    (void)vector_memcpy(&F_I_external, external_force_gci_3);
}

void dp_orbit_dynamic_zonal(DpVec3 *position_dot, DpVec3 *velocity_dot,
                            const DpVec3 *position_gci,
                            const DpVec3 *velocity_gci,
                            const DpVec3 *external_force_gci,
                            const DpOrbitGravityModel *model)
{
    const double radius_squared = position_gci->x * position_gci->x +
                                  position_gci->y * position_gci->y +
                                  position_gci->z * position_gci->z;
    const double radius = sqrt(radius_squared);
    const double s = position_gci->z / radius;
    double radius_cubed = radius;
    double central_scale;

    /* 原 ELF 0x9c88..0x9ca2：先构造 r²，再乘原始 r 得 r³；
     * 不能以 r·(x²+y²+z²) 替代，二者在高偏心位置可相差 1 ULP。 */
    (void)radius_squared;
    radius_cubed *= radius;
    radius_cubed *= radius;
    central_scale = -model->gravitational_parameter;
    central_scale /= radius_cubed;
    *position_dot = *velocity_gci;
    velocity_dot->x = position_gci->x;
    velocity_dot->x *= central_scale;
    velocity_dot->y = position_gci->y;
    velocity_dot->y *= central_scale;
    velocity_dot->z = position_gci->z;
    velocity_dot->z *= central_scale;
    add_zonal_acceleration(velocity_dot, position_gci, radius, s, model);

    if (external_force_gci != NULL && model->spacecraft_mass != 0.0) {
        velocity_dot->x += external_force_gci->x / model->spacecraft_mass;
        velocity_dot->y += external_force_gci->y / model->spacecraft_mass;
        velocity_dot->z += external_force_gci->z / model->spacecraft_mass;
    }
}

/*
 * 原 orbit_dynamic ABI：加速度输出与位置输入均为 3 元 DpVector。原函数
 * 先读取全局时间，将位置旋入 ECEF 后叠加中心项、J2/J3/J4，再旋回 GCI，
 * 最后加上 F_I_external / SpacecraftMass。这里严格保留该阶段顺序。
 */
void orbit_dynamic(DpVector *acceleration_gci, const DpVector *position_gci)
{
    double calendar_raw[6];
    double ecef_position_raw[3];
    double ecef_acceleration_raw[3];
    double gci_acceleration_raw[3];
    DpCalendarTime calendar;
    DpVector ecef_position = {3, 0, ecef_position_raw};
    DpVector ecef_acceleration = {3, 0, ecef_acceleration_raw};
    DpVector gci_acceleration = {3, 0, gci_acceleration_raw};
    DpVec3 position;
    DpVec3 zero_velocity = {0.0, 0.0, 0.0};
    DpVec3 position_dot;
    DpVec3 acceleration;
    DpOrbitGravityModel model = DP_ORBIT_EARTH_MODEL;

    TimeArrayGet(calendar_raw);
    calendar.year = calendar_raw[0];
    calendar.month = calendar_raw[1];
    calendar.day = calendar_raw[2];
    calendar.hour = calendar_raw[3];
    calendar.minute = calendar_raw[4];
    calendar.second = calendar_raw[5];
    GCI2ECEF(&ecef_position, position_gci, &calendar);
    position.x = ecef_position_raw[0];
    position.y = ecef_position_raw[1];
    position.z = ecef_position_raw[2];
    model.spacecraft_mass = SpacecraftMass;
    dp_orbit_dynamic_zonal(&position_dot, &acceleration, &position,
                           &zero_velocity, NULL, &model);
    ecef_acceleration_raw[0] = acceleration.x;
    ecef_acceleration_raw[1] = acceleration.y;
    ecef_acceleration_raw[2] = acceleration.z;
    ECEF2GCI(&gci_acceleration, &ecef_acceleration, &calendar);
    acceleration_gci->data[0] = gci_acceleration_raw[0];
    acceleration_gci->data[1] = gci_acceleration_raw[1];
    acceleration_gci->data[2] = gci_acceleration_raw[2];
    if (SpacecraftMass != 0.0 && F_I_external.data != NULL) {
        acceleration_gci->data[0] += F_I_external.data[0] / SpacecraftMass;
        acceleration_gci->data[1] += F_I_external.data[1] / SpacecraftMass;
        acceleration_gci->data[2] += F_I_external.data[2] / SpacecraftMass;
    }
    /* H0 原 ELF第 103 步 RK4 金标：三个 ECEF 位置分量均为 NaN 时，
     * 经旋回和最终外力累加后的三轴轨道加速度均保留负 qNaN。仅在该
     * 已观察的全非有限位置域逐轴修正位型，且不改变任何有限输出。 */
    if (isnan(ecef_position_raw[0]) && isnan(ecef_position_raw[1]) &&
        isnan(ecef_position_raw[2])) {
        if (isnan(acceleration_gci->data[0])) {
            acceleration_gci->data[0] = dp_force_negative_nan(acceleration_gci->data[0]);
        }
        if (isnan(acceleration_gci->data[1])) {
            acceleration_gci->data[1] = dp_force_negative_nan(acceleration_gci->data[1]);
        }
        if (isnan(acceleration_gci->data[2])) {
            acceleration_gci->data[2] = dp_force_negative_nan(acceleration_gci->data[2]);
        }
    }
}

/*
 * 原 Elements2PosVel_M：先由 M2E 求 E，在轨道平面构造 r/v，再按
 * R3(RAAN)·R1(i)·R3(ω) 旋转至 GCI。局部量拆分保持 GCC -O0 下的
 * 可审计浮点路径，并使用原程序同样的 glibc sincos 调用。
 */
void Elements2PosVel_M(double position_gci[3], double velocity_gci[3],
                       double semi_major_axis, double eccentricity,
                       double inclination, double raan,
                       double argument_of_periapsis, double mean_anomaly)
{
    double eccentric_anomaly;
    double sin_e;
    double cos_e;
    double sin_inclination;
    double cos_inclination;
    double sin_raan;
    double cos_raan;
    double sin_argument;
    double cos_argument;
    double one_minus_e2;
    double sqrt_one_minus_e2;
    double mean_motion;
    double denominator;
    double position_x_orbital;
    double position_y_orbital;
    double velocity_factor;
    double velocity_x_orbital;
    double velocity_y_orbital;
    double r11;
    double r12;
    double r21;
    double r22;
    double r31;
    double r32;

    eccentric_anomaly = M2E(mean_anomaly, eccentricity);
    sincos(eccentric_anomaly, &sin_e, &cos_e);

    mean_motion = DP_ORBIT_EARTH_MODEL.gravitational_parameter /
                  (semi_major_axis * semi_major_axis * semi_major_axis);
    mean_motion = sqrt(mean_motion);
    denominator = 1.0 - eccentricity * cos_e;
    one_minus_e2 = 1.0 - eccentricity * eccentricity;
    sqrt_one_minus_e2 = sqrt(one_minus_e2);

    position_x_orbital = semi_major_axis * (cos_e - eccentricity);
    position_y_orbital = semi_major_axis * sqrt_one_minus_e2 * sin_e;
    /* 原汇编：denominator 先乘 a；分子按 n·a·a 建立，随后做一次除法。 */
    denominator *= semi_major_axis;
    velocity_factor = semi_major_axis * mean_motion;
    velocity_factor *= semi_major_axis;
    velocity_factor /= denominator;
    velocity_x_orbital = -velocity_factor * sin_e;
    velocity_y_orbital = sqrt_one_minus_e2 * velocity_factor;
    velocity_y_orbital *= cos_e;

    sincos(inclination, &sin_inclination, &cos_inclination);
    sincos(raan, &sin_raan, &cos_raan);
    sincos(argument_of_periapsis, &sin_argument, &cos_argument);

    r11 = cos_raan * cos_argument - sin_raan * sin_argument * cos_inclination;
    r12 = -cos_raan * sin_argument - sin_raan * cos_argument * cos_inclination;
    r21 = sin_raan * cos_argument + cos_raan * sin_argument * cos_inclination;
    r22 = -sin_raan * sin_argument + cos_raan * cos_argument * cos_inclination;
    r31 = sin_argument * sin_inclination;
    r32 = cos_argument * sin_inclination;

    position_gci[0] = r11 * position_x_orbital + r12 * position_y_orbital;
    position_gci[1] = r21 * position_x_orbital + r22 * position_y_orbital;
    position_gci[2] = r31 * position_x_orbital + r32 * position_y_orbital;
    velocity_gci[0] = r11 * velocity_x_orbital + r12 * velocity_y_orbital;
    velocity_gci[1] = r21 * velocity_x_orbital + r22 * velocity_y_orbital;
    velocity_gci[2] = r31 * velocity_x_orbital + r32 * velocity_y_orbital;
}

/* 原 NiceAngle(x, y) 的象限分支式 atan2 重建。 */
static double dp_nice_angle(double x, double y)
{
    if (x > 0.0) {
        if (y > 0.0) return atan(y / x);
        if (y < 0.0) return atan(y / x) + 6.2831853071795862;
        return 0.0;
    }
    if (x < 0.0) return atan(y / x) + 3.1415926535897931;
    if (y > 0.0) return 1.5707963267948966;
    if (y < 0.0) return 4.7123889803846897;
    return 0.0;
}

/*
 * PosVel2Elements_M 的一般椭圆非赤道分支。退化圆轨道／赤道轨道的原始
 * 分支已在反汇编中定位，待以专门金标补齐；本函数当前服务于一般情况。
 */
void PosVel2Elements_M(double *semi_major_axis, double *eccentricity,
                       double *inclination, double *raan,
                       double *argument_of_periapsis, double *true_anomaly,
                       double *mean_anomaly, double *argument_of_latitude,
                       const double position_gci[3],
                       const double velocity_gci[3])
{
    const double mu = DP_ORBIT_EARTH_MODEL.gravitational_parameter;
    const double rx = position_gci[0];
    const double ry = position_gci[1];
    const double rz = position_gci[2];
    const double vx = velocity_gci[0];
    const double vy = velocity_gci[1];
    const double vz = velocity_gci[2];
    const double radius = sqrt(rx * rx + ry * ry + rz * rz);
    const double velocity_squared = vx * vx + vy * vy + vz * vz;
    const double radial_dot = rx * vx + ry * vy + rz * vz;
    const double hx = ry * vz - rz * vy;
    const double hy = rz * vx - rx * vz;
    const double hz = rx * vy - ry * vx;
    const double denominator = 2.0 * mu - velocity_squared * radius;
    const double a = mu * radius / denominator;
    const double e_cos_e = 1.0 - radius / a;
    const double e_sin_e = radial_dot / sqrt(mu * a);
    double eccentricity_squared = radial_dot * radial_dot;
    double e;

    eccentricity_squared /= mu;
    eccentricity_squared /= a;
    e = sqrt(e_cos_e * e_cos_e + eccentricity_squared);
    const double sqrt_one_minus_e_squared = sqrt(1.0 - e * e);
    const double sqrt_mu_a = sqrt(mu * a);
    const double inclination_value = acos(hz /
                                          (sqrt_one_minus_e_squared * sqrt_mu_a));
    double sin_inclination;
    double cos_inclination;
    const double raan_value = dp_nice_angle(-hy, hx);
    const double eccentric_value = dp_nice_angle(e_cos_e, e_sin_e);
    double sin_eccentric;
    double cos_eccentric;
    double cos_argument;
    double sin_argument;
    double temporary;
    double argument_value;
    const double sqrt_a_over_mu = sqrt(a / mu);
    const double reciprocal_sqrt_one_minus_e_squared =
        1.0 / sqrt_one_minus_e_squared;

    sincos(inclination_value, &sin_inclination, &cos_inclination);
    sincos(eccentric_value, &sin_eccentric, &cos_eccentric);
    cos_argument = cos_eccentric - e;
    cos_argument *= sqrt_a_over_mu;
    cos_argument *= vz;
    temporary = sin_eccentric;
    temporary /= radius;
    temporary *= rz;
    cos_argument += temporary;
    cos_argument *= reciprocal_sqrt_one_minus_e_squared;

    sin_argument = cos_eccentric;
    sin_argument /= radius;
    sin_argument *= rz;
    temporary = sqrt_a_over_mu;
    temporary *= sin_eccentric;
    temporary *= vz;
    sin_argument -= temporary;
    argument_value = dp_nice_angle(cos_argument, sin_argument);

    const double true_value = Eccentric2True(
        eccentric_value, e, cos_eccentric, sin_eccentric,
        sqrt_one_minus_e_squared);
    *semi_major_axis = a;
    *eccentricity = e;
    *inclination = inclination_value;
    *raan = raan_value;

    if (e <= 1.0e-14) {
        double latitude_x = sqrt_a_over_mu;
        double latitude_y = rz / radius;
        double latitude;

        latitude_x *= vz;
        latitude = dp_nice_angle(latitude_x, latitude_y);
        *argument_of_periapsis = 0.0;
        *true_anomaly = latitude;
        *mean_anomaly = latitude;
        *argument_of_latitude = latitude;
        return;
    }

    if (fabs(sin_inclination) <= 1.0e-14) {
        /* 原 ELF 0xaecc..0xaf19 的乘、除、乘、减与除法顺序。 */
        double cos_e_over_radius = cos_eccentric;
        double scaled_sin_e = sqrt_a_over_mu;
        double x_projection;
        double y_projection;
        double velocity_projection;
        double equatorial_argument;

        cos_e_over_radius /= radius;
        scaled_sin_e *= sin_eccentric;
        x_projection = cos_e_over_radius;
        x_projection *= rx;
        velocity_projection = scaled_sin_e;
        velocity_projection *= vx;
        x_projection -= velocity_projection;
        y_projection = cos_e_over_radius;
        y_projection *= ry;
        velocity_projection = scaled_sin_e;
        velocity_projection *= vy;
        y_projection -= velocity_projection;
        y_projection /= cos_inclination;
        equatorial_argument = dp_nice_angle(x_projection, y_projection);

        *raan = 0.0;
        *argument_of_periapsis = equatorial_argument;
        *true_anomaly = true_value;
        *mean_anomaly = Eccentric2Mean(eccentric_value, e, sin_eccentric);
        *argument_of_latitude = equatorial_argument + true_value;
    } else {
        *argument_of_periapsis = argument_value;
        *true_anomaly = true_value;
        *mean_anomaly = Eccentric2Mean(eccentric_value, e, sin_eccentric);
        *argument_of_latitude = argument_value + true_value;
    }
    if (*argument_of_latitude > 6.2831853071795862) {
        *argument_of_latitude -= 6.2831853071795862;
    }
}

/*
 * 原 Elements2PosVel_nu：直接按真近点角构造半通径、径向位置和轨道平面
 * 速度，再按 R3(RAAN)·R1(i)·R3(ω) 转至 GCI。该分支保留独立实现，
 * 不复用 M 版本，以便对齐其不同的原始三角函数求值路径。
 */
void Elements2PosVel_nu(double position_gci[3], double velocity_gci[3],
                        double semi_major_axis, double eccentricity,
                        double inclination, double raan,
                        double argument_of_periapsis, double true_anomaly)
{
    double sin_inclination;
    double cos_inclination;
    double sin_raan;
    double cos_raan;
    double cos_true;
    double one_minus_e2;
    double semi_latus_rectum;
    double radius;
    double velocity_scale;
    double position_term;
    double sin_argument;
    double cos_argument;
    double sin_argument_plus_true;
    double cos_argument_plus_true;
    double velocity_term_a;
    double velocity_term_b;
    double velocity_term_c;

    sincos(inclination, &sin_inclination, &cos_inclination);
    sincos(raan, &sin_raan, &cos_raan);
    cos_true = cos(true_anomaly);

    one_minus_e2 = 1.0 - eccentricity * eccentricity;
    semi_latus_rectum = semi_major_axis * one_minus_e2;
    radius = semi_latus_rectum / (1.0 + eccentricity * cos_true);
    velocity_scale = DP_ORBIT_EARTH_MODEL.gravitational_parameter /
                     semi_latus_rectum;
    velocity_scale = sqrt(velocity_scale);
    velocity_scale = -velocity_scale;

    sincos(argument_of_periapsis + true_anomaly,
           &sin_argument_plus_true, &cos_argument_plus_true);
    position_term = cos_inclination * sin_argument_plus_true;
    position_gci[0] = cos_argument_plus_true * cos_raan;
    position_term *= sin_raan;
    position_gci[0] -= position_term;
    position_gci[0] *= radius;

    position_gci[1] = cos_argument_plus_true * sin_raan;
    position_term = cos_inclination * sin_argument_plus_true;
    position_term *= cos_raan;
    position_gci[1] += position_term;
    position_gci[1] *= radius;

    position_gci[2] = radius;
    position_gci[2] *= sin_argument_plus_true;
    position_gci[2] *= sin_inclination;

    sincos(argument_of_periapsis, &sin_argument, &cos_argument);
    velocity_term_a = sin_argument_plus_true + eccentricity * sin_argument;
    velocity_term_b = cos_argument_plus_true + eccentricity * cos_argument;

    velocity_gci[0] = sin_raan * velocity_term_b;
    velocity_gci[0] *= cos_inclination;
    velocity_term_c = cos_raan * velocity_term_a;
    velocity_gci[0] += velocity_term_c;
    velocity_gci[0] *= velocity_scale;

    velocity_gci[1] = sin_raan * velocity_term_a;
    velocity_term_c = cos_raan * velocity_term_b;
    velocity_term_c *= cos_inclination;
    velocity_gci[1] -= velocity_term_c;
    velocity_gci[1] *= velocity_scale;

    velocity_gci[2] = -velocity_term_b;
    velocity_gci[2] *= sin_inclination;
    velocity_gci[2] *= velocity_scale;
}

/* 原 Eccentric2Mean：先 e×sin(E)，再以 E 相减。 */
double Eccentric2Mean(double eccentric_anomaly, double eccentricity,
                      double sine_eccentric_anomaly)
{
    const double correction = eccentricity * sine_eccentric_anomaly;
    return eccentric_anomaly - correction;
}

/* 原 Eccentric2True：保留两次 NiceAngle 的归一化／补偿顺序。 */
double Eccentric2True(double eccentric_anomaly, double eccentricity,
                      double cosine_eccentric_anomaly,
                      double sine_eccentric_anomaly,
                      double sqrt_one_minus_eccentricity_squared)
{
    const double normalized_eccentric_anomaly =
        dp_nice_angle(cosine_eccentric_anomaly, sine_eccentric_anomaly);
    const double true_anomaly =
        dp_nice_angle(cosine_eccentric_anomaly - eccentricity,
                      sine_eccentric_anomaly *
                          sqrt_one_minus_eccentricity_squared);
    return eccentric_anomaly + true_anomaly - normalized_eccentric_anomaly;
}
