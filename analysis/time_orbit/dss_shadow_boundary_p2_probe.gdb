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
  set $sun = (char *)calloc(0x10,1)
  set $position = (char *)calloc(0x10,1)
  set *(int *)$sun = 3
  set *(void **)($sun+8) = $sun_data
  set *(int *)$position = 3
  set *(void **)($position+8) = $position_data
  set $deltas = (double *)calloc(4,8)
  set $deltas[0] = 0.080
  set $deltas[1] = 0.090
  set $deltas[2] = 0.093
  set $deltas[3] = 0.100
  set $flags = (int *)calloc(8,4)
  set $shadow_results = (int *)calloc(4,4)
  set $positions = (double *)calloc(12,8)
  set $positions[0] = -69776119.441183358
  set $positions[1] = 5594028.5778420884
  set $positions[3] = -69716691.310839593
  set $positions[4] = 6291498.443860773
  set $positions[6] = -69697503.118944138
  set $positions[7] = 6500619.8923465591
  set $positions[9] = -69650291.569461808
  set $positions[10] = 6988339.165277971
  set $i = 0
  while $i < 4
    set $position_data[0] = $positions[$i*3]
    set $position_data[1] = $positions[$i*3+1]
    set $position_data[2] = $positions[$i*3+2]
    set $shadow_results[$i] = ((int (*)(void *,void *))isEarthShadow)($sun,$position)
    call ((void (*)(void *,void *))UpdateSunSensorValidFlag)($sun,$position)
    set $flags[$i*2] = *(int *)((char *)&DSS + 0x108)
    set $flags[$i*2+1] = *(int *)((char *)&DSS + 0x260)
    set $i = $i + 1
  end
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_dss_shadow_boundary_p2_flags.bin $flags $flags+8
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_dss_shadow_boundary_p2_deltas.bin $deltas $deltas+4
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_dss_shadow_boundary_p2_shadow_results.bin $shadow_results $shadow_results+4
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_dss_shadow_boundary_p2_positions.bin $positions $positions+12
  kill
  quit
end
run
