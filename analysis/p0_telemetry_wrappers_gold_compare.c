/* P0 遥测封装：四个原 ELF 公共 ABI 的联合位级比较。 */
#include "dynamic_ipc_telemetry.h"

#include <stdio.h>
#include <string.h>

#define GOLD_DIR "analysis/coverage_inventory/p0_telemetry/"

static int read_exact(const char *name, unsigned char expected[4])
{
    char path[256];
    FILE *fp;
    int ok = 0;

    if (snprintf(path, sizeof(path), "%s%s", GOLD_DIR, name) < 0) {
        return 0;
    }
    fp = fopen(path, "rb");
    if (fp != NULL) {
        ok = fread(expected, 1, 4, fp) == 4 && fgetc(fp) == EOF;
        (void)fclose(fp);
    }
    return ok;
}

static int compare_i32(const char *label, const char *gold_name, int32_t actual)
{
    unsigned char expected[4];
    unsigned char observed[4];
    size_t i;
    size_t differences = 0;

    if (!read_exact(gold_name, expected)) {
        (void)fprintf(stderr, "cannot read %s%s\n", GOLD_DIR, gold_name);
        return 2;
    }
    memcpy(observed, &actual, sizeof(observed));
    for (i = 0; i < sizeof(observed); ++i) {
        differences += expected[i] != observed[i];
    }
    (void)printf("telemetry wrapper P0 %s mismatched bytes: %zu/4\n", label, differences);
    return differences == 0 ? 0 : 1;
}

int main(void)
{
    int failed = 0;

    dp_algorithm_telemetry_size_set_for_test(INT32_C(0x12345678));
    Update_Telemetry_Frame_1();
    failed |= compare_i32("Update_Telemetry_Frame_1 size",
                          "gold_update_telemetry_frame_1_size.bin",
                          dp_algorithm_telemetry_size_get());

    dp_algorithm_telemetry_size_set_for_test(INT32_C(0x7f7f7f7f));
    Update_Telemetry_Frame_2();
    failed |= compare_i32("Update_Telemetry_Frame_2 size",
                          "gold_update_telemetry_frame_2_size.bin",
                          dp_algorithm_telemetry_size_get());

    dp_algorithm_telemetry_size_set_for_test(INT32_C(0x2468ace0));
    failed |= compare_i32("Set_Algorithm_Telemetry_Frame_1 size",
                          "gold_set_telemetry_frame_1_size.bin",
                          dp_algorithm_telemetry_size_get());
    failed |= compare_i32("Set_Algorithm_Telemetry_Frame_1 return",
                          "gold_set_telemetry_frame_1_ret.bin",
                          Set_Algorithm_Telemetry_Frame_1());

    dp_algorithm_telemetry_size_set_for_test(INT32_C(0x13579bdf));
    failed |= compare_i32("Set_Algorithm_Telemetry_Frame_2 size",
                          "gold_set_telemetry_frame_2_size.bin",
                          dp_algorithm_telemetry_size_get());
    failed |= compare_i32("Set_Algorithm_Telemetry_Frame_2 return",
                          "gold_set_telemetry_frame_2_ret.bin",
                          Set_Algorithm_Telemetry_Frame_2());

    (void)printf("P0 telemetry wrappers original-ELF compare: %s (bitwise)\n",
                 failed == 0 ? "PASS" : "FAIL");
    return failed == 0 ? 0 : 1;
}
