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

#ifdef __cplusplus
}
#endif
#endif /* DYNAMIC_RNG_H */
