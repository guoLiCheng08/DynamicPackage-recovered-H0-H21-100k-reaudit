#define _GNU_SOURCE

#include "dynamic_recovered.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

static int process_base(pid_t pid, unsigned long *base)
{
    char path[64];
    char line[512];
    FILE *maps;

    (void)snprintf(path, sizeof(path), "/proc/%ld/maps", (long)pid);
    maps = fopen(path, "r");
    if (maps == NULL) return -1;
    while (fgets(line, sizeof(line), maps) != NULL) {
        unsigned long start;
        unsigned long end;
        unsigned long offset;
        char permissions[5];
        char mapped_path[256] = {0};

        if (sscanf(line, "%lx-%lx %4s %lx %*s %*s %255s", &start, &end,
                   permissions, &offset, mapped_path) >= 5 &&
            strcmp(permissions, "r-xp") == 0 &&
            strstr(mapped_path, "c_shadow_runtime") != NULL) {
            *base = start - offset;
            (void)fclose(maps);
            return 0;
        }
        (void)end;
    }
    (void)fclose(maps);
    return -1;
}

static int patch_byte(pid_t pid, unsigned long address, unsigned long *saved)
{
    unsigned long word;

    errno = 0;
    word = (unsigned long)ptrace(PTRACE_PEEKTEXT, pid, (void *)address, NULL);
    if (errno != 0) return -1;
    *saved = word;
    word = (word & ~0xfful) | 0xccul;
    return ptrace(PTRACE_POKETEXT, pid, (void *)address, (void *)word) == 0 ? 0 : -1;
}

static int restore_word(pid_t pid, unsigned long address, unsigned long saved)
{
    return ptrace(PTRACE_POKETEXT, pid, (void *)address, (void *)saved) == 0 ? 0 : -1;
}

static int read_process_bytes(pid_t pid, unsigned long address, void *buffer, size_t bytes)
{
    size_t offset;

    for (offset = 0u; offset < bytes; offset += sizeof(unsigned long)) {
        unsigned long word;
        size_t remaining = bytes - offset;
        size_t copy_bytes = remaining < sizeof(word) ? remaining : sizeof(word);

        errno = 0;
        word = (unsigned long)ptrace(PTRACE_PEEKDATA, pid, (void *)(address + offset), NULL);
        if (errno != 0) return -1;
        memcpy((unsigned char *)buffer + offset, &word, copy_bytes);
    }
    return 0;
}

int main(int argc, char **argv)
{
    pid_t pid;
    unsigned long base;
    unsigned long function_address;
    unsigned long entry_saved = 0ul;
    unsigned long return_address = 0ul;
    unsigned long return_saved = 0ul;
    unsigned long output_address = 0ul;
    unsigned target_sequence;
    unsigned calls = 0u;
    int entry_breakpoint = 0;
    int return_breakpoint = 0;
    int status;
    FILE *output;

    if (argc != 5) {
        fprintf(stderr, "用法: %s 进程号 differential_equation偏移 序号 输出文件\n", argv[0]);
        return 2;
    }
    pid = (pid_t)strtol(argv[1], NULL, 10);
    target_sequence = (unsigned)strtoul(argv[3], NULL, 10);
    function_address = strtoul(argv[2], NULL, 0);
    output = fopen(argv[4], "wb");
    if (target_sequence == 0u || output == NULL || process_base(pid, &base) != 0 ||
        ptrace(PTRACE_ATTACH, pid, NULL, NULL) != 0 || waitpid(pid, &status, 0) < 0 ||
        !WIFSTOPPED(status) || patch_byte(pid, base + function_address, &entry_saved) != 0) {
        fprintf(stderr, "无法附加C RHS采样器: %s\n", strerror(errno));
        if (output != NULL) (void)fclose(output);
        return 3;
    }
    entry_breakpoint = 1;
    while (calls < target_sequence * 4u) {
        struct user_regs_struct regs;

        if (ptrace(PTRACE_CONT, pid, NULL, NULL) != 0 || waitpid(pid, &status, 0) < 0 ||
            !WIFSTOPPED(status) || WSTOPSIG(status) != SIGTRAP ||
            ptrace(PTRACE_GETREGS, pid, NULL, &regs) != 0) break;
        if (entry_breakpoint != 0 && regs.rip == base + function_address + 1u) {
            output_address = regs.rdi;
            return_address = (unsigned long)ptrace(PTRACE_PEEKDATA, pid, (void *)regs.rsp, NULL);
            if (return_address == (unsigned long)-1 ||
                restore_word(pid, base + function_address, entry_saved) != 0 ||
                patch_byte(pid, return_address, &return_saved) != 0) break;
            regs.rip = base + function_address;
            if (ptrace(PTRACE_SETREGS, pid, NULL, &regs) != 0) break;
            entry_breakpoint = 0;
            return_breakpoint = 1;
            continue;
        }
        if (return_breakpoint != 0 && regs.rip == return_address + 1u) {
            double rhs[DP_STATE_DIM];

            if (read_process_bytes(pid, output_address, rhs, sizeof(rhs)) != 0 ||
                restore_word(pid, return_address, return_saved) != 0) break;
            ++calls;
            if (calls > (target_sequence - 1u) * 4u &&
                fwrite(rhs, sizeof(rhs), 1u, output) != 1u) break;
            regs.rip = return_address;
            if (ptrace(PTRACE_SETREGS, pid, NULL, &regs) != 0) break;
            return_breakpoint = 0;
            if (calls < target_sequence * 4u &&
                patch_byte(pid, base + function_address, &entry_saved) != 0) break;
            entry_breakpoint = calls < target_sequence * 4u;
            continue;
        }
        break;
    }
    if (entry_breakpoint != 0) (void)restore_word(pid, base + function_address, entry_saved);
    if (return_breakpoint != 0) (void)restore_word(pid, return_address, return_saved);
    (void)ptrace(PTRACE_DETACH, pid, NULL, NULL);
    (void)fclose(output);
    printf("1. 结果=%s\n2. C RHS调用=%u\n3. 目标序号=%u\n",
           calls == target_sequence * 4u ? "通过" : "失败", calls, target_sequence);
    return calls == target_sequence * 4u ? 0 : 1;
}
