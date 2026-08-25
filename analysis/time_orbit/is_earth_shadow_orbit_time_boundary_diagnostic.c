#include <math.h>
#include <stdio.h>

#include "dynamic_math.h"
#include "dynamic_sensors.h"

static int load_vector(const char *path, double data[3])
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        return -1;
    }
    if (fread(data, sizeof(double), 3u, file) != 3u || fclose(file) != 0) {
        return -1;
    }
    return 0;
}

int main(void)
{
    double sun_data[3];
    double position_data[3];
    DpVector sun = {3, 0, sun_data};
    DpVector position = {3, 0, position_data};
    double position_norm;
    double earth_limb_angle;
    double separation_angle;

    if (load_vector("analysis/time_orbit/gold_orbit_time_boundary_step1_dss_sun_input.bin", sun_data) != 0 ||
        load_vector("analysis/time_orbit/gold_orbit_time_boundary_step1_dss_position_input.bin", position_data) != 0) {
        return 1;
    }
    position_norm = vector_nrm2(&position);
    earth_limb_angle = acos(6378137.0 / position_norm);
    separation_angle = vector2angle(&sun, &position);
    printf("position_norm=%.17g\n", position_norm);
    printf("earth_limb_angle=%.17g rad %.17g deg\n", earth_limb_angle, earth_limb_angle * 180.0 / M_PI);
    printf("separation_angle=%.17g rad %.17g deg\n", separation_angle, separation_angle * 180.0 / M_PI);
    printf("threshold=%.17g rad %.17g deg\n", earth_limb_angle + M_PI / 2.0,
           (earth_limb_angle + M_PI / 2.0) * 180.0 / M_PI);
    printf("recovered_is_earth_shadow=%d\n", dp_is_earth_shadow(&sun, &position));
    return 0;
}
