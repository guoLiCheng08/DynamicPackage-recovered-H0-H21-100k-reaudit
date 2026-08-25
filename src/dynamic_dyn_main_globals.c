#include "dynamic_recovered.h"
#include "dynamic_satellite_globals.h"
#include "dynamic_telemetry_layout.h"

#include <stdint.h>
#include <string.h>

static int32_t dp_dyn_main_array_i32_at(const unsigned char *source, size_t offset)
{
    int32_t value;

    memcpy(&value, source + offset, sizeof(value));
    return value;
}

static void dp_dyn_main_array_store_double(unsigned char *destination, size_t offset,
                                           double value)
{
    memcpy(destination + offset, &value, sizeof(value));
}

void dyn_main(void *main_output, void *core_dynamic_output, const void *device_command)
{
    unsigned char core_dynamic_input[0xa0];

    if (main_output == NULL || core_dynamic_output == NULL || device_command == NULL) {
        return;
    }
    memset(core_dynamic_input, 0, sizeof(core_dynamic_input));

    /* dyn_main 对设备控制入口采用 0.01 秒采样尺度；独立 UpdateDeviceControl
     * 调用仍按其传入 scale 直接驱动 SADA。 */
    UpdateDeviceControl(device_command, 0.01);
    /* 端到端原 ELF快照表明 CoreDynamic 从 DynamicInit 建立的全局 y 积分，
     * rsi 缓冲区由调用返回时覆盖为输出；不以前置缓冲区改写全局积分状态。 */
    UpdateCoreDynInput(core_dynamic_input);
    CoreDynamic(core_dynamic_output, core_dynamic_input);
    UpdateDeviceMeasure(core_dynamic_output);
    UpdateMainOut(main_output, core_dynamic_output);
}

/* 原 dyn_main_array：rdi=0x1e8 旧式输出，rsi=核心输出接收区，rdx=至少 16 个
 * double 的旧式命令数组。它在栈上重排 0x78 设备命令、调用 dyn_main，并将
 * 0x220 主遥测和 0x148 核心对象按固定洞孔/整数提升规则展开。 */
void dyn_main_array(double *legacy_out, double *legacy_state, const double *legacy_command)
{
    unsigned char core_output[0x148];
    DpMainTelemetryFrame main_output;
    DpDeviceControlCommand command;
    unsigned char *out = (unsigned char *)legacy_out;
    unsigned char *state = (unsigned char *)legacy_state;
    const unsigned char *main_raw = main_output.raw;
    unsigned index;

    memcpy(command.wheel_torque_command, legacy_command + 0, 4u * sizeof(double));
    memcpy(command.mtq_moment_command, legacy_command + 4, 6u * sizeof(double));
    command.sada_command_flag = (uint32_t)(int32_t)legacy_command[10];
    command.sada_command_angle[0] = legacy_command[11];
    command.sada_command_angle[1] = legacy_command[12];
    command.thruster_work_status = (uint32_t)(int32_t)legacy_command[14];
    command.inertia_update_flag = (uint32_t)(int32_t)legacy_command[15];

    dyn_main(&main_output, core_output, &command);
    memcpy(state, core_output, 0x130u);

    memcpy(out, main_raw, 0x60u);
    dp_dyn_main_array_store_double(out, 0x60u,
                                   (double)dp_dyn_main_array_i32_at(main_raw, 0x60u));
    dp_dyn_main_array_store_double(out, 0x68u,
                                   (double)dp_dyn_main_array_i32_at(main_raw, 0x64u));
    dp_dyn_main_array_store_double(out, 0x70u,
                                   (double)dp_dyn_main_array_i32_at(main_raw, 0x68u));
    for (index = 0u; index < 10u; ++index) {
        memcpy(out + 0x78u + index * sizeof(double),
               main_raw + 0x70u + index * sizeof(double), sizeof(double));
    }
    dp_dyn_main_array_store_double(out, 0xc8u,
                                   (double)dp_dyn_main_array_i32_at(main_raw, 0xc0u));
    dp_dyn_main_array_store_double(out, 0xd0u,
                                   (double)dp_dyn_main_array_i32_at(main_raw, 0xc4u));
    for (index = 0u; index < 10u; ++index) {
        memcpy(out + 0xd8u + index * sizeof(double),
               main_raw + 0xc8u + index * sizeof(double), sizeof(double));
    }
    for (index = 0u; index < 6u; ++index) {
        static const size_t main_offsets[6] = {0x138u, 0x140u, 0x148u,
                                               0x150u, 0x158u, 0x160u};
        memcpy(out + 0x128u + index * sizeof(double), main_raw + main_offsets[index],
               sizeof(double));
    }
    for (index = 0u; index < 6u; ++index) {
        dp_dyn_main_array_store_double(out, 0x158u + index * sizeof(double),
                                       (double)dp_dyn_main_array_i32_at(main_raw,
                                                                        0x168u + index * 4u));
        memcpy(out + 0x188u + index * sizeof(double), main_raw + 0x180u + index * sizeof(double),
               sizeof(double));
        memcpy(out + 0x1b8u + index * sizeof(double), main_raw + 0x1b0u + index * sizeof(double),
               sizeof(double));
    }
}
