#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/* 仅用于隔离验收：在被测 ELF 的 main 之前等待测试器放行，确保采样器能
 * 捕获真正的首个 dyn_main，而不是附加到已经运行数拍的进程。 */
__attribute__((constructor)) static void dp_start_gate_wait(void)
{
    const char *release_path = getenv("DP_START_GATE_RELEASE_FILE");

    if (release_path == NULL || release_path[0] == '\0') {
        return;
    }
    while (access(release_path, F_OK) != 0) {
        const struct timespec delay = {0, 1000000};
        nanosleep(&delay, NULL);
    }
}
