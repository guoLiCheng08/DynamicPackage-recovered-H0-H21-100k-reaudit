/* DynamicPackage 陀螺仪与磁强计恢复：矩阵投影 + 可选高斯偏置。 */
#ifndef DYNAMIC_SENSORS_H
#define DYNAMIC_SENSORS_H

#include <stddef.h>
#include <stdint.h>

#include "dynamic_math.h"
#include "dynamic_recovered.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t reserved_00[0x18];
    uint32_t gaussian_noise_flag;      /* +0x18 */
    uint32_t reserved_1c;
    double gaussian_mean[3];           /* +0x20 */
    double gaussian_sigma[3];          /* +0x38 */
    DpMatrix installation_matrix;      /* +0x50：配置源矩阵 */
    DpMatrix projection_matrix;        /* +0x68：Gyro_Init 转置输出 */
    uint8_t reserved_80[0x98];
    DpVector measure;                  /* +0x118; data pointer at +0x120 */
    uint8_t reserved_128[0x1f8];
} DpGyroRecovered;

_Static_assert(offsetof(DpGyroRecovered, projection_matrix) == 0x68, "Gyro matrix");
_Static_assert(offsetof(DpGyroRecovered, measure) == 0x118, "Gyro measure");
_Static_assert(sizeof(DpGyroRecovered) == 0x320, "Gyro item size");

/* 原 ELF 全局 Gyro 为连续 2 × 0x320 字节对象。 */
extern DpGyroRecovered Gyro[2];
/* 原 ELF ABI：无参数；为两个 Gyro 对象安装内部 backing 并转置配置矩阵。 */
void Gyro_Init(void);
/* 原 ELF ABI：rdi=三轴机体系角速度 DpVector，更新全局 Gyro[2]。 */
void UpdateGyro(const DpVector *body_rate);

typedef struct {
    uint8_t reserved_00[0x10];
    uint32_t gaussian_noise_flag;      /* +0x10 */
    uint32_t reserved_14;
    double gaussian_mean[3];           /* +0x18 */
    double gaussian_sigma[3];          /* +0x30 */
    DpMatrix installation_matrix;      /* +0x48：配置源矩阵 */
    DpMatrix projection_matrix;        /* +0x60：MagMeter_Init 转置输出 */
    uint8_t reserved_78[0x98];
    DpVector measure;                  /* +0x110; data pointer at +0x118 */
    uint8_t reserved_120[0x18];
} DpMagMeterRecovered;

_Static_assert(offsetof(DpMagMeterRecovered, projection_matrix) == 0x60, "MagMeter matrix");
_Static_assert(offsetof(DpMagMeterRecovered, measure) == 0x110, "MagMeter measure");
_Static_assert(sizeof(DpMagMeterRecovered) == 0x138, "MagMeter item size");

/* 原 ELF 全局 MagMeter 为连续 2 × 0x138 字节对象。 */
extern DpMagMeterRecovered MagMeter[2];
/* 原 ELF ABI：无参数；为两个 MagMeter 对象安装内部 backing 并转置配置矩阵。 */
void MagMeter_Init(void);
/* 原 ELF ABI：rdi=三轴机体系磁场 DpVector，更新全局 MagMeter[2]。 */
void UpdateMagMeter(const DpVector *magnetic_body);

typedef double (*DpGaussian2Sampler)(double mean, double sigma, void *opaque);

/* 对应原 ran_gaussian / ran_gaussian2。 */
double dp_ran_gaussian_recovered(double sigma);
double dp_ran_gaussian2(double mean, double sigma);

void dp_update_gyro(DpGyroRecovered gyros[2], const DpVec3 *body_rate,
                    DpGaussian2Sampler gaussian2, void *opaque);
void dp_update_magmeter(DpMagMeterRecovered meters[2], const DpVec3 *magnetic_body,
                        DpGaussian2Sampler gaussian2, void *opaque);

/* 原 DSS 为 2 × 0x158；UpdateDSS 使用三维投影后以 z 分量归一化并输出双角。 */
typedef struct {
    uint8_t reserved_00[0x10];
    double valid_angle_threshold;       /* +0x10 */
    uint32_t gaussian_noise_flag;      /* +0x18 */
    uint32_t reserved_1c;
    double gaussian_mean[2];           /* +0x20,+0x28 */
    double gaussian_sigma[2];          /* +0x30,+0x38 */
    DpMatrix installation_matrix;      /* +0x40：配置源矩阵 */
    DpMatrix projection_matrix;        /* +0x58：DSS_Init 转置输出 */
    uint8_t reserved_70[0x98];
    int32_t valid_flag;                /* +0x108 */
    uint32_t reserved_10c;
    DpVector measure;                  /* +0x110 */
    uint8_t reserved_120[0x18];
    double angle_x;                    /* +0x138 = atan2(x,z) */
    double ratio_x;                    /* +0x140 */
    double angle_y;                    /* +0x148 = atan2(y,z) */
    double ratio_y;                    /* +0x150 */
} DpDssRecovered;

