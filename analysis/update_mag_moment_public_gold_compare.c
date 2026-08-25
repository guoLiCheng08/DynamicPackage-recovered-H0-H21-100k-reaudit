#include <stdio.h>
#include <string.h>

#include "dynamic_devices.h"

static int read_doubles(const char *path, double *out, unsigned count)
{
    FILE *file = fopen(path, "rb");
    return file != NULL && fread(out, sizeof(*out), count, file) == count && fclose(file) == 0;
}

int main(void)
{
    static const double mapping[18] = {
        1.0, -2.0, 0.5, 3.0, -1.0, 2.0,
        -0.25, 1.5, 2.0, -3.0, 0.75, 1.25,
        4.0, -1.5, 0.25, 0.5, 2.5, -2.0
    };
    double expected_channel[6];
    double expected_group[3];
    unsigned index;

    if (!read_doubles("analysis/coverage_inventory/gold_update_mag_moment_p1_channel.bin", expected_channel, 6u) ||
        !read_doubles("analysis/coverage_inventory/gold_update_mag_moment_p1_group.bin", expected_group, 3u)) return 2;
    dp_device_globals_reset();
    for (index = 0; index < 6u; ++index) MTQ[index].actual_moment = (index + 1u) * 1.25;
    memcpy(MTQ_Group.mapping_3x6.data, mapping, sizeof(mapping));
    UpdateMagMoment();
    if (memcmp(MTQ_Group.channel_moment.data, expected_channel, sizeof(expected_channel)) != 0 ||
        memcmp(MTQ_Group.group_moment.data, expected_group, sizeof(expected_group)) != 0) return 1;
    puts("UpdateMagMoment public ABI original-ELF compare: PASS (bitwise, 6-channel copy + 3x6 map)");
    return 0;
}
