#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_devices.h"

int main(void)
{
    uint32_t expected;
    FILE *file = fopen("analysis/coverage_inventory/gold_set_thruster_work_status_p1.bin", "rb");

    if (file == NULL || fread(&expected, sizeof(expected), 1u, file) != 1u || fclose(file) != 0) return 2;
    dp_device_globals_reset();
    Thruster.work_status = 0u;
    SetThrusterWorkStatus(UINT32_C(0xa5a5f00d));
    if (memcmp(&Thruster.work_status, &expected, sizeof(expected)) != 0) return 1;
    puts("SetThrusterWorkStatus public ABI original-ELF compare: PASS (bitwise, 32-bit global store)");
    return 0;
}
