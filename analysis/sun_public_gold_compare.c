#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "dynamic_environment.h"
int main(void){double g[4],a[3]={0};DpVector v={3,0,a};FILE*f=fopen("analysis/time_orbit/sun_gold.bin","rb");unsigned i;if(!f||fread(g,8,4,f)!=4||fclose(f))return 2;Sun(&v,g[0]);for(i=0;i<3;i++){uint64_t x,y;memcpy(&x,a+i,8);memcpy(&y,g+i+1,8);if(x!=y)return 1;}puts("Sun public ABI original-ELF gold compare: PASS (bitwise)");return 0;}
