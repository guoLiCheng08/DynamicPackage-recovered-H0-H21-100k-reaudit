set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $all = (unsigned char *)calloc(64,1)
  set $i = 0
  while $i < 64
    set $all[$i] = 0xa5
    set $i = $i + 1
  end
  call ((void (*)(void *,unsigned short,unsigned char))Put_UINT16_In_Buff)($all+0,0x1234,0)
  call ((void (*)(void *,unsigned short,unsigned char))Put_UINT16_In_Buff)($all+8,0x1234,1)
  call ((void (*)(void *,unsigned int,unsigned char))Put_UINT32_In_Buff)($all+16,0x12345678,0)
  call ((void (*)(void *,unsigned int,unsigned char))Put_UINT32_In_Buff)($all+24,0x12345678,1)
  call ((void (*)(void *,unsigned char,float))Put_FLOAT32_In_Buff)($all+32,0,1.25f)
  call ((void (*)(void *,unsigned char,float))Put_FLOAT32_In_Buff)($all+40,1,1.25f)
  call ((void (*)(void *,unsigned char,double))Put_FLOAT64_In_Buff)($all+48,0,-2.5)
  call ((void (*)(void *,unsigned char,double))Put_FLOAT64_In_Buff)($all+56,1,-2.5)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_protocol_put_p3_output.bin $all $all+64
  kill
  quit
end
run
