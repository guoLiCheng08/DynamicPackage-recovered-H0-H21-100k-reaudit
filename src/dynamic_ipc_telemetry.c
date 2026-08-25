/* DynamicPackage sendDynTele 输出序列化恢复。 */
#define _POSIX_C_SOURCE 200809L
#include "dynamic_ipc_telemetry.h"

/* 协议单对象测试不链接命令包装模块；完整恢复库中该弱符号解析至真实 DRC 状态。 */
extern const unsigned char *dp_desk_command_drc_for_test(void) __attribute__((weak));

#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

/* 对应原 ELF 的 g_shm_fd（初值 -1）和 g_data（初值 NULL）。 */
static DpIpcSharedHandle g_ipc_shared_handle = { -1, NULL };

void var2buff(uint8_t *destination, const void *source, int32_t byte_count,
              uint8_t endian_flag)
{
    const uint8_t *bytes = source;
    int32_t index;

    if (endian_flag == 1u) {
        for (index = 0; index < byte_count; ++index) destination[index] = bytes[index];
    } else if (endian_flag == 0u) {
        for (index = 0; index < byte_count; ++index) {
            destination[index] = bytes[byte_count - 1 - index];
        }
    }
}

void Put_UINT16_In_Buff(uint8_t *destination, uint16_t value, uint8_t endian_flag)
{
    var2buff(destination, &value, 2, endian_flag);
}

void Put_UINT32_In_Buff(uint8_t *destination, uint32_t value, uint8_t endian_flag)
{
    var2buff(destination, &value, 4, endian_flag);
}

void Put_FLOAT32_In_Buff(uint8_t *destination, uint8_t endian_flag, float value)
{
    var2buff(destination, &value, 4, endian_flag);
}

void Put_FLOAT64_In_Buff(uint8_t *destination, uint8_t endian_flag, double value)
{
    var2buff(destination, &value, 8, endian_flag);
}

int32_t escape_character_procotol(uint8_t *destination, const uint8_t *source,
                                  int32_t byte_count)
{
    int32_t out_index = 1;
    int32_t index;
    destination[0] = 0x7du;
    for (index = 0; index < byte_count; ++index) {
        const uint8_t value = source[index];
        if (value == 0x7du) {
            destination[out_index++] = 0x7fu;
            destination[out_index++] = 0x00u;
        } else if (value == 0x7eu) {
            destination[out_index++] = 0x7fu;
            destination[out_index++] = 0x01u;
        } else if (value == 0x7fu) {
            destination[out_index++] = 0x7fu;
            destination[out_index++] = 0x02u;
        } else {
            destination[out_index++] = value;
        }
    }
    destination[out_index++] = 0x7eu;
    return out_index;
}

void Change_Endian(uint8_t *destination, const uint8_t *source, int32_t byte_count)
{
    int32_t index;
    for (index = 0; index < byte_count; ++index) {
        destination[index] = source[byte_count - 1 - index];
    }
}

uint8_t get_uint8_para(const uint8_t *source)
{
    return source[0];
}

uint32_t get_uint32_para(const uint8_t *source, uint8_t endian_flag)
{
    return endian_flag == 1u ? (uint32_t)source[0] | ((uint32_t)source[1] << 8) |
               ((uint32_t)source[2] << 16) | ((uint32_t)source[3] << 24) :
           ((uint32_t)source[0] << 24) | ((uint32_t)source[1] << 16) |
               ((uint32_t)source[2] << 8) | (uint32_t)source[3];
}

float get_float32_para(const uint8_t *source, uint8_t endian_flag)
{
    const uint32_t bits = get_uint32_para(source, endian_flag);
    float value;
    memcpy(&value, &bits, sizeof(value));
    return value;
}

double get_float64_para(const uint8_t *source, uint8_t endian_flag)
{
    uint8_t ordered[8];
    double value;
    unsigned index;
    for (index = 0u; index < 8u; ++index) {
        ordered[index] = endian_flag == 1u ? source[index] : source[7u - index];
    }
    memcpy(&value, ordered, sizeof(value));
    return value;
}

