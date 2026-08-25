#include <stdio.h>
#include <string.h>
#include "dynamic_torque.h"
#define D "analysis/coverage_inventory/"
static int r(const char *n, double x[3]){char p[256];FILE*f;snprintf(p,sizeof p,"%s%s",D,n);f=fopen(p,"rb");if(!f)return 0;return fread(x,8,3,f)==3&&fclose(f)==0;}
int main(void){double in[3]={1.25,-2.5,3.75},out[3]={9,8,7},gi[3],go[3];DpVec3 a={1.25,-2.5,3.75},b={9,8,7};if(!r("gold_rwheel_torque_p1_in.bin",gi)||!r("gold_rwheel_torque_p1_out.bin",go))return 2;if(RWheelTorque(&b,&a)!=0)return 1;memcpy(out,&b,24);if(memcmp(in,gi,24)||memcmp(out,go,24))return 1;puts("RWheelTorque public ABI original-ELF compare: PASS (bitwise)");return 0;}
