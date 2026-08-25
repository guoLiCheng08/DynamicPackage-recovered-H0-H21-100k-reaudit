#include <stdio.h>
#include <string.h>

#include "dynamic_satellite_globals.h"

int main(void)
{
    DpSatOpaque before;

    SatParaInit();
    memcpy(&before, &Sat, sizeof(before));
    UpdateSatellite();
    if (memcmp(&Sat, &before, sizeof(before)) != 0) {
        (void)puts("UpdateSatellite observable Sat state mismatch");
        return 1;
    }
    (void)puts("PASS: UpdateSatellite controlled original-ELF gold compare");
    return 0;
}
