#define _GNU_SOURCE
#include <dlfcn.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

static int (*real_rand)(void);
static void (*real_srand)(unsigned int);
static int record_fd = -1;

static void dp_rng_record_init(void)
{
    const char *path;
    if (real_rand == NULL) real_rand = (int (*)(void))dlsym(RTLD_NEXT, "rand");
    if (real_srand == NULL) real_srand = (void (*)(unsigned int))dlsym(RTLD_NEXT, "srand");
    if (record_fd < 0) {
        path = getenv("DP_RNG_RECORD_FILE");
        if (path != NULL && path[0] != '\0')
            record_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0600);
    }
}

int rand(void)
{
    int value;
    const char *arm;
    dp_rng_record_init();
    value = real_rand != NULL ? real_rand() : 0;
    arm = getenv("DP_RNG_ARM_FILE");
    if (record_fd >= 0 && arm != NULL && access(arm, F_OK) == 0)
        (void)write(record_fd, &value, sizeof(value));
    return value;
}

void srand(unsigned int seed)
{
    dp_rng_record_init();
    if (real_srand != NULL) real_srand(seed);
}