uint8_t read_Uint8(const uint8_t *source)
{
    return source[0];
}

uint32_t read_long(const uint8_t *source, int32_t endian_flag)
{
    if (endian_flag == 1) {
        return (uint32_t)source[0] | ((uint32_t)source[1] << 8) |
               ((uint32_t)source[2] << 16) | ((uint32_t)source[3] << 24);
    }
    return ((uint32_t)source[0] << 24) | ((uint32_t)source[1] << 16) |
           ((uint32_t)source[2] << 8) | (uint32_t)source[3];
}

float read_float32(const uint8_t *source, int32_t endian_flag)
{
    const uint32_t bits = read_long(source, endian_flag);
    float value;
    memcpy(&value, &bits, sizeof(value));
    return value;
}

int dp_ipc_shared_open(DpIpcSharedHandle *handle)
{
    int fd;
    int created = 0;
    DpIpcMappedShared *mapped;

    if (handle == NULL) {
        return -1;
    }
    if (handle->mapped != NULL) {
        return 0;
    }
    handle->fd = -1;
    fd = shm_open(DP_IPC_SHM_NAME, O_CREAT | O_EXCL | O_RDWR, 0666);
    if (fd >= 0) {
        created = 1;
        if (ftruncate(fd, (off_t)DP_IPC_SHM_BYTES) < 0) {
            (void)close(fd);
            return -2;
        }
    } else {
        fd = shm_open(DP_IPC_SHM_NAME, O_RDWR, 0666);
        if (fd < 0) {
            return -1;
        }
    }
    mapped = mmap(NULL, DP_IPC_SHM_BYTES, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped == MAP_FAILED) {
        (void)close(fd);
        return -3;
    }
    if (created != 0) {
        pthread_rwlockattr_t attributes;
        (void)pthread_rwlockattr_init(&attributes);
        (void)pthread_rwlockattr_setpshared(&attributes, PTHREAD_PROCESS_SHARED);
        if (pthread_rwlock_init(&mapped->lock, &attributes) != 0) {
            (void)pthread_rwlockattr_destroy(&attributes);
            (void)munmap(mapped, DP_IPC_SHM_BYTES);
            (void)close(fd);
            return -4;
        }
        (void)pthread_rwlockattr_destroy(&attributes);
        memset(((uint8_t *)mapped) + DP_IPC_RWLOCK_BYTES, 0,
               DP_IPC_SHM_BYTES - DP_IPC_RWLOCK_BYTES);
    }
    handle->fd = fd;
    handle->mapped = mapped;
    return 0;
}

void dp_ipc_shared_close(DpIpcSharedHandle *handle)
{
    if (handle == NULL) {
        return;
    }
    if (handle->mapped != NULL) {
        (void)munmap(handle->mapped, DP_IPC_SHM_BYTES);
        handle->mapped = NULL;
    }
    if (handle->fd >= 0) {
        (void)close(handle->fd);
        handle->fd = -1;
    }
}

int32_t init_shared(void)
{
    return (int32_t)dp_ipc_shared_open(&g_ipc_shared_handle);
}

void close_shared(void)
{
    dp_ipc_shared_close(&g_ipc_shared_handle);
}

static int32_t ensure_global_shared(void)
{
    if (g_ipc_shared_handle.mapped != NULL) {
        return 0;
    }
    return init_shared() == 0 ? 0 : -20;
}

int32_t get_float_value(int32_t index, float *out)
{
    int32_t result = ensure_global_shared();

    if (result != 0) {
        return result;
    }
    if ((uint32_t)index > 0x257u) {
        return -21;
    }
    if (out == NULL) {
        return -22;
    }
    (void)pthread_rwlock_rdlock(&g_ipc_shared_handle.mapped->lock);
    memcpy(out, ((const uint8_t *)g_ipc_shared_handle.mapped) + DP_IPC_FLOAT_BASE +
                    (size_t)(uint32_t)index * sizeof(*out), sizeof(*out));
    (void)pthread_rwlock_unlock(&g_ipc_shared_handle.mapped->lock);
    return 0;
}

