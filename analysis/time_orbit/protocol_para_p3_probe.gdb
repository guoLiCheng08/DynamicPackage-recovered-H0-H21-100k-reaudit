set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $b4 = (unsigned char *)calloc(4,1)
  set $b8 = (unsigned char *)calloc(8,1)
  set $b4[0]=0x12
  set $b4[1]=0x34
  set $b4[2]=0x56
  set $b4[3]=0x78
  set $b8[0]=0x12
  set $b8[1]=0x34
  set $b8[2]=0x56
  set $b8[3]=0x78
  set $b8[4]=0x9a
  set $b8[5]=0xbc
  set $b8[6]=0xde
  set $b8[7]=0xf0
  set $out = (unsigned char *)calloc(40,1)
  set *(unsigned char *)$out = ((unsigned char (*)(void *))get_uint8_para)($b4)
  set *(unsigned int *)($out+4) = ((unsigned int (*)(void *,int))get_uint32_para)($b4,0)
  set *(unsigned int *)($out+8) = ((unsigned int (*)(void *,int))get_uint32_para)($b4,1)
  set *(float *)($out+12) = ((float (*)(void *,int))get_float32_para)($b4,0)
  set *(float *)($out+16) = ((float (*)(void *,int))get_float32_para)($b4,1)
  set *(double *)($out+20) = ((double (*)(void *,int))get_float64_para)($b8,0)
  set *(double *)($out+28) = ((double (*)(void *,int))get_float64_para)($b8,1)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_protocol_para_p3_output.bin $out $out+40
  kill
  quit
end
run
