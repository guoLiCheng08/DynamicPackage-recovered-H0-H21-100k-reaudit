#include <stdio.h>

#include "dynamic_core_bridge.h"

static void print_vector(const char *name, const double values[10])
{
    unsigned index;
    printf("%s\n", name);
    for (index = 0u; index < 10u; ++index) printf(" % .17g", values[index]);
    putchar('\n');
}

int main(void)
{
    DpCoreDefaultModel model;
    const char *names[6] = {"m3_e8", "m3_448", "m3_4a8", "m3_508", "m3_568", "m9"};
    DpMatrix matrices[6];
    double command_data[3] = {0.0, 0.01, -0.01};
    double acceleration_data[3] = {0.0, 1.7453292519943296e-05, -8.726646259971648e-06};
    DpVector command = {3, 0, command_data};
    DpVector acceleration = {3, 0, acceleration_data};
    double transpose_data[30] = {0.0};
    DpMatrix transpose = {10, 3, 3, 0, transpose_data};
    unsigned index;

    if (dp_core_default_model_init(&model) != 0 ||
        matrix_trans(&transpose, &model.coupling) != 0) return 1;
    matrices[0] = (DpMatrix){3, 3, 3, 0, model.m3_e8};
    matrices[1] = (DpMatrix){3, 3, 3, 0, model.m3_448};
    matrices[2] = (DpMatrix){3, 3, 3, 0, model.m3_4a8};
    matrices[3] = (DpMatrix){3, 3, 3, 0, model.m3_508};
    matrices[4] = (DpMatrix){3, 3, 3, 0, model.m3_568};
    matrices[5] = model.sada_command_momentum_map;
    for (index = 0u; index < 6u; ++index) {
        double mapped_data[3] = {0.0, 0.0, 0.0};
        double modal_data[10] = {0.0};
        DpVector mapped = {3, 0, mapped_data};
        DpVector modal = {10, 0, modal_data};
        char label[64];
        if (blas_gemv(&matrices[index], &command, &mapped, 1.0, 0.0) != 0 ||
            blas_gemv(&transpose, &mapped, &modal, -1.0, 0.0) != 0) return 1;
        (void)snprintf(label, sizeof(label), "-C^T*%s*command", names[index]);
        print_vector(label, modal_data);
        if (blas_gemv(&matrices[index], &acceleration, &mapped, 1.0, 0.0) != 0 ||
            blas_gemv(&transpose, &mapped, &modal, -1.0, 0.0) != 0) return 1;
        (void)snprintf(label, sizeof(label), "-C^T*%s*acceleration", names[index]);
        print_vector(label, modal_data);
    }
    {
        double m6_transpose_data[30] = {0.0};
        double modal_data[10] = {0.0};
        DpMatrix m6_transpose = {10, 3, 3, 0, m6_transpose_data};
        DpVector modal = {10, 0, modal_data};
        if (matrix_trans(&m6_transpose, &(DpMatrix){3, 10, 10, 0, model.m6}) != 0 ||
            blas_gemv(&m6_transpose, &acceleration, &modal, -1.0, 0.0) != 0) return 1;
        print_vector("-Sat+0x688^T*acceleration", modal_data);
    }
    return 0;
}
