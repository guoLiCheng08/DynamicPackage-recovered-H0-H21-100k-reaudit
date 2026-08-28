/* DynamicPackage 随机数适配：测试回放模式不依赖外部模型库。 */
#include "dynamic_rng.h"

#include <stdio.h>
#include <stdlib.h>

static FILE *dp_rng_replay_file;
static int dp_rng_error;
static uint64_t dp_rng_count_value;

int dp_rng_open_replay(const char *path)
{
    if (path == NULL || path[0] == '\0') return -1;
    dp_rng_close();
    dp_rng_error = 0;
    dp_rng_count_value = 0u;
    dp_rng_replay_file = fopen(path, "rb");
    return dp_rng_replay_file != NULL ? 0 : -1;
}

void dp_rng_close(void)
{
    if (dp_rng_replay_file != NULL) {
        (void)fclose(dp_rng_replay_file);
        dp_rng_replay_file = NULL;
    }
}

int dp_rng_had_error(void)
{
    return dp_rng_error;
}

uint64_t dp_rng_count(void)
{
    return dp_rng_count_value;
}

int dp_rng_next(int32_t *value)
{
    if (value == NULL) return -1;
    if (dp_rng_replay_file != NULL) {
        if (fread(value, sizeof(*value), 1u, dp_rng_replay_file) != 1u) {
            dp_rng_error = 1;
            return -1;
        }
        if (*value < 0 || *value > RAND_MAX) {
            dp_rng_error = 1;
            return -1;
        }
        ++dp_rng_count_value;
        return 0;
    }
    *value = (int32_t)rand();
    ++dp_rng_count_value;
    return 0;
}
