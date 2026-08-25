#include <stdio.h>
#include "dynamic_orbit.h"
#define GOLD "analysis/time_orbit/gold_eccentric2mean_p3_outputs.bin"
int main(void){double got[4];unsigned char ref[32];FILE*f=fopen(GOLD,"rb");size_t i;if(!f||fread(ref,1,32,f)!=32)return 1;fclose(f);got[0]=Eccentric2Mean(1.0,0.2,0.5);got[1]=Eccentric2Mean(-2.0,0.7,-0.3);got[2]=Eccentric2Mean(6.0,0.0,-1.0);got[3]=Eccentric2Mean(0.1,0.9,0.09983341664682815);for(i=0;i<32;i++)if(((unsigned char*)got)[i]!=ref[i])return 1;puts("Eccentric2Mean P3 original-ELF compare: PASS (bitwise)");return 0;}
