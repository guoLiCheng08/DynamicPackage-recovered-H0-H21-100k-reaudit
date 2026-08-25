/* 原 ELF 命令入口：0x1ac0..0x1bb1。 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define DP_COMMAND_FRAME_BYTES 0x90u
#define DP_TC_COMMAND_BYTES 0x90u

/* 原 tc_cmd 仅在 Command_Execute 中观察到 +0x88..+0x8f。 */
static unsigned char dp_tc_command[DP_TC_COMMAND_BYTES];

static uint32_t dp_read_u32(const unsigned char *base, size_t offset)
{
    uint32_t value;
    memcpy(&value, base + offset, sizeof(value));
    return value;
}

static void dp_write_u16(unsigned char *base, size_t offset, uint16_t value)
{
    memcpy(base + offset, &value, sizeof(value));
}

static void dp_write_u32(unsigned char *base, size_t offset, uint32_t value)
{
    memcpy(base + offset, &value, sizeof(value));
}

static void dp_write_u64(unsigned char *base, size_t offset, uint64_t value)
{
    memcpy(base + offset, &value, sizeof(value));
}

/* 原 ELF 0x1ac0：从 raw[4..10] 构造供 Command_Execute 使用的字段。 */
void Analyze_Command(void *command_frame)
{
    unsigned char *frame = command_frame;
    uint64_t command_id =
        (uint64_t)frame[4] |
        ((uint64_t)frame[5] << 8) |
        ((uint64_t)frame[6] << 16) |
        ((uint64_t)frame[7] << 24);

    dp_write_u64(frame, 0x68, command_id);
    dp_write_u32(frame, 0x70, frame[8]);
    dp_write_u32(frame, 0x74, frame[9]);
    dp_write_u16(frame, 0x78, frame[10]);
    {
        const unsigned char *payload = frame + 0x0b;
        memcpy(frame + 0x80, &payload, sizeof(payload));
    }
}

/* 原 ELF 0x1b50：0/0x77 更新 tc_cmd +0x88/+0x8a；0x66 更新 +0x8c/+0x8e。 */
void Command_Execute(void *command_frame)
{
    const unsigned char *frame = command_frame;
    const uint32_t opcode = dp_read_u32(frame, 0x70);
    const uint32_t parameter = dp_read_u32(frame, 0x74);
    uint16_t count;

    if (opcode == 0u || opcode == 0x77u) {
        dp_tc_command[0x88] = (unsigned char)parameter;
        memcpy(&count, dp_tc_command + 0x8a, sizeof(count));
        dp_write_u16(dp_tc_command, 0x8a, (uint16_t)(count + 1u));
    } else if (opcode == 0x66u) {
        dp_tc_command[0x8c] = (unsigned char)parameter;
        memcpy(&count, dp_tc_command + 0x8e, sizeof(count));
        dp_write_u16(dp_tc_command, 0x8e, (uint16_t)(count + 1u));
    }
}

/* 原 ELF 0x1ba0 与 0x1bb0：仅 repz ret，无参数且无可观察写入。 */
void Algorithm_Command_Execute(void)
{
}

void DYN_Command_Execute(void)
{
}

/* 仅供比较器重置和读取原 tc_cmd 的已观察 0x90 字节区域。 */
void dp_command_tc_reset_for_test(unsigned char fill)
{
    memset(dp_tc_command, fill, sizeof(dp_tc_command));
}

const unsigned char *dp_command_tc_bytes_for_test(void)
{
    return dp_tc_command;
}

#include "dynamic_ipc_telemetry.h"
#include "dynamic_satellite_globals.h"

/* 原全局：Antenna/solar/Test/RW 四个相邻 byte，以及至少 0x70 字节 DRC_DATA。 */
static unsigned char dp_desk_flags[4];
static unsigned char dp_drc_data[0x90];

void getDeskCommand(void)
{
    float x;
    float y;
    float z;

    (void)get_uint8_value(0x14, &dp_desk_flags[3]);
    (void)get_uint8_value(0x15, &dp_desk_flags[2]);
    (void)get_uint8_value(0x16, &dp_desk_flags[1]);
    (void)get_uint8_value(0x17, &dp_desk_flags[0]);
    (void)get_float_value(0x40, &x);
    (void)get_float_value(0x41, &y);
    (void)get_float_value(0x42, &z);
    dp_dynamic_dll_initial_set_angular_rate_for_ipc(x, y, z);

    (void)get_uint8_value(0x18, &dp_drc_data[0x58]);
    (void)get_uint8_value(0x19, &dp_drc_data[0x5c]);
    (void)get_uint8_value(0x1a, &dp_drc_data[0x60]);
    (void)get_uint8_value(0x1b, &dp_drc_data[0x64]);
    (void)get_uint8_value(0x1c, &dp_drc_data[0x0c]);
    (void)get_uint8_value(0x1d, &dp_drc_data[0x10]);
    (void)get_uint8_value(0x1e, &dp_drc_data[0x14]);
    (void)get_uint8_value(0x1f, &dp_drc_data[0x20]);
    (void)get_uint8_value(0x20, &dp_drc_data[0x24]);
    (void)get_uint8_value(0x21, &dp_drc_data[0x30]);
    (void)get_uint8_value(0x22, &dp_drc_data[0x34]);
    (void)get_uint8_value(0x23, &dp_drc_data[0x40]);
    (void)get_uint8_value(0x24, &dp_drc_data[0x44]);
}

/* 仅供比较器访问原 ELF 的已观察状态块。 */
void dp_desk_command_state_reset_for_test(void)
{
    memset(dp_desk_flags, 0, sizeof(dp_desk_flags));
    memset(dp_drc_data, 0, sizeof(dp_drc_data));
}

const unsigned char *dp_desk_command_flags_for_test(void)
{
    return dp_desk_flags;
}

const unsigned char *dp_desk_command_drc_for_test(void)
{
    return dp_drc_data;
}
