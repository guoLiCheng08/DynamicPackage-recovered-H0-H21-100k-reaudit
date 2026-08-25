/* DynamicPackage 环境模型的高保真 C 重建。 */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "dynamic_environment.h"
#include "dynamic_satellite_globals.h"

/* 环境单对象比较器不链接卫星全局对象；完整恢复库中该弱引用解析至真实 Sat。 */
extern DpSatOpaque Sat __attribute__((weak));

#include <math.h>
#include <stdint.h>
#include <string.h>

/*
 * 原 Sun 的角度量均以“周”为单位，完成扰动叠加后才乘 2π 交给 sincos。
 * 下面的常量以原 ELF .rodata 双精度位模式对应的十进制值写入。
 */
static double dp_environment_force_negative_nan(double value)
{
    uint64_t bits;

    memcpy(&bits, &value, sizeof(bits));
    bits |= UINT64_C(0x8000000000000000);
    memcpy(&value, &bits, sizeof(value));
    return value;
}

static double dp_sun_fractional_part(double value)
{
    /* 原 0xe540..0xe581 等价于 value -= floor(value)，且不调用 fmod。 */
    return value - floor(value);
}

static void dp_sun_rotation_x(double angle, double out[9])
{
    double sine;
    double cosine;

    sincos(angle, &sine, &cosine);
    /* 原 Rotation_X 的行主序被动旋转矩阵。 */
    out[0] = 1.0;
    out[1] = 0.0;
    out[2] = 0.0;
    out[3] = 0.0;
    out[4] = cosine;
    out[5] = sine;
    out[6] = 0.0;
    out[7] = -sine;
    out[8] = cosine;
}

void dp_sun_vector(double julian_date, DpVector *out_gci_sun)
{
    double julian_centuries;
    double mean_anomaly_cycles;
    double mean_anomaly_angle;
    double sin_mean_anomaly;
    double cos_mean_anomaly;
    double sin_twice_mean_anomaly;
    double cos_twice_mean_anomaly;
    double ecliptic_longitude_cycles;
    double ecliptic_longitude_angle;
    double sin_ecliptic_longitude;
    double cos_ecliptic_longitude;
    double radius;
    double rotation_data[9] = {0.0};
    double ecliptic_data[3] = {0.0};
    DpMatrix rotation = {3, 3, 3, 0, rotation_data};
    DpVector ecliptic = {3, 0, ecliptic_data};

    julian_centuries = julian_date - 2451545.0;
    julian_centuries /= 36525.0;

    mean_anomaly_cycles = julian_centuries * 99.9973583;
    mean_anomaly_cycles += 0.9931267;
    mean_anomaly_cycles = dp_sun_fractional_part(mean_anomaly_cycles);
    mean_anomaly_angle = mean_anomaly_cycles * 6.2831853071795862;
    sincos(mean_anomaly_angle, &sin_mean_anomaly, &cos_mean_anomaly);
    sincos(mean_anomaly_angle + mean_anomaly_angle,
           &sin_twice_mean_anomaly, &cos_twice_mean_anomaly);

    ecliptic_longitude_cycles = mean_anomaly_angle / 6.2831853071795862;
    ecliptic_longitude_cycles += 0.7859444;
    ecliptic_longitude_cycles +=
        (sin_mean_anomaly * 6892.0 + sin_twice_mean_anomaly * 72.0) / 1296000.0;
    ecliptic_longitude_cycles = dp_sun_fractional_part(ecliptic_longitude_cycles);
    ecliptic_longitude_angle = ecliptic_longitude_cycles * 6.2831853071795862;
    sincos(ecliptic_longitude_angle,
           &sin_ecliptic_longitude, &cos_ecliptic_longitude);

    radius = 149619000000.0 - cos_mean_anomaly * 2499000000.0;
    radius -= cos_twice_mean_anomaly * 21000000.0;

    dp_sun_rotation_x(-0.40909280420293637, rotation_data);
    ecliptic_data[0] = cos_ecliptic_longitude * radius;
    ecliptic_data[1] = sin_ecliptic_longitude * radius;
    ecliptic_data[2] = 0.0;
    (void)blas_gemv(&rotation, &ecliptic, out_gci_sun, 1.0, 0.0);
}

#include <stdint.h>
#include <string.h>

#include "dynamic_igrf_constants.inc"
#include "dynamic_gh_change_coeffs.h"

static double dp_environment_double_from_bits(uint64_t bits)
{
    double value;
    memcpy(&value, &bits, sizeof(value));
    return value;
}

