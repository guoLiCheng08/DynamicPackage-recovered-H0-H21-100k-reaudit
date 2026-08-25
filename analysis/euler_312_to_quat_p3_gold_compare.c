#include <stdio.h>
#include <string.h>
#include "dynamic_math.h"
#define QGOLD "analysis/time_orbit/euler_angle_312_to_quat_gold.bin"
#define XGOLD "analysis/time_orbit/euler_angle_312_to_quat_xyz_gold.bin"
int main(void){double xyz[3]={0};DpQuatAbi q={0,{3,0,xyz}};unsigned char qe[24],xe[24];FILE*f;size_t n;f=fopen(QGOLD,"rb");if(!f){perror(QGOLD);return 1;}n=fread(qe,1,24,f);if(fclose(f)||n!=24)return 1;f=fopen(XGOLD,"rb");if(!f){perror(XGOLD);return 1;}n=fread(xe,1,24,f);if(fclose(f)||n!=24)return 1;euler_angle_312_to_quat(&q,0.3,-0.4,0.7);if(memcmp(&q,qe,16)||memcmp(xyz,xe,24)){size_t i; for(i=0;i<16;i++)if(((unsigned char *)&q)[i]!=qe[i]){fprintf(stderr,"descriptor mismatch +0x%zx actual=%02x expected=%02x\\n",i,((unsigned char *)&q)[i],qe[i]);break;} for(i=0;i<24;i++)if(((unsigned char *)xyz)[i]!=xe[i]){fprintf(stderr,"xyz mismatch +0x%zx actual=%02x expected=%02x\\n",i,((unsigned char *)xyz)[i],xe[i]);break;}return 1;}puts("euler_angle_312_to_quat P3 original-ELF compare: PASS (bitwise)");return 0;}
