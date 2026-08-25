#include <stdio.h>
#include <string.h>
#include "dynamic_devices.h"
#include "dynamic_satellite_globals.h"
int main(void){unsigned char gold[0x18];double a[2]={1.25,-2.5};FILE*f=fopen("analysis/coverage_inventory/gold_setsada_p1_prefix.bin","rb");if(!f||fread(gold,1,sizeof gold,f)!=sizeof gold||fclose(f))return 2;memset(&SADA,0,sizeof SADA);SetSADA(0xa5a55a5aU,a);if(memcmp(gold,&SADA,sizeof gold))return 1;puts("SetSADA public ABI original-ELF compare: PASS (bitwise)");return 0;}