int32_t get_uint8_value(int32_t index, uint8_t *out)
{
    int32_t result = ensure_global_shared();

    if (result != 0) {
        return result;
    }
    if ((uint32_t)index > 0x257u) {
        return -21;
    }
    if (out == NULL) {
        return -22;
    }
    (void)pthread_rwlock_rdlock(&g_ipc_shared_handle.mapped->lock);
    *out = ((const uint8_t *)g_ipc_shared_handle.mapped)[DP_IPC_U8_BASE + (uint32_t)index];
    (void)pthread_rwlock_unlock(&g_ipc_shared_handle.mapped->lock);
    return 0;
}

int32_t get_all_floats(float *out)
{
    int32_t result = ensure_global_shared();

    if (result != 0) {
        return result;
    }
    if (out == NULL) {
        return -22;
    }
    (void)pthread_rwlock_rdlock(&g_ipc_shared_handle.mapped->lock);
    memcpy(out, ((const uint8_t *)g_ipc_shared_handle.mapped) + DP_IPC_FLOAT_BASE, 0x960u);
    (void)pthread_rwlock_unlock(&g_ipc_shared_handle.mapped->lock);
    return 0;
}

int32_t get_all_uint8s(uint8_t *out)
{
    int32_t result = ensure_global_shared();

    if (result != 0) {
        return result;
    }
    if (out == NULL) {
        return -22;
    }
    (void)pthread_rwlock_rdlock(&g_ipc_shared_handle.mapped->lock);
    memcpy(out, ((const uint8_t *)g_ipc_shared_handle.mapped) + DP_IPC_U8_BASE, 0x258u);
    (void)pthread_rwlock_unlock(&g_ipc_shared_handle.mapped->lock);
    return 0;
}

static double get_f64(const uint8_t *raw, unsigned offset)
{
    double value;
    memcpy(&value, raw + offset, sizeof(value));
    return value;
}

static int32_t get_i32(const uint8_t *raw, unsigned offset)
{
    int32_t value;
    memcpy(&value, raw + offset, sizeof(value));
    return value;
}

static void dp_frame_set_float_value(DpIpcSharedFrame *shared, unsigned index, float value)
{
    memcpy(shared->raw + DP_IPC_FLOAT_BASE + index * sizeof(value), &value, sizeof(value));
}

static void dp_frame_set_uint8_value(DpIpcSharedFrame *shared, unsigned index, uint8_t value)
{
    shared->raw[DP_IPC_U8_BASE + index] = value;
}

static void set_telemetry_f64_as_f32(DpIpcSharedFrame *shared, unsigned ipc_index,
                                     const DpMainTelemetryFrame *telemetry,
                                     unsigned telemetry_offset)
{
    dp_frame_set_float_value(shared, ipc_index, (float)get_f64(telemetry->raw, telemetry_offset));
}

