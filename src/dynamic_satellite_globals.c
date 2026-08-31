#include "dynamic_satellite_globals.h"

#include "dynamic_core_bridge.h"
#include "dynamic_core_environment.h"
#include "dynamic_core_layout.h"
#include "dynamic_dynamics.h"
#include "dynamic_environment.h"
#include "dynamic_flex.h"
#include "dynamic_orbit.h"
#include "dynamic_time.h"
#include "dynamic_torque.h"

#include "dynamic_sat_inertia_flag1_constants.inc"

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define DP_SAT_INERTIA_DESCRIPTOR_OFFSET 0x08u
#define DP_SAT_INERTIA_INV_DESCRIPTOR_OFFSET 0x68u
#define DP_SAT_M3E8_DESCRIPTOR_OFFSET 0x3e8u
#define DP_SAT_M448_DESCRIPTOR_OFFSET 0x448u
#define DP_SAT_M4A8_DESCRIPTOR_OFFSET 0x4a8u
#define DP_SAT_M508_DESCRIPTOR_OFFSET 0x508u
#define DP_SAT_M568_DESCRIPTOR_OFFSET 0x568u
#define DP_SAT_M688_DESCRIPTOR_OFFSET 0x688u
#define DP_SAT_COUPLING_DESCRIPTOR_OFFSET 0x898u
#define DP_SAT_M9A0_DESCRIPTOR_OFFSET 0x9a0u
#define DP_SAT_MODAL_A_DESCRIPTOR_OFFSET 0xa00u
#define DP_SAT_MODAL_D_DESCRIPTOR_OFFSET 0xd38u

double J_c_B_mem[9] = {0.0};
double J_c_B_inv_mem[9] = {0.0};
double H_w_B_mem[3] = {0.0};
double L_c_B_mem[3] = {0.0};
double B_I_static_mem[3] = {0.0};
DpMatrix J_c_B = {3, 3, 3, 0, J_c_B_mem};
DpMatrix J_c_B_inv = {3, 3, 3, 0, J_c_B_inv_mem};
DpVector H_w_B = {3, 0, H_w_B_mem};
DpVector L_c_B = {3, 0, L_c_B_mem};
DpVector B_I_static = {3, 0, B_I_static_mem};
DpSatOpaque Sat = {{0}};
DpSadaRecovered SADA = {0};
DpSatTorqueOpaque SatTorque = {{0}};

/* 原 TorqueInit 不写 descriptor 计数或任何 backing 值，仅重绑定 data 指针。 */
void TorqueInit(void)
{
    void *backing = &SatTorque.raw[0x80];

    memcpy(&SatTorque.raw[0x08], &backing, sizeof(backing));
    backing = &SatTorque.raw[0x98];
    memcpy(&SatTorque.raw[0x18], &backing, sizeof(backing));
    backing = &SatTorque.raw[0xb0];
    memcpy(&SatTorque.raw[0x28], &backing, sizeof(backing));
    backing = &SatTorque.raw[0xc8];
    memcpy(&SatTorque.raw[0x38], &backing, sizeof(backing));
    backing = &SatTorque.raw[0xe0];
    memcpy(&SatTorque.raw[0x48], &backing, sizeof(backing));
    backing = &SatTorque.raw[0xf8];
    memcpy(&SatTorque.raw[0x58], &backing, sizeof(backing));
    backing = &SatTorque.raw[0x110];
    memcpy(&SatTorque.raw[0x68], &backing, sizeof(backing));
    backing = &SatTorque.raw[0x128];
    memcpy(&SatTorque.raw[0x78], &backing, sizeof(backing));
    backing = &SatTorque.raw[0x150];
    memcpy(&SatTorque.raw[0x148], &backing, sizeof(backing));
}

/* 原 SatParaInit：只安装下列 25 个 descriptor 的 data 指针。 */
void SatParaInit(void)
{
    static const size_t pointer_offsets[25] = {
        0x018u, 0x078u, 0x0d0u, 0x0f8u, 0x120u, 0x148u,
        0x1b8u, 0x218u, 0x278u, 0x2d8u, 0x338u, 0x398u,
        0x3f8u, 0x458u, 0x4b8u, 0x518u, 0x578u, 0x5d8u,
        0x638u, 0x698u, 0x7a0u, 0x8a8u, 0x9b0u, 0xa10u,
        0xd48u
    };
    static const size_t backing_offsets[25] = {
        0x020u, 0x080u, 0x0d8u, 0x100u, 0x128u, 0x150u,
        0x1c0u, 0x220u, 0x280u, 0x2e0u, 0x340u, 0x3a0u,
        0x400u, 0x460u, 0x4c0u, 0x520u, 0x580u, 0x5e0u,
        0x640u, 0x6a0u, 0x7a8u, 0x8b0u, 0x9b8u, 0xa18u,
        0xd50u
    };
    size_t index;

    for (index = 0u; index < sizeof(pointer_offsets) / sizeof(pointer_offsets[0]); ++index) {
        void *backing = &Sat.raw[backing_offsets[index]];
        memcpy(&Sat.raw[pointer_offsets[index]], &backing, sizeof(backing));
    }
}

void UpdateTorque(void)
{
    DpVector term_0;
    DpVector term_1;
    DpVector term_2;
    DpVector total;

    memcpy(&term_0, &SatTorque.raw[0x00], sizeof(term_0));
    memcpy(&term_1, &SatTorque.raw[0x10], sizeof(term_1));
    memcpy(&term_2, &SatTorque.raw[0x20], sizeof(term_2));
    memcpy(&total, &SatTorque.raw[0x70], sizeof(total));
    (void)vector_set_zero(&total);
    (void)vector_add(&total, &term_0);
    (void)vector_add(&total, &term_1);
    (void)vector_add(&total, &term_2);
    SetTorque(&total);
}

/* 原 GetInertialMag：rdi 为输出，源固定为全局 B_I_static+0x8 data 指针。 */
void GetInertialMag(DpVector *magnetic_gci_out_3)
{
    magnetic_gci_out_3->data[0] = B_I_static.data[0];
    magnetic_gci_out_3->data[1] = B_I_static.data[1];
    magnetic_gci_out_3->data[2] = B_I_static.data[2];
}

/* 原 MagUpdate：局部位置/速度 descriptor 均为 count=3、reserved=0，
 * 先经 Get_Orbit_RV 从全局 y 复制，再更新全局 B_I_static。 */
void MagUpdate(const DpCalendarTime *calendar_time)
{
    double position_data[3] = {0.0, 0.0, 0.0};
    double velocity_data[3] = {0.0, 0.0, 0.0};
    DpVector position = {3, 0, position_data};
    DpVector velocity = {3, 0, velocity_data};

    Get_Orbit_RV(position_data, velocity_data);
    Calc_InertialMagneticVector(&B_I_static, &position, &velocity, calendar_time);
}

/* 原 SetTorque/GetTorque 与 Set/GetWheelAngularMoment 均内联三个标量复制，
 * 不访问描述符的 count 或 reserved 字段。保留该明确的元素写入顺序。 */
void SetTorque(const DpVector *torque_3)
{
    L_c_B.data[0] = torque_3->data[0];
    L_c_B.data[1] = torque_3->data[1];
    L_c_B.data[2] = torque_3->data[2];
}

