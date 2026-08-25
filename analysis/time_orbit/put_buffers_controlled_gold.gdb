set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/put_buffers_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $u16=(unsigned char *)malloc(2)
  set $u32=(unsigned char *)malloc(4)
  set $f32=(unsigned char *)malloc(4)
  set $f64=(unsigned char *)malloc(8)
  call (void)memset($u16,0xa5,2)
  call (void)memset($u32,0xa5,4)
  call (void)memset($f32,0xa5,4)
  call (void)memset($f64,0xa5,8)
  call ((void (*)(void *, unsigned short, unsigned char))Put_UINT16_In_Buff)((void *)$u16,0x1234,0)
  call ((void (*)(void *, unsigned int, unsigned char))Put_UINT32_In_Buff)((void *)$u32,0x12345678,1)
  call ((void (*)(void *, unsigned char, float))Put_FLOAT32_In_Buff)((void *)$f32,0,1.25)
  call ((void (*)(void *, unsigned char, double))Put_FLOAT64_In_Buff)((void *)$f64,1,-2.5)
  printf "PUT_BUFFER_OUTPUTS\n"
  x/2bx $u16
  x/4bx $u32
  x/4bx $f32
  x/8bx $f64
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/putuint16inbuff_gold.bin $u16 $u16+2
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/putuint32inbuff_gold.bin $u32 $u32+4
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/putfloat32inbuff_gold.bin $f32 $f32+4
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/putfloat64inbuff_gold.bin $f64 $f64+8
  kill
  quit
end
run
