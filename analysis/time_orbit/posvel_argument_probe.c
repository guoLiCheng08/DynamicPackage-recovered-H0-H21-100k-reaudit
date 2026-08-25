#include <math.h>
#include <stdio.h>

int main(void)
{
    const double mu = 398600441500000.0;
    const double rx = -4073306.892903815;
    const double ry = -5851722.1589736138;
    const double rz = 1987412.5991544391;
    const double vx = 3643.5044963902051;
    const double vy = -4634.4388877243673;
    const double vz = -4001.236670030642;
    const double radius = sqrt(rx * rx + ry * ry + rz * rz);
    const double velocity_squared = vx * vx + vy * vy + vz * vz;
    const double radial_dot = rx * vx + ry * vy + rz * vz;
    const double hx = ry * vz - rz * vy;
    const double hy = rz * vx - rx * vz;
    const double hz = rx * vy - ry * vx;
    const double h = sqrt(hx * hx + hy * hy + hz * hz);
    const double a = mu * radius / (2.0 * mu - velocity_squared * radius);
    const double ecos_e = 1.0 - radius / a;
    const double esin_e = radial_dot / sqrt(mu * a);
    const double e = sqrt(ecos_e * ecos_e + esin_e * esin_e);
    const double incl = acos(hz / (sqrt(1.0 - e * e) * sqrt(mu * a)));
    const double ex = ((velocity_squared - mu / radius) * rx - radial_dot * vx) / mu;
    const double ey = ((velocity_squared - mu / radius) * ry - radial_dot * vy) / mu;
    const double ez = ((velocity_squared - mu / radius) * rz - radial_dot * vz) / mu;
    const double n = sqrt(hx * hx + hy * hy);
    const double current_x = (-hy * ex + hx * ey) / (n * e);
    const double current_y = ez / (e * sin(incl));
    const double desired_x = 0.58008394975752287;
    const double desired_y = 0.2802124904132422;

    printf("evec=%a %a %a\n", ex, ey, ez);
    printf("h=%a %a %a hnorm=%a n=%a\n", hx, hy, hz, h, n);
    printf("current projection x=%a y=%a ratio=%a\n", current_x, current_y, current_y / current_x);
    printf("target projection  x=%a y=%a ratio=%a\n", desired_x, desired_y, desired_y / desired_x);
    printf("combined projection x=%a y=%a\n",
           ((-hy) * ex + hx * ey) / (h * e), ez / e);
    {
        const double eccentric_angle = atan(esin_e / ecos_e) + 3.1415926535897931;
        double sin_e;
        double cos_e;
        double candidate_x;
        double candidate_y;
        sincos(eccentric_angle, &sin_e, &cos_e);
        candidate_x = cos_e - e;
        candidate_x *= sqrt(1.0 - e * e);
        candidate_x *= vz;
        candidate_y = sin_e / radius;
        candidate_y *= rz;
        candidate_x += candidate_y;
        candidate_x /= sqrt(1.0 - e * e);
        candidate_y = cos_e / radius;
        candidate_y *= rz;
        candidate_y -= sqrt(1.0 - e * e) * sin_e * vz;
        printf("ac73 candidate x=%a y=%a\n", candidate_x, candidate_y);
        candidate_x = cos_e - e;
        candidate_x *= sqrt(a / mu);
        candidate_x *= vz;
        candidate_y = sin_e / radius;
        candidate_y *= rz;
        candidate_x += candidate_y;
        candidate_x /= sqrt(1.0 - e * e);
        candidate_y = cos_e / radius;
        candidate_y *= rz;
        candidate_y -= sqrt(a / mu) * sin_e * vz;
        printf("sqrt(a/mu) candidate x=%a y=%a\n", candidate_x, candidate_y);
    }
    return 0;
}