void GetTorque(DpVector *torque_3_out)
{
    torque_3_out->data[0] = L_c_B.data[0];
    torque_3_out->data[1] = L_c_B.data[1];
    torque_3_out->data[2] = L_c_B.data[2];
}

void SetWheelAngularMoment(const DpVector *angular_momentum_3)
{
    H_w_B.data[0] = angular_momentum_3->data[0];
    H_w_B.data[1] = angular_momentum_3->data[1];
    H_w_B.data[2] = angular_momentum_3->data[2];
}

void GetWheelAngularMoment(DpVector *angular_momentum_3_out)
{
    angular_momentum_3_out->data[0] = H_w_B.data[0];
    angular_momentum_3_out->data[1] = H_w_B.data[1];
    angular_momentum_3_out->data[2] = H_w_B.data[2];
}

double y[DP_STATE_DIM] = {0.0};

/* 原 get_attitude 逐元素从 y[0..6] 复制，不重写任一输出描述符字段。 */
void get_attitude(DpQuatAbi *attitude_out, DpVector *angular_rate_out)
{
    attitude_out->w = y[0];
    attitude_out->xyz.data[0] = y[1];
    attitude_out->xyz.data[1] = y[2];
    attitude_out->xyz.data[2] = y[3];
    angular_rate_out->data[0] = y[4];
    angular_rate_out->data[1] = y[5];
    angular_rate_out->data[2] = y[6];
}

void get_Inertial2Body(DpMatrix *out_inertial_to_body_3x3)
{
    DpQuatAbi attitude = {y[0], {3, 0, &y[1]}};

    quat_att_mat(out_inertial_to_body_3x3, &attitude);
}

void get_Body2Inertial(DpMatrix *out_body_to_inertial_3x3)
{
    double conjugate_xyz[3];
    DpQuatAbi attitude = {y[0], {3, 0, &y[1]}};
    DpQuatAbi conjugate = {0.0, {3, 0, conjugate_xyz}};

    quat_conjugate(&conjugate, &attitude);
    quat_att_mat(out_body_to_inertial_3x3, &conjugate);
}

/* 原 Get_Orbit_RV 使用原始 rdi/rsi double 缓冲区，无 DpVector 解引用。 */
void Get_Orbit_RV(double position_gci_out[3], double velocity_gci_out[3])
{
    position_gci_out[0] = y[7];
    position_gci_out[1] = y[8];
    position_gci_out[2] = y[9];
    velocity_gci_out[0] = y[10];
    velocity_gci_out[1] = y[11];
    velocity_gci_out[2] = y[12];
}

/* 原 SetInertiaTensor 在复制输入前先于栈上计算逆矩阵，随后两数组交替逐元写入。 */
void SetInertiaTensor(const double inertia_3x3[9])
{
    double inverse_3x3[9];
    unsigned index;

    inv_CAL_M3(inertia_3x3, inverse_3x3);
    for (index = 0u; index < 9u; ++index) {
        J_c_B.data[index] = inertia_3x3[index];
        J_c_B_inv.data[index] = inverse_3x3[index];
    }
}

/* 原 SetSatInertiaTensor 读取 Sat 内两份矩阵 descriptor 的当前 data 指针，
 * 仅写入其 backing；descriptor 本体的 rows/cols/stride 不变。 */
void SetSatInertiaTensor(const double inertia_3x3[9])
{
    DpMatrix sat_inertia;
    DpMatrix sat_inverse;
    double inverse_3x3[9];
    unsigned index;

    memcpy(&sat_inertia, &Sat.raw[DP_SAT_INERTIA_DESCRIPTOR_OFFSET], sizeof(sat_inertia));
    memcpy(&sat_inverse, &Sat.raw[DP_SAT_INERTIA_INV_DESCRIPTOR_OFFSET], sizeof(sat_inverse));
    inv_CAL_M3(inertia_3x3, inverse_3x3);
    for (index = 0u; index < 9u; ++index) {
        sat_inertia.data[index] = inertia_3x3[index];
        sat_inverse.data[index] = inverse_3x3[index];
    }
}

/* 原 Update_sat_inertia：flag=0 写入基惯量；flag=1 逐元加装载增量；其他
 * flag 保留当前 Sat 惯量。三分支随后均重算 Sat 逆矩阵，并将当前 Sat 惯量写回
 * 全局 J_c_B/J_c_B_inv。bit 常量来自 0xb321..0xb451 的 rodata 加载序列。 */
void Update_sat_inertia(int32_t flag)
{
    static const uint64_t base_bits[9] = {
        UINT64_C(0x405e000000000000), UINT64_C(0xbfe428f5c28f5c29),
        UINT64_C(0x3ff4cccccccccccd), UINT64_C(0xbfe428f5c28f5c29),
        UINT64_C(0x4076900000000000), UINT64_C(0xbfe0000000000000),
        UINT64_C(0x3ff4cccccccccccd), UINT64_C(0xbfe0000000000000),
        UINT64_C(0x407d500000000000)
    };
    static const uint64_t increment_bits[9] = {
        UINT64_C(0x409ca40000000000), UINT64_C(0x3fc0a3d70a3d70a4),
        UINT64_C(0x3fc70a3d70a3d70a), UINT64_C(0x3fc0a3d70a3d70a4),
        UINT64_C(0x4066e00000000000), UINT64_C(0xc031333333333333),
        UINT64_C(0x3fc70a3d70a3d70a), UINT64_C(0xc031333333333333),
        UINT64_C(0x4099080000000000)
    };
    DpMatrix sat_inertia;
    DpMatrix sat_inverse;
    double inertia_3x3[9];
    double inverse_3x3[9];
    unsigned index;

    memcpy(&sat_inertia, &Sat.raw[DP_SAT_INERTIA_DESCRIPTOR_OFFSET], sizeof(sat_inertia));
    memcpy(&sat_inverse, &Sat.raw[DP_SAT_INERTIA_INV_DESCRIPTOR_OFFSET], sizeof(sat_inverse));
    if (flag == 0 || flag == 1) {
        for (index = 0u; index < 9u; ++index) {
            memcpy(&inertia_3x3[index], &base_bits[index], sizeof(inertia_3x3[index]));
        }
        if (flag == 1) {
            for (index = 0u; index < 9u; ++index) {
                double increment;

                memcpy(&increment, &increment_bits[index], sizeof(increment));
                inertia_3x3[index] += increment;
            }
        }
        for (index = 0u; index < 9u; ++index) {
            sat_inertia.data[index] = inertia_3x3[index];
        }
    }
    inv_CAL_M3(sat_inertia.data, inverse_3x3);
    for (index = 0u; index < 9u; ++index) {
        sat_inverse.data[index] = inverse_3x3[index];
    }
    SetInertiaTensor(sat_inertia.data);
}

double t = 0.0;
double step_time = 0.0;

/* 原 intergrator_init：rdi=DpQuatAbi，rsi=三维 DpVector，rdx/rcx 为裸
 * 三元素 double 位置/速度缓冲区，第五个 double 参数位于 xmm0。写入 t 后依次
 * 重建 y[0..12]，最后将全部 20 个柔性状态槽 y[13..32] 精确归零。 */
