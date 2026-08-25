set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  call ((void (*)())close_shared)()
  set $init=((int (*)())init_shared)()
  set $shared=(unsigned char *)g_data
  set *(unsigned int *)($shared+0x38+0x95c)=0xff800000
  set *(unsigned char *)($shared+0x998+0x257)=0x5a
  set $fout=(float *)calloc(2,4)
  set $uout=(unsigned char *)calloc(1,1)
  set $ret=(int *)calloc(8,4)
  set $ret[0]=$init
  set $ret[1]=((int (*)(int,void *))get_float_value)(599,$fout)
  set $ret[2]=((int (*)(int,void *))get_uint8_value)(599,$uout)
  set $ret[3]=((int (*)(int,void *))get_float_value)(-1,$fout)
  set $ret[4]=((int (*)(int,void *))get_uint8_value)(-1,$uout)
  set $ret[5]=((int (*)(int,void *))get_float_value)(0,0)
  set $ret[6]=((int (*)(int,void *))get_uint8_value)(0,0)
  call ((void (*)())close_shared)()
  set $ret[7]=((int (*)(int,void *))get_float_value)(599,$fout+1)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_shared_ipc_getters_boundary_p3_returns.bin $ret $ret+8
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_shared_ipc_getters_boundary_p3_floats.bin $fout $fout+2
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_shared_ipc_getters_boundary_p3_u8.bin $uout $uout+1
  call ((void (*)())close_shared)()
  kill
  quit
end
run
