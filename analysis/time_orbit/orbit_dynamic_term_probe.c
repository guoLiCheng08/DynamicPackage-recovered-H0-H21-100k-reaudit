#include <math.h>
#include <stdio.h>

int main(void)
{
    const double mu = 398600441500000.0;
    const double re = 6378137.0;
    const double j2 = 0.00108262668355;
    const double j3 = -2.53265648533e-6;
    const double j4 = -1.61962159137e-6;
    const double x = -6525845.0559908431;
    const double y = -2795474.185543275;
    const double z = 2313605.4942796603;
    const double r = sqrt(x * x + y * y + z * z);
    const double ir = 1.0 / r;
    const double xn = x * ir;
    const double yn = y * ir;
    const double zn = z * ir;
    const double q = re * ir;
    const double base = mu / (re * re);
    const double s = zn;
    const double s2 = s * s;
    const double s3 = s2 * s;
    const double s4 = s3 * s;
    double h, v, scale;

    h = 1.0 - 5.0 * s2;
    v = 3.0 - 5.0 * s2;
    scale = base * (-1.5 * j2) * q * q;
    printf("j2=(%.17g, %.17g, %.17g)\n", xn * h * scale, yn * h * scale, zn * v * scale);

    h = (7.0 * s3 - 3.0 * s) * 5.0;
    v = (10.0 * s2 - 11.666666666666666 * s4 - 1.0) * 3.0;
    scale = base * (-0.5 * j3) * q * q * q;
    printf("j3=(%.17g, %.17g, %.17g)\n", xn * h * scale, yn * h * scale, zn * v * scale);

    h = 3.0 - 42.0 * s2 + 63.0 * s4;
    v = 15.0 - 70.0 * s2 + 63.0 * s4;
    scale = base * (-0.625 * j4) * q * q * q * q;
    printf("j4=(%.17g, %.17g, %.17g)\n", xn * h * scale, yn * h * scale, -zn * v * scale);
    return 0;
}
