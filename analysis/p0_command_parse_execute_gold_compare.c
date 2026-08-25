/* P0 命令解析/执行：opcode 0x77、0x66 与未命中分支的原 ELF 比较。 */
#include "dynamic_recovered.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define GOLD_DIR "analysis/coverage_inventory/p0_command_family/"
#define FRAME_SIZE 0x90u
#define TC_SIZE 0x90u

static int read_exact(const char *name, unsigned char *dst, size_t size)
{
    char path[256];
    FILE *fp;
    int ok = 0;

    if (snprintf(path, sizeof(path), "%s%s", GOLD_DIR, name) < 0) {
        return 0;
    }
    fp = fopen(path, "rb");
    if (fp != NULL) {
        ok = fread(dst, 1, size, fp) == size && fgetc(fp) == EOF;
        (void)fclose(fp);
    }
    return ok;
}

static size_t compare_except_payload_pointer(const unsigned char *actual,
                                             const unsigned char *expected)
{
    size_t i;
    size_t differences = 0;
    const unsigned char *payload;

    for (i = 0; i < FRAME_SIZE; ++i) {
        if (i < 0x80u || i >= 0x88u) {
            differences += actual[i] != expected[i];
        }
    }
    memcpy(&payload, actual + 0x80, sizeof(payload));
    differences += payload != actual + 0x0bu;
    return differences;
}

static int compare_frame(const char *label, const char *gold_name,
                         unsigned char frame[FRAME_SIZE])
{
    unsigned char expected[FRAME_SIZE];
    size_t differences;

    if (!read_exact(gold_name, expected, sizeof(expected))) {
        (void)fprintf(stderr, "cannot read %s%s\n", GOLD_DIR, gold_name);
        return 2;
    }
    differences = compare_except_payload_pointer(frame, expected);
    (void)printf("P0 command %s frame differences: %zu\n", label, differences);
    return differences == 0 ? 0 : 1;
}

static int compare_tc(const char *label, const char *gold_name)
{
    unsigned char expected[TC_SIZE];
    size_t i;
    size_t differences = 0;
    const unsigned char *actual = dp_command_tc_bytes_for_test();

    if (!read_exact(gold_name, expected, sizeof(expected))) {
        (void)fprintf(stderr, "cannot read %s%s\n", GOLD_DIR, gold_name);
        return 2;
    }
    for (i = 0; i < TC_SIZE; ++i) {
        differences += actual[i] != expected[i];
    }
    (void)printf("P0 command %s tc_cmd differences: %zu/%u\n",
                 label, differences, TC_SIZE);
    return differences == 0 ? 0 : 1;
}

static void seed_frame(unsigned char frame[FRAME_SIZE], uint8_t b4, uint8_t b5,
                       uint8_t b6, uint8_t b7, uint8_t opcode,
                       uint8_t parameter, uint8_t extra)
{
    memset(frame, 0xa5, FRAME_SIZE);
    frame[4] = b4;
    frame[5] = b5;
    frame[6] = b6;
    frame[7] = b7;
    frame[8] = opcode;
    frame[9] = parameter;
    frame[10] = extra;
}

int main(void)
{
    unsigned char frame[FRAME_SIZE];
    int failed = 0;

    seed_frame(frame, 0x11, 0x22, 0x33, 0x44, 0x77, 0x34, 0x12);
    dp_command_tc_reset_for_test(0x00);
    Analyze_Command(frame);
    failed |= compare_frame("opcode77 Analyze_Command", "gold_analyze_command_p0_opcode77_frame.bin", frame);
    Command_Execute(frame);
    failed |= compare_tc("opcode77 Command_Execute", "gold_command_execute_p0_opcode77_tc.bin");

    seed_frame(frame, 0xaa, 0xbb, 0xcc, 0xdd, 0x66, 0x5e, 0x7f);
    dp_command_tc_reset_for_test(0x00);
    Analyze_Command(frame);
    failed |= compare_frame("opcode66 Analyze_Command", "gold_analyze_command_p0_opcode66_frame.bin", frame);
    Command_Execute(frame);
    failed |= compare_tc("opcode66 Command_Execute", "gold_command_execute_p0_opcode66_tc.bin");

    seed_frame(frame, 0xa5, 0xa5, 0xa5, 0xa5, 0x01, 0xee, 0xa5);
    dp_command_tc_reset_for_test(0x3c);
    Analyze_Command(frame);
    Command_Execute(frame);
    failed |= compare_tc("opcode01 Command_Execute", "gold_command_execute_p0_opcode01_tc.bin");

    (void)printf("P0 Analyze_Command and Command_Execute original-ELF compare: %s\n",
                 failed == 0 ? "PASS" : "FAIL");
    return failed == 0 ? 0 : 1;
}
