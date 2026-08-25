set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $time = (double *)calloc(6,8)
  set $position = (double *)calloc(3,8)
  set $velocity = (double *)calloc(3,8)
  set $snapshots = (char *)calloc(0x1c0,1)
  set $flags = (int *)calloc(4,4)
  set $index = 0
  while $index < 4
    set $time[0] = 2025.9 + $index
    set $time[1] = -1.9 - $index
    set $time[2] = 3.1 + $index
    set $time[3] = 4.9 - $index
    set $time[4] = 5.0 + $index
    set $time[5] = 6.99 - $index
    set $position[0] = 7000000.25 + $index
    set $position[1] = -123.5 - $index
    set $position[2] = 42.125 + $index
    set $velocity[0] = 1.5 + $index
    set $velocity[1] = -2.25 - $index
    set $velocity[2] = 3.75 + $index
    if $index == 0
      set {int}&init_flag = 1
    end
    if $index == 1
      set {int}&init_flag = 0
    end
    if $index == 2
      set {int}&init_flag = 2
    end
    if $index == 3
      set {int}&init_flag = -1
    end
    call ((void (*)(void *, void *, void *, double)) UpdateGPS)($time,$position,$velocity,0.1)
    set $flags[$index] = {int}&init_flag
    if $index == 0
      dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_update_gps_init_flag_p2_snapshots.bin &GPS_Kalman ((char *)&GPS_Kalman+0x70)
    else
      append binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_update_gps_init_flag_p2_snapshots.bin &GPS_Kalman ((char *)&GPS_Kalman+0x70)
    end
    set $index = $index + 1
  end
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_update_gps_init_flag_p2_flags.bin $flags ($flags+4)
  kill
  quit
end
run
