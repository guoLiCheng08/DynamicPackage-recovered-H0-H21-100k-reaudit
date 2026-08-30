/* DynamicPackage 随机数适配：纯 C 本地随机与测试回放共用入口。 */
#ifndef DYNAMIC_RNG_H
#define DYNAMIC_RNG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 回放文件由连续的 int32 little-endian rand() 返回值组成。 */
int dp_rng_open_replay(const char *path);
void dp_rng_close(void);
int dp_rng_next(int32_t *value);
int dp_rng_had_error(void);
uint64_t dp_rng_count(void);

/* 仅用于影子回放诊断：记录阶段边界处已消费的 rand() 原始值数量。
 * 未设置 C_SHADOW_RNG_STAGE_TRACE 时为空操作，不参与模型计算。 */
void dp_rng_trace_stage(const char *stage);

#ifdef __cplusplus
}
#endif
#endif /* DYNAMIC_RNG_H */