void intergrator_init(const DpQuatAbi *attitude, const DpVector *body_rate,
                      const double position_gci[3], const double velocity_gci[3],
                      double initial_time)
{
    size_t index;

    t = initial_time;
    y[0] = attitude->w;
    y[1] = attitude->xyz.data[0];
    y[2] = attitude->xyz.data[1];
    y[3] = attitude->xyz.data[2];
    y[4] = body_rate->data[0];
    y[5] = body_rate->data[1];
    y[6] = body_rate->data[2];
    y[7] = position_gci[0];
    y[8] = position_gci[1];
    y[9] = position_gci[2];
    y[10] = velocity_gci[0];
    y[11] = velocity_gci[1];
    y[12] = velocity_gci[2];
    for (index = 13u; index < DP_STATE_DIM; ++index) {
        y[index] = 0.0;
    }
}

/* 原 intergrator_show 仅从 y[0..6] 构造栈上四元数/方向余弦矩阵，调用
 * quat_conjugate、quat_att_mat 和 blas_gemv 后即返回；没有全局、参数或 I/O
 * 写回。当前恢复的可观察 ABI 因此为无副作用。 */
void intergrator_show(void)
{
}

static DpCoreDefaultModel dp_global_default_model;
static int dp_global_model_ready;
/* Update_sat_inertia_xw(flag=1) 保留此前 Sat 模型；初始静态模型不注入独立
 * SADA 加速度反作用，已由 flag=0 建立的 SADA 历史模型则继续注入该项。 */
static unsigned dp_global_sada_reaction_enabled = 1u;
static unsigned dp_global_sat_model_is_sada_history;
/* 原 dyn_main 的 MTQ 路径在本步使用前一 CoreDynamic 环境更新留下的 B_I_static。 */
static DpVec3 dp_prior_magnetic_body;
static unsigned dp_prior_magnetic_body_valid;
#ifdef DP_DIAGNOSTIC_SADA_STAGE_TRACE
static unsigned dp_sada_stage_trace_count;
#endif

static DpMatrix dp_sat_matrix_load(size_t offset)
{
    DpMatrix matrix = {0};

    memcpy(&matrix, &Sat.raw[offset], sizeof(matrix));
    return matrix;
}

static void dp_sat_matrix_store(size_t offset, const DpMatrix *matrix)
{
    memcpy(&Sat.raw[offset], matrix, sizeof(*matrix));
}

static void dp_copy_model_bits(double *destination, const uint64_t *source, size_t count)
{
    size_t index;

    for (index = 0u; index < count; ++index) {
        memcpy(&destination[index], &source[index], sizeof(destination[index]));
    }
}

static const uint64_t dp_update_sat_inertia_flag1_j_bits[9] = {
    UINT64_C(0x405e000000000000), UINT64_C(0xbfe428f5c28f5c29), UINT64_C(0x3ff4cccccccccccd),
    UINT64_C(0xbfe428f5c28f5c29), UINT64_C(0x4076900000000000), UINT64_C(0xbfe0000000000000),
    UINT64_C(0x3ff4cccccccccccd), UINT64_C(0xbfe0000000000000), UINT64_C(0x407d500000000000)
};
static const uint64_t dp_update_sat_inertia_flag1_jinv_bits[9] = {
    UINT64_C(0x3f81113cdb6f56f3), UINT64_C(0x3eee6ed1eab39b43), UINT64_C(0xbef8349fe3dcda19),
    UINT64_C(0x3eee6ed1eab39b43), UINT64_C(0x3f66b158cdc4be55), UINT64_C(0x3ec86f97ee84395f),
    UINT64_C(0xbef8349fe3dcda19), UINT64_C(0x3ec86f97ee84395f), UINT64_C(0x3f6177ae1e1da471)
};

static void dp_global_default_sat_descriptors_sync(void)
{
    const DpMatrix m4a8 = {3, 3, 3, 0, dp_global_default_model.m3_4a8};
    const DpMatrix m688 = {3, 10, 10, 0, dp_global_default_model.m6};

    dp_sat_matrix_store(DP_SAT_M4A8_DESCRIPTOR_OFFSET, &m4a8);
    dp_sat_matrix_store(DP_SAT_M688_DESCRIPTOR_OFFSET, &m688);
    dp_sat_matrix_store(DP_SAT_COUPLING_DESCRIPTOR_OFFSET,
                        &dp_global_default_model.coupling);
    dp_sat_matrix_store(DP_SAT_M9A0_DESCRIPTOR_OFFSET,
                        &dp_global_default_model.sada_command_momentum_map);
    dp_sat_matrix_store(DP_SAT_MODAL_A_DESCRIPTOR_OFFSET,
                        &dp_global_default_model.modal_a);
    dp_sat_matrix_store(DP_SAT_MODAL_D_DESCRIPTOR_OFFSET,
                        &dp_global_default_model.modal_d);
}

static int dp_global_copy_sat_matrix(size_t offset, double *destination, size_t count)
{
    DpMatrix matrix = dp_sat_matrix_load(offset);

    if (destination == NULL || matrix.data == NULL ||
        (size_t)matrix.rows * (size_t)matrix.row_stride < count) {
        return -1;
    }
    memcpy(destination, matrix.data, count * sizeof(*destination));
    return 0;
}

int dp_global_restore_sat_model_from_runtime(void)
{
    DpMatrix inertia = dp_sat_matrix_load(DP_SAT_INERTIA_DESCRIPTOR_OFFSET);
    DpMatrix inverse = dp_sat_matrix_load(DP_SAT_INERTIA_INV_DESCRIPTOR_OFFSET);

    if (dp_global_model_ready == 0 &&
        dp_core_default_model_init(&dp_global_default_model) != 0) {
        return -1;
    }
    if (inertia.data == NULL || inverse.data == NULL ||
        dp_global_copy_sat_matrix(DP_SAT_M3E8_DESCRIPTOR_OFFSET,
                                  dp_global_default_model.m3_e8, 9u) != 0 ||
        dp_global_copy_sat_matrix(DP_SAT_M448_DESCRIPTOR_OFFSET,
                                  dp_global_default_model.m3_448, 9u) != 0 ||
        dp_global_copy_sat_matrix(DP_SAT_M4A8_DESCRIPTOR_OFFSET,
                                  dp_global_default_model.m3_4a8, 9u) != 0 ||
        dp_global_copy_sat_matrix(DP_SAT_M508_DESCRIPTOR_OFFSET,
                                  dp_global_default_model.m3_508, 9u) != 0 ||
        dp_global_copy_sat_matrix(DP_SAT_M568_DESCRIPTOR_OFFSET,
                                  dp_global_default_model.m3_568, 9u) != 0 ||
        dp_global_copy_sat_matrix(DP_SAT_M688_DESCRIPTOR_OFFSET,
                                  dp_global_default_model.m6, 30u) != 0 ||
        dp_global_copy_sat_matrix(DP_SAT_COUPLING_DESCRIPTOR_OFFSET,
                                  dp_global_default_model.coupling_data, 30u) != 0 ||
        dp_global_copy_sat_matrix(DP_SAT_M9A0_DESCRIPTOR_OFFSET,
                                  dp_global_default_model.m9, 9u) != 0 ||
        dp_global_copy_sat_matrix(DP_SAT_MODAL_A_DESCRIPTOR_OFFSET,
                                  dp_global_default_model.modal_a_data, 100u) != 0 ||
        dp_global_copy_sat_matrix(DP_SAT_MODAL_D_DESCRIPTOR_OFFSET,
                                  dp_global_default_model.modal_d_data, 100u) != 0) {
        return -1;
    }
    memcpy(dp_global_default_model.inertia, inertia.data,
           sizeof(dp_global_default_model.inertia));
    memcpy(dp_global_default_model.inverse, inverse.data,
           sizeof(dp_global_default_model.inverse));
    dp_global_default_model.coupling = (DpMatrix){3, 10, 10, 0,
                                                   dp_global_default_model.coupling_data};
    dp_global_default_model.base_inertia = (DpMatrix){3, 3, 3, 0, J_c_B_mem};
    dp_global_default_model.modal_a = (DpMatrix){10, 10, 10, 0,
                                                  dp_global_default_model.modal_a_data};
    dp_global_default_model.modal_d = (DpMatrix){10, 10, 10, 0,
                                                  dp_global_default_model.modal_d_data};
    dp_global_default_model.sada_command_momentum_map =
        (DpMatrix){3, 3, 3, 0, dp_global_default_model.m9};
    dp_global_default_model.sada_modal_pre_map_3x3 =
        (DpMatrix){3, 3, 3, 0, dp_global_default_model.m3_4a8};
    dp_global_default_model.sada_modal_acceleration_map_3xn =
        (DpMatrix){3, 10, 10, 0, dp_global_default_model.m6};
    dp_global_model_ready = 1;
    return 0;
}

