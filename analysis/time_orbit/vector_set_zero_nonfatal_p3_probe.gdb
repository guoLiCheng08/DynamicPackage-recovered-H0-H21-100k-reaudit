set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $data = (double *)calloc(3,8)
  set $data[0] = 1.5
  set $data[1] = -2.0
  set $data[2] = -0.0
  set $vector = (char *)calloc(0x10,1)
  set *(int *)$vector = 0
  set *(void **)($vector+8) = $data
  set $snapshots = (double *)calloc(6,8)
  call ((void (*)(void *))vector_set_zero)($vector)
  set $snapshots[0] = $data[0]
  set $snapshots[1] = $data[1]
  set $snapshots[2] = $data[2]
  set *(int *)$vector = 3
  call ((void (*)(void *))vector_set_zero)($vector)
  set $snapshots[3] = $data[0]
  set $snapshots[4] = $data[1]
  set $snapshots[5] = $data[2]
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_vector_set_zero_nonfatal_p3_snapshots.bin $snapshots ($snapshots+6)
  kill
  quit
end
run
