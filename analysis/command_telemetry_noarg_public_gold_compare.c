#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_ipc_telemetry.h"
#include "dynamic_recovered.h"

static int compare_gold(const char *label, const void *actual, size_t byte_count,
                        const char *gold_path)
{
    unsigned char expected[sizeof(int32_t)];
    FILE *file;
    size_t index;

    if (byte_count > sizeof(expected)) return 1;
    file = fopen(gold_path, "rb");
    if (file == NULL) {
        (void)fprintf(stderr, "%s: unable to open %s\n", label, gold_path);
        return 1;
    }
    if (fread(expected, 1u, byte_count, file) != byte_count || fgetc(file) != EOF) {
        (void)fclose(file);
        (void)fprintf(stderr, "%s: invalid gold size in %s\n", label, gold_path);
        return 1;
    }
    (void)fclose(file);
    if (memcmp(actual, expected, byte_count) == 0) return 0;
    for (index = 0u; index < byte_count; ++index) {
        const unsigned char *actual_bytes = actual;
        if (actual_bytes[index] != expected[index]) {
            (void)fprintf(stderr, "%s: byte %zu mismatch: got=%#x expected=%#x\n",
                          label, index, actual_bytes[index], expected[index]);
            return 1;
        }
    }
    return 1;
}

int main(void)
{
    unsigned char algorithm_sentinel = 0xa5u;
    unsigned char dyn_sentinel = 0x5au;
    int32_t telemetry_1;
    int32_t telemetry_2;

    Algorithm_Command_Execute();
    if (compare_gold("Algorithm_Command_Execute", &algorithm_sentinel,
                     sizeof(algorithm_sentinel),
                     "analysis/time_orbit/algorithm_command_execute_gold.bin") != 0) {
        return 1;
    }
    DYN_Command_Execute();
    if (compare_gold("DYN_Command_Execute", &dyn_sentinel, sizeof(dyn_sentinel),
                     "analysis/time_orbit/dyn_command_execute_gold.bin") != 0) {
        return 1;
    }
    telemetry_1 = Set_Algorithm_Telemetry_Frame_1();
    if (compare_gold("Set_Algorithm_Telemetry_Frame_1", &telemetry_1,
                     sizeof(telemetry_1),
                     "analysis/time_orbit/set_algorithm_telemetry_frame_1_gold.bin") != 0) {
        return 1;
    }
    telemetry_2 = Set_Algorithm_Telemetry_Frame_2();
    if (compare_gold("Set_Algorithm_Telemetry_Frame_2", &telemetry_2,
                     sizeof(telemetry_2),
                     "analysis/time_orbit/set_algorithm_telemetry_frame_2_gold.bin") != 0) {
        return 1;
    }
    (void)puts("PASS: no-argument command and telemetry wrappers controlled original-ELF gold compare");
    return 0;
}
