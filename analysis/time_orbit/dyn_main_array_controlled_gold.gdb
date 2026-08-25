set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/dyn_main_array_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $initial=(double *)calloc(30,8)
  set $initial[0]=0.01
  set $initial[1]=1.0
  set $initial[2]=0.0
  set $initial[3]=0.0
  set $initial[4]=0.0
  set $initial[5]=0.0
  set $initial[6]=0.0
  set $initial[7]=0.0
  set $initial[8]=2024.0
  set $initial[9]=10.0
  set $initial[10]=28.0
  set $initial[11]=4.0
  set $initial[12]=16.0
  set $initial[13]=16.0
  set $initial[14]=6910593.0
  set $initial[15]=0.00058
  set $initial[16]=1.7018327148814874
  set $initial[17]=0.24732846154513649
  set $initial[18]=3.0651957525056472
  set $initial[19]=6.2273473025797976
  set $initial[20]=120.0
  set $initial[21]=-0.63
  set $initial[22]=1.3
  set $initial[23]=-0.63
  set $initial[24]=361.0
  set $initial[25]=-0.5
  set $initial[26]=1.3
  set $initial[27]=-0.5
  set $initial[28]=469.0
  set $initial[29]=600.0
  call (void)dyn_init_array((void *)$initial)
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/dyn_main_array_gold_sts.bin &STS (char *)&STS+0x450
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/dyn_main_array_gold_gyro.bin &Gyro (char *)&Gyro+0x640
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/dyn_main_array_gold_dss.bin &DSS (char *)&DSS+0x2b0
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/dyn_main_array_gold_magmeter.bin &MagMeter (char *)&MagMeter+0x270
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/dyn_main_array_gold_rwheel.bin &RWheel (char *)&RWheel+0x1e0
  set $out=(char *)malloc(0x1e8)
  set $state=(char *)malloc(0x200)
  set $command=(double *)calloc(16,8)
  call (void)memset($out, 0xa5, 0x1e8)
  call (void)memset($state, 0x5a, 0x200)
  call ((void (*)(void *, void *, void *))dyn_main_array)((void *)$out,(void *)$state,(void *)$command)
  printf "DYN_MAIN_ARRAY_OUT_FIRST_LAST\n"
  x/4gx $out
  x/4gx $out+0x1c8
  printf "DYN_MAIN_ARRAY_STATE_FIRST_LAST\n"
  x/4gx $state
  x/4gx $state+0x1c0
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/dyn_main_array_gold_out.bin $out $out+0x1e8
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/dyn_main_array_gold_state.bin $state $state+0x200
  kill
  quit
end
run