int dp_global_default_inertia_baseline_get(double out_inertia_3x3[9])
{
    if (out_inertia_3x3 == NULL || dp_core_default_model_init(&dp_global_default_model) != 0) {
        return -1;
    }
    memcpy(out_inertia_3x3, dp_global_default_model.inertia, sizeof(J_c_B_mem));
    return 0;
}

int dp_global_apply_sat_inertia_flag0(void)
{
    DpSatInertiaFlag0Outputs outputs;

    memset(&outputs, 0, sizeof(outputs));
    outputs.inertia_3x3 = J_c_B_mem;
    outputs.inverse_inertia_3x3 = J_c_B_inv_mem;
    outputs.matrix_3x3_e8 = dp_global_default_model.m3_e8;
    outputs.matrix_3x3_448 = dp_global_default_model.m3_448;
    outputs.matrix_3x3_4a8 = dp_global_default_model.m3_4a8;
    outputs.matrix_3x3_508 = dp_global_default_model.m3_508;
    outputs.matrix_3x3_568 = dp_global_default_model.m3_568;
    outputs.matrix_3x10_688 = dp_global_default_model.m6;
    outputs.coupling_3x10_898 = dp_global_default_model.coupling_data;
    outputs.matrix_3x3_9a0 = dp_global_default_model.m9;
    outputs.modal_matrix_a_10x10 = dp_global_default_model.modal_a_data;
    outputs.modal_matrix_d_10x10 = dp_global_default_model.modal_d_data;
    if (dp_load_sat_inertia_flag0_static(&outputs) != 0) {
        return -1;
    }
    dp_global_default_model.coupling = (DpMatrix){3, 10, 10, 0,
                                                   dp_global_default_model.coupling_data};
    dp_global_default_model.base_inertia = (DpMatrix){3, 3, 3, 0, J_c_B_mem};
    dp_global_default_model.modal_a = (DpMatrix){10, 10, 10, 0,
                                                  dp_global_default_model.modal_a_data};
    dp_global_default_model.modal_d = (DpMatrix){10, 10, 10, 0,
                                                  dp_global_default_model.modal_d_data};
    dp_global_default_model.sada_command_momentum_map =
        (DpMatrix){3, 3, 3, 0, dp_global_default_model.m9};
    /* dynamics_flex 的 SADA 路径使用独立反作用块；它不是 Sat+0x448。
     * 模态支路则严格使用 Sat+0x4a8（见原 ELF f2ff 的 blas_gemv）。 */
    dp_global_default_model.sada_acceleration_reaction_map_3x3 =
        (DpMatrix){3, 3, 3, 0, dp_global_default_model.sada_acceleration_reaction_data};
    dp_global_sada_reaction_enabled = 1u;
    dp_global_sat_model_is_sada_history = 1u;
    dp_global_default_model.sada_modal_pre_map_3x3 =
        (DpMatrix){3, 3, 3, 0, dp_global_default_model.m3_4a8};
    dp_global_default_model.sada_modal_acceleration_map_3xn =
        (DpMatrix){3, 10, 10, 0, dp_global_default_model.m6};
    J_c_B = (DpMatrix){3, 3, 3, 0, J_c_B_mem};
    J_c_B_inv = (DpMatrix){3, 3, 3, 0, J_c_B_inv_mem};
    dp_global_default_sat_descriptors_sync();
    return 0;
}

int dp_global_apply_sat_inertia_flag1(void)
{
    if (dp_global_model_ready == 0) {
        /* 独立 ABI 可在未调用 DynamicInit 时重放全局对象；只建立私有 backing。 */
        if (dp_core_default_model_init(&dp_global_default_model) != 0) {
            return -1;
        }
        dp_global_model_ready = 1;
    }

    /* 原 Update_sat_inertia_xw(1) 不覆写已有 Sat backing。恢复端的初始 backing
     * 不是该 ELF 初始化模型本身，故首个无 SADA 历史的 flag=1 必须装配已取证静态
     * 常量；一旦 flag=0 已形成 SADA 历史模型，后续 flag=1 必须保留该模型。 */
    if (dp_global_sat_model_is_sada_history == 0u) {
        dp_copy_model_bits(dp_global_default_model.m3_e8, dp_flag1_m3_e8_bits, 9u);
        dp_copy_model_bits(dp_global_default_model.m3_448, dp_flag1_m3_448_bits, 9u);
        dp_copy_model_bits(dp_global_default_model.m3_4a8, dp_flag1_m3_4a8_bits, 9u);
        dp_copy_model_bits(dp_global_default_model.m3_508, dp_flag1_m3_508_bits, 9u);
        dp_copy_model_bits(dp_global_default_model.m3_568, dp_flag1_m3_568_bits, 9u);
        dp_copy_model_bits(dp_global_default_model.m6, dp_flag1_m6_bits, 30u);
        dp_copy_model_bits(dp_global_default_model.coupling_data, dp_flag1_coupling_bits, 30u);
        dp_copy_model_bits(dp_global_default_model.m9, dp_flag1_m9_bits, 9u);
        dp_copy_model_bits(dp_global_default_model.modal_a_data, dp_flag1_modal_a_bits, 100u);
        dp_copy_model_bits(dp_global_default_model.modal_d_data, dp_flag1_modal_d_bits, 100u);
        dp_global_default_model.coupling = (DpMatrix){3, 10, 10, 0,
                                                       dp_global_default_model.coupling_data};
        dp_global_default_model.modal_a = (DpMatrix){10, 10, 10, 0,
                                                      dp_global_default_model.modal_a_data};
        dp_global_default_model.modal_d = (DpMatrix){10, 10, 10, 0,
                                                      dp_global_default_model.modal_d_data};
        dp_global_default_model.sada_command_momentum_map =
            (DpMatrix){3, 3, 3, 0, dp_global_default_model.m9};
        dp_global_default_model.sada_modal_pre_map_3x3 =
            (DpMatrix){3, 3, 3, 0, dp_global_default_model.m3_4a8};
        dp_global_default_model.sada_modal_acceleration_map_3xn =
            (DpMatrix){3, 10, 10, 0, dp_global_default_model.m6};
        dp_global_default_sat_descriptors_sync();
    }
    dp_global_default_model.base_inertia = (DpMatrix){3, 3, 3, 0, J_c_B_mem};
    dp_global_sada_reaction_enabled = dp_global_sat_model_is_sada_history != 0u ? 1u : 0u;
    J_c_B = (DpMatrix){3, 3, 3, 0, J_c_B_mem};
    J_c_B_inv = (DpMatrix){3, 3, 3, 0, J_c_B_inv_mem};
    return 0;
}

