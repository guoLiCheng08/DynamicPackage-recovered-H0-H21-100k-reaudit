/* UpdateCoreDynInput 的直接内存拷贝部分（高可信恢复）。 */
#include "dynamic_core_layout.h"
#include "dynamic_ipc_telemetry.h"

#include <stdint.h>
#include <string.h>

/* 原 getDynInput：rdi/rsi 在函数体中未读取；rdx 指向至少 0x74 字节的动态
 * 命令帧。正常共享输入路径将 3 个 u8 零扩展为 dword，12 个 float 逐项提升为
 * double，并分别写入 +0x00、+0x08..+0x50、+0x58、+0x60/+0x68、+0x70。 */
void getDynInput(void *reserved_0, void *reserved_1, void *dynamic_input)
{
    unsigned char *out = dynamic_input;
    uint8_t byte_value;
    float float_value;
    uint32_t widened_byte;
    unsigned index;

    (void)reserved_0;
    (void)reserved_1;
    (void)get_uint8_value(0, &byte_value);
    widened_byte = byte_value;
    memcpy(out + 0x00u, &widened_byte, sizeof(widened_byte));
    for (index = 0u; index < 4u; ++index) {
        double value;

        (void)get_float_value((int32_t)index, &float_value);
        value = (double)float_value;
        memcpy(out + 0x08u + index * sizeof(value), &value, sizeof(value));
    }
    for (index = 0u; index < 6u; ++index) {
        double value;

        (void)get_float_value((int32_t)(4u + index), &float_value);
        value = (double)float_value;
        memcpy(out + 0x28u + index * sizeof(value), &value, sizeof(value));
    }
    (void)get_uint8_value(1, &byte_value);
    widened_byte = byte_value;
    memcpy(out + 0x58u, &widened_byte, sizeof(widened_byte));
    (void)get_float_value(10, &float_value);
    {
        const double value = (double)float_value;
        memcpy(out + 0x60u, &value, sizeof(value));
    }
    (void)get_float_value(11, &float_value);
    {
        const double value = (double)float_value;
        memcpy(out + 0x68u, &value, sizeof(value));
    }
    (void)get_uint8_value(2, &byte_value);
    widened_byte = byte_value;
    memcpy(out + 0x70u, &widened_byte, sizeof(widened_byte));
}

/*
 * 该快照由各设备模块提供；字段顺序严格对应原函数读取的 WheelGroup、
 * MTQ_Group、Thruster 和 getSADAangle 输出。全局设备对象的完整布局尚在恢复中，
 * 因而将“取数”与“打包”解耦，避免猜测全局对象内的未知字节。
 */
typedef struct {
    DpVec3 wheel_group_at_30;
    DpVec3 wheel_group_at_08;
    DpVec3 mtq_group_at_08;
    DpVec3 thruster_at_68;
    DpVec3 thruster_at_90;
    DpVec3 sada_angle;
} DpCoreInputSourceSnapshot;

void dp_update_core_dyn_input_from_snapshot(DpCoreDynInput *out,
                                            const DpCoreInputSourceSnapshot *source)
{
    /* UpdateCoreDynInput: source +0x30 -> out +0x00; source +0x08 -> out +0x18. */
    memcpy(&out->wheel_group_vector_0, &source->wheel_group_at_30, sizeof(DpVec3));
    memcpy(&out->wheel_group_vector_1, &source->wheel_group_at_08, sizeof(DpVec3));
    memcpy(&out->magnetic_torque_command, &source->mtq_group_at_08, sizeof(DpVec3));
    memcpy(&out->thruster_vector_0, &source->thruster_at_68, sizeof(DpVec3));
    memcpy(&out->thruster_vector_1, &source->thruster_at_90, sizeof(DpVec3));
    memcpy(&out->sada_angle_or_rate, &source->sada_angle, sizeof(DpVec3));
}