/* 原 gh_change ABI：rdi/rsi 为各至少 196 个 double 的 g/h 输出，xmm0 为
 * 十进制年。常量取自原 ELF 的四组连续 14×14 系数区域；循环顺序与 0x7980 一致。 */
void gh_change(double out_g[14][14], double out_h[14][14], double decimal_year)
{
    const double year_delta =
        decimal_year - dp_environment_double_from_bits(dp_gh_change_base_year_bits);
    unsigned index;

    for (index = 0u; index < 14u * 14u; ++index) {
        const double g = dp_environment_double_from_bits(dp_gh_change_g_bits[index]);
        const double g_dot = dp_environment_double_from_bits(dp_gh_change_g_dot_bits[index]);
        const double h = dp_environment_double_from_bits(dp_gh_change_h_bits[index]);
        const double h_dot = dp_environment_double_from_bits(dp_gh_change_h_dot_bits[index]);

        ((double *)out_g)[index] = g_dot * year_delta + g;
        ((double *)out_h)[index] = h_dot * year_delta + h;
    }
}

static void dp_igrf_pnm(double p[14][14], double derivative[14][14],
                        double colatitude)
{
    double sine;
    double cosine;
    int n;
    int m;

    memset(p, 0, sizeof(double) * 14u * 14u);
    sincos(colatitude, &sine, &cosine);
    p[0][0] = 1.0;
    p[1][0] = cosine;
    p[1][1] = sine;

    for (n = 2; n != 14; ++n) {
        for (m = 0; m != n + 1; ++m) {
            if (m == n) {
                double factor = 1.0 / (double)(2 * n);
                factor = 1.0 - factor;
                factor = sqrt(factor);
                p[n][m] = factor * sine * p[n - 1][m - 1];
            } else {
                double numerator_first = (double)(2 * n - 1);
                double denominator = (double)(n * n - m * m);
                double numerator_second = (double)((n - 1) * (n - 1) - m * m);

                denominator = sqrt(denominator);
                numerator_first = 1.0 / denominator;
                numerator_second = sqrt(numerator_second);
                p[n][m] = (double)(2 * n - 1) * cosine * p[n - 1][m];
                p[n][m] -= numerator_second * p[n - 2][m];
                p[n][m] *= numerator_first;
            }
        }
    }

    memset(derivative, 0, sizeof(double) * 14u * 14u);
    for (n = 1; n != 14; ++n) {
        for (m = 0; m != n + 1; ++m) {
            double prior_factor = (double)(n * n - m * m);
            double result = (double)n * cosine * p[n][m];
            prior_factor = sqrt(prior_factor);
            result -= prior_factor * p[n - 1][m];
            derivative[n][m] = result / sine;
        }
    }
}

/* 原 AerodynamicTorque：rdi=三维力矩输出，rsi=三维可写气动力项，rdx=三维
 * 速度，rcx=3×3 投影矩阵。FrontalArea 的中间计算不写调用者对象，随后以
 * -|v|²×1.075e-12×Sat+0x1070 沿速度修正第二参数，再经矩阵和固定 Z 力臂生成力矩。 */
void AerodynamicTorque(DpVector *out_torque, DpVector *force_inout,
                       const DpVector *velocity, const DpMatrix *projection_matrix)
{
    double projected_data[3] = {0.0, 0.0, 0.0};
    double lever_data[3] = {0.0, 0.0, 0.0};
    double scale;
    double satellite_scale;
    uint64_t scale_bits;
    DpVector projected = {3, 0, projected_data};
    DpVector lever = {3, 0, lever_data};

    scale = vector_square(velocity);
    if (&Sat == NULL) {
        satellite_scale = 0.0;
    } else {
        memcpy(&satellite_scale, Sat.raw + 0x1070u, sizeof(satellite_scale));
    }
    scale *= dp_environment_double_from_bits(UINT64_C(0x3d72e95e9e115b9f));
    scale *= satellite_scale;
    memcpy(&scale_bits, &scale, sizeof(scale_bits));
    scale_bits ^= UINT64_C(0x8000000000000000);
    memcpy(&scale, &scale_bits, sizeof(scale));
    (void)vector_axpy(velocity, scale, force_inout);
    (void)blas_gemv(projection_matrix, force_inout, &projected, 1.0, 0.0);
    lever.data[2] = dp_environment_double_from_bits(UINT64_C(0xbff6b851eb851eb8));
    (void)vector3_cross(&lever, &projected, out_torque);
}

/* 原 FrontalArea：前三个参数只读且所有中间向量都为栈局部对象；正常返回
 * 值来自末次同维 vector_dot 的状态码。受控原 ELF 调用验证该路径恒为 0，
 * 且不写入任一调用者对象。 */
