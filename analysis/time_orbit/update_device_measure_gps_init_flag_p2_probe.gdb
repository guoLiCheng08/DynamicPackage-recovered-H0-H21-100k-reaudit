set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $core = (char *)calloc(0x148,1)
  set {double}($core+0x00) = 1.0
  set {double}($core+0x20) = 0.01
  set {double}($core+0x28) = -0.02
  set {double}($core+0x30) = 0.03
  set {double}($core+0x38) = 0.2
  set {double}($core+0x40) = -0.4
  set {double}($core+0x48) = 0.8
  set {double}($core+0x50) = -1.0
  set {double}($core+0x68) = 20.0
  set {double}($core+0x70) = -30.0
  set {double}($core+0x78) = 40.0
  set {double}($core+0x98) = 7000000.0
  set {double}($core+0xa0) = 1000.0
  set {double}($core+0xa8) = -500.0
  set {double}($core+0xb0) = 1.5
  set {double}($core+0xb8) = -2.25
  set {double}($core+0xc0) = 3.75
  set {double}($core+0xc8) = 2025.9
  set {double}($core+0xd0) = 1.1
  set {double}($core+0xd8) = 2.2
  set {double}($core+0xe0) = 3.3
  set {double}($core+0xe8) = 4.4
  set {double}($core+0xf0) = 5.5
  set {double}&step_time = 0.1
  call ((void (*)(void))Gyro_Init)()
  call ((void (*)(void))MagMeter_Init)()
  call ((void (*)(void))STS_Init)()
  call ((void (*)(void))DSS_Init)()
  set *(int *)&init_flag = 1
  set $flags = (int *)calloc(2,4)
  call ((void (*)(void *))UpdateDeviceMeasure)($core)
  set $flags[0] = *(int *)&init_flag
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_update_device_measure_gps_init_flag_p2_step1_gps.bin &GPS_Kalman ((char *)&GPS_Kalman + 0x70)
  set {double}($core+0x98) = 7100000.25
  set {double}($core+0xa0) = -2000.5
  set {double}($core+0xa8) = 3000.75
  set {double}($core+0xb0) = -4.5
  set {double}($core+0xb8) = 5.25
  set {double}($core+0xc0) = -6.75
  set {double}($core+0xc8) = 2031.9
  set {double}($core+0xd0) = 12.8
  set {double}($core+0xd8) = 31.7
  set {double}($core+0xe0) = 23.6
  set {double}($core+0xe8) = 59.5
  set {double}($core+0xf0) = 58.4
  call ((void (*)(void *))UpdateDeviceMeasure)($core)
  set $flags[1] = *(int *)&init_flag
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_update_device_measure_gps_init_flag_p2_step2_gps.bin &GPS_Kalman ((char *)&GPS_Kalman + 0x70)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_update_device_measure_gps_init_flag_p2_flags.bin $flags $flags+2
  kill
  quit
end
run