_Static_assert(offsetof(DpDssRecovered, valid_angle_threshold) == 0x10, "DSS validity threshold");
_Static_assert(offsetof(DpDssRecovered, projection_matrix) == 0x58, "DSS matrix");
_Static_assert(offsetof(DpDssRecovered, valid_flag) == 0x108, "DSS validity flag");
_Static_assert(offsetof(DpDssRecovered, measure) == 0x110, "DSS measure");
_Static_assert(offsetof(DpDssRecovered, angle_x) == 0x138, "DSS angle x");
_Static_assert(sizeof(DpDssRecovered) == 0x158, "DSS item size");

/* 原 ELF 全局 DSS 为连续 2 × 0x158 字节对象。 */
extern DpDssRecovered DSS[2];
/* 原 ELF ABI：无参数；为两个 DSS 对象安装内部 backing 并转置配置矩阵。 */
void DSS_Init(void);
extern int temp;
/* 原 ELF ABI：rdi=sun GCI 向量，rsi=位置 GCI 向量，更新全局 DSS.valid_flag。 */
void UpdateSunSensorValidFlag(const DpVector *sun_gci, const DpVector *position_gci);

void dp_update_dss(DpDssRecovered sensors[2], const DpVec3 *sun_body,
                   DpGaussian2Sampler gaussian2, void *opaque);
/* 原 ELF ABI：rdi 为三元素 DpVector，更新全局 DSS[2] 并推进 temp。 */
void UpdateDSS(const DpVector *sun_body);
int dp_is_earth_shadow(const DpVector *sun_gci, const DpVector *position_gci);
/* 原 ELF ABI：rdi=sun 向量，rsi=位置向量，eax 返回阴影判定。 */
int isEarthShadow(const DpVector *sun_gci, const DpVector *position_gci);
void dp_update_dss_valid_flag(DpDssRecovered sensors[2], const DpVector *sun_gci,
                              const DpVector *position_gci);

/* 原 STS 为 3 × 0x170；其周期触发时将误差四元数左乘真值四元数。 */
typedef struct {
    uint8_t reserved_00[0x08];
    double update_period;               /* +0x08 */
    uint8_t reserved_10[0x08];
    uint32_t gaussian_noise_flag;       /* +0x18：GetStarTrackerQuat */
    uint32_t reserved_1c;
    double euler_mean_deg[3];           /* +0x20..+0x30 */
    double euler_sigma_deg[3];          /* +0x38..+0x48 */
    double sun_exclusion_angle;         /* +0x50 */
    double earth_margin_angle;          /* +0x58 */
    DpMatrix installation_matrix;       /* +0x60 */
    uint8_t reserved_78[0x48];
    DpQuatAbi installation_quat;        /* +0xc0：matrix2quat 输出 */
    uint8_t reserved_d8[0x20];
    DpQuatAbi error_quat;               /* +0xf8 */
    uint8_t reserved_110[0x20];
    double accumulated_time;            /* +0x130 */
    int32_t valid_flag;                 /* +0x138 */
    uint32_t reserved_13c;
    DpQuatAbi measure_quat;             /* +0x140 */
    uint8_t reserved_158[0x18];
} DpStsRecovered;

_Static_assert(offsetof(DpStsRecovered, update_period) == 0x08, "STS period");
_Static_assert(offsetof(DpStsRecovered, gaussian_noise_flag) == 0x18, "STS Gaussian flag");
_Static_assert(offsetof(DpStsRecovered, euler_mean_deg) == 0x20, "STS Euler mean");
_Static_assert(offsetof(DpStsRecovered, euler_sigma_deg) == 0x38, "STS Euler sigma");
_Static_assert(offsetof(DpStsRecovered, sun_exclusion_angle) == 0x50, "STS sun threshold");
_Static_assert(offsetof(DpStsRecovered, earth_margin_angle) == 0x58, "STS earth margin");
_Static_assert(offsetof(DpStsRecovered, installation_matrix) == 0x60, "STS installation matrix");
_Static_assert(offsetof(DpStsRecovered, error_quat) == 0xf8, "STS error quaternion");
_Static_assert(offsetof(DpStsRecovered, accumulated_time) == 0x130, "STS accumulated time");
_Static_assert(offsetof(DpStsRecovered, valid_flag) == 0x138, "STS validity flag");
_Static_assert(offsetof(DpStsRecovered, measure_quat) == 0x140, "STS output quaternion");
_Static_assert(sizeof(DpStsRecovered) == 0x170, "STS item size");

/* 原 ELF 全局 STS 为连续 3 × 0x170 字节对象。 */
extern DpStsRecovered STS[3];
/* 原 ELF ABI：无参数；安装内部 backing，矩阵转四元数并初始化误差/测量四元数。 */
void STS_Init(void);
/* 原 ELF ABI：rdi=4-double 输出，esi=STS 索引；无返回值。 */
void GetStarTrackerQuat(double out_quat[4], int32_t index);

void dp_update_sts_quat(DpStsRecovered sensors[3], const DpQuatAbi *truth_quat,
                        double step_time);
