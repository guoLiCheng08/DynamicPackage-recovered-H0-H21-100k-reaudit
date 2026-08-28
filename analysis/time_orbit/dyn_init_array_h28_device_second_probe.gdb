set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $init=(double *)calloc(30,8)
  set $init[0]=0.01
  set $init[1]=1.0
  set $init[8]=2024.0
  set $init[9]=10.0
  set $init[10]=28.0
  set $init[11]=4.0
  set $init[12]=16.0
  set $init[13]=16.0
  set $init[14]=6910593.0
  set $init[15]=0.00058
  set $init[16]=1.7018327148814874
  set $init[17]=0.24732846154513649
  set $init[18]=3.0651957525056472
  set $init[19]=6.2273473025797976
  set $init[20]=120.0
  set $init[21]=-0.63
  set $init[22]=1.3
  set $init[23]=-0.63
  set $init[24]=361.0
  set $init[25]=-0.5
  set $init[26]=1.3
  set $init[27]=-0.5
  set $init[28]=469.0
  set $init[29]=600.0
  call ((void (*)(const double *))dyn_init_array)($init)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_dyn_init_array_h28_device_second_rwheel.bin &RWheel ((char *)&RWheel+0x1e0)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_dyn_init_array_h28_device_second_mtq.bin &MTQ ((char *)&MTQ+0x150)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_dyn_init_array_h28_device_second_thruster.bin &Thruster ((char *)&Thruster+0xb0)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_dyn_init_array_h28_device_second_sada.bin &SADA ((char *)&SADA+0x68)
  quit
end
run
