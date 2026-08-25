#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "dynamic_devices.h"
#include "dynamic_satellite_globals.h"

static int read_gold(double expected[6])
{
    FILE *file = fopen("analysis/coverage_inventory/gold_drive_sada_once_p1_state.bin", "rb");
    return file != NULL && fread(expected, sizeof(*expected), 6u, file) == 6u && fclose(file) == 0;
}

int main(void)
{
    static const char expected_stdout[] =
        " 0: drive_flag=7, drive_value=85.943669\n"
        " 1: drive_flag=7, drive_value=-71.619724\n"
        "SADA A: angle=7.8782 deg, omega=8.5944 deg/s, acc=22.918312 deg/s^2\n"
        "SADA B: angle=-15.0401 deg, omega=-14.3239 deg/s, acc=-34.377468 deg/s^2\n";
    double expected[6];
    double actual[6];
    char output[sizeof(expected_stdout) + 1u];
    FILE *capture;
    int saved_stdout;
    size_t output_size;

    if (!read_gold(expected)) return 2;
    dp_device_globals_reset();
    SADA.command_flag = 7u;
    SADA.command_angle[0] = 1.5;
    SADA.command_angle[1] = -1.25;
    SADA.current_angle[0] = 0.1;
    SADA.current_angle[1] = -0.2;
    SADA.angular_velocity[0] = 0.05;
    SADA.angular_velocity[1] = -0.10;
    SADA.command_limit[0] = 1.0;
    SADA.command_limit[1] = 1.0;
    SADA.acceleration_limit[0] = 0.4;
    SADA.acceleration_limit[1] = 0.6;

    if (fflush(stdout) != 0 || (capture = tmpfile()) == NULL || (saved_stdout = dup(STDOUT_FILENO)) < 0 ||
        dup2(fileno(capture), STDOUT_FILENO) < 0) return 2;
    drive_SADA_once(0.25);
    if (fflush(stdout) != 0 || dup2(saved_stdout, STDOUT_FILENO) < 0 || close(saved_stdout) != 0) return 2;
    if (fseek(capture, 0L, SEEK_END) != 0 || (output_size = (size_t)ftell(capture)) != sizeof(expected_stdout) - 1u ||
        fseek(capture, 0L, SEEK_SET) != 0 || fread(output, 1u, output_size, capture) != output_size || fclose(capture) != 0) return 2;
    output[output_size] = '\0';
    actual[0] = SADA.current_angle[0];
    actual[1] = SADA.current_angle[1];
    actual[2] = SADA.angular_velocity[0];
    actual[3] = SADA.angular_velocity[1];
    actual[4] = SADA.angular_acceleration[0];
    actual[5] = SADA.angular_acceleration[1];
    if (memcmp(actual, expected, sizeof(expected)) != 0 ||
        memcmp(output, expected_stdout, sizeof(expected_stdout)) != 0) return 1;
    puts("drive_SADA_once public ABI original-ELF compare: PASS (bitwise state + exact stdout)");
    return 0;
}
