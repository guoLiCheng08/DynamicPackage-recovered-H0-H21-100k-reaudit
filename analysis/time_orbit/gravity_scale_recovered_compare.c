#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "dynamic_math.h"
#include "dynamic_recovered.h"

#define GOLD_DIR "analysis/time_orbit/"
static int read_blob(const char *name, void *out, size_t n) {
    char path[256]; FILE *f; (void)snprintf(path,sizeof(path),"%s%s",GOLD_DIR,name);
    f=fopen(path,"rb"); if(f==NULL) return -1;
    if (fread(out, 1u, n, f) != n || fclose(f) != 0) return -1;
    return 0;
}
int main(void) {
    uint8_t sat[0x1078]; double expected[2]; DpVec3 pos; DpVector p={3,0,&pos.x};
    double r,r3,numerator; uint64_t a,e; unsigned i;
    if(read_blob("gold_core_dynamic_sat_step2.bin",sat,sizeof(sat)) ||
       read_blob("gold_gravity_step2_scale_operands.bin",expected,sizeof(expected))) return 1;
    memcpy(&pos,sat+0x128,sizeof(pos)); r=vector_nrm2(&p); r3=r; r3*=r; r*=r3;
    numerator=3.0*3.986004418e14;
    for(i=0;i<2;i++) { double got=i==0?numerator:r; memcpy(&a,&got,8); memcpy(&e,&expected[i],8);
       if(a!=e) { printf("operand%u got=%016llx expected=%016llx\n",i,(unsigned long long)a,(unsigned long long)e); return 1; }}
    puts("GravityGradientTorque scale operands original-ELF compare: PASS (bitwise)"); return 0;
}
