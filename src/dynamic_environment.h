/* DynamicPackage 环境模型的高保真恢复接口。 */
#ifndef DYNAMIC_ENVIRONMENT_H
#define DYNAMIC_ENVIRONMENT_H

#include "dynamic_math.h"
#include "dynamic_time.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 原 Sun：输入儒略日，向目标三维向量写入 GCI 太阳位置，单位为 m。 */
void dp_sun_vector(double julian_date, DpVector *out_gci_sun);
/* 原 ELF ABI：输出向量在 rdi，儒略日在 xmm0。 */
void Sun(DpVector *out_gci_sun, double julian_date);
/* 原 ELF ABI：rdi 为输出 DpVector，rsi 为连续 6×double 日历时间对象。 */
void GetSunVector(DpVector *out_gci_sun, const DpCalendarTime *calendar_time);

/* 原 R_ECEF2LocalGeo ABI：rdi=3×3 输出 DpMatrix，rsi=三维 ECEF 位置 DpVector；
 * 输出连续 north/east/down 三个轴向量。 */
void R_ECEF2LocalGeo(DpMatrix *out_ecef_to_local_geo_3x3,
                     const DpVector *position_ecef);
/* 原 LocalGeo2LVLH ABI：rdi=三维输出，rsi=待变换向量，rdx=ECEF 位置，
 * rcx=局地方位参考向量。 */
void LocalGeo2LVLH(DpVector *out_lvlh, const DpVector *input_vector,
                   const DpVector *position_ecef, const DpVector *reference_vector);

/* 原 AerodynamicTorque ABI：rdi=力矩输出，rsi=可写气动力，rdx=速度，
 * rcx=3×3 投影矩阵。 */
void AerodynamicTorque(DpVector *out_torque, DpVector *force_inout,
                       const DpVector *velocity, const DpMatrix *projection_matrix);

/* 原 FrontalArea ABI：rdi=三维向量，rsi/rdx=3×3 矩阵，xmm0=标量；
 * 正常路径不写调用者对象，返回零状态。 */
int FrontalArea(const DpVector *input_vector, const DpMatrix *first_matrix,
                const DpMatrix *second_matrix, double reference_scale);

/* 原 Pnm ABI：rdi/rsi 为 14×14 P/导数输出阵列，xmm0 为余纬。 */
void Pnm(double p[14][14], double derivative[14][14], double colatitude);

/* 原 gh_change ABI：rdi/rsi 为 14×14 g/h 输出阵列，xmm0 为十进制年；
 * 按原 IGRF 基准年和年变化率生成当年系数。 */
void gh_change(double out_g[14][14], double out_h[14][14], double decimal_year);

/* 原 MagneticVector：半径 km、余纬/经度 rad、十进制年，输出当地地理坐标三分量。 */
void dp_magnetic_vector(double out_local[3], double radius_km,
                        double colatitude, double longitude,
                        double decimal_year);
/* 原 ELF ABI 与上方内部实现一致。 */
void MagneticVector(double out_local[3], double radius_km,
                    double colatitude, double longitude,
                    double decimal_year);

/* 原 Calc_InertialMagneticVector：将 IGRF 当地地理场转换为 GCI 磁场，单位 T。 */
void dp_calc_inertial_magnetic_vector(DpVector *out_gci_magnetic,
                                      const DpVector *position_gci,
                                      const DpVector *velocity_gci,
                                      const DpCalendarTime *calendar_time);
/* 原 ELF ABI 与上方内部实现一致。 */
void Calc_InertialMagneticVector(DpVector *out_gci_magnetic,
                                 const DpVector *position_gci,
                                 const DpVector *velocity_gci,
                                 const DpCalendarTime *calendar_time);
/* 原 ELF ABI：rdi 为连续日历对象；从全局 y 读取轨道状态并更新 B_I_static。 */
void MagUpdate(const DpCalendarTime *calendar_time);

#ifdef __cplusplus
}
#endif

#endif /* DYNAMIC_ENVIRONMENT_H */
