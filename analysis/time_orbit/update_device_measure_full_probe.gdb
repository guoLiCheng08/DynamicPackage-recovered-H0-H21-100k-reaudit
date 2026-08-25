set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/update_device_measure_full_probe.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $core = (char *)calloc(0x100,1)
  # +0x00 attitude quaternion: unit identity
  set {double}($core+0x00) = 1.0
  # +0x20 body rate
  set {double}($core+0x20) = 0.01
  set {double}($core+0x28) = -0.02
  set {double}($core+0x30) = 0.03
  # +0x38 sun_body
  set {double}($core+0x38) = 0.2
  set {double}($core+0x40) = -0.4
  set {double}($core+0x48) = 0.8
  # +0x50 sun_gci
  set {double}($core+0x50) = -1.0
  # +0x68 magnetic_body
  set {double}($core+0x68) = 20.0
  set {double}($core+0x70) = -30.0
  set {double}($core+0x78) = 40.0
  # +0x98 position GCI, +0xb0 velocity GCI
  set {double}($core+0x98) = 7000000.0
  set {double}($core+0xa0) = 1000.0
  set {double}($core+0xa8) = -500.0
  set {double}($core+0xb0) = 1.5
  set {double}($core+0xb8) = -2.25
  set {double}($core+0xc0) = 3.75
  # +0xc8 time values
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
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sensor_init_gyro.bin &Gyro ((char *)&Gyro + 0x640)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sensor_init_magmeter.bin &MagMeter ((char *)&MagMeter + 0x270)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sensor_init_sts.bin &STS ((char *)&STS + 0x450)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sensor_init_dss.bin &DSS ((char *)&DSS + 0x2b0)
  call ((void (*)(void *))UpdateDeviceMeasure)($core)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sensor_after_gyro.bin &Gyro ((char *)&Gyro + 0x640)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sensor_after_magmeter.bin &MagMeter ((char *)&MagMeter + 0x270)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sensor_after_sts.bin &STS ((char *)&STS + 0x450)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sensor_after_dss.bin &DSS ((char *)&DSS + 0x2b0)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sensor_after_gps.bin &GPS_Kalman ((char *)&GPS_Kalman + 0x70)
  printf "sts_valid\n"
  x/1wx ((char *)&STS + 0x138)
  x/1wx ((char *)&STS + 0x2a8)
  x/1wx ((char *)&STS + 0x418)
  printf "sts_measure_w\n"
  x/1gx ((char *)&STS + 0x140)
  x/1gx ((char *)&STS + 0x2b0)
  x/1gx ((char *)&STS + 0x420)
  printf "gyro_measure\n"
  x/3gx *(void **)((char *)&Gyro + 0x120)
  x/3gx *(void **)((char *)&Gyro + 0x440)
  printf "dss_angles_ratios_and_valid\n"
  x/4gx ((char *)&DSS + 0x138)
  x/1wx ((char *)&DSS + 0x108)
  x/4gx ((char *)&DSS + 0x290)
  x/1wx ((char *)&DSS + 0x260)
  printf "magmeter_measure\n"
  x/3gx *(void **)((char *)&MagMeter + 0x118)
  x/3gx *(void **)((char *)&MagMeter + 0x250)
  printf "gps\n"
  x/6gx ((char *)&GPS_Kalman + 0x28)
  x/6wx ((char *)&GPS_Kalman + 0x58)
  kill
  quit
end
run
