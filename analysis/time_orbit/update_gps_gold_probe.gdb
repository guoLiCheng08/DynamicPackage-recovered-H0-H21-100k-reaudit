set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/update_gps_gold_probe.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $time = (double *)calloc(6,8)
  set $time[0] = 2025.9
  set $time[1] = -1.9
  set $time[2] = 3.1
  set $time[3] = 4.9
  set $time[4] = 5.0
  set $time[5] = 6.99
  set $position = (double *)calloc(3,8)
  set $velocity = (double *)calloc(3,8)
  set $position[0] = 7000000.25
  set $position[1] = -123.5
  set $position[2] = 42.125
  set $velocity[0] = 1.5
  set $velocity[1] = -2.25
  set $velocity[2] = 3.75
  set {int}&init_flag = 1
  call ((void (*)(void *, void *, void *, double)) UpdateGPS)($time,$position,$velocity,0.1)
  printf "gps_kalman_position_velocity_bits\n"
  x/6gx ((char *)&GPS_Kalman + 0x28)
  printf "gps_kalman_time_i32\n"
  x/6wx ((char *)&GPS_Kalman + 0x58)
  printf "init_flag=%d\n", {int}&init_flag
  kill
  quit
end
run
