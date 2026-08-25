#include <stdio.h>
#include <string.h>
#include "dynamic_ipc_telemetry.h"
#define GOLD "analysis/time_orbit/escape_character_procotol_gold.bin"
int main(void){uint8_t source[5]={0x11,0x7d,0x7e,0x7f,0x22},out[16]={0},gold[16];FILE*f=fopen(GOLD,"rb");size_t n;if(!f){perror(GOLD);return 1;}n=fread(gold,1,16,f);if(fclose(f)||n!=16)return 1;*(int32_t *)(out+12)=escape_character_procotol(out,source,5);if(memcmp(out,gold,16)){fputs("escape_character_procotol P3 mismatch\n",stderr);return 1;}puts("escape_character_procotol P3 original-ELF compare: PASS (bitwise)");return 0;}
