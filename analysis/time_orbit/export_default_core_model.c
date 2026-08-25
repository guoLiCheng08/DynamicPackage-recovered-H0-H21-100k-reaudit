#include <stdio.h>

#include "dynamic_core_bridge.h"

int main(void)
{
    DpCoreDefaultModel model;
    FILE *file;

    if (dp_core_default_model_init(&model) != 0) return 1;
    file = fopen("/tmp/e10_recovered_default_model.bin", "wb");
    if (file == NULL) return 1;
    if (fwrite(model.inertia, sizeof(double), 9u, file) != 9u ||
        fwrite(model.inverse, sizeof(double), 9u, file) != 9u ||
        fwrite(model.m3_e8, sizeof(double), 9u, file) != 9u ||
        fwrite(model.m3_448, sizeof(double), 9u, file) != 9u ||
        fwrite(model.m3_4a8, sizeof(double), 9u, file) != 9u ||
        fwrite(model.m3_568, sizeof(double), 9u, file) != 9u ||
        fwrite(model.m6, sizeof(double), 30u, file) != 30u ||
        fwrite(model.coupling_data, sizeof(double), 30u, file) != 30u ||
        fwrite(model.m9, sizeof(double), 9u, file) != 9u ||
        fwrite(model.modal_a_data, sizeof(double), 100u, file) != 100u ||
        fwrite(model.modal_d_data, sizeof(double), 100u, file) != 100u ||
        fclose(file) != 0) return 1;
    return 0;
}
