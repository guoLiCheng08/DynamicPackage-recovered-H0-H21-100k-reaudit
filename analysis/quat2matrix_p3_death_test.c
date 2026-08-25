#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "dynamic_math.h"

int main(void)
{
    pid_t child;
    int status;

    child = fork();
    if (child < 0) {
        perror("fork");
        return 1;
    }
    if (child == 0) {
        double xyz[3] = {-0.5, 0.5, 0.5};
        double matrix_data[9] = {0.0};
        DpQuatAbi quat = {0.5, {3, 0, xyz}};
        DpMatrix matrix = {3, 3, 3, 0, matrix_data};

        quat2matrix(&matrix, &quat);
        _exit(0);
    }
    if (waitpid(child, &status, 0) != child) {
        perror("waitpid");
        return 1;
    }
    if (!WIFSIGNALED(status) || WTERMSIG(status) != SIGSEGV) {
        fprintf(stderr, "quat2matrix expected SIGSEGV, status=0x%x\n", status);
        return 1;
    }
    puts("quat2matrix P3 death ABI: PASS (SIGSEGV)");
    return 0;
}
