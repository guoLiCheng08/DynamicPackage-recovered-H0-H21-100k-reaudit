#include <stdio.h>
#include "dynamic_devices.h"
#define GOLD "analysis/time_orbit/gold_wheel_matrix_calc_p3_data.bin"
int main(void){double data[12]={0};double cols[4][3]={{1,2,3},{4,5,6},{7,8,9},{10,11,12}};DpMatrix out={3,4,4,0,data};DpVector v[4]={{3,0,cols[0]},{3,0,cols[1]},{3,0,cols[2]},{3,0,cols[3]}};unsigned char ref[96];FILE*f=fopen(GOLD,"rb");size_t i;if(!f||fread(ref,1,96,f)!=96)return 1;fclose(f);wheel_matrix_calc(&out,&v[0],&v[1],&v[2],&v[3]);for(i=0;i<96;i++)if(((unsigned char*)data)[i]!=ref[i])return 1;puts("wheel_matrix_calc P3 original-ELF compare: PASS (bitwise)");return 0;}