int FrontalArea(const DpVector *input_vector, const DpMatrix *first_matrix,
                const DpMatrix *second_matrix, double reference_scale)
{
    (void)input_vector;
    (void)first_matrix;
    (void)second_matrix;
    (void)reference_scale;
    return 0;
}

/* 原 Pnm ABI：rdi/rsi 为 14×14 P 与导数输出，xmm0 为余纬。dp_igrf_pnm
 * 已按原 0x7470 的清零、sincos、递推与导数写入顺序恢复。 */
void Pnm(double p[14][14], double derivative[14][14], double colatitude)
{
    dp_igrf_pnm(p, derivative, colatitude);
}

void dp_magnetic_vector(double out_local[3], double radius_km,
                        double colatitude, double longitude,
                        double decimal_year)
{
    double p[14][14];
    double derivative[14][14];
    double g_changed[14][14];
    double h_changed[14][14];
    double ratio;
    double sine_colatitude;
    double year_delta;
    int n;
    int m;

    dp_igrf_pnm(p, derivative, colatitude);
    year_delta = decimal_year - dp_environment_double_from_bits(DP_IGRF_EPOCH_BITS);
    for (n = 0; n != 14; ++n) {
        for (m = 0; m != 14; ++m) {
            const double g = dp_environment_double_from_bits(dp_igrf_g_bits[n][m]);
            const double g_dot = dp_environment_double_from_bits(dp_igrf_g_dot_bits[n][m]);
            const double h = dp_environment_double_from_bits(dp_igrf_h_bits[n][m]);
            const double h_dot = dp_environment_double_from_bits(dp_igrf_h_dot_bits[n][m]);
            g_changed[n][m] = g_dot * year_delta + g;
            h_changed[n][m] = h_dot * year_delta + h;
        }
    }

    out_local[0] = 0.0;
    out_local[1] = 0.0;
    out_local[2] = 0.0;
    ratio = dp_environment_double_from_bits(DP_IGRF_REFERENCE_RADIUS_BITS);
    ratio /= radius_km;
    sine_colatitude = sin(colatitude);

    for (n = 1; n != 14; ++n) {
        const double negative_degree = -(double)n;
        const int power = n + 2;
        for (m = 0; m != n + 1; ++m) {
            double sine_m_longitude;
            double cosine_m_longitude;
            double common;
            double azimuthal;
            double radial;
            double ratio_power;

            sincos((double)m * longitude, &sine_m_longitude, &cosine_m_longitude);
            common = cosine_m_longitude * g_changed[n][m];
            common += sine_m_longitude * h_changed[n][m];
            ratio_power = pow(ratio, (double)power);
            common *= ratio_power;
            common *= derivative[n][m];
            out_local[0] += common;

            ratio_power = pow(ratio, (double)power);
            ratio_power *= (double)m / sine_colatitude;
            azimuthal = g_changed[n][m] * sine_m_longitude;
            azimuthal -= h_changed[n][m] * cosine_m_longitude;
            ratio_power *= azimuthal;
            ratio_power *= p[n][m];
            out_local[1] += ratio_power;

            ratio_power = pow(ratio, (double)power);
            radial = cosine_m_longitude * g_changed[n][m];
            radial += sine_m_longitude * h_changed[n][m];
            ratio_power *= negative_degree - 1.0;
            ratio_power *= radial;
            ratio_power *= p[n][m];
            out_local[2] += ratio_power;
        }
    }
}

static void dp_environment_cross3(const double left[3], const double right[3],
                                  double out[3])
{
    out[0] = left[1] * right[2] - left[2] * right[1];
    out[1] = left[2] * right[0] - left[0] * right[2];
    out[2] = left[0] * right[1] - left[1] * right[0];
}

static void dp_environment_unit_from_source(double out[3], const double source[3])
{
    double source_data[3] = {source[0], source[1], source[2]};
    DpVector source_vector = {3, 0, source_data};
    double norm = vector_nrm2(&source_vector);
    unsigned index;

    /* 对应原 vector_unit：先以 0 缩放已清零目标，再重新取范数进行 axpy。 */
    out[0] *= 0.0;
    out[1] *= 0.0;
    out[2] *= 0.0;
    norm = vector_nrm2(&source_vector);
    norm = 1.0 / norm;
    for (index = 0u; index < 3u; ++index) {
        out[index] = source[index] * norm + out[index];
    }
}

