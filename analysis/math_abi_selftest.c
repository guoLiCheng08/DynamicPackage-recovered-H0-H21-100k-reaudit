#include "../src/dynamic_math.h"

#include <math.h>
#include <stdio.h>

static int nearly_equal(double left, double right)
{
    return fabs(left - right) < 1e-12;
}

int main(void)
{
    double a_data[3] = {1.0, 2.0, 3.0};
    double b_data[3] = {4.0, 5.0, 6.0};
    double c_data[3] = {0.0, 0.0, 0.0};
    double matrix_data[6] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    double x_data[3] = {1.0, 0.0, -1.0};
    double y_data[2] = {2.0, 3.0};
    double xyz[3] = {2.0, 0.0, 0.0};
    double identity_xyz[3] = {0.0, 0.0, 0.0};
    double derivative_xyz[3] = {0.0, 0.0, 0.0};
    double angular_rate_data[3] = {1.0, 2.0, 3.0};
    DpVector a = {3, 0, a_data};
    DpVector b = {3, 0, b_data};
    DpVector c = {3, 0, c_data};
    DpMatrix matrix = {2, 3, 3, 0, matrix_data};
    DpVector x = {3, 0, x_data};
    DpVector y = {2, 0, y_data};
    DpQuatAbi q = {2.0, {3, 0, xyz}};
    DpQuatAbi identity_q = {1.0, {3, 0, identity_xyz}};
    DpQuatAbi derivative_q = {0.0, {3, 0, derivative_xyz}};
    DpVector angular_rate = {3, 0, angular_rate_data};

    if (vector3_cross(&a, &b, &c) != 0 ||
        !nearly_equal(c.data[0], -3.0) || !nearly_equal(c.data[1], 6.0) ||
        !nearly_equal(c.data[2], -3.0)) return 1;
    if (blas_gemv(&matrix, &x, &y, 1.0, 1.0) != 0 ||
        !nearly_equal(y.data[0], 0.0) || !nearly_equal(y.data[1], 1.0)) return 2;
    quat_reunit(&q);
    if (!nearly_equal(q.w, sqrt(0.5)) || !nearly_equal(q.xyz.data[0], sqrt(0.5))) return 3;
    quat_diff(&derivative_q, &identity_q, &angular_rate);
    if (!nearly_equal(derivative_q.xyz.data[0], 0.5) ||
        !nearly_equal(derivative_q.xyz.data[1], 1.0) ||
        !nearly_equal(derivative_q.xyz.data[2], 1.5) ||
        !nearly_equal(derivative_q.w, 0.0)) return 4;

    puts("dynamic_math selftest: PASS");
    return 0;
}
