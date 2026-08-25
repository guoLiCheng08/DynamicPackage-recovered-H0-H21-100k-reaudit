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
        double xyz[3] = {0.0, 0.0, 0.0};
        double angle_a = 123.0;
        double angle_b = 456.0;
        double angle_c = 789.0;
        DpQuatAbi quat = {1.0, {3, 0, xyz}};

        quat_to_euler_angle_312(&angle_a, &angle_b, &angle_c, &quat);
        _exit(0);
    }
    if (waitpid(child, &status, 0) != child) {
        perror("waitpid");
        return 1;
    }
    if (!WIFSIGNALED(status) || WTERMSIG(status) != SIGSEGV) {
        fprintf(stderr, "quat_to_euler_angle_312 expected SIGSEGV, status=0x%x\n", status);
        return 1;
    }
    puts("quat_to_euler_angle_312 P3 death ABI: PASS (SIGSEGV)");
    return 0;
}
