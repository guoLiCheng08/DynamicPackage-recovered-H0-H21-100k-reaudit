/*
 * DynamicPackage 原 ELF 金标录制器。
 *
 * 设计：在外层私有 user/mount/network namespace 内运行；本程序启动原 ELF，
 * 写入受控命令，再在进程共享 rwlock 保护下记录共享内存的可比较快照。
 * 它不会修改原 ELF 文件。
 */
#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define DP_SHM_NAME "/sharedvars_example"
#define DP_SHM_SIZE 0xBF0u
#define DP_RWLOCK_BYTES 0x38u
#define DP_FLOAT_BASE 0x38u
#define DP_BYTE_BASE 0x998u
#define DP_MAGIC "DPGOLD01"
#define DP_VERSION 1u

typedef struct {
    char magic[8];
    uint32_t version;
    uint32_t shm_size;
    uint32_t lock_bytes_excluded;
    uint32_t frame_count;
    uint32_t interval_ms;
    uint32_t reserved[3];
} GoldenHeader;

typedef struct {
    uint32_t logical_step;
    uint32_t reserved;
    float command_f32[12];
    uint8_t command_u8[3];
    uint8_t command_padding;
    uint8_t shared_memory[DP_SHM_SIZE];
} GoldenFrame;

static void sleep_ms(long milliseconds)
{
    struct timespec request;
    request.tv_sec = milliseconds / 1000L;
    request.tv_nsec = (milliseconds % 1000L) * 1000000L;
    while (nanosleep(&request, &request) != 0 && errno == EINTR) {}
}

static int write_exact(FILE *file, const void *data, size_t size)
{
    return fwrite(data, 1u, size, file) == size ? 0 : -1;
}

static int apply_case(unsigned char *shared, const char *case_name)
{
    float *floats = (float *)(shared + DP_FLOAT_BASE);
    unsigned char *bytes = shared + DP_BYTE_BASE;
    unsigned i;
    for (i = 0; i < 12u; ++i) floats[i] = 0.0f;
    bytes[0] = 0u; /* RW_FLAG */
    bytes[1] = 0u; /* SADA_FLAG */
    bytes[2] = 0u; /* Thruster_Enable */

    if (strcmp(case_name, "zero") == 0) {
        return 0;
    }
    if (strcmp(case_name, "rw1_pos") == 0) {
        bytes[0] = 1u;
        floats[0] = 0.001f;
        return 0;
    }
    if (strcmp(case_name, "rw1_neg") == 0) {
        bytes[0] = 1u;
        floats[0] = -0.001f;
        return 0;
    }
    if (strcmp(case_name, "mtq_x_pos") == 0) {
        floats[4] = 0.001f;
        return 0;
    }
    if (strcmp(case_name, "sada_position") == 0) {
        bytes[1] = 1u;
        floats[10] = 0.01f;
        floats[11] = -0.01f;
        return 0;
    }
    return -1;
}