/* 原 ELF ABI：edi=状态标志；非 0/1 标志不触发已观察的两套惯量更新。 */
void Update_sat_inertia_xw(int32_t flag)
{
    if (flag == 0) {
        (void)dp_global_apply_sat_inertia_flag0();
    } else if (flag == 1) {
        dp_copy_model_bits(J_c_B_mem, dp_update_sat_inertia_flag1_j_bits, 9u);
        dp_copy_model_bits(J_c_B_inv_mem, dp_update_sat_inertia_flag1_jinv_bits, 9u);
        (void)dp_global_apply_sat_inertia_flag1();
    }
}

int dp_global_default_flex_config_get(DpFlexDynamicsConfig *out_config)
{
    if (out_config == NULL) return -1;
    if (dp_global_model_ready == 0) {
        if (dp_core_default_model_init(&dp_global_default_model) != 0) return -1;
        dp_global_model_ready = 1;
    }
    dp_global_default_model.base_inertia = (DpMatrix){3, 3, 3, 0, J_c_B_mem};
    out_config->rigid_matrices.coupling_3xn = dp_global_default_model.coupling;
    out_config->rigid_matrices.base_inertia_3x3 = dp_global_default_model.base_inertia;
    out_config->modal_matrix_a_nxn = &dp_global_default_model.modal_a;
    out_config->modal_matrix_d_nxn = &dp_global_default_model.modal_d;
    out_config->angular_momentum = NULL;
    out_config->minus_term = NULL;
    out_config->rigid_map_3x3 = NULL;
    out_config->rigid_aux = NULL;
    dp_global_default_model.sada_drive.command_angle[0] = SADA.command_angle[0];
    dp_global_default_model.sada_drive.command_angle[1] = SADA.command_angle[1];
    dp_global_default_model.sada_drive.angular_acceleration[0] = SADA.angular_acceleration[0];
    dp_global_default_model.sada_drive.angular_acceleration[1] = SADA.angular_acceleration[1];
    out_config->sada_drive = &dp_global_default_model.sada_drive;
    out_config->sada_command_momentum_map_3x3 = &dp_global_default_model.sada_command_momentum_map;
    out_config->sada_acceleration_reaction_map_3x3 = &dp_global_default_model.sada_acceleration_reaction_map_3x3;
    out_config->sada_modal_pre_map_3x3 = &dp_global_default_model.sada_modal_pre_map_3x3;
    out_config->sada_modal_acceleration_map_3xn = &dp_global_default_model.sada_modal_acceleration_map_3xn;
    return 0;
}

void dynamics_flex(DpVector *angular_acceleration, const DpVector *body_rate,
                   DpVector *modal_acceleration, const DpVector *modal_position,
                   DpVector *modal_velocity_output, const DpVector *modal_velocity,
                   const DpVector *minus_term, const DpVector *angular_momentum,
                   const DpMatrix *inertia, const DpMatrix *inertia_inverse)
{
    DpFlexDynamicsConfig config;
    DpVec3 alpha;
    DpVec3 rate;
    DpVec3 momentum;
    DpVec3 minus;
    DpMatrix coupling;
    DpMatrix modal_a;
    DpMatrix modal_d;
    DpMatrix sada_command_momentum_map;
    DpMatrix sada_modal_pre_map;
    DpMatrix sada_modal_acceleration_map;
    double total_momentum_data[3] = {0.0, 0.0, 0.0};
    double sada_command_input_data[3] = {0.0, 0.0, 0.0};
    double sada_command_pre_data[3] = {0.0, 0.0, 0.0};
    double sada_command_momentum_data[3] = {0.0, 0.0, 0.0};
    double coupling_momentum_data[3] = {0.0, 0.0, 0.0};
    DpVector total_momentum = {3, 0, total_momentum_data};
    DpVector input_momentum = {3, 0, NULL};
    DpVector rate_view = {3, 0, NULL};
    DpVector sada_command_input = {3, 0, sada_command_input_data};
    DpVector sada_command_pre = {3, 0, sada_command_pre_data};
    DpVector sada_command_momentum = {3, 0, sada_command_momentum_data};
    DpVector coupling_momentum = {3, 0, coupling_momentum_data};

    (void)inertia_inverse;
    if (angular_acceleration == NULL || body_rate == NULL || modal_acceleration == NULL ||
        modal_position == NULL || modal_velocity_output == NULL || modal_velocity == NULL ||
        minus_term == NULL || angular_momentum == NULL || inertia == NULL ||
        angular_acceleration->count != 3 || body_rate->count != 3 || minus_term->count != 3 ||
        angular_momentum->count != 3 || modal_acceleration->count != 10 ||
        modal_position->count != 10 || modal_velocity_output->count != 10 ||
        modal_velocity->count != 10 || angular_acceleration->data == NULL ||
        body_rate->data == NULL || minus_term->data == NULL || angular_momentum->data == NULL) return;
    rate = (DpVec3){body_rate->data[0], body_rate->data[1], body_rate->data[2]};
    /* 原 f211 循环从 L_c_B - omega×H_total 开始；内部核按“减去 minus”
     * 实现这一项，故此处传入 -L_c_B。 */
    minus = (DpVec3){-minus_term->data[0], -minus_term->data[1], -minus_term->data[2]};
    input_momentum.data = angular_momentum->data;
    rate_view.data = &rate.x;
    if (dp_global_default_flex_config_get(&config) != 0) return;
    coupling = dp_sat_matrix_load(DP_SAT_COUPLING_DESCRIPTOR_OFFSET);
    modal_a = dp_sat_matrix_load(DP_SAT_MODAL_A_DESCRIPTOR_OFFSET);
    modal_d = dp_sat_matrix_load(DP_SAT_MODAL_D_DESCRIPTOR_OFFSET);
    sada_command_momentum_map = dp_sat_matrix_load(DP_SAT_M9A0_DESCRIPTOR_OFFSET);
    sada_modal_pre_map = dp_sat_matrix_load(DP_SAT_M4A8_DESCRIPTOR_OFFSET);
    sada_modal_acceleration_map = dp_sat_matrix_load(DP_SAT_M688_DESCRIPTOR_OFFSET);
    config.rigid_matrices.coupling_3xn = coupling;
    config.modal_matrix_a_nxn = &modal_a;
    config.modal_matrix_d_nxn = &modal_d;
    config.sada_command_momentum_map_3x3 = &sada_command_momentum_map;
    config.sada_modal_pre_map_3x3 = &sada_modal_pre_map;
    config.sada_modal_acceleration_map_3xn = &sada_modal_acceleration_map;
    /* 原 eb60..edff：H_total 严格按 J·omega、H_w、Sat+0x9a0·
     * (Sat+0x4a8·[0,cmd0,cmd1])、C·eta 的顺序累加。 */
    if (blas_gemv(inertia, &rate_view, &total_momentum, 1.0, 0.0) != 0 ||
        vector_add(&total_momentum, &input_momentum) != 0) return;
    sada_command_input_data[1] = SADA.command_angle[0];
    sada_command_input_data[2] = SADA.command_angle[1];
    if (blas_gemv(&sada_modal_pre_map, &sada_command_input, &sada_command_pre,
                  1.0, 0.0) != 0 ||
        blas_gemv(&sada_command_momentum_map, &sada_command_pre,
                  &sada_command_momentum, 1.0, 0.0) != 0 ||
        vector_add(&total_momentum, &sada_command_momentum) != 0 ||
        blas_gemv(&coupling, modal_position, &coupling_momentum, 1.0, 0.0) != 0 ||
        vector_add(&total_momentum, &coupling_momentum) != 0) return;
    momentum = (DpVec3){total_momentum.data[0], total_momentum.data[1],
                         total_momentum.data[2]};
    /* eb41..eb5f 明确以第 9 个参数 J 作为 C·C^T 扣减前的惯量基矩阵。 */
    config.rigid_matrices.base_inertia_3x3 = *inertia;
    config.angular_momentum = &momentum;
    config.minus_term = &minus;
    if (dp_flex_dynamics_step(&alpha, modal_acceleration, modal_velocity_output, &rate,
                              modal_position, modal_velocity, &config) != 0) return;
    /* 原公开入口尾部将 rcx 的 10 维描述符复制到 r8 输出；内部核的速度复制
     * 仅供其可重入接口使用，公开 ABI 在此以按位 memcpy 恢复真实副作用。 */
    memcpy(modal_velocity_output->data, modal_position->data,
           modal_position->count * sizeof(modal_position->data[0]));
    angular_acceleration->data[0] = alpha.x;
    angular_acceleration->data[1] = alpha.y;
    angular_acceleration->data[2] = alpha.z;
}

