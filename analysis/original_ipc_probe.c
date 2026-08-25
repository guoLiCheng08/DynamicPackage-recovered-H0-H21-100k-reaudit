/*
 * 受控原 ELF 探针：仅用于私有 mount/network namespace 内。
 * 它不修改原 ELF；只向程序自己创建的 /sharedvars_example 写入最小命令，
 * 并读取由 sendDynTele() 发布的浮点遥测区。
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
#define DP_FLOAT_BASE 0x38u
#define DP_BYTE_BASE 0x998u

static void sleep_ms(long milliseconds)
{
    struct timespec request;
    request.tv_sec = milliseconds / 1000L;
    request.tv_nsec = (milliseconds % 1000L) * 1000000L;
    while (nanosleep(&request, &request) != 0 && errno == EINTR) {}
}

int main(void)
{
    const char *loader = "/lib64/ld-linux-x86-64.so.2";
    const char *sample = "/home/ubuntu/dynamicpackage_restore/input/DynamicPackage.elf";
    pid_t child;
    int fd = -1;
    unsigned char *base = MAP_FAILED;
    pthread_rwlock_t *lock;
    float *floats;
    unsigned char *bytes;
    int ready = 0;
    int i;

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

    /* 等待子进程执行 init_shared() 并初始化进程共享读写锁。 */
    for (i = 0; i < 100; ++i) {
        fd = shm_open(DP_SHM_NAME, O_RDWR, 0);
        if (fd >= 0) {
            ready = 1;
            break;
        }
        sleep_ms(10);
    }
    if (!ready) {
        fprintf(stderr, "未在 1 秒内观察到私有共享内存对象。\n");
        kill(child, SIGKILL);
        waitpid(child, NULL, 0);
        return 3;
    }

    base = mmap(NULL, DP_SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (base == MAP_FAILED) {
        perror("mmap");
        close(fd);
        kill(child, SIGKILL);
        waitpid(child, NULL, 0);
        return 4;
    }

    lock = (pthread_rwlock_t *)base;
    floats = (float *)(base + DP_FLOAT_BASE);
    bytes = base + DP_BYTE_BASE;

    /*
     * 与 getDynInput() 对齐的最小化命令：
     * byte[0]=RW_FLAG, float[0..3]=RW_Torque,
     * float[4..9]=MTQ, byte[1]=SADA_FLAG,
     * float[10..11]=SADA 参数, byte[2]=Thruster_Enable。
     */
    if (pthread_rwlock_wrlock(lock) != 0) {
        fprintf(stderr, "无法获取共享内存写锁。\n");
        munmap(base, DP_SHM_SIZE);
        close(fd);
        kill(child, SIGKILL);
        waitpid(child, NULL, 0);
        return 5;
    }
    for (i = 0; i < 12; ++i) floats[i] = 0.0f;
    bytes[0] = 0u;
    bytes[1] = 0u;
    bytes[2] = 0u;
    (void)pthread_rwlock_unlock(lock);

    /* 原 main 每 25 个周期调用 sendDynTele()；保留充足的两个发布周期。 */
    sleep_ms(700);

    if (pthread_rwlock_rdlock(lock) != 0) {
        fprintf(stderr, "无法获取共享内存读锁。\n");
        munmap(base, DP_SHM_SIZE);
        close(fd);
        kill(child, SIGKILL);
        waitpid(child, NULL, 0);
        return 6;
    }

    /* 输出索引由 sendDynTele() 反汇编确定，值均是 double 到 float 的发布结果。 */
    printf("{\n");
    printf("  \"test_input\": {\"rw_flag\": %u, \"sada_flag\": %u, \"thruster_enable\": %u},\n",
           (unsigned)bytes[0], (unsigned)bytes[1], (unsigned)bytes[2]);
    printf("  \"sts_q_0\": [%.9g, %.9g, %.9g, %.9g],\n",
           floats[12], floats[13], floats[14], floats[15]);
    printf("  \"gyro_0\": [%.9g, %.9g, %.9g],\n", floats[24], floats[25], floats[26]);
    printf("  \"magmeter_0\": [%.9g, %.9g, %.9g],\n", floats[34], floats[35], floats[36]);
    printf("  \"rw_omega\": [%.9g, %.9g, %.9g, %.9g],\n",
           floats[40], floats[41], floats[42], floats[43]);
    printf("  \"gci_position\": [%.9g, %.9g, %.9g],\n", floats[50], floats[51], floats[52]);
    printf("  \"gci_velocity\": [%.9g, %.9g, %.9g]\n", floats[53], floats[54], floats[55]);
    printf("}\n");

    (void)pthread_rwlock_unlock(lock);
    munmap(base, DP_SHM_SIZE);
    close(fd);
    kill(child, SIGKILL);
    waitpid(child, NULL, 0);
    return 0;
}
