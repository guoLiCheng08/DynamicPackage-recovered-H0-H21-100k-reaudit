#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static uint64_t bits(double value) { uint64_t out; memcpy(&out, &value, sizeof(out)); return out; }

int main(void)
{
    const double w = 1.0, x = 0.1, y = -0.2, z = 0.3;
    const double a = 2.0 * (x * z - w * y);
    const double b = 2.0 * x * z - 2.0 * w * y;
    const double c = (x * z + x * z) - (w * y + w * y);
    const double d = x * z - w * y + x * z - w * y;
    const double e = (x * z - w * y) + (x * z - w * y);
    printf("a=%a %016" PRIx64 "\n", a, bits(a));
    printf("b=%a %016" PRIx64 "\n", b, bits(b));
    printf("c=%a %016" PRIx64 "\n", c, bits(c));
    printf("d=%a %016" PRIx64 "\n", d, bits(d));
    printf("e=%a %016" PRIx64 "\n", e, bits(e));
    return 0;
}
