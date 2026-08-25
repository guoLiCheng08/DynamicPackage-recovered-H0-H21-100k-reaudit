#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "dynamic_math.h"
#include "dynamic_recovered.h"

#define GOLD_DIR "analysis/time_orbit/"

static int read_blob(const char *name, void *out, size_t bytes)
{
    char path[256];
    FILE *file;
    (void)snprintf(path, sizeof(path), "%s%s", GOLD_DIR, name);
    file = fopen(path, "rb");
    if (file == NULL) return -1;
    if (fread(out, 1u, bytes, file) != bytes || fclose(file) != 0) return -1;
    return 0;
}
static int check(const char *label, const DpVec3 *got, const char *file_name)
{
    double expected[3];
    unsigned i;
    if (read_blob(file_name, expected, sizeof(expected)) != 0) return 1;
    for (i=0; i<3; ++i) {
        uint64_t a,e;
        memcpy(&a,((const double *)got) + i,8); memcpy(&e,&expected[i],8);
        if (a != e) { fprintf(stderr, "%s[%u] %016llx %016llx\n",label,i,(unsigned long long)a,(unsigned long long)e); return 1; }
    }
    return 0;
}
int main(void)
{
    uint8_t sat[0x1078];
    DpMatrix attitude={3,3,3,0,NULL};
    DpMatrix inertia={3,3,0,0,NULL};
    DpVec3 position, unit={0}, junit={0};
    DpVector pos={3,0,&position.x}, u={3,0,&unit.x}, ju={3,0,&junit.x};
    double radius;
    if (read_blob("gold_core_dynamic_sat_step2.bin",sat,sizeof(sat)) != 0) return 1;
    attitude.data=(double *)(sat+0x1c0); inertia.data=(double *)(sat+0x20);
    memcpy(&position,sat+0x128,sizeof(position));
    radius=vector_nrm2(&pos);
    if (blas_gemv(&attitude,&pos,&u,1.0/radius,0.0) != 0 ||
        blas_gemv(&inertia,&u,&ju,1.0,0.0) != 0 ||
        check("unit",&unit,"gold_gravity_step2_unit.bin") != 0 ||
        check("junit",&junit,"gold_gravity_step2_junit.bin") != 0) return 1;
    puts("GravityGradientTorque internal vectors original-ELF compare: PASS (bitwise)");
    return 0;
}