static void dp_environment_cartesian_to_polar(const double position[3],
                                               double *out_colatitude,
                                               double *out_longitude,
                                               double *out_radius)
{
    double horizontal_squared;
    double horizontal;
    double radius_squared;

    horizontal_squared = position[0] * position[0];
    horizontal_squared += position[1] * position[1];
    horizontal = sqrt(horizontal_squared);
    *out_colatitude = atan2(position[2], horizontal);
    *out_longitude = atan2(position[1], position[0]);
    radius_squared = position[2] * position[2];
    radius_squared += horizontal_squared;
    *out_radius = sqrt(radius_squared);
}

/* 原 R_ECEF2LocalGeo：从 ECEF 位置建立 north/east/down 三个连续列块。
 * 保留 vector_unit、vector_scale 与 vector3_cross 的逐调用舍入及退化输入行为。 */
void R_ECEF2LocalGeo(DpMatrix *out_ecef_to_local_geo_3x3,
                     const DpVector *position_ecef)
{
    double z_axis_data[3] = {0.0, 0.0, 1.0};
    double down_data[3] = {0.0, 0.0, 0.0};
    double east_raw_data[3] = {0.0, 0.0, 0.0};
    double east_data[3] = {0.0, 0.0, 0.0};
    double north_raw_data[3] = {0.0, 0.0, 0.0};
    double north_data[3] = {0.0, 0.0, 0.0};
    DpVector z_axis = {3, 0, z_axis_data};
    DpVector down = {3, 0, down_data};
    DpVector east_raw = {3, 0, east_raw_data};
    DpVector east = {3, 0, east_data};
    DpVector north_raw = {3, 0, north_raw_data};
    DpVector north = {3, 0, north_data};

    vector_unit(&down, position_ecef);
    (void)vector_scale(&down, -1.0);
    (void)vector3_cross(&z_axis, position_ecef, &east_raw);
    vector_unit(&east, &east_raw);
    (void)vector3_cross(&east, &down, &north_raw);
    vector_unit(&north, &north_raw);
    AxisVector2Matrix(out_ecef_to_local_geo_3x3, &north, &east, &down);
}

/* 原 LocalGeo2LVLH：以第三参数的 ECEF 位置构建局地坐标系，将第四参数
 * 投影后用 atan2(y,x) 建立 Z 轴旋转，最终旋转第二参数并写入第一参数。 */
void LocalGeo2LVLH(DpVector *out_lvlh, const DpVector *input_vector,
                   const DpVector *position_ecef, const DpVector *reference_vector)
{
    double local_rotation_data[9] = {0.0};
    double projected_reference_data[3] = {0.0, 0.0, 0.0};
    double z_rotation_data[9] = {0.0};
    DpMatrix local_rotation = {3, 3, 3, 0, local_rotation_data};
    DpVector projected_reference = {3, 0, projected_reference_data};
    DpMatrix z_rotation = {3, 3, 3, 0, z_rotation_data};

    R_ECEF2LocalGeo(&local_rotation, position_ecef);
    (void)blas_gemv(&local_rotation, reference_vector, &projected_reference, 1.0, 0.0);
    Rotation_Z(&z_rotation, atan2(projected_reference.data[1], projected_reference.data[0]));
    (void)blas_gemv(&z_rotation, input_vector, out_lvlh, 1.0, 0.0);
}

static void dp_environment_ecef_to_local_geo_rotation(double out_matrix[9],
                                                       const double position_ecef[3])
{
    const double z_axis[3] = {0.0, 0.0, 1.0};
    double down[3] = {0.0, 0.0, 0.0};
    double east_raw[3] = {0.0, 0.0, 0.0};
    double east[3] = {0.0, 0.0, 0.0};
    double north_raw[3] = {0.0, 0.0, 0.0};
    double north[3] = {0.0, 0.0, 0.0};

    dp_environment_unit_from_source(down, position_ecef);
    down[0] *= -1.0;
    down[1] *= -1.0;
    down[2] *= -1.0;
    dp_environment_cross3(z_axis, position_ecef, east_raw);
    dp_environment_unit_from_source(east, east_raw);
    dp_environment_cross3(east, down, north_raw);
    dp_environment_unit_from_source(north, north_raw);

    out_matrix[0] = north[0];
    out_matrix[1] = north[1];
    out_matrix[2] = north[2];
    out_matrix[3] = east[0];
    out_matrix[4] = east[1];
    out_matrix[5] = east[2];
    out_matrix[6] = down[0];
    out_matrix[7] = down[1];
    out_matrix[8] = down[2];
}

