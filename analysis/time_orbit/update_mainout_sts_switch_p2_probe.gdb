set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $initial = (double *)calloc(1,232)
  set $initial[0] = 0.1
  set $initial[7] = 2020.0
  set $initial[8] = 1.0
  set $initial[9] = 2.0
  set $initial[10] = 3.0
  set $initial[11] = 4.0
  set $initial[12] = 5.0
  set $initial[13] = 7000000.0
  set $initial[14] = 0.001
  set $initial[15] = 0.1
  set $initial[16] = 0.2
  set $initial[17] = 0.3
  set $initial[18] = 0.4
  set $initial[19] = 10.0
  set $initial[23] = 11.0
  set $initial[27] = 12.0
  set $initial[28] = 100.0
  call ((void (*)(void *))DynamicInit)($initial)
  set $core = (char *)calloc(0x148,1)
  set $out_all_off = (char *)calloc(0x220,1)
  set $out_ch1_on = (char *)calloc(0x220,1)
  call ((void (*)(void))Gyro_Init)()
  call ((void (*)(void))MagMeter_Init)()
  call ((void (*)(void))DSS_Init)()
  call ((void (*)(void))STS_Init)()
  set *(int *)((char *)&STS + 0x018) = 0
  set *(int *)((char *)&STS + 0x188) = 0
  set *(int *)((char *)&STS + 0x2f8) = 0
  call ((void (*)(unsigned int))srand)(1)
  call ((void (*)(void *,void *))UpdateMainOut)($out_all_off,$core)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_update_mainout_sts_all_off_p2_frame.bin $out_all_off ($out_all_off+0x220)
  call ((void (*)(void))STS_Init)()
  set *(int *)((char *)&STS + 0x018) = 0
  set *(int *)((char *)&STS + 0x188) = 1
  set *(int *)((char *)&STS + 0x2f8) = 0
  call ((void (*)(unsigned int))srand)(1)
  call ((void (*)(void *,void *))UpdateMainOut)($out_ch1_on,$core)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_update_mainout_sts_ch1_on_p2_frame.bin $out_ch1_on ($out_ch1_on+0x220)
  kill
  quit
end
run
