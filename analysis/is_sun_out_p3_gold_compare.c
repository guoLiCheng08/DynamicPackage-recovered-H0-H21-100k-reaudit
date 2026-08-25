#include <stdio.h>
#include "dynamic_sensors.h"
#define GOLD "analysis/time_orbit/gold_is_sun_out_p3_returns.bin"
int main(void){double a[3]={0,0,1},b[3]={0,0,1},c[3]={0,1,0},d[3]={0.479425538604203,0,0.8775825618903728}; DpVector x={3,0,a},y={3,0,b}; int got[3]; unsigned char ref[12]; FILE*f=fopen(GOLD,"rb"); size_t i;if(!f||fread(ref,1,12,f)!=12)return 1;fclose(f);got[0]=isSunOut(&x,&y,0.0);y.data=c;got[1]=isSunOut(&x,&y,0.5);y.data=d;got[2]=isSunOut(&x,&y,0.5);for(i=0;i<12;i++)if(((unsigned char*)got)[i]!=ref[i])return 1;puts("isSunOut P3 original-ELF compare: PASS (bitwise)");return 0;}
