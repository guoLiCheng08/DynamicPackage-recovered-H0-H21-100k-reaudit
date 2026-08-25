set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_matrix_print_p3_stdout.txt
break main
commands
  silent
  set $data = (double *)calloc(8,8)
  set $index = 0
  while $index < 8
    set $data[$index] = $index + 1.0
    set $index = $index + 1
  end
  set $matrix = (char *)calloc(0x18,1)
  set *(int *)$matrix = 2
  set *(int *)($matrix+4) = 2
  set *(int *)($matrix+8) = 4
  set *(void **)($matrix+0x10) = $data
  call ((void (*)(void *))matrix_print)($matrix)
  call ((int (*)(void *))fflush)(0)
  kill
  quit
end
run
