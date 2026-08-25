#include <dlfcn.h>
#include <stdio.h>
#include <link.h>
#include <pthread.h>
#include <stdint.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

volatile unsigned h11_native_step = 9999u;
volatile unsigned h11_native_phase = 0u;

static uintptr_t h11_main_base;

static int find_main_base(struct dl_phdr_info *info, size_t size, void *opaque)
{
    (void)size;
    (void)opaque;
    if (info->dlpi_name == NULL || info->dlpi_name[0] == '\0') {
        h11_main_base = (uintptr_t)info->dlpi_addr;
        return 1;
    }
    return 0;
}

typedef void (*H11DynInit)(void *initial_conditions);
typedef void (*H11DynMain)(void *out, void *state, void *command);
typedef int (*H11Main)(int argc, char **argv, char **envp);
typedef int (*H11LibcStartMain)(H11Main main_function, int argc, char **argv,
                                void (*init)(void), void (*fini)(void),
                                void (*rtld_fini)(void), void *stack_end);

static int h11_native_main(int argc, char **argv, char **envp)
{
    unsigned char *initial;
    double *state;
    unsigned char *out;
    unsigned char *command;
    unsigned char *shared;
    double *y;
    H11DynInit dyn_init;
    H11DynMain dyn_main;
    unsigned step;

    (void)argc;
    (void)argv;
    (void)envp;
    h11_native_phase = 1u;
    (void)dl_iterate_phdr(find_main_base, NULL);
    h11_native_phase = 2u;
    (void)fprintf(stderr, "H11_NATIVE phase=2 base=%p\\n", (void *)h11_main_base);
    if (h11_main_base == 0u) {
        return 127;
    }

    h11_native_phase = 3u;
    dyn_init = (H11DynInit)(h11_main_base + 0x3680u);
    dyn_main = (H11DynMain)(h11_main_base + 0x4020u);
    y = (double *)(h11_main_base + 0x2184e0u);
    initial = calloc(0xe8u, 1u);
    state = calloc(33u, sizeof(*state));
    out = calloc(0x220u, 1u);
    command = calloc(0x78u, 1u);
    shared = calloc(0xbf0u, 1u);
    if (initial == NULL || state == NULL || out == NULL || command == NULL || shared == NULL) {
        free(initial);
        free(state);
        free(out);
        free(command);
        free(shared);
        return 127;
    }

    *(double *)(initial + 0x00u) = 0.1;
    *(double *)(initial + 0x08u) = 86400.0;
    *(float *)(initial + 0x28u) = 0.0125f;
    *(float *)(initial + 0x2cu) = -0.00875f;
    *(float *)(initial + 0x30u) = 0.00425f;
    *(double *)(initial + 0x38u) = 2031.0;
    *(double *)(initial + 0x40u) = 12.0;
    *(double *)(initial + 0x48u) = 31.0;
    *(double *)(initial + 0x50u) = 23.0;
    *(double *)(initial + 0x58u) = 59.0;
    *(double *)(initial + 0x60u) = 50.0;
    *(double *)(initial + 0x68u) = 26560000.0;
    *(double *)(initial + 0x70u) = 0.65;
    *(double *)(initial + 0x78u) = 1.1;
    *(double *)(initial + 0x80u) = 1.7;
    *(double *)(initial + 0x88u) = 2.4;
    *(double *)(initial + 0x90u) = 0.9;
    *(double *)(initial + 0x98u) = 120.0;
    *(double *)(initial + 0xc0u) = 100.0;
    *(double *)(initial + 0xd8u) = 80.0;
    *(double *)(initial + 0xe0u) = 1000.0;
    h11_native_phase = 4u;
    (void)fprintf(stderr, "H11_NATIVE phase=4 initial=%p\\n", (void *)initial);
    dyn_init(initial);
    h11_native_phase = 5u;
    (void)fprintf(stderr, "H11_NATIVE phase=5\\n");

    y[0] = 0.5;
    y[1] = 0.5;
    y[2] = 0.5;
    y[3] = -0.5;
    y[13] = -2.0;
    y[14] = 1.0;
    y[15] = -0.5;
    y[16] = 3.0;
    y[17] = -1.5;
    y[18] = 0.75;
    y[19] = -0.375;
    y[20] = 0.1875;
    y[21] = -0.09375;
    y[22] = 0.046875;
    y[23] = -0.2;
    y[24] = 0.4;
    y[25] = -0.6;
    y[26] = 0.8;
    y[27] = -0.4;
    y[28] = 0.2;
    y[29] = -0.1;
    y[30] = 0.05;
    y[31] = -0.025;
    y[32] = 0.0125;

    state[0] = 0.5;
    state[1] = 0.5;
    state[2] = 0.5;
    state[3] = -0.5;
    state[4] = 0.0125;
    state[5] = -0.00875;
    state[6] = 0.00425;
    state[7] = 15000000.0;
    state[8] = -90000000.0;
    state[9] = -39000000.0;
    state[10] = -2500.0;
    state[11] = 5500.0;
    state[12] = 1000.0;

    (void)pthread_rwlock_init((pthread_rwlock_t *)shared, NULL);
    *(void **)(h11_main_base + 0xc44b0u) = shared;
    srand(12345u);
    h11_native_phase = 6u;
    (void)fprintf(stderr, "H11_NATIVE phase=6\\n");
    for (step = 0u; step < 100u; ++step) {
        h11_native_step = step;
        dyn_main(out, state, command);
    }
    h11_native_step = 100u;

    free(initial);
    free(state);
    free(out);
    free(command);
    free(shared);
    return 0;
}

int __libc_start_main(H11Main main_function, int argc, char **argv,
                      void (*init)(void), void (*fini)(void),
                      void (*rtld_fini)(void), void *stack_end)
{
    H11LibcStartMain real_start_main;
    void *real_start_main_symbol;

    (void)main_function;
    real_start_main_symbol = dlsym(RTLD_NEXT, "__libc_start_main");
    memcpy(&real_start_main, &real_start_main_symbol, sizeof(real_start_main));
    if (real_start_main == NULL) {
        return 127;
    }
    return real_start_main(h11_native_main, argc, argv, init, fini, rtld_fini, stack_end);
}
