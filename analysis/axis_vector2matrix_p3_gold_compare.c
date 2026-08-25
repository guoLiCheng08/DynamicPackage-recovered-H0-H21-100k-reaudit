#include <stdio.h>
#include <string.h>
#include "dynamic_math.h"
#define GOLD "analysis/time_orbit/gold_axis_vector2matrix_p3_output.bin"
int main(void){double x[3]={1,2,3},y[3]={4,5,6},z[3]={7,8,9},out_data[9],gold[9]; DpVector vx={3,0,x},vy={3,0,y},vz={3,0,z}; DpMatrix out={3,3,3,0,out_data}; FILE*f=fopen(GOLD,"rb"); size_t n;if(!f){perror(GOLD);return 1;}n=fread(gold,1,sizeof(gold),f);if(fclose(f)||n!=sizeof(gold))return 1;AxisVector2Matrix(&out,&vx,&vy,&vz);if(memcmp(out_data,gold,sizeof(gold))){fputs("AxisVector2Matrix P3 mismatch\n",stderr);return 1;}puts("AxisVector2Matrix P3 original-ELF compare: PASS (bitwise)");return 0;}
