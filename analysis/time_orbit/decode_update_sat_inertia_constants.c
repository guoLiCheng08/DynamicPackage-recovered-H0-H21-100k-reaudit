#include <stdint.h>
#include <stdio.h>
#include <string.h>
int main(void) {
    static const uint64_t bits[] = {
        UINT64_C(0x405e000000000000), UINT64_C(0x4050000000000000),
        UINT64_C(0xbfe428f5c28f5c29), UINT64_C(0x3f947ae147ae147b),
        UINT64_C(0x3ff4cccccccccccd), UINT64_C(0x3fbeb851eb851eb8),
        UINT64_C(0x4076900000000000), UINT64_C(0x4024000000000000),
        UINT64_C(0xbfe0000000000000), UINT64_C(0xc02e666666666666),
        UINT64_C(0x3ff4cccccccccccd), UINT64_C(0x3fbeb851eb851eb8),
        UINT64_C(0x407d500000000000), UINT64_C(0x4027600000000000)
    };
    size_t i;
    for (i = 0; i < sizeof(bits)/sizeof(bits[0]); ++i) { double x; memcpy(&x,&bits[i],sizeof x); printf("%zu %#.17g\n",i,x); }
}
