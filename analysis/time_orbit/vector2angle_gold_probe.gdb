set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/vector2angle_gold_probe.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $a_data = (double *)calloc(3, 8)
  set $b_data = (double *)calloc(3, 8)
  set $a = (char *)calloc(1, 16)
  set $b = (char *)calloc(1, 16)
  set *(int *)$a = 3
  set *(int *)$b = 3
  set *(void **)($a+8) = $a_data
  set *(void **)($b+8) = $b_data
  set $a_data[0] = 3.0
  set $a_data[1] = 4.0
  set $b_data[1] = 5.0
  set $angle_p = (double *)calloc(1, 8)
  set $angle_p[0] = ((double (*)(void *, void *)) vector2angle)($a,$b)
  printf "vector2angle_general_bits\n"
  x/1gx $angle_p
  set *(int *)$b = 2
  set $angle_p[0] = ((double (*)(void *, void *)) vector2angle)($a,$b)
  printf "vector2angle_mismatch_bits\n"
  x/1gx $angle_p
  set *(int *)$b = 3
  set $a_data[0] = 0.0
  set $a_data[1] = 0.0
  set $angle_p[0] = ((double (*)(void *, void *)) vector2angle)($a,$b)
  printf "vector2angle_zero_bits\n"
  x/1gx $angle_p
  kill
  quit
end
run