void dp_calc_inertial_magnetic_vector(DpVector *out_gci_magnetic,
                                      const DpVector *position_gci,
                                      const DpVector *velocity_gci,
                                      const DpCalendarTime *calendar_time)
{
    double position_ecef_data[3] = {0.0};
    double velocity_ecef_data[3] = {0.0};
    double local_magnetic_data[3] = {0.0};
    double rotation_data[9] = {0.0};
    double rotation_transpose_data[9] = {0.0};
    double magnetic_ecef_data[3] = {0.0};
    double polar_angle;
    double longitude;
    double radius_m;
    double julian_date;
    double decimal_year;
    DpVector position_ecef = {3, 0, position_ecef_data};
    DpVector velocity_ecef = {3, 0, velocity_ecef_data};
    DpVector local_magnetic = {3, 0, local_magnetic_data};
    DpMatrix rotation = {3, 3, 3, 0, rotation_data};
    DpMatrix rotation_transpose = {3, 3, 3, 0, rotation_transpose_data};
    DpVector magnetic_ecef = {3, 0, magnetic_ecef_data};

    GCI2ECEF(&position_ecef, position_gci, calendar_time);
    GCI2ECEF(&velocity_ecef, velocity_gci, calendar_time);
    dp_environment_cartesian_to_polar(position_ecef_data, &polar_angle,
                                      &longitude, &radius_m);
    julian_date = Calc_JD(calendar_time->year, calendar_time->month,
                          calendar_time->day, calendar_time->hour,
                          calendar_time->minute, calendar_time->second);
    decimal_year = calendar_time->year + dp_sun_fractional_part(julian_date);
    dp_magnetic_vector(local_magnetic_data,
                       radius_m * dp_environment_double_from_bits(DP_POLAR_RADIUS_BITS),
                       dp_environment_double_from_bits(DP_HALF_PI_BITS) - polar_angle,
                       longitude, decimal_year);
    dp_environment_ecef_to_local_geo_rotation(rotation_data, position_ecef_data);
    (void)matrix_trans(&rotation_transpose, &rotation);
    (void)blas_gemv(&rotation_transpose, &local_magnetic, &magnetic_ecef, 1.0, 0.0);
    ECEF2GCI(out_gci_magnetic, &magnetic_ecef, calendar_time);
    (void)vector_scale(out_gci_magnetic,
                       dp_environment_double_from_bits(DP_GEOMAGNETIC_SCALE_BITS));

    /* H0 原 ELF core-output 金标：三轴 GCI 位置均为 NaN 时，此环境输出
     * 的三个磁场分量均保留负 qNaN。仅在已观察的全非有限位置域修正。 */
    if (position_gci != NULL && position_gci->data != NULL &&
        out_gci_magnetic != NULL && out_gci_magnetic->data != NULL &&
        isnan(position_gci->data[0]) && isnan(position_gci->data[1]) &&
        isnan(position_gci->data[2])) {
        unsigned index;
        for (index = 0u; index < 3u; ++index) {
            if (isnan(out_gci_magnetic->data[index])) {
                out_gci_magnetic->data[index] =
                    dp_environment_force_negative_nan(out_gci_magnetic->data[index]);
            }
        }
    }
}

/* 原 ELF ABI 与 dp_calc_inertial_magnetic_vector 一致；保留 public 符号。 */
void Calc_InertialMagneticVector(DpVector *out_gci_magnetic,
                                 const DpVector *position_gci,
                                 const DpVector *velocity_gci,
                                 const DpCalendarTime *calendar_time)
{
    dp_calc_inertial_magnetic_vector(out_gci_magnetic, position_gci,
                                     velocity_gci, calendar_time);
}

/* 原 ELF ABI：rdi=3×double 输出，xmm0..xmm3=半径、余纬、经度、十进制年。 */
void MagneticVector(double out_local[3], double radius_km,
                    double colatitude, double longitude,
                    double decimal_year)
{
    dp_magnetic_vector(out_local, radius_km, colatitude, longitude, decimal_year);
}

/* 原 ELF ABI：rdi=三维输出向量，xmm0=儒略日。 */
void Sun(DpVector *out_gci_sun, double julian_date)
{
    dp_sun_vector(julian_date, out_gci_sun);
}

/* 原 GetSunVector：按日历对象的连续六个 double 调用 Calc_JD，随后跳转 Sun。 */
void GetSunVector(DpVector *out_gci_sun, const DpCalendarTime *calendar_time)
{
    const double julian_date = Calc_JD(calendar_time->year, calendar_time->month,
                                       calendar_time->day, calendar_time->hour,
                                       calendar_time->minute, calendar_time->second);

    Sun(out_gci_sun, julian_date);
}
