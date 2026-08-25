/*
 * DynamicPackage 全局运行时对象布局恢复。
 *
 * 所有对象尺寸来自 ELF 符号表；已确认字段使用数学 ABI 描述符，
 * 未确认区域保持原始字节长度，禁止擅自重排。
 */
#ifndef DYNAMIC_RUNTIME_LAYOUT_H
#define DYNAMIC_RUNTIME_LAYOUT_H

#include "dynamic_math.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DP_SAT_BYTES 4216u
#define DP_SADA_BYTES 104u
#define DP_GPS_KALMAN_BYTES 112u
#define DP_GYRO_BYTES 1600u
#define DP_MAGMETER_BYTES 624u
#define DP_STS_BYTES 1104u
#define DP_RWHEEL_BYTES 480u
#define DP_MTQ_BYTES 336u
#define DP_THRUSTER_BYTES 176u

/* `SatParaInit` 把这些位置初始化为嵌入三元向量的描述符。 */
typedef struct {
    uint8_t raw[DP_SAT_BYTES];
} DpSatOpaque;

/*
 * J_c_B / J_c_B_inv 是 3x3 双精度矩阵描述符；H_w_B / L_c_B 是三元向量描述符。
 * 这些对象数据存储区分别由 *_mem 符号提供，不能把描述符本身误当作数组。
 */
typedef struct {
    double j_c_b_mem[9];
    double j_c_b_inv_mem[9];
    double h_w_b_mem[3];
    double l_c_b_mem[3];
    DpMatrix j_c_b;
    DpMatrix j_c_b_inv;
    DpVector h_w_b;
    DpVector l_c_b;
} DpRigidBodyMatrices;

/* 与原符号大小对应的保守设备容器。字段在未恢复前以 raw 保留。 */
typedef struct { uint8_t raw[DP_SADA_BYTES]; } DpSadaOpaque;
typedef struct { uint8_t raw[DP_GPS_KALMAN_BYTES]; } DpGpsKalmanOpaque;
typedef struct { uint8_t raw[DP_GYRO_BYTES]; } DpGyroOpaque;
typedef struct { uint8_t raw[DP_MAGMETER_BYTES]; } DpMagMeterOpaque;
typedef struct { uint8_t raw[DP_STS_BYTES]; } DpStsOpaque;
typedef struct { uint8_t raw[DP_RWHEEL_BYTES]; } DpReactionWheelOpaque;
typedef struct { uint8_t raw[DP_MTQ_BYTES]; } DpMtqOpaque;
typedef struct { uint8_t raw[DP_THRUSTER_BYTES]; } DpThrusterOpaque;

/* `SatParaInit` 直接验证的描述符字段位置。 */
#define DP_SAT_VEC0_DESCRIPTOR_OFFSET 0x018u
#define DP_SAT_VEC1_DESCRIPTOR_OFFSET 0x078u
#define DP_SAT_VEC2_DESCRIPTOR_OFFSET 0x0d0u
#define DP_SAT_POSITION_GCI_DESCRIPTOR_OFFSET 0x120u
#define DP_SAT_VELOCITY_GCI_DESCRIPTOR_OFFSET 0x148u

#ifdef __cplusplus
}
#endif
#endif /* DYNAMIC_RUNTIME_LAYOUT_H */
