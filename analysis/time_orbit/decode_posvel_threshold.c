#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    const uint64_t raw = UINT64_C(0x3d06849b86a12b9b);
    double value;
    memcpy(&value, &raw, sizeof(value));
    printf("raw=0x%016llx value=%.17g hex=%a\n",
           (unsigned long long)raw, value, value);
    return 0;
}
