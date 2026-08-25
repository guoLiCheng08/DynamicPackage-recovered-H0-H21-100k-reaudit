#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_core_layout.h"
#include "dynamic_ipc_telemetry.h"

static const uint64_t expected_words[15] = {
    UINT64_C(0xa5a5a5a500000009), UINT64_C(0x3ff4000000000000),
    UINT64_C(0xc004000000000000), UINT64_C(0x400e000000000000),
    UINT64_C(0xc010800000000000), UINT64_C(0x3fc0000000000000),
    UINT64_C(0xbfd0000000000000), UINT64_C(0x3fe0000000000000),
    UINT64_C(0xbff0000000000000), UINT64_C(0x4000000000000000),
    UINT64_C(0xc008000000000000), UINT64_C(0xa5a5a5a500000007),
    UINT64_C(0x3fb0000000000000), UINT64_C(0xbfa0000000000000),
    UINT64_C(0xa5a5a5a500000005)
};

int main(void)
{
    static const float input_floats[12] = {
        1.25f, -2.5f, 3.75f, -4.125f, 0.125f, -0.25f,
        0.5f, -1.0f, 2.0f, -3.0f, 0.0625f, -0.03125f
    };
    unsigned char frame[0x78];
    unsigned index;
    int dummy_0 = 0;
    int dummy_1 = 0;

    for (index = 0u; index < 12u; ++index) {
        if (dp_ipc_seed_float_for_test(index, input_floats[index]) != 0) {
            (void)puts("getDynInput unable to seed float input");
            return 1;
        }
    }
    if (dp_ipc_seed_u8_for_test(0u, 9u) != 0 ||
        dp_ipc_seed_u8_for_test(1u, 7u) != 0 ||
        dp_ipc_seed_u8_for_test(2u, 5u) != 0) {
        (void)puts("getDynInput unable to seed u8 input");
        return 1;
    }
    memset(frame, 0xa5, sizeof(frame));
    getDynInput(&dummy_0, &dummy_1, frame);
    if (memcmp(frame, expected_words, sizeof(frame)) != 0) {
        (void)puts("getDynInput 0x78-byte command frame mismatch");
        return 1;
    }
    (void)puts("PASS: getDynInput controlled original-ELF gold compare");
    return 0;
}
