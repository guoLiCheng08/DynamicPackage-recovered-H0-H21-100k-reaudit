set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $data = (double *)calloc(12,8)
  set $index = 0
  while $index < 12
    set $data[$index] = $index + 1.0
    set $index = $index + 1
  end
  set $matrix = (char *)calloc(0x18,1)
  set *(int *)$matrix = 0
  set *(int *)($matrix+4) = 2
  set *(int *)($matrix+8) = 4
  set *(void **)($matrix+0x10) = $data
  set $snapshots = (double *)calloc(24,8)
  call ((void (*)(void *,double))matrix_set_all)($matrix,-1.25)
  set $index = 0
  while $index < 12
    set $snapshots[$index] = $data[$index]
    set $index = $index + 1
  end
  set *(int *)$matrix = 2
  call ((void (*)(void *,double))matrix_set_all)($matrix,-1.25)
  set $index = 0
  while $index < 12
    set $snapshots[$index+12] = $data[$index]
    set $index = $index + 1
  end
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_matrix_set_all_nonfatal_p3_snapshots.bin $snapshots ($snapshots+24)
  kill
  quit
end
run