void dp_send_dyn_tele(DpIpcSharedFrame *shared, const DpMainTelemetryFrame *telemetry,
                      uint8_t drc_data_20, uint8_t drc_data_24,
                      uint8_t drc_data_40, uint8_t drc_data_44,
                      uint8_t drc_data_8c)
{
    unsigned group;
    unsigned element;

    /* STS 有效标志 -> byte 3..5。 */
    for (group = 0; group < 3u; ++group) {
        dp_frame_set_uint8_value(shared, group + 3u,
                        (uint8_t)get_i32(telemetry->raw, DP_TM_STS_VALID + group * 4u));
    }
    /* 三组四元数 -> float 12..23。 */
    for (group = 0; group < 3u; ++group) {
        for (element = 0; element < 4u; ++element) {
            set_telemetry_f64_as_f32(shared, 12u + group * 4u + element, telemetry,
                                     DP_TM_STS_QUAT_0 + group * 0x20u + element * 8u);
        }
    }

    dp_frame_set_uint8_value(shared, 6u, drc_data_20);
    dp_frame_set_uint8_value(shared, 7u, drc_data_24);
    for (element = 0; element < 3u; ++element) {
        set_telemetry_f64_as_f32(shared, 24u + element, telemetry, DP_TM_GYRO_0 + element * 8u);
        set_telemetry_f64_as_f32(shared, 27u + element, telemetry, DP_TM_GYRO_1 + element * 8u);
    }
    dp_frame_set_uint8_value(shared, 8u, (uint8_t)get_i32(telemetry->raw, DP_TM_DSS_VALID));
    dp_frame_set_uint8_value(shared, 9u, (uint8_t)get_i32(telemetry->raw, DP_TM_DSS_VALID + 4u));
    for (element = 0; element < 4u; ++element) {
        set_telemetry_f64_as_f32(shared, 30u + element, telemetry, DP_TM_DSS_VALUE + element * 8u);
    }

    dp_frame_set_uint8_value(shared, 10u, drc_data_40);
    dp_frame_set_uint8_value(shared, 11u, drc_data_44);
    for (element = 0; element < 3u; ++element) {
        set_telemetry_f64_as_f32(shared, 34u + element, telemetry, DP_TM_MAGMETER_0 + element * 8u);
        set_telemetry_f64_as_f32(shared, 37u + element, telemetry, DP_TM_MAGMETER_1 + element * 8u);
    }
    for (element = 0; element < 4u; ++element) {
        set_telemetry_f64_as_f32(shared, 40u + element, telemetry, DP_TM_RW_OMEGA + element * 8u);
    }

    set_telemetry_f64_as_f32(shared, 62u, telemetry, DP_TM_SADA + 0x00u);
    set_telemetry_f64_as_f32(shared, 63u, telemetry, DP_TM_SADA + 0x08u);
    set_telemetry_f64_as_f32(shared, 219u, telemetry, DP_TM_SADA + 0x10u);
    set_telemetry_f64_as_f32(shared, 220u, telemetry, DP_TM_SADA + 0x18u);
    for (element = 0; element < 3u; ++element) {
        set_telemetry_f64_as_f32(shared, 44u + element, telemetry, DP_TM_GPS_VALUE + element * 8u);
        set_telemetry_f64_as_f32(shared, 47u + element, telemetry, DP_TM_GPS_VALUE + (element + 3u) * 8u);
        set_telemetry_f64_as_f32(shared, 50u + element, telemetry, DP_TM_POSITION_ECEF + element * 8u);
        set_telemetry_f64_as_f32(shared, 53u + element, telemetry, DP_TM_VELOCITY_ECEF + element * 8u);
    }
    set_telemetry_f64_as_f32(shared, 56u, telemetry, DP_TM_ORBIT_ELEMENTS + 0x00u);
    set_telemetry_f64_as_f32(shared, 57u, telemetry, DP_TM_ORBIT_ELEMENTS + 0x08u);
    set_telemetry_f64_as_f32(shared, 58u, telemetry, DP_TM_ORBIT_ELEMENTS + 0x10u);
    set_telemetry_f64_as_f32(shared, 59u, telemetry, DP_TM_ORBIT_ELEMENTS + 0x18u);
    set_telemetry_f64_as_f32(shared, 60u, telemetry, DP_TM_ORBIT_ELEMENTS + 0x20u);
    set_telemetry_f64_as_f32(shared, 61u, telemetry, DP_TM_ORBIT_ELEMENTS + 0x30u);

    /* 原函数对第一个 GPS/Kalman 状态字节加 0x30，再写到 index 12。 */
    dp_frame_set_uint8_value(shared, 12u, (uint8_t)(get_i32(telemetry->raw, DP_TM_GPS_FLAG) + 0x30));
    for (element = 1; element < 6u; ++element) {
        dp_frame_set_uint8_value(shared, 12u + element,
                        (uint8_t)get_i32(telemetry->raw, DP_TM_GPS_FLAG + element * 4u));
    }
    dp_frame_set_uint8_value(shared, 18u, drc_data_8c);
    dp_frame_set_uint8_value(shared, 19u, drc_data_8c);
}

