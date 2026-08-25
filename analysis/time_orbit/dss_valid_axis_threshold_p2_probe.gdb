set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  call ((void (*)(void))DSS_Init)()
  set *(double *)((char *)&DSS + 0x10) = 100.0
  set *(double *)((char *)&DSS + 0x168) = 100.0
  set $sun_data = (double *)calloc(3,8)
  set $position_data = (double *)calloc(3,8)
  set $sun_data[0] = 1.0
  set $position_data[0] = -70000000.0
  set $sun = (char *)calloc(0x10,1)
  set $position = (char *)calloc(0x10,1)
  set *(int *)$sun = 3
  set *(void **)($sun+8) = $sun_data
  set *(int *)$position = 3
  set *(void **)($position+8) = $position_data
  call ((void (*)(void *,void *))UpdateSunSensorValidFlag)($sun,$position)
  set $flags = (int *)calloc(2,4)
  set $flags[0] = *(int *)((char *)&DSS + 0x108)
  set $flags[1] = *(int *)((char *)&DSS + 0x260)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_dss_valid_axis_threshold_p2_flags.bin $flags $flags+2
  kill
  quit
end
run
