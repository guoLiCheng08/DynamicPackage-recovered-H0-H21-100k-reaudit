#define _GNU_SOURCE
#include <execinfo.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int (*real_rand)(void);
static void (*real_srand)(unsigned int);
static int record_fd = -1;
static int caller_fd = -1;
static int gaussian_caller_fd = -1;
static int gaussian_parent_fd = -1;

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
    if (caller_fd < 0) {
        path = getenv("DP_RNG_CALLER_RECORD_FILE");
        if (path != NULL && path[0] != '\0')
            caller_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0600);
    }
    if (gaussian_caller_fd < 0) {
        path = getenv("DP_RNG_GAUSSIAN_CALLER_RECORD_FILE");
        if (path != NULL && path[0] != '\0')
            gaussian_caller_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0600);
    }
    if (gaussian_parent_fd < 0) {
        path = getenv("DP_RNG_GAUSSIAN_PARENT_RECORD_FILE");
        if (path != NULL && path[0] != '\0')
            gaussian_parent_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0600);
    }
}

int rand(void)
{
    int value;
    const char *arm;
    dp_rng_record_init();
    value = real_rand != NULL ? real_rand() : 0;
    arm = getenv("DP_RNG_ARM_FILE");
    if (arm != NULL && access(arm, F_OK) == 0) {
        if (caller_fd >= 0) {
            char line[48];
            void *caller = __builtin_return_address(0);
            Dl_info caller_info;
            uintptr_t offset = (uintptr_t)caller;
            if (dladdr(caller, &caller_info) != 0 && caller_info.dli_fbase != NULL)
                offset -= (uintptr_t)caller_info.dli_fbase;
            int length = snprintf(line, sizeof(line), "0x%lx\n", (unsigned long)offset);
            if (length > 0 && (size_t)length < sizeof(line))
                (void)write(caller_fd, line, (size_t)length);
        }
        if (gaussian_caller_fd >= 0) {
            void *frames[4];
            int frame_count = backtrace(frames, 4);
            if (frame_count >= 3) {
                char line[48];
                Dl_info caller_info;
                uintptr_t offset = (uintptr_t)frames[2];
                int length;
                if (dladdr(frames[2], &caller_info) != 0 && caller_info.dli_fbase != NULL)
                    offset -= (uintptr_t)caller_info.dli_fbase;
                length = snprintf(line, sizeof(line), "0x%lx\n", (unsigned long)offset);
                if (length > 0 && (size_t)length < sizeof(line))
                    (void)write(gaussian_caller_fd, line, (size_t)length);
            }
            if (gaussian_parent_fd >= 0 && frame_count >= 4) {
                char line[48];
                Dl_info caller_info;
                uintptr_t offset = (uintptr_t)frames[3];
                int length;
                if (dladdr(frames[3], &caller_info) != 0 && caller_info.dli_fbase != NULL)
                    offset -= (uintptr_t)caller_info.dli_fbase;
                length = snprintf(line, sizeof(line), "0x%lx\n", (unsigned long)offset);
                if (length > 0 && (size_t)length < sizeof(line))
                    (void)write(gaussian_parent_fd, line, (size_t)length);
            }
        }
        if (record_fd >= 0)
        (void)write(record_fd, &value, sizeof(value));
    }
    return value;
}

void srand(unsigned int seed)
{
    dp_rng_record_init();
    if (real_srand != NULL) real_srand(seed);
}
