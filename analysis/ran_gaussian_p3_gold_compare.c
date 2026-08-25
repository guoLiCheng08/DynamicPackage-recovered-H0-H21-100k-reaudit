#include <stdio.h>
#include <stdlib.h>
#include "dynamic_sensors.h"
#define GOLD "analysis/time_orbit/gold_ran_gaussian_p3_outputs.bin"
int main(void){double got[4];unsigned char ref[32];FILE*f=fopen(GOLD,"rb");size_t i;if(!f||fread(ref,1,32,f)!=32)return 1;fclose(f);srand(1);got[0]=ran_gaussian(1.0);got[1]=ran_gaussian(2.0);srand(1);got[2]=ran_gaussian2(3.0,1.0);got[3]=ran_gaussian2(-2.0,0.5);for(i=0;i<32;i++)if(((unsigned char*)got)[i]!=ref[i])return 1;puts("ran_gaussian P3 original-ELF compare: PASS (bitwise)");return 0;}
