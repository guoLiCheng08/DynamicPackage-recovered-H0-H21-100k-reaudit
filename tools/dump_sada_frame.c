#include "c_shadow_state_ipc.h"

#include <stdio.h>
#include <stdlib.h>

static int read_frame(FILE *input, unsigned row, DpCShadowStateFrame *frame)
{
    for (unsigned index = 0u; index < row; ++index) {
        if (fread(frame, sizeof(*frame), 1u, input) != 1u) return 0;
    }
    return 1;
}

static void print_sada(const char *name, const DpCShadowStateFrame *frame)
{
    const DpSadaRecovered *sada = (const DpSadaRecovered *)(frame->device_globals + 0x1020u);

    printf("%s 序号=%u flag=%u cmd=[%.17g,%.17g] angle=[%.17g,%.17g] "
           "omega=[%.17g,%.17g] acc=[%.17g,%.17g] limit=[%.17g,%.17g] "
           "acc_limit=[%.17g,%.17g]\n",
           name, frame->input_sequence, sada->command_flag,
           sada->command_angle[0], sada->command_angle[1],
           sada->current_angle[0], sada->current_angle[1],
           sada->angular_velocity[0], sada->angular_velocity[1],
           sada->angular_acceleration[0], sada->angular_acceleration[1],
           sada->command_limit[0], sada->command_limit[1],
           sada->acceleration_limit[0], sada->acceleration_limit[1]);
}

int main(int argc, char **argv)
{
    FILE *c_file;
    FILE *elf_file;
    DpCShadowStateFrame c_frame;
    DpCShadowStateFrame elf_frame;
    unsigned row;

    if (argc != 4 || (row = (unsigned)strtoul(argv[3], NULL, 10)) == 0u) return 2;
    c_file = fopen(argv[1], "rb");
    elf_file = fopen(argv[2], "rb");
    if (c_file == NULL || elf_file == NULL ||
        !read_frame(c_file, row, &c_frame) || !read_frame(elf_file, row, &elf_frame)) return 2;
    print_sada("C", &c_frame);
    print_sada("ELF", &elf_frame);
    return 0;
}
