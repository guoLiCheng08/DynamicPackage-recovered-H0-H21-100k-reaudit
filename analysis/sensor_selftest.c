#include "../src/dynamic_sensors.h"

#include <stdio.h>
#include <string.h>

static double no_noise(double mean, double sigma, void *opaque)
{
    (void)mean;
    (void)sigma;
    (void)opaque;
    return 0.0;
}

static void set_identity_3(DpMatrix *matrix, double storage[9])
{
    memset(storage, 0, 9u * sizeof(double));
    storage[0] = 1.0;
    storage[4] = 1.0;
    storage[8] = 1.0;
    matrix->rows = 3;
    matrix->cols = 3;
    matrix->row_stride = 3;
    matrix->reserved_0c = 0;
    matrix->data = storage;
}

int main(void)
{
    DpGyroRecovered gyros[2] = {0};
    DpMagMeterRecovered meters[2] = {0};
    double gyro_matrix[2][9];
    double gyro_output[2][3] = {{0}};
    double mag_matrix[2][9];
    double mag_output[2][3] = {{0}};
    DpVec3 rate = {1.0, 2.0, 3.0};
    DpVec3 magnetic = {-1.0, 4.0, 2.0};
    unsigned i;

    for (i = 0; i < 2u; ++i) {
        set_identity_3(&gyros[i].projection_matrix, gyro_matrix[i]);
        gyros[i].measure.count = 3;
        gyros[i].measure.data = gyro_output[i];
        set_identity_3(&meters[i].projection_matrix, mag_matrix[i]);
        meters[i].measure.count = 3;
        meters[i].measure.data = mag_output[i];
    }
    dp_update_gyro(gyros, &rate, no_noise, NULL);
    dp_update_magmeter(meters, &magnetic, no_noise, NULL);
    if (gyro_output[1][0] != 1.0 || gyro_output[1][1] != 2.0 || gyro_output[1][2] != 3.0) return 1;
    if (mag_output[0][0] != -1.0 || mag_output[0][1] != 4.0 || mag_output[0][2] != 2.0) return 2;
    puts("dynamic_sensors selftest: PASS");
    return 0;
}
