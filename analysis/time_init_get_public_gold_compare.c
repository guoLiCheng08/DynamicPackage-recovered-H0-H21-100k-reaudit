#include <stdio.h>
#include <string.h>

#include "dynamic_time.h"

static int load_gold(const char *path, void *destination, size_t bytes)
{
    FILE *file = fopen(path, "rb");
    size_t count;

    if (file == NULL) {
        (void)fprintf(stderr, "cannot open gold: %s\n", path);
        return 1;
    }
    count = fread(destination, 1u, bytes, file);
    if (fclose(file) != 0 || count != bytes) {
        (void)fprintf(stderr, "invalid gold: %s\n", path);
        return 1;
    }
    return 0;
}

int main(void)
{
    double calendar[6];
    double expected_init[6];
    double expected_get[6];
    double expected_total;
    double actual_total;

    if (load_gold("analysis/time_orbit/timeinit_gold.bin", expected_init, sizeof(expected_init)) != 0 ||
        load_gold("analysis/time_orbit/timearrayget_gold.bin", expected_get, sizeof(expected_get)) != 0 ||
        load_gold("analysis/time_orbit/timetotalget_gold.bin", &expected_total, sizeof(expected_total)) != 0) {
        return 1;
    }
    TimeInit(2024.0, 10.0, 28.0, 4.0, 16.0, 16.25);
    TimeArrayGet(calendar);
    actual_total = TimeTotalGet();
    if (memcmp(calendar, expected_init, sizeof(calendar)) != 0 ||
        memcmp(calendar, expected_get, sizeof(calendar)) != 0 ||
        memcmp(&actual_total, &expected_total, sizeof(actual_total)) != 0) {
        (void)puts("TimeInit/TimeArrayGet/TimeTotalGet original-ELF gold mismatch");
        return 1;
    }
    (void)puts("PASS: TimeInit / TimeArrayGet / TimeTotalGet controlled original-ELF gold compare");
    return 0;
}