int main(int argc, char **argv)
{
    const char *sample = "/home/ubuntu/dynamicpackage_restore/input/DynamicPackage.elf";
    const char *loader = "/lib64/ld-linux-x86-64.so.2";
    const char *out_path;
    const char *case_name;
    long frame_count;
    long interval_ms;
    pid_t child;
    int fd = -1;
    unsigned char *shared = MAP_FAILED;
    pthread_rwlock_t *lock;
    FILE *out = NULL;
    GoldenHeader header;
    long step;
    int ready = 0;

    if (argc != 5) {
        fprintf(stderr, "用法: %s <output.bin> <frame_count> <interval_ms> <zero|rw1_pos|rw1_neg|mtq_x_pos|sada_position>\n", argv[0]);
        return 64;
    }
    out_path = argv[1];
    frame_count = strtol(argv[2], NULL, 10);
    interval_ms = strtol(argv[3], NULL, 10);
    case_name = argv[4];
    if (frame_count <= 0 || frame_count > 10000 || interval_ms <= 0 || interval_ms > 10000) {
        fprintf(stderr, "无效参数。\n");
        return 64;
    }

    child = fork();
    if (child < 0) {
        perror("fork");
        return 2;
    }
    if (child == 0) {
        int nullfd = open("/dev/null", O_WRONLY);
        if (nullfd >= 0) {
            (void)dup2(nullfd, STDOUT_FILENO);
            (void)dup2(nullfd, STDERR_FILENO);
            if (nullfd > STDERR_FILENO) close(nullfd);
        }
        execl(loader, loader, sample, (char *)NULL);
        _exit(127);
    }

    for (step = 0; step < 200; ++step) {
        fd = shm_open(DP_SHM_NAME, O_RDWR, 0);
        if (fd >= 0) {
            ready = 1;
            break;
        }
        sleep_ms(5);
    }
    if (!ready) {
        fprintf(stderr, "原 ELF 未创建共享内存。\n");
        kill(child, SIGKILL);
        waitpid(child, NULL, 0);
        return 3;
    }

    shared = mmap(NULL, DP_SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared == MAP_FAILED) {
        perror("mmap");
        close(fd);
        kill(child, SIGKILL);
        waitpid(child, NULL, 0);
        return 4;
    }
    lock = (pthread_rwlock_t *)shared;

    out = fopen(out_path, "wb");
    if (out == NULL) {
        perror("fopen");
        munmap(shared, DP_SHM_SIZE);
        close(fd);
        kill(child, SIGKILL);
        waitpid(child, NULL, 0);
        return 5;
    }

    memset(&header, 0, sizeof(header));
    memcpy(header.magic, DP_MAGIC, sizeof(header.magic));
    header.version = DP_VERSION;
    header.shm_size = DP_SHM_SIZE;
    header.lock_bytes_excluded = DP_RWLOCK_BYTES;
    header.frame_count = (uint32_t)frame_count;
    header.interval_ms = (uint32_t)interval_ms;
    if (write_exact(out, &header, sizeof(header)) != 0) {
        perror("write header");
        fclose(out);
        munmap(shared, DP_SHM_SIZE);
        close(fd);
        kill(child, SIGKILL);
        waitpid(child, NULL, 0);
        return 6;
    }

    for (step = 0; step < frame_count; ++step) {
        GoldenFrame frame;
        memset(&frame, 0, sizeof(frame));
        frame.logical_step = (uint32_t)step;

        if (pthread_rwlock_wrlock(lock) != 0) {
            fprintf(stderr, "步骤 %ld 无法取得写锁。\n", step);
            break;
        }
        if (apply_case(shared, case_name) != 0) {
            fprintf(stderr, "未知测试用例: %s\n", case_name);
            (void)pthread_rwlock_unlock(lock);
            break;
        }
        memcpy(frame.command_f32, shared + DP_FLOAT_BASE, sizeof(frame.command_f32));
        frame.command_u8[0] = shared[DP_BYTE_BASE + 0u];
        frame.command_u8[1] = shared[DP_BYTE_BASE + 1u];
        frame.command_u8[2] = shared[DP_BYTE_BASE + 2u];
        (void)pthread_rwlock_unlock(lock);

        sleep_ms(interval_ms);

        if (pthread_rwlock_rdlock(lock) != 0) {
            fprintf(stderr, "步骤 %ld 无法取得读锁。\n", step);
            break;
        }
        memcpy(frame.shared_memory, shared, DP_SHM_SIZE);
        /* 锁内部状态与采样锁行为相关，不作为输出等价比较对象。 */
        memset(frame.shared_memory, 0, DP_RWLOCK_BYTES);
        (void)pthread_rwlock_unlock(lock);

        if (write_exact(out, &frame, sizeof(frame)) != 0) {
            perror("write frame");
            break;
        }
    }

    fflush(out);
    fclose(out);
    munmap(shared, DP_SHM_SIZE);
    close(fd);
    kill(child, SIGKILL);
    waitpid(child, NULL, 0);
    return step == frame_count ? 0 : 7;
}
