set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/is_earth_out_gold_probe.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $position_data = (double *)calloc(3, 8)
  set $position_data[0] = 7000000.0
  set $position = (char *)calloc(1, 16)
  set *(int *)$position = 3
  set *(void **)($position + 8) = $position_data
  set $direction_data = (double *)calloc(3, 8)
  set $direction = (char *)calloc(1, 16)
  set *(int *)$direction = 3
  set *(void **)($direction + 8) = $direction_data
  set $direction_data[0] = -1.0
  printf "earth_out_toward=%d\n", ((int (*)(void *, void *, double)) isEarthOut)($position, $direction, 0.0)
  set $direction_data[0] = 1.0
  printf "earth_out_away=%d\n", ((int (*)(void *, void *, double)) isEarthOut)($position, $direction, 0.0)
  set $direction_data[0] = 0.0
  set $direction_data[1] = 1.0
  printf "earth_out_orthogonal=%d\n", ((int (*)(void *, void *, double)) isEarthOut)($position, $direction, 0.0)
  kill
  quit
end
run
