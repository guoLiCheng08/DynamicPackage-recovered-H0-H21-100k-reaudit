#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "dynamic_math.h"

int main(void)
{
    const pid_t child = fork();
    int status = 0;

    if (child < 0) {
        perror("fork");
        return 1;
    }
    if (child == 0) {
        double destination_data[2] = {1.0, 2.0};
        DpVector source = {2, 0, NULL};
        DpVector destination = {2, 0, destination_data};

        (void)vector_axpby(&source, 1.0, &destination, 0.0);
        _Exit(0);
    }
    if (waitpid(child, &status, 0) < 0) {
        perror("waitpid");
        return 1;
    }
    if (!WIFSIGNALED(status) || WTERMSIG(status) != SIGSEGV) {
        printf("vector_axpby NULL-source P3 expected SIGSEGV, status=0x%x\n", status);
        return 1;
    }
    puts("vector_axpby NULL-source P3 death test: PASS (SIGSEGV)");
    return 0;
}