/* 原 ELF ABI：rdi=真值四元数，xmm0=时间步；更新全局 STS[3]。 */
void Update_STS_Quat(const DpQuatAbi *truth_quat, double step_time);
/* GetStarTrackerQuat：无噪声时复制 measure_quat；返回 -2 表示需走尚在校准的 Euler 噪声分支。 */
int dp_get_star_tracker_quat(double out_quat[4], const DpStsRecovered *sensor,
                             DpGaussian2Sampler gaussian2, void *opaque);
int dp_is_star_tracker_valid(const DpQuatAbi *attitude_gci_to_body,
                             const DpVector *position_gci, const DpVector *sun_gci,
                             const DpMatrix *installation_matrix,
                             double sun_exclusion_angle, double earth_margin_angle);
/* 原 ELF ABI：rdi/rsi/rdx/rcx 为姿态、位置、太阳、安装矩阵；xmm0/xmm1 为两阈值。 */
int isStarTrackerValid(const DpQuatAbi *attitude_gci_to_body,
                       const DpVector *position_gci, const DpVector *sun_gci,
                       const DpMatrix *installation_matrix,
                       double sun_exclusion_angle, double earth_margin_angle);
void dp_update_sts_valid_flag(DpStsRecovered sensors[3], const DpQuatAbi *attitude_gci_to_body,
                              const DpVector *position_gci, const DpVector *sun_gci);
/* 原 ELF ABI：rdi=姿态，rsi=位置，rdx=太阳；更新全局 STS[3].valid_flag。 */
void Update_STS_ValidFlag(const DpQuatAbi *attitude_gci_to_body,
                          const DpVector *position_gci, const DpVector *sun_gci);

/* 原 isEarthOut：比较视线与 -position 的夹角是否超过 asin(R_Earth/|position|)+margin。 */
int dp_is_earth_out(const DpVector *position_gci, const DpVector *view_direction,
                    double margin_angle);
int isEarthOut(const DpVector *position_gci, const DpVector *view_direction,
               double margin_angle);
int isSunOut(const DpVector *first, const DpVector *second, double threshold);
double ran_gaussian(double sigma);
double ran_gaussian2(double mean, double sigma);

/* 原 GPS_Kalman 全局对象大小为 0x70；UpdateGPS 只写 +0x28..+0x6f。 */
typedef struct {
    uint8_t reserved_00[0x28];
    double position_gci[3];             /* +0x28 */
    double velocity_gci[3];             /* +0x40 */
    int32_t time_components[6];         /* +0x58：cvttsd2si */
} DpGpsKalmanRecovered;

_Static_assert(offsetof(DpGpsKalmanRecovered, position_gci) == 0x28, "GPS position");
_Static_assert(offsetof(DpGpsKalmanRecovered, velocity_gci) == 0x40, "GPS velocity");
_Static_assert(offsetof(DpGpsKalmanRecovered, time_components) == 0x58, "GPS time fields");
_Static_assert(sizeof(DpGpsKalmanRecovered) == 0x70, "GPS Kalman size");

/* 原 ELF 全局 GPS 对象和初始化标志。 */
extern DpGpsKalmanRecovered GPS_Kalman;
extern int init_flag;
/* 原 ELF ABI：rdi=6 个时间 double，rsi=位置 DpVec3，rdx=速度 DpVec3。 */
void UpdateGPS(const double time_values[6], const DpVec3 *position_gci,
               const DpVec3 *velocity_gci);

void dp_update_gps(DpGpsKalmanRecovered *gps, const double time_values[6],
                   const DpVec3 *position_gci, const DpVec3 *velocity_gci,
                   int *init_flag_inout);

/* UpdateDeviceMeasure 的已恢复传感器全局对象集合与环境尾区输入。 */
typedef struct {
    DpStsRecovered sts[3];
    DpGyroRecovered gyro[2];
    DpDssRecovered dss[2];
    DpMagMeterRecovered magmeter[2];
    DpGpsKalmanRecovered gps;
    int gps_init_flag;
} DpDeviceMeasureRecovered;

typedef struct {
    DpVec3 sun_body;                   /* 原 CoreDynamic 输出 +0x38 */
    DpVec3 sun_gci;                    /* 原 CoreDynamic 输出 +0x50 */
    DpVec3 magnetic_body;              /* 原 CoreDynamic 输出 +0x68 */
    double time_values[6];             /* 原 CoreDynamic 输出 +0xc8 */
} DpDeviceMeasureEnvironment;

void dp_update_device_measure_recovered(DpDeviceMeasureRecovered *devices,
                                        const DpState *state,
                                        const DpDeviceMeasureEnvironment *environment,
                                        double step_time,
                                        DpGaussian2Sampler gaussian2, void *opaque);

/* 同名 UpdateDeviceMeasure 全局 ABI 的设备状态；调用 reset/relocate 后可装载原始对象快照。 */
extern DpDeviceMeasureRecovered DeviceMeasure;
void dp_device_measure_globals_reset(void);
void dp_device_measure_globals_relocate(void);
void UpdateDeviceMeasure(const void *core_dynamic_output);

#ifdef __cplusplus
}
#endif
#endif /* DYNAMIC_SENSORS_H */
