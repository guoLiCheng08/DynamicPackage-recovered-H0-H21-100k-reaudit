/* DynamicDllInit：共享输入不可读时的默认启动分支恢复。 */
#include "dynamic_devices.h"
#include "dynamic_sensors.h"
#include "dynamic_satellite_globals.h"
#include "dynamic_default_sensor_config.h"

#include <stdint.h>
#include <string.h>

/*
 * 原 ELF 0x17c0 无显式参数。此最小恢复仅覆盖新进程、共享输入尚不可读取
 * 时的默认启动分支：构造后续入口使用的 0xe8 初始条件块，并按 dyn_init
 * 的可观察设备结果建立零群组 backing 与 SADA 静态默认值。
 * 共享输入 64/65/66 成功返回时的分支、诊断 stdout 与传感器内部对象仍需
 * 独立原 ELF 金标后再扩大范围。
 */
static DpInitialConditions dp_dynamic_dll_initial;

const DpInitialConditions *dp_dynamic_dll_initial_conditions(void)
{
    return &dp_dynamic_dll_initial;
}

static void dp_load_default_sada_config(void)
{
    memset(&SADA, 0, sizeof(SADA));
    SADA.current_angle[0] = 3.14159265358979323846;
    SADA.current_angle[1] = 3.14159265358979323846;
    SADA.command_limit[0] = 0.003490658503988659;
    SADA.command_limit[1] = 0.001308996938995747;
    SADA.acceleration_limit[0] = 1.7453292519943296e-05;
    SADA.acceleration_limit[1] = 8.726646259971648e-06;
}

/* H28 dyn_init_array 原 ELF 设备快照：前三个 MTQ 通道限幅为 400；
 * Thruster 的静态标定和对象内三维向量也在执行第一帧命令前已就绪。 */
static void dp_load_dyn_init_command_device_defaults(void)
{
    static const uint64_t force_scale_bits = UINT64_C(0x3f2d8b2b41cd29ea);
    static const uint64_t lever_z_bits = UINT64_C(0x3f847ae147ae147b);
    unsigned index;

    for (index = 0u; index < 3u; ++index) {
        MTQ[index].moment_limit = 400.0;
        MTQ[index].installation_axis.count = 3;
        MTQ[index].installation_axis.reserved_04 = 0;
        MTQ[index].installation_axis.data =
            (double *)((unsigned char *)&MTQ[index] + 0x20u);
        MTQ[index].installation_axis.data[0] = index < 2u ? 1.0 : 0.0;
        MTQ[index].installation_axis.data[1] = index == 2u ? 1.0 : 0.0;
        MTQ[index].installation_axis.data[2] = 0.0;
    }
    memcpy(&Thruster.force_scale, &force_scale_bits, sizeof(Thruster.force_scale));
    if (Thruster.lever_arm.data != NULL && Thruster.force_input.data != NULL) {
        Thruster.lever_arm.data[0] = 0.0;
        Thruster.lever_arm.data[1] = 0.0;
        memcpy(&Thruster.lever_arm.data[2], &lever_z_bits, sizeof(double));
        Thruster.force_input.data[0] = 0.0;
        Thruster.force_input.data[1] = -1.0;
        Thruster.force_input.data[2] = 0.0;
    }
}

void DynamicDllInit(void)
{
    static const double inertia[9] = {
        120.0, -0.63, 1.3,
        -0.63, 361.0, -0.5,
        1.3, -0.5, 469.0
    };

    memset(&dp_dynamic_dll_initial, 0, sizeof(dp_dynamic_dll_initial));
    dp_dynamic_dll_initial.step_time = 0.01;
    dp_dynamic_dll_initial.initial_time_or_epoch = 1.0;
    dp_dynamic_dll_initial.time_parameters[0] = 2024.0;
    dp_dynamic_dll_initial.time_parameters[1] = 10.0;
    dp_dynamic_dll_initial.time_parameters[2] = 28.0;
    dp_dynamic_dll_initial.time_parameters[3] = 4.0;
    dp_dynamic_dll_initial.time_parameters[4] = 16.0;
    dp_dynamic_dll_initial.time_parameters[5] = 16.0;
    dp_dynamic_dll_initial.orbit_elements[0] = 6910593.0;
    dp_dynamic_dll_initial.orbit_elements[1] = 0.00058;
    dp_dynamic_dll_initial.orbit_elements[2] = 1.7018327148814874;
    dp_dynamic_dll_initial.orbit_elements[3] = 0.24732846154513649;
    dp_dynamic_dll_initial.orbit_elements[4] = 3.0651957525056472;
    dp_dynamic_dll_initial.orbit_elements[5] = 6.2273473025797976;
    memcpy(dp_dynamic_dll_initial.inertia_tensor, inertia,
           sizeof(dp_dynamic_dll_initial.inertia_tensor));
    dp_dynamic_dll_initial.spacecraft_mass = 600.0;

    dp_device_globals_reset();
    dp_load_default_sada_config();
}

static void dp_relocate_default_rwheel(void)
{
    unsigned index;

    for (index = 0u; index < DP_WHEEL_COUNT; ++index) {
        RWheel[index].installation_axis.data =
            (double *)((unsigned char *)&RWheel[index] + 0x60u);
    }
}

