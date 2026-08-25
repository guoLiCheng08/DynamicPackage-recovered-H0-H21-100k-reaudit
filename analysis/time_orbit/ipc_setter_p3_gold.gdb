set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  # Original ABI symbols under test: set_float_value and set_uint8_value.
  call (void)close_shared()
  set $init=(int)init_shared()
  set $float_value=(float *)calloc(1,4)
  set $u8_value=(unsigned char *)calloc(1,1)
  set $results=(char *)calloc(16,1)
  set *(int *)($results+0)=(int)set_float_value(0x257,-12.75)
  set *(int *)($results+4)=(int)get_float_value(0x257,$float_value)
  set *(int *)($results+8)=(int)set_float_value(0x258,1.0)
  set *(float *)($results+12)=*$float_value
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/set_float_value_gold.bin $results $results+16
  set *(int *)($results+0)=(int)set_uint8_value(0x257,0xa7)
  set *(int *)($results+4)=(int)get_uint8_value(0x257,$u8_value)
  set *(int *)($results+8)=(int)set_uint8_value(0x258,0)
  set *(unsigned char *)($results+12)=*$u8_value
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/set_uint8_value_gold.bin $results $results+16
  call (void)close_shared()
  kill
  quit
end
run
