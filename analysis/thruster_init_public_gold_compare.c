#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_devices.h"

static int pointer_offset_matches(const void *base, const void *pointer,
                                  size_t expected, const char *label)
{
    const uintptr_t actual_offset = (uintptr_t)pointer - (uintptr_t)base;

    if (actual_offset != expected) {
        (void)printf("%s offset=%#llx expected=%#llx\n", label,
                     (unsigned long long)actual_offset,
                     (unsigned long long)expected);
        return -1;
    }
    return 0;
}

int main(void)
{
    static const uint64_t scale_bits = UINT64_C(0x1122334455667788);
    uint64_t actual_scale_bits;
    unsigned char *base = (unsigned char *)&Thruster;

    memset(&Thruster, 0, sizeof(Thruster));
    memcpy(&Thruster.force_scale, &scale_bits, sizeof(scale_bits));
    Thruster.work_status = UINT32_C(0x12345678);
    Thruster.lever_arm = (DpVector){3, 0, NULL};
    Thruster.force_input = (DpVector){3, 0, NULL};
    Thruster.force_output = (DpVector){3, 0, NULL};
    Thruster.torque_output = (DpVector){3, 0, NULL};
    ((uint64_t *)(void *)(base + 0x20u))[0] = UINT64_C(0x3ff0000000000000);
    ((uint64_t *)(void *)(base + 0x48u))[1] = UINT64_C(0xbff0000000000000);
    ((uint64_t *)(void *)(base + 0x70u))[2] = UINT64_C(0x7ff8000000001234);
    ((uint64_t *)(void *)(base + 0x98u))[0] = UINT64_C(0x8000000000000000);

    Thruster_Init();
    memcpy(&actual_scale_bits, &Thruster.force_scale, sizeof(actual_scale_bits));
    if (actual_scale_bits != scale_bits || Thruster.work_status != UINT32_C(0x12345678) ||
        Thruster.lever_arm.count != 3 || Thruster.force_input.count != 3 ||
        Thruster.force_output.count != 3 || Thruster.torque_output.count != 3 ||
        pointer_offset_matches(base, Thruster.lever_arm.data, 0x20u, "lever") != 0 ||
        pointer_offset_matches(base, Thruster.force_input.data, 0x48u, "force_input") != 0 ||
        pointer_offset_matches(base, Thruster.force_output.data, 0x70u, "force_output") != 0 ||
        pointer_offset_matches(base, Thruster.torque_output.data, 0x98u, "torque_output") != 0 ||
        ((const uint64_t *)(const void *)(base + 0x20u))[0] != UINT64_C(0x3ff0000000000000) ||
        ((const uint64_t *)(const void *)(base + 0x48u))[1] != UINT64_C(0xbff0000000000000) ||
        ((const uint64_t *)(const void *)(base + 0x70u))[2] != UINT64_C(0x7ff8000000001234) ||
        ((const uint64_t *)(const void *)(base + 0x98u))[0] != UINT64_C(0x8000000000000000)) {
        (void)puts("Thruster_Init controlled original-ELF gold mismatch");
        return 1;
    }
    (void)puts("PASS: Thruster_Init controlled original-ELF gold compare");
    return 0;
}
