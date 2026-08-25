#include <stdio.h>
#include <string.h>

#include "dynamic_satellite_globals.h"

static int load_gold(const char *path, double output[3])
{
    FILE *file = fopen(path, "rb");
    size_t count;

    if (file == NULL) {
        (void)fprintf(stderr, "cannot open gold: %s\n", path);
        return 1;
    }
    count = fread(output, 1u, 3u * sizeof(double), file);
    if (fclose(file) != 0 || count != 3u * sizeof(double)) {
        (void)fprintf(stderr, "invalid gold: %s\n", path);
        return 1;
    }
    return 0;
}

int main(void)
{
    double torque_in_data[3] = {1.25, -2.5, 3.75};
    double torque_out_data[3] = {0.0, 0.0, 0.0};
    double moment_in_data[3] = {-4.5, 5.25, -6.75};
    double moment_out_data[3] = {0.0, 0.0, 0.0};
    double expected_set_torque[3];
    double expected_get_torque[3];
    double expected_set_moment[3];
    double expected_get_moment[3];
    DpVector torque_in = {3, 0, torque_in_data};
    DpVector torque_out = {3, 0, torque_out_data};
    DpVector moment_in = {3, 0, moment_in_data};
    DpVector moment_out = {3, 0, moment_out_data};

    if (load_gold("analysis/time_orbit/settorque_gold.bin", expected_set_torque) != 0 ||
        load_gold("analysis/time_orbit/gettorque_gold.bin", expected_get_torque) != 0 ||
        load_gold("analysis/time_orbit/setwheelangularmoment_gold.bin", expected_set_moment) != 0 ||
        load_gold("analysis/time_orbit/getwheelangularmoment_gold.bin", expected_get_moment) != 0) {
        return 1;
    }
    SetTorque(&torque_in);
    GetTorque(&torque_out);
    SetWheelAngularMoment(&moment_in);
    GetWheelAngularMoment(&moment_out);
    if (memcmp(torque_out_data, expected_set_torque, sizeof(torque_out_data)) != 0 ||
        memcmp(torque_out_data, expected_get_torque, sizeof(torque_out_data)) != 0 ||
        memcmp(moment_out_data, expected_set_moment, sizeof(moment_out_data)) != 0 ||
        memcmp(moment_out_data, expected_get_moment, sizeof(moment_out_data)) != 0 ||
        torque_in.count != 3 || torque_out.count != 3 ||
        moment_in.count != 3 || moment_out.count != 3) {
        (void)puts("torque/moment original-ELF gold mismatch");
        return 1;
    }
    (void)puts("PASS: SetTorque / GetTorque / SetWheelAngularMoment / GetWheelAngularMoment controlled original-ELF gold compare");
    return 0;
}
