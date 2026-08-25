#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_satellite_globals.h"

static const size_t pointer_offsets[25] = {
    0x018u, 0x078u, 0x0d0u, 0x0f8u, 0x120u, 0x148u,
    0x1b8u, 0x218u, 0x278u, 0x2d8u, 0x338u, 0x398u,
    0x3f8u, 0x458u, 0x4b8u, 0x518u, 0x578u, 0x5d8u,
    0x638u, 0x698u, 0x7a0u, 0x8a8u, 0x9b0u, 0xa10u,
    0xd48u
};
static const size_t expected_backing_offsets[25] = {
    0x020u, 0x080u, 0x0d8u, 0x100u, 0x128u, 0x150u,
    0x1c0u, 0x220u, 0x280u, 0x2e0u, 0x340u, 0x3a0u,
    0x400u, 0x460u, 0x4c0u, 0x520u, 0x580u, 0x5e0u,
    0x640u, 0x6a0u, 0x7a8u, 0x8b0u, 0x9b8u, 0xa18u,
    0xd50u
};

static int is_pointer_byte(size_t offset)
{
    size_t index;

    for (index = 0u; index < sizeof(pointer_offsets) / sizeof(pointer_offsets[0]); ++index) {
        if (offset >= pointer_offsets[index] &&
            offset < pointer_offsets[index] + sizeof(void *)) {
            return 1;
        }
    }
    return 0;
}

int main(void)
{
    size_t index;

    memset(&Sat, 0xa5, sizeof(Sat));
    SatParaInit();
    for (index = 0u; index < sizeof(pointer_offsets) / sizeof(pointer_offsets[0]); ++index) {
        void *pointer;

        memcpy(&pointer, &Sat.raw[pointer_offsets[index]], sizeof(pointer));
        if ((const unsigned char *)pointer != &Sat.raw[expected_backing_offsets[index]]) {
            (void)printf("SatParaInit pointer[%zu] mismatch\n", index);
            return 1;
        }
    }
    for (index = 0u; index < sizeof(Sat.raw); ++index) {
        if (is_pointer_byte(index) == 0 && Sat.raw[index] != 0xa5u) {
            (void)printf("SatParaInit unexpectedly modified raw+%#zx\n", index);
            return 1;
        }
    }
    (void)puts("PASS: SatParaInit controlled original-ELF gold compare");
    return 0;
}
