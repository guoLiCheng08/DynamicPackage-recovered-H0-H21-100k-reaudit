set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/get_para_buffers_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $u8src=(unsigned char *)calloc(1,1)
  set $u32src=(unsigned char *)calloc(4,1)
  set $f32src=(unsigned char *)calloc(4,1)
  set $f64src=(unsigned char *)calloc(8,1)
  set $u8out=(unsigned char *)calloc(1,1)
  set $u32out=(unsigned int *)calloc(1,4)
  set $f32out=(float *)calloc(1,4)
  set $f64out=(double *)calloc(1,8)
  set $u8src[0]=0x12
  set $u32src[0]=0x78
  set $u32src[1]=0x56
  set $u32src[2]=0x34
  set $u32src[3]=0x12
  set $f32src[0]=0x3f
  set $f32src[1]=0xa0
  set $f32src[2]=0x00
  set $f32src[3]=0x00
  set $f64src[0]=0x00
  set $f64src[1]=0x00
  set $f64src[2]=0x00
  set $f64src[3]=0x00
  set $f64src[4]=0x00
  set $f64src[5]=0x00
  set $f64src[6]=0x04
  set $f64src[7]=0xc0
  set *$u8out=((unsigned char (*)(void *))get_uint8_para)((void *)$u8src)
  set *$u32out=((unsigned int (*)(void *, unsigned char))get_uint32_para)((void *)$u32src,1)
  set *$f32out=((float (*)(void *, unsigned char))get_float32_para)((void *)$f32src,0)
  set *$f64out=((double (*)(void *, unsigned char))get_float64_para)((void *)$f64src,1)
  printf "GET_PARA_OUTPUTS\n"
  x/1bx $u8out
  x/1wx $u32out
  x/1wx $f32out
  x/1gx $f64out
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/getuint8para_gold.bin $u8out $u8out+1
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/getuint32para_gold.bin $u32out $u32out+1
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/getfloat32para_gold.bin $f32out $f32out+1
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/getfloat64para_gold.bin $f64out $f64out+1
  kill
  quit
end
run
