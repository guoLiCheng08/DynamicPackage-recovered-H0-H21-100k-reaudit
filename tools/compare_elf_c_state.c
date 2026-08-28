#define _POSIX_C_SOURCE 200809L

#include "c_shadow_state_ipc.h"

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define ELF_Y_OFFSET 0x2184e0u

static int elf_base(pid_t pid, unsigned long *base)
{
    char path[64];
    FILE *maps;
    char line[512];

    (void)snprintf(path, sizeof(path), "/proc/%ld/maps", (long)pid);
    maps = fopen(path, "r");
    if (maps == NULL) return -1;
    while (fgets(line, sizeof(line), maps) != NULL) {
        unsigned long start;
        unsigned long end;
        char permissions[5];
        unsigned long file_offset;
        char mapped_path[256] = {0};

        if (sscanf(line, "%lx-%lx %4s %lx %*s %*s %255s",
                   &start, &end, permissions, &file_offset, mapped_path) < 5) {
            continue;
        }
        if (strcmp(permissions, "r-xp") == 0 && file_offset == 0u &&
            strstr(mapped_path, "DynamicPackage") != NULL) {
            *base = start;
            (void)fclose(maps);
            return 0;
        }
    }
    (void)fclose(maps);
    return -1;
}

static int read_elf_state(pid_t pid, double state[DP_STATE_DIM])
{
    char path[64];
    unsigned long base;
    int fd;
    ssize_t got;

    if (elf_base(pid, &base) != 0) return -1;
    (void)snprintf(path, sizeof(path), "/proc/%ld/mem", (long)pid);
    fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd < 0) return -1;
    if (lseek(fd, (off_t)(base + ELF_Y_OFFSET), SEEK_SET) < 0) {
        (void)close(fd);
        return -1;
    }
    got = read(fd, state, DP_STATE_DIM * sizeof(state[0]));
    (void)close(fd);
    return got == (ssize_t)(DP_STATE_DIM * sizeof(state[0])) ? 0 : -1;
}

int main(int argc, char **argv)
{
    DpCShadowStateFrame *frame;
    double elf_state[DP_STATE_DIM];
    const char *name = getenv("C_SHADOW_STATE_SHM");
    unsigned long sequence;
    uint32_t input_sequence;
    unsigned index;
    unsigned mismatches = 0u;
    int fd;
    pid_t pid;

    if (argc != 2) {
        fprintf(stderr, "用法: %s ELF进程号\n", argv[0]);
        return 2;
    }
    pid = (pid_t)strtol(argv[1], NULL, 10);
    if (name == NULL || name[0] != '/') name = DP_C_SHADOW_STATE_SHM;
    fd = shm_open(name, O_RDONLY, 0);
    if (fd < 0) {
        fprintf(stderr, "无法打开纯C状态共享内存 %s: %s\n", name, strerror(errno));
        return 2;
    }
    frame = mmap(NULL, sizeof(*frame), PROT_READ, MAP_SHARED, fd, 0);
    (void)close(fd);
    if (frame == MAP_FAILED || frame->magic != DP_C_SHADOW_STATE_MAGIC ||
        frame->version != DP_C_SHADOW_STATE_VERSION ||
        frame->state_bytes != sizeof(frame->state)) {
        fprintf(stderr, "纯C状态帧无效或未就绪\n");
        return 3;
    }
    sequence = frame->sequence;
    input_sequence = frame->input_sequence;
    if (sequence == 0ul || read_elf_state(pid, elf_state) != 0) {
        fprintf(stderr, "无法读取正式ELF的y[33]状态\n");
        (void)munmap(frame, sizeof(*frame));
        return 3;
    }
    for (index = 0u; index < DP_STATE_DIM; ++index) {
        double c_value = frame->state[index];
        double e_value = elf_state[index];
        double error = fabs(c_value - e_value);
        double tolerance = 1.0e-9 * fmax(1.0, fmax(fabs(c_value), fabs(e_value)));
        if (!isfinite(c_value) || !isfinite(e_value) || error > tolerance) {
            printf("状态[%u] C=%.17g ELF=%.17g 误差=%.17g 容差=%.17g\n",
                   index, c_value, e_value, error, tolerance);
            ++mismatches;
        }
    }
    (void)munmap(frame, sizeof(*frame));
    if (mismatches != 0u) {
        printf("1. 结果=失败\n2. C状态序号=%lu\n3. 输入序号=%u\n4. 状态不一致项=%u/%u\n",
               sequence, input_sequence, mismatches, DP_STATE_DIM);
        return 1;
    }
    printf("1. 结果=通过\n2. C状态序号=%lu\n3. 输入序号=%u\n4. 33维状态=全部一致\n",
           sequence, input_sequence);
    return 0;
}