static void dp_load_default_sensor_config(void)
{
    _Static_assert(sizeof(STS) == sizeof(dp_default_sts_bytes), "STS snapshot size");
    _Static_assert(sizeof(Gyro) == sizeof(dp_default_gyro_bytes), "Gyro snapshot size");
    _Static_assert(sizeof(DSS) == sizeof(dp_default_dss_bytes), "DSS snapshot size");
    _Static_assert(sizeof(MagMeter) == sizeof(dp_default_magmeter_bytes), "MagMeter snapshot size");
    _Static_assert(sizeof(RWheel) == sizeof(dp_default_rwheel_bytes), "RWheel snapshot size");
    memcpy(STS, dp_default_sts_bytes, sizeof(STS));
    memcpy(Gyro, dp_default_gyro_bytes, sizeof(Gyro));
    memcpy(DSS, dp_default_dss_bytes, sizeof(DSS));
    memcpy(MagMeter, dp_default_magmeter_bytes, sizeof(MagMeter));
    memcpy(RWheel, dp_default_rwheel_bytes, sizeof(RWheel));
    dp_relocate_default_rwheel();
}

static void dp_sync_device_measure_from_defaults(void)
{
    dp_device_measure_globals_reset();
    memcpy(DeviceMeasure.sts, STS, sizeof(DeviceMeasure.sts));
    memcpy(DeviceMeasure.gyro, Gyro, sizeof(DeviceMeasure.gyro));
    memcpy(DeviceMeasure.dss, DSS, sizeof(DeviceMeasure.dss));
    memcpy(DeviceMeasure.magmeter, MagMeter, sizeof(DeviceMeasure.magmeter));
    dp_device_measure_globals_relocate();
}

/* 原 dyn_init：七个设备初始化器按固定顺序执行，随后尾调用 DynamicInit。 */
void dyn_init(const DpInitialConditions *initial)
{
    dp_load_default_sensor_config();
    /* H28 原 ELF dyn_init_array 双采集表明，该入口在 DynamicInit 前也装入
     * 与 DynamicDllInit 相同的 SADA 静态角度及限幅；遗漏会使首个 SADA 命令分叉。 */
    dp_load_default_sada_config();
    STS_Init();
    Gyro_Init();
    DSS_Init();
    MagMeter_Init();
    Wheel_Init();
    MagTorque_Init();
    Thruster_Init();
    dp_load_dyn_init_command_device_defaults();
    MagTorque_Init();
    DynamicInit(initial);

    /* DynamicInit 的恢复端复用全局导数 reset，会清除设备 descriptor backing；
     * 原 dyn_init 的可观察最终状态保留前置安装结果，故在该恢复专用 reset 后
     * 先恢复设备群组 backing，再重装相同设备布局，不改变 y/J/质量结果。 */
    dp_device_globals_reset();
    memcpy(RWheel, dp_default_rwheel_bytes, sizeof(RWheel));
    dp_relocate_default_rwheel();
    STS_Init();
    Gyro_Init();
    DSS_Init();
    MagMeter_Init();
    Wheel_Init();
    MagTorque_Init();
    Thruster_Init();
    dp_load_dyn_init_command_device_defaults();
    MagTorque_Init();
    dp_sync_device_measure_from_defaults();
}

/*
 * 原 ELF ABI：rdi 指向 30 个 double 的旧式初始化数组。其适配规则是
 * [0]=step, [1..4]=q, [5..7] 先转为 float, [8..13]=time,
 * [14..19]=orbit, [20..28]=inertia, [29]=mass，随后调用 dyn_init。
 */
void dyn_init_array(const double initial_array[30])
{
    DpInitialConditions initial;
    size_t i;

    memset(&initial, 0, sizeof(initial));
    initial.step_time = initial_array[0];
    initial.initial_time_or_epoch = initial_array[1];
    initial.initial_attitude_vector.x = initial_array[2];
    initial.initial_attitude_vector.y = initial_array[3];
    initial.initial_attitude_vector.z = initial_array[4];
    initial.initial_angular_rate_f32[0] = (float)initial_array[5];
    initial.initial_angular_rate_f32[1] = (float)initial_array[6];
    initial.initial_angular_rate_f32[2] = (float)initial_array[7];
    for (i = 0; i < 6; ++i) {
        initial.time_parameters[i] = initial_array[8 + i];
        initial.orbit_elements[i] = initial_array[14 + i];
    }
    for (i = 0; i < 9; ++i) {
        initial.inertia_tensor[i] = initial_array[20 + i];
    }
    initial.spacecraft_mass = initial_array[29];
    dyn_init(&initial);
}

/* getDeskCommand（共享输入索引 64/65/66）写入原 init +0x28..+0x30。 */
void dp_dynamic_dll_initial_set_angular_rate_for_ipc(float x, float y, float z)
{
    dp_dynamic_dll_initial.initial_angular_rate_f32[0] = x;
    dp_dynamic_dll_initial.initial_angular_rate_f32[1] = y;
    dp_dynamic_dll_initial.initial_angular_rate_f32[2] = z;
}
