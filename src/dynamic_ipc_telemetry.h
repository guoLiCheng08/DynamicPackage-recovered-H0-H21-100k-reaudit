/* DynamicPackage sendDynTele 输出序列化恢复。 */
#ifndef DYNAMIC_IPC_TELEMETRY_H
#define DYNAMIC_IPC_TELEMETRY_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdint.h>
#include <pthread.h>
#include "dynamic_telemetry_layout.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DP_IPC_SHM_BYTES 0xBF0u
#define DP_IPC_RWLOCK_BYTES 0x038u
#define DP_IPC_FLOAT_BASE 0x038u
#define DP_IPC_U8_BASE 0x998u
#define DP_IPC_SHM_NAME "/sharedvars_example"
#define DP_IPC_FLOAT_COUNT ((DP_IPC_U8_BASE - DP_IPC_FLOAT_BASE) / sizeof(float))
#define DP_IPC_U8_COUNT (DP_IPC_SHM_BYTES - DP_IPC_U8_BASE)

typedef struct {
    uint8_t raw[DP_IPC_SHM_BYTES];
} DpIpcSharedFrame;

typedef struct {
    pthread_rwlock_t lock; /* 原帧 +0x000，Linux x86-64 下为 0x38 字节。 */
    uint8_t payload[DP_IPC_SHM_BYTES - DP_IPC_RWLOCK_BYTES];
} DpIpcMappedShared;

_Static_assert(sizeof(pthread_rwlock_t) == DP_IPC_RWLOCK_BYTES, "IPC rwlock ABI");
_Static_assert(sizeof(DpIpcMappedShared) == DP_IPC_SHM_BYTES, "IPC mapped shared size");

typedef struct {
    int fd;
    DpIpcMappedShared *mapped;
} DpIpcSharedHandle;

/* 对应 init_shared/close_shared：创建时初始化锁并清零 +0x38..+0xBEF；已有对象仅映射。 */
int dp_ipc_shared_open(DpIpcSharedHandle *handle);
int dp_ipc_shared_open_named(DpIpcSharedHandle *handle, const char *name);
void dp_ipc_shared_close(DpIpcSharedHandle *handle);

/* 原 ELF 的进程全局共享对象接口；getter 在尚未映射时自动调用 init_shared。 */
int32_t init_shared(void);
void close_shared(void);
int32_t get_float_value(int32_t index, float *out);
int32_t get_uint8_value(int32_t index, uint8_t *out);
int32_t get_all_floats(float *out);
int32_t get_all_uint8s(uint8_t *out);
/* 原公共 setter：写入映射 float/byte payload，成功 0。 */
int32_t set_float_value(int32_t index, float value);
int32_t set_uint8_value(int32_t index, uint8_t value);

/* 将 UpdateMainOut 形成的输出对象按 sendDynTele 的 set_float/set_uint8 调用顺序写入共享内存。 */
void dp_send_dyn_tele(DpIpcSharedFrame *shared, const DpMainTelemetryFrame *telemetry,
                      uint8_t drc_data_20, uint8_t drc_data_24,
                      uint8_t drc_data_40, uint8_t drc_data_44,
                      uint8_t drc_data_8c);
/* 原 ELF ABI：rdi 为未使用的保留指针，rsi 为 0x220 字节主遥测帧；写入全局共享映射。 */
void sendDynTele(void *reserved, const DpMainTelemetryFrame *telemetry);

/* 原 ELF 的算法遥测封装；两个 Update 清零全局计数，两个 Set 返回零。 */
void Update_Telemetry_Frame_1(void);
void Update_Telemetry_Frame_2(void);
int32_t Set_Algorithm_Telemetry_Frame_1(void);
int32_t Set_Algorithm_Telemetry_Frame_2(void);
/* 仅供恢复端比较器读取/设置原 algorithm_telemetry_size 对应状态。 */
int32_t dp_algorithm_telemetry_size_get(void);
void dp_algorithm_telemetry_size_set_for_test(int32_t value);
/* 仅供离线原 ELF 比较器写入受控共享输入，索引范围与生产 getter 一致。 */
int dp_ipc_seed_float_for_test(uint32_t index, float value);
int dp_ipc_seed_u8_for_test(uint32_t index, uint8_t value);

/* 原 var2buff/Put_*_In_Buff：endian_flag=0 写高字节优先，1 原样复制。 */
void var2buff(uint8_t *destination, const void *source, int32_t byte_count,
              uint8_t endian_flag);
void Put_UINT16_In_Buff(uint8_t *destination, uint16_t value, uint8_t endian_flag);
void Put_UINT32_In_Buff(uint8_t *destination, uint32_t value, uint8_t endian_flag);
void Put_FLOAT32_In_Buff(uint8_t *destination, uint8_t endian_flag, float value);
void Put_FLOAT64_In_Buff(uint8_t *destination, uint8_t endian_flag, double value);

uint8_t read_Uint8(const uint8_t *source);
float read_float32(const uint8_t *source, int32_t endian_flag);
uint32_t read_long(const uint8_t *source, int32_t endian_flag);
void Change_Endian(uint8_t *destination, const uint8_t *source, int32_t byte_count);

uint8_t get_uint8_para(const uint8_t *source);
uint32_t get_uint32_para(const uint8_t *source, uint8_t endian_flag);
float get_float32_para(const uint8_t *source, uint8_t endian_flag);
double get_float64_para(const uint8_t *source, uint8_t endian_flag);
int32_t escape_character_procotol(uint8_t *destination, const uint8_t *source,
                                  int32_t byte_count);

#ifdef __cplusplus
}
#endif
#endif /* DYNAMIC_IPC_TELEMETRY_H */
