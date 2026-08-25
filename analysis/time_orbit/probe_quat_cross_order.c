#include <stdint.h>
#include <stdio.h>
#include <string.h>

static uint64_t bits(double x) { uint64_t b; memcpy(&b, &x, sizeof(b)); return b; }

int main(void)
{
    const double ew = -0.4005133631523476;
    const double ex = -0.5827520913733386;
    const double ey = 0.5827520913733386;
    const double ez = -0.4005133631523476;
    const double tw = 0.9999999300000451;
    const double tx = 0.00010003277698151727;
    const double ty = -0.00019996313662065492;
    const double tz = 0.0003000134872547473;
    volatile double out[3];
    volatile double a;
    volatile double b;
    unsigned i;

    a = ey * tz; b = ez * ty; out[0] = a - b; out[0] = out[0] * -1.0;
    a = tx * ew; out[0] = a + out[0]; a = ex * tw; out[0] = a + out[0];
    a = ez * tx; b = ex * tz; out[1] = a - b; out[1] = out[1] * -1.0;
    a = ty * ew; out[1] = a + out[1]; a = ey * tw; out[1] = a + out[1];
    a = ex * ty; b = ey * tx; out[2] = a - b; out[2] = out[2] * -1.0;
    a = tz * ew; out[2] = a + out[2]; a = ez * tw; out[2] = a + out[2];
    for (i = 0; i < 3; ++i) printf("volatile[%u]=%016llx %.17g\n", i,
        (unsigned long long)bits(out[i]), out[i]);
    return 0;
}
