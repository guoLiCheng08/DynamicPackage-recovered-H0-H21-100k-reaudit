#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

static int read_part(const char *path, double *out, size_t count)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) return -1;
    if (fread(out, sizeof(double), count, file) != count || fclose(file) != 0) return -1;
    return 0;
}

/* Candidate conventional 3-1-2 composition: q_z(a) * q_x(b) * q_y(c). */
static void candidate(double out[4], double a, double b, double c)
{
    double sa, ca, sb, cb, sc, cc;
    sa = sin(0.5 * a); ca = cos(0.5 * a);
    sb = sin(0.5 * b); cb = cos(0.5 * b);
    sc = sin(0.5 * c); cc = cos(0.5 * c);
    out[0] = ca * cb * cc - sa * sb * sc;
    out[1] = ca * sb * cc - sa * cb * sc;
    out[2] = ca * cb * sc + sa * sb * cc;
    out[3] = sa * cb * cc + ca * sb * sc;
}

static int run_case(unsigned number, double a, double b, double c)
{
    char path[256];
    double expected[4];
    double actual[4];
    unsigned index;
    (void)snprintf(path, sizeof(path), "analysis/time_orbit/gold_euler_312_case%u_w.bin", number);
    if (read_part(path, &expected[0], 1u) != 0) return 1;
    (void)snprintf(path, sizeof(path), "analysis/time_orbit/gold_euler_312_case%u_xyz.bin", number);
    if (read_part(path, &expected[1], 3u) != 0) return 1;
    candidate(actual, a, b, c);
    for (index = 0u; index < 4u; ++index) {
        uint64_t x, y;
        memcpy(&x, &actual[index], sizeof(x));
        memcpy(&y, &expected[index], sizeof(y));
        printf("case%u[%u] actual=%a expected=%a bits=%016llx/%016llx\n", number, index,
               actual[index], expected[index], (unsigned long long)x, (unsigned long long)y);
    }
    return 0;
}

int main(void)
{
    return run_case(1u, 0.2, 0.3, 0.4) || run_case(2u, -0.7, 0.5, -0.9);
}