int dp_differential_equation_global_reset(void)
{
    if (dp_core_default_model_init(&dp_global_default_model) != 0) {
        dp_global_model_ready = 0;
        return -1;
    }

    memcpy(J_c_B_mem, dp_global_default_model.inertia, sizeof(J_c_B_mem));
    memcpy(J_c_B_inv_mem, dp_global_default_model.inverse, sizeof(J_c_B_inv_mem));
    J_c_B = (DpMatrix){3, 3, 3, 0, J_c_B_mem};
    J_c_B_inv = (DpMatrix){3, 3, 3, 0, J_c_B_inv_mem};
    H_w_B = (DpVector){3, 0, H_w_B_mem};
    L_c_B = (DpVector){3, 0, L_c_B_mem};
    memset(H_w_B_mem, 0, sizeof(H_w_B_mem));
    memset(L_c_B_mem, 0, sizeof(L_c_B_mem));
    memset(&Sat, 0, sizeof(Sat));
    memset(&SADA, 0, sizeof(SADA));
    memset(&dp_prior_magnetic_body, 0, sizeof(dp_prior_magnetic_body));
    dp_prior_magnetic_body_valid = 0u;
    dp_global_sada_reaction_enabled = 1u;
    dp_global_sat_model_is_sada_history = 0u;
    dp_global_default_sat_descriptors_sync();

    /* orbit_dynamic 的全局 ABI 由它自己的 backing storage 提供。 */
    SpacecraftMass = 1.0;
    F_I_external.count = 3;
    F_I_external.reserved_04 = 0;
    if (F_I_external.data != NULL) {
        F_I_external.data[0] = 0.0;
        F_I_external.data[1] = 0.0;
        F_I_external.data[2] = 0.0;
    }
    dp_global_model_ready = 1;
    return 0;
}

void differential_equation(double dydt[DP_STATE_DIM], const double y[DP_STATE_DIM])
{
    DpDynamicsContext context = {0};
    DpVec3 angular_momentum = {0.0, 0.0, 0.0};
    DpVec3 minus_term = {0.0, 0.0, 0.0};
    DpFlexSadaDrive sada_drive = {0};
    DpMatrix coupling;
    DpMatrix modal_a;
    DpMatrix modal_d;
    DpMatrix sada_command_momentum_map;
    DpMatrix sada_modal_pre_map;
    DpMatrix sada_modal_acceleration_map;
    DpMatrix observed_j_c_b_inv;

    if (dydt == NULL || y == NULL) {
        return;
    }
    if (dp_global_model_ready == 0 && dp_differential_equation_global_reset() != 0) {
        memset(dydt, 0, DP_STATE_DIM * sizeof(double));
        return;
    }

    coupling = dp_sat_matrix_load(DP_SAT_COUPLING_DESCRIPTOR_OFFSET);
    modal_a = dp_sat_matrix_load(DP_SAT_MODAL_A_DESCRIPTOR_OFFSET);
    modal_d = dp_sat_matrix_load(DP_SAT_MODAL_D_DESCRIPTOR_OFFSET);
    sada_command_momentum_map = dp_sat_matrix_load(DP_SAT_M9A0_DESCRIPTOR_OFFSET);
    sada_modal_pre_map = dp_sat_matrix_load(DP_SAT_M4A8_DESCRIPTOR_OFFSET);
    sada_modal_acceleration_map = dp_sat_matrix_load(DP_SAT_M688_DESCRIPTOR_OFFSET);

    /*
     * 原 dynamics_flex 接收 J_c_B_inv 作为第十个实际参数。当前已验收的
     * 可重入核按相同 J_c_B/C 输入显式重建有效惯量并求逆；保留 descriptor
     * 读取可使该全局 ABI 的对象观测边界完整，并为后续将该路径直接映射到
     * 原矩阵子程序保留单一来源。
     */
    observed_j_c_b_inv = J_c_B_inv;
    (void)observed_j_c_b_inv;

    if (H_w_B.data != NULL) {
        angular_momentum.x = H_w_B.data[0];
        angular_momentum.y = H_w_B.data[1];
        angular_momentum.z = H_w_B.data[2];
    }
    if (L_c_B.data != NULL) {
        /* dp_flex_compose_rigid_rhs_coupled 计算 -minus_term；原 dynamics_flex
         * 在叉乘之后形成 L_c_B - (omega×H)，故这里传入 -L_c_B。 */
        minus_term.x = -L_c_B.data[0];
        minus_term.y = -L_c_B.data[1];
        minus_term.z = -L_c_B.data[2];
    }
    sada_drive.command_angle[0] = SADA.command_angle[0];
    sada_drive.command_angle[1] = SADA.command_angle[1];
    sada_drive.angular_acceleration[0] = SADA.angular_acceleration[0];
    sada_drive.angular_acceleration[1] = SADA.angular_acceleration[1];

    context.flex.rigid_matrices.coupling_3xn = coupling;
    context.flex.rigid_matrices.base_inertia_3x3 = J_c_B;
    context.flex.modal_matrix_a_nxn = &modal_a;
    context.flex.modal_matrix_d_nxn = &modal_d;
    context.flex.angular_momentum = &angular_momentum;
    context.flex.minus_term = &minus_term;
    context.flex.rigid_map_3x3 = NULL;
    context.flex.rigid_aux = NULL;
    context.flex.sada_drive = &sada_drive;
    context.flex.sada_command_momentum_map_3x3 = &sada_command_momentum_map;
    context.flex.sada_acceleration_reaction_map_3x3 =
        dp_global_sada_reaction_enabled != 0u
            ? &dp_global_default_model.sada_acceleration_reaction_map_3x3
            : NULL;
    context.flex.sada_modal_pre_map_3x3 = &sada_modal_pre_map;
    context.flex.sada_modal_acceleration_map_3xn = &sada_modal_acceleration_map;

    if (dp_differential_equation_33(dydt, y, &context) != 0) {
        memset(dydt, 0, DP_STATE_DIM * sizeof(double));
    }
#ifdef DP_DIAGNOSTIC_SADA_STAGE_TRACE
    if (dp_sada_stage_trace_count < 8u) {
        FILE *trace = fopen("/tmp/dp_sada_rk4_trace.bin",
                            dp_sada_stage_trace_count == 0u ? "wb" : "ab");
        if (trace != NULL) {
            (void)fwrite(y, sizeof(y[0]), DP_STATE_DIM, trace);
            (void)fwrite(dydt, sizeof(dydt[0]), DP_STATE_DIM, trace);
            (void)fclose(trace);
        }
        ++dp_sada_stage_trace_count;
    }
#endif
}

