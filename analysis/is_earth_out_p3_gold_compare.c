#include <stdio.h>
#include "dynamic_sensors.h"
#define GOLD "analysis/time_orbit/gold_is_earth_out_p3_returns.bin"
static int read_gold(void *out, size_t n) { FILE *f=fopen(GOLD,"rb"); size_t k; if(!f){perror(GOLD);return -1;} k=fread(out,1,n,f); return fclose(f)==0&&k==n?0:-1; }
int main(void) {
    double p[3]={7000000.0,0.0,0.0};
    double v[3][3]={{-1.0,0.0,0.0},{0.0,1.0,0.0},{1.0,0.0,0.0}};
    DpVector pos={3,0,p}; DpVector view={3,0,v[0]}; int actual[3]; unsigned char expected[sizeof(actual)]; size_t i;
    if(read_gold(expected,sizeof(expected))!=0)return 1;
    for(i=0;i<3;i++){view.data=v[i];actual[i]=isEarthOut(&pos,&view,0.0);}
    for(i=0;i<sizeof(actual);i++)if(((unsigned char*)actual)[i]!=expected[i]){fprintf(stderr,"isEarthOut P3 mismatch +0x%zx\n",i);return 1;}
    puts("isEarthOut P3 original-ELF compare: PASS (bitwise)"); return 0;
}