/* 原 ELF ABI：rdi 为保留指针，rsi 为主遥测帧；正常路径逐字段调用全局 IPC setter。 */
void sendDynTele(void *reserved, const DpMainTelemetryFrame *telemetry)
{
    DpIpcSharedFrame frame;
    const unsigned char *drc;
    static const unsigned char zero_drc[0x90];
    unsigned index;

    (void)reserved;
    if (telemetry == NULL) return;
    drc = dp_desk_command_drc_for_test != NULL ? dp_desk_command_drc_for_test() : zero_drc;
    memset(&frame, 0, sizeof(frame));
    dp_send_dyn_tele(&frame, telemetry, drc[0x20], drc[0x24], drc[0x40], drc[0x44], drc[0x8c]);
    for (index = 3u; index <= 19u; ++index) {
        (void)set_uint8_value((int32_t)index, frame.raw[DP_IPC_U8_BASE + index]);
    }
    for (index = 12u; index <= 63u; ++index) {
        float value;
        memcpy(&value, frame.raw + DP_IPC_FLOAT_BASE + index * sizeof(value), sizeof(value));
        (void)set_float_value((int32_t)index, value);
    }
    for (index = 219u; index <= 220u; ++index) {
        float value;
        memcpy(&value, frame.raw + DP_IPC_FLOAT_BASE + index * sizeof(value), sizeof(value));
        (void)set_float_value((int32_t)index, value);
    }
}

/* 仅供离线原 ELF 比较器设置受控共享输入；写入布局与 getter 完全相同。 */
int dp_ipc_seed_float_for_test(uint32_t index, float value)
{
    if (index > 0x257u || ensure_global_shared() != 0) {
        return -1;
    }
    (void)pthread_rwlock_wrlock(&g_ipc_shared_handle.mapped->lock);
    memcpy(((uint8_t *)g_ipc_shared_handle.mapped) + DP_IPC_FLOAT_BASE +
               (size_t)index * sizeof(value), &value, sizeof(value));
    (void)pthread_rwlock_unlock(&g_ipc_shared_handle.mapped->lock);
    return 0;
}

int dp_ipc_seed_u8_for_test(uint32_t index, uint8_t value)
{
    if (index > 0x257u || ensure_global_shared() != 0) {
        return -1;
    }
    (void)pthread_rwlock_wrlock(&g_ipc_shared_handle.mapped->lock);
    ((uint8_t *)g_ipc_shared_handle.mapped)[DP_IPC_U8_BASE + index] = value;
    (void)pthread_rwlock_unlock(&g_ipc_shared_handle.mapped->lock);
    return 0;
}

/* 原公共 setter ABI：成功 0，映射失败 -10，索引越界 -11。 */
int32_t set_float_value(int32_t index, float value)
{
    if (ensure_global_shared() != 0) {
        return -10;
    }
    if ((uint32_t)index > 0x257u) {
        return -11;
    }
    (void)pthread_rwlock_wrlock(&g_ipc_shared_handle.mapped->lock);
    memcpy(((uint8_t *)g_ipc_shared_handle.mapped) + DP_IPC_FLOAT_BASE +
               (size_t)(uint32_t)index * sizeof(value), &value, sizeof(value));
    (void)pthread_rwlock_unlock(&g_ipc_shared_handle.mapped->lock);
    return 0;
}

int32_t set_uint8_value(int32_t index, uint8_t value)
{
    if (ensure_global_shared() != 0) {
        return -10;
    }
    if ((uint32_t)index > 0x257u) {
        return -11;
    }
    (void)pthread_rwlock_wrlock(&g_ipc_shared_handle.mapped->lock);
    ((uint8_t *)g_ipc_shared_handle.mapped)[DP_IPC_U8_BASE + (uint32_t)index] = value;
    (void)pthread_rwlock_unlock(&g_ipc_shared_handle.mapped->lock);
    return 0;
}
