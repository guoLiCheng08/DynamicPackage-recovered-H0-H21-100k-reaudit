#include "dynamic_ipc_telemetry.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
    float expected = -12.75f;
    float actual = 0.0f;
    uint8_t byte = 0;
    int failed = 0;

    failed |= set_float_value(0x257, expected) != 0;
    failed |= get_float_value(0x257, &actual) != 0;
    failed |= memcmp(&expected, &actual, sizeof(expected)) != 0;
    failed |= set_uint8_value(0x257, 0xa7u) != 0;
    failed |= get_uint8_value(0x257, &byte) != 0 || byte != 0xa7u;
    failed |= set_float_value(0x258, 1.0f) != -11;
    failed |= set_uint8_value(0x258, 0u) != -11;
    close_shared();
    (void)printf("P1 public IPC setters ABI compare: %s\n", failed == 0 ? "PASS" : "FAIL");
    return failed == 0 ? 0 : 1;
}
