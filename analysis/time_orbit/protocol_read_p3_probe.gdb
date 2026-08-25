set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  # Original ABI symbols under test: read_float32, read_long, and read_Uint8.
  set $bytes = (unsigned char *)calloc(4,1)
  set $bytes[0] = 0x12
  set $bytes[1] = 0x34
  set $bytes[2] = 0x56
  set $bytes[3] = 0x78
  set $float_results = (float *)calloc(2,4)
  set $long_results = (unsigned long *)calloc(2,8)
  set $uint8_result = (unsigned char *)calloc(1,1)
  set $float_results[0] = ((float (*)(void *,int))read_float32)($bytes,0)
  set $float_results[1] = ((float (*)(void *,int))read_float32)($bytes,1)
  set $long_results[0] = ((unsigned long (*)(void *,int))read_long)($bytes,0)
  set $long_results[1] = ((unsigned long (*)(void *,int))read_long)($bytes,1)
  set $uint8_result[0] = ((unsigned char (*)(void *))read_Uint8)($bytes)
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/read_float32_gold.bin $float_results $float_results+2
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/read_long_gold.bin $long_results $long_results+2
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/read_uint8_gold.bin $uint8_result $uint8_result+1
  kill
  quit
end
run