static void dp_global_rk4_derivative(double stage_time,
                                     const double state[DP_STATE_DIM],
                                     double derivative[DP_STATE_DIM],
                                     void *opaque)
{
    (void)stage_time;
    (void)opaque;
    differential_equation(derivative, state);
}

/* 原 y_q_unit（0x5da0）：四次直接除法；禁止替换为倒数乘法。
 * 原 ELF 在范数严格小于 0.01 时调用 __printf_chk；格式串没有换行，
 * 且 q0..q3 均在第一次写回前传给可变参数调用。 */
void y_q_unit(double state[DP_STATE_DIM])
{
    double norm_squared;
    double norm;

    norm_squared = state[0] * state[0];
    norm_squared += state[1] * state[1];
    norm_squared += state[2] * state[2];
    norm_squared += state[3] * state[3];
    norm = sqrt(norm_squared);
    if (norm < 0.01) {
        (void)printf("norm = %f, q0 = %f,q1= %f,q2= %f,q3= %f", norm,
                     state[0], state[1], state[2], state[3]);
    }
    state[0] /= norm;
    state[1] /= norm;
    state[2] /= norm;
    state[3] /= norm;
}

void RK4_Intergrator(double step)
{
    double next_time = t;

    /*
     * `dp_rk4_step_33` 保留原 h*k1..h*k4 与加权舍入顺序。使用局部时间槽
     * 使全局 t 在四元数归一化结束前仍保持原值，匹配 0x62f8..0x6321 的
     * 可观察存储顺序。
     */
    dp_rk4_step_33(y, &next_time, step, dp_global_rk4_derivative, NULL);
    y_q_unit(y);
    t = next_time;
}

/*
 * 原 DynamicInit（0x5ba0）经 dyn_init 的设备初始化包装进入。当前全局恢复
 * 模型以 reset 建立与 SatParaInit/TorqueInit 对应的已映射 backing 与零力矩，
 * 随后严格按可观察写入顺序设置时间、积分状态、惯量及质量。
 */
void DynamicInit(const DpInitialConditions *initial)
{
    double position_gci[3] = {0.0, 0.0, 0.0};
    double velocity_gci[3] = {0.0, 0.0, 0.0};
    DpSadaRecovered sada_before;

    /* 原入口直接解引用 rdi；故刻意不把 NULL 变为额外的成功返回路径。 */
    sada_before = SADA;
    (void)dp_differential_equation_global_reset();
    /* 原函数不写 SADA；它由 dyn_init 的设备初始化包装层建立。 */
    SADA = sada_before;

    step_time = initial->step_time;
    TimeInit(initial->time_parameters[0], initial->time_parameters[1],
             initial->time_parameters[2], initial->time_parameters[3],
             initial->time_parameters[4], initial->time_parameters[5]);
    Elements2PosVel_M(position_gci, velocity_gci,
                      initial->orbit_elements[0], initial->orbit_elements[1],
                      initial->orbit_elements[2], initial->orbit_elements[3],
                      initial->orbit_elements[4], initial->orbit_elements[5]);

    /* intergrator_init: t=0; q0=+0x08; qv=+0x10..+0x20; omega 由三个 float 转 double。 */
    t = 0.0;
    y[0] = initial->initial_time_or_epoch;
    y[1] = initial->initial_attitude_vector.x;
    y[2] = initial->initial_attitude_vector.y;
    y[3] = initial->initial_attitude_vector.z;
    y[4] = (double)initial->initial_angular_rate_f32[0];
    y[5] = (double)initial->initial_angular_rate_f32[1];
    y[6] = (double)initial->initial_angular_rate_f32[2];
    y[7] = position_gci[0];
    y[8] = position_gci[1];
    y[9] = position_gci[2];
    y[10] = velocity_gci[0];
    y[11] = velocity_gci[1];
    y[12] = velocity_gci[2];
    memset(&y[13], 0, (DP_STATE_DIM - 13u) * sizeof(y[0]));

    /* SetInertiaTensor 与 SetSatInertiaTensor 的共享可观测结果。 */
    memcpy(J_c_B_mem, initial->inertia_tensor, sizeof(J_c_B_mem));
    inv_CAL_M3(J_c_B_mem, J_c_B_inv_mem);
    SpacecraftMass = initial->spacecraft_mass;
}

/*
 * 已基于三组隔离 CoreDynamic 原 ELF 快照验证的同名全局 ABI。
 * 验证边界记录在 `analysis/elf_c_diff/core_dynamic_global_abi_checkpoint.md`；
 * 未被这些快照覆盖的输入空间仍需以新增原 ELF 金标扩展验证。
 */
static void dp_core_store_doubles(void *base, size_t offset,
                                  const double *values, size_t count)
{
    memcpy((unsigned char *)base + offset, values, count * sizeof(*values));
}

static void dp_core_body_to_inertial(DpMatrix *out_3x3)
{
    double q_xyz_data[3] = {y[1], y[2], y[3]};
    double conjugate_xyz_data[3] = {0.0, 0.0, 0.0};
    DpQuatAbi attitude = {y[0], {3, 0, q_xyz_data}};
    DpQuatAbi conjugate = {0.0, {3, 0, conjugate_xyz_data}};

    quat_conjugate(&conjugate, &attitude);
    quat_att_mat(out_3x3, &conjugate);
}

static void dp_core_gci_to_body(DpMatrix *out_3x3)
{
    double q_xyz_data[3] = {y[1], y[2], y[3]};
    DpQuatAbi attitude = {y[0], {3, 0, q_xyz_data}};

    quat_att_mat(out_3x3, &attitude);
}

