#include <stdio.h>
#include <string.h>
#include "dynamic_ipc_telemetry.h"
#define GOLD "analysis/time_orbit/gold_protocol_para_p3_output.bin"
int main(void){ uint8_t b4[4]={0x12,0x34,0x56,0x78}, b8[8]={0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0}; unsigned char a[40]={0},e[40]; FILE*f=fopen(GOLD,"rb"); size_t i,n; if(!f){perror(GOLD);return 1;} n=fread(e,1,sizeof(e),f); if(fclose(f)||n!=sizeof(e))return 1; a[0]=get_uint8_para(b4); memcpy(a+4,&(uint32_t){get_uint32_para(b4,0)},4); memcpy(a+8,&(uint32_t){get_uint32_para(b4,1)},4); memcpy(a+12,&(float){get_float32_para(b4,0)},4); memcpy(a+16,&(float){get_float32_para(b4,1)},4); memcpy(a+20,&(double){get_float64_para(b8,0)},8); memcpy(a+28,&(double){get_float64_para(b8,1)},8); for(i=0;i<40;i++)if(a[i]!=e[i]){fprintf(stderr,"protocol para P3 mismatch +0x%zx\n",i);return 1;} puts("protocol para P3 original-ELF compare: PASS (bitwise)");return 0;}
