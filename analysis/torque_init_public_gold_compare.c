#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_satellite_globals.h"

static int is_pointer_slot(size_t offset)
{
    static const size_t slots[9] = {0x08u, 0x18u, 0x28u, 0x38u, 0x48u,
                                    0x58u, 0x68u, 0x78u, 0x148u};
    size_t index;

    for (index = 0u; index < sizeof(slots) / sizeof(slots[0]); ++index) {
        if (offset >= slots[index] && offset < slots[index] + sizeof(void *)) {
            return 1;
        }
    }
    return 0;
}

int main(void)
{
    static const size_t slots[9] = {0x08u, 0x18u, 0x28u, 0x38u, 0x48u,
                                    0x58u, 0x68u, 0x78u, 0x148u};
    static const size_t expected_offsets[9] = {0x80u, 0x98u, 0xb0u, 0xc8u, 0xe0u,
                                               0xf8u, 0x110u, 0x128u, 0x150u};
    size_t index;

    memset(&SatTorque, 0xa5, sizeof(SatTorque));
    TorqueInit();
    for (index = 0u; index < sizeof(slots) / sizeof(slots[0]); ++index) {
        void *pointer;

        memcpy(&pointer, &SatTorque.raw[slots[index]], sizeof(pointer));
        if ((const unsigned char *)pointer != &SatTorque.raw[expected_offsets[index]]) {
            (void)printf("TorqueInit pointer[%zu] mismatch\n", index);
            return 1;
        }
    }
    for (index = 0u; index < sizeof(SatTorque.raw); ++index) {
        if (is_pointer_slot(index) == 0 && SatTorque.raw[index] != 0xa5u) {
            (void)printf("TorqueInit unexpectedly modified raw+%#zx\n", index);
            return 1;
        }
    }
    (void)puts("PASS: TorqueInit controlled original-ELF gold compare");
    return 0;
}