void CoreDynamic(void *core_dynamic_output, const void *core_dyn_input)
{
#ifdef DP_DIAGNOSTIC_RK4_STAGE_TRACE
    static unsigned long dp_rk4_trace_call_index = 0u;
#endif
    const DpCoreDynInput *input = core_dyn_input;
    double pre_state[DP_STATE_DIM];
    double body_to_inertial_data[9] = {0.0};
    double gci_to_body_data[9] = {0.0};
    double external_body_data[3];
    double external_gci_data[3] = {0.0, 0.0, 0.0};
    double position_data[3];
    double velocity_data[3];
    double magnetic_gci_data[3] = {0.0, 0.0, 0.0};
    double magnetic_body_data[3] = {0.0, 0.0, 0.0};
    double total_torque_data[3];
    double gravity_torque_data[3] = {0.0, 0.0, 0.0};
    double magnetic_torque_data[3] = {0.0, 0.0, 0.0};
    double inertia_tail[6];
    DpCalendarTime calendar;
    DpMatrix body_to_inertial = {3, 3, 3, 0, body_to_inertial_data};
    DpMatrix gci_to_body = {3, 3, 3, 0, gci_to_body_data};
    DpVector external_body = {3, 0, external_body_data};
    DpVector external_gci = {3, 0, external_gci_data};
    DpVector position = {3, 0, position_data};
    DpVector velocity = {3, 0, velocity_data};
    DpVector magnetic_gci = {3, 0, magnetic_gci_data};
    DpVector magnetic_body = {3, 0, magnetic_body_data};
    DpVec3 wheel_torque;
    DpVec3 thruster_torque;
    DpVec3 gravity_torque;
    DpVec3 magnetic_torque;
    DpCoreEnvironmentOutputs environment;

    if (core_dynamic_output == NULL || input == NULL) {
        return;
    }
    if (dp_global_model_ready == 0 && dp_differential_equation_global_reset() != 0) {
        return;
    }

    memcpy(pre_state, y, sizeof(pre_state));
    TimeAdd(step_time);
    TimeArrayGet((double *)&calendar);

    external_body_data[0] = input->thruster_vector_0.x;
    external_body_data[1] = input->thruster_vector_0.y;
    external_body_data[2] = input->thruster_vector_0.z;
    dp_core_body_to_inertial(&body_to_inertial);
    (void)blas_gemv(&body_to_inertial, &external_body, &external_gci, 1.0, 0.0);
    if (F_I_external.data != NULL) {
        F_I_external.data[0] = external_gci_data[0];
        F_I_external.data[1] = external_gci_data[1];
        F_I_external.data[2] = external_gci_data[2];
    }

    H_w_B_mem[0] = input->wheel_group_vector_1.x;
    H_w_B_mem[1] = input->wheel_group_vector_1.y;
    H_w_B_mem[2] = input->wheel_group_vector_1.z;
    /* CoreDynamic 输出尾部读取 +0x78；刚柔 RHS 的驱动目标由 SADA 全局对象提供。 */

    wheel_torque = input->wheel_group_vector_0;
    thruster_torque = input->thruster_vector_1;
    total_torque_data[0] = -wheel_torque.x + thruster_torque.x;
    total_torque_data[1] = -wheel_torque.y + thruster_torque.y;
    total_torque_data[2] = -wheel_torque.z + thruster_torque.z;

    position_data[0] = y[7];
    position_data[1] = y[8];
    position_data[2] = y[9];
    velocity_data[0] = y[10];
    velocity_data[1] = y[11];
    velocity_data[2] = y[12];
    dp_core_gci_to_body(&gci_to_body);
    gravity_torque.x = 0.0;
    gravity_torque.y = 0.0;
    gravity_torque.z = 0.0;
    /* 临时诊断：当前受控 Sat+0x1a8 重力梯度第三矩阵为零；不可用 J_c_B 代替。 */
    if (0) {
        gravity_torque_data[0] = gravity_torque.x;
        gravity_torque_data[1] = gravity_torque.y;
        gravity_torque_data[2] = gravity_torque.z;
        total_torque_data[0] += gravity_torque_data[0];
        total_torque_data[1] += gravity_torque_data[1];
        total_torque_data[2] += gravity_torque_data[2];
    }
    (void)dp_calc_inertial_magnetic_vector(&magnetic_gci, &position, &velocity, &calendar);
    (void)blas_gemv(&gci_to_body, &magnetic_gci, &magnetic_body, 1.0, 0.0);
    if (dp_prior_magnetic_body_valid != 0u) {
        const DpVec3 magnetic_moment = input->magnetic_torque_command;
        (void)dp_mag_torque(&magnetic_torque, &magnetic_moment, &dp_prior_magnetic_body);
        magnetic_torque_data[0] = magnetic_torque.x;
        magnetic_torque_data[1] = magnetic_torque.y;
        magnetic_torque_data[2] = magnetic_torque.z;
        /* SatTorque 将 MagTorque 直接累加到刚体力矩项。 */
        total_torque_data[0] += magnetic_torque_data[0];
        total_torque_data[1] += magnetic_torque_data[1];
        total_torque_data[2] += magnetic_torque_data[2];
    }
    L_c_B_mem[0] = total_torque_data[0];
    L_c_B_mem[1] = total_torque_data[1];
    L_c_B_mem[2] = total_torque_data[2];
#ifdef DP_DIAGNOSTIC_RK4_STAGE_TRACE
    if (dp_rk4_trace_call_index == 102u) {
        FILE *trace = fopen("/tmp/dp_h0_step103_pre_rk4_globals.bin", "wb");
        if (trace != NULL) {
            (void)fwrite(H_w_B_mem, sizeof(H_w_B_mem[0]), 3u, trace);
            (void)fwrite(L_c_B_mem, sizeof(L_c_B_mem[0]), 3u, trace);
            (void)fwrite(F_I_external.data, sizeof(F_I_external.data[0]), 3u, trace);
            (void)fwrite(J_c_B_mem, sizeof(J_c_B_mem[0]), 9u, trace);
            (void)fwrite(J_c_B_inv_mem, sizeof(J_c_B_inv_mem[0]), 9u, trace);
            (void)fclose(trace);
        }
    }
    ++dp_rk4_trace_call_index;
#endif

    RK4_Intergrator(step_time);
    (void)dp_core_environment_from_states(&environment, pre_state, y, &calendar);
    dp_prior_magnetic_body = environment.device_environment.magnetic_body;
    dp_prior_magnetic_body_valid = 1u;

    memset(core_dynamic_output, 0, 0x148u);
    dp_core_store_doubles(core_dynamic_output, 0x000u, y, 7u);
    dp_core_store_doubles(core_dynamic_output, 0x038u,
                          (const double *)&environment.device_environment.sun_body, 3u);
    dp_core_store_doubles(core_dynamic_output, 0x050u,
                          (const double *)&environment.device_environment.sun_gci, 3u);
    dp_core_store_doubles(core_dynamic_output, 0x068u,
                          (const double *)&environment.device_environment.magnetic_body, 3u);
    dp_core_store_doubles(core_dynamic_output, 0x080u,
                          (const double *)&environment.magnetic_gci, 3u);
    dp_core_store_doubles(core_dynamic_output, 0x098u, &y[7], 6u);
    dp_core_store_doubles(core_dynamic_output, 0x0c8u,
                          environment.device_environment.time_values, 6u);
    *((double *)((unsigned char *)core_dynamic_output + 0x0f8u)) = TimeTotalGet();
    inertia_tail[0] = J_c_B_mem[0];
    inertia_tail[1] = J_c_B_mem[4];
    inertia_tail[2] = J_c_B_mem[8];
    inertia_tail[3] = J_c_B_mem[1];
    inertia_tail[4] = J_c_B_mem[2];
    inertia_tail[5] = J_c_B_mem[5];
    dp_core_store_doubles(core_dynamic_output, 0x100u, inertia_tail, 3u);
    dp_core_store_doubles(core_dynamic_output, 0x118u, &inertia_tail[3], 3u);
    dp_core_store_doubles(core_dynamic_output, 0x130u,
                          (const double *)&input->sada_angle_or_rate, 3u);
}

void UpdateSatellite(void)
{
    /* 原 ELF 0xc5a0: f3 c3 (repz ret)。 */
}
