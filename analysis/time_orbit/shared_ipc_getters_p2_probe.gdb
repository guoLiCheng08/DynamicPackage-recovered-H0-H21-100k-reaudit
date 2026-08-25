set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  # Original ABI symbols under test: init_shared, close_shared, get_float_value,
  # get_uint8_value, get_all_floats, and get_all_uint8s.
  call ((void (*)())close_shared)()
  set $init=((int (*)())init_shared)()
  set $shared=(unsigned char *)g_data
  set *(unsigned int *)($shared+0x38+28)=0xc0200000
  set *(unsigned char *)($shared+0x998+7)=0xa5
  set $fout=(float *)calloc(1,4)
  set $uout=(unsigned char *)calloc(1,1)
  set $fall=(unsigned char *)calloc(0x960,1)
  set $uall=(unsigned char *)calloc(0x258,1)
  set $rf=((int (*)(int,void *))get_float_value)(7,$fout)
  set $ru=((int (*)(int,void *))get_uint8_value)(7,$uout)
  set $rfa=((int (*)(void *))get_all_floats)($fall)
  set $rua=((int (*)(void *))get_all_uint8s)($uall)
  set $rfo=((int (*)(int,void *))get_float_value)(600,$fout)
  set $rfn=((int (*)(int,void *))get_float_value)(0,0)
  set $returns=(int *)calloc(6,4)
  set $returns[0]=$init
  set $returns[1]=$rf
  set $returns[2]=$ru
  set $returns[3]=$rfa
  set $returns[4]=$rua
  set $returns[5]=$rfo
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_shared_ipc_getters_p2_returns.bin $returns $returns+6
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_shared_ipc_getters_p2_single.bin $fout $fout+1
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_shared_ipc_getters_p2_u8.bin $uout $uout+1
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_shared_ipc_getters_p2_floats.bin $fall $fall+0x960
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_shared_ipc_getters_p2_uint8s.bin $uall $uall+0x258
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/init_shared_gold.bin $returns $returns+1
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/get_float_value_gold.bin $fout $fout+1
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/get_uint8_value_gold.bin $uout $uout+1
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/get_all_floats_gold.bin $fall $fall+0x960
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/get_all_uint8s_gold.bin $uall $uall+0x258
  call ((void (*)())close_shared)()
  kill
  quit
end
run
