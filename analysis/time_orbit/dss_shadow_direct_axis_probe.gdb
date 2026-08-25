set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $sun_data = (double *)calloc(3,8)
  set $position_data = (double *)calloc(3,8)
  set $sun = (char *)calloc(0x10,1)
  set $position = (char *)calloc(0x10,1)
  set *(int *)$sun = 3
  set *(void **)($sun+8) = $sun_data
  set *(int *)$position = 3
  set *(void **)($position+8) = $position_data
  set $result = (int *)calloc(3,4)
  set $position_data[0] = 70000000.0
  set $sun_data[0] = -1.0
  set $result[0] = ((int (*)(void *,void *))isEarthShadow)($sun,$position)
  set $position_data[0] = -70000000.0
  set $sun_data[0] = 1.0
  set $result[1] = ((int (*)(void *,void *))isEarthShadow)($sun,$position)
  set $position_data[0] = -70000000.0
  set $sun_data[0] = -1.0
  set $result[2] = ((int (*)(void *,void *))isEarthShadow)($sun,$position)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_dss_shadow_direct_axis_results.bin $result $result+3
  kill
  quit
end
run
