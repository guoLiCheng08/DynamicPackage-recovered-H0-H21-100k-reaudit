#include <stdio.h>
#include <string.h>
#include "dynamic_math.h"
#define GOLD "analysis/time_orbit/gold_cartesian2polar_p3_output.bin"
int main(void){double data[3]={3,4,12},out[3],gold[3];DpVector v={3,0,data};FILE*f=fopen(GOLD,"rb");size_t n;if(!f){perror(GOLD);return 1;}n=fread(gold,1,sizeof(gold),f);if(fclose(f)||n!=sizeof(gold))return 1;Cartesian2Polar(&out[0],&out[1],&out[2],&v);if(memcmp(out,gold,sizeof(out))){fputs("Cartesian2Polar P3 mismatch\n",stderr);return 1;}puts("Cartesian2Polar P3 original-ELF compare: PASS (bitwise)");return 0;}
