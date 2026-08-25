set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/quat_reunit_rk4_stage_probe.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $q = (char *)calloc(1, 64)
  set *(double *)$q = 1.0
  set *(int *)($q + 8) = 3
  set *(void **)($q + 16) = $q + 32
  set ((double *)($q + 32))[0] = 0.00025
  set ((double *)($q + 32))[1] = -0.0005
  set ((double *)($q + 32))[2] = 0.00075
  printf "input_bits\n"
  x/1gx $q
  x/3gx ($q + 32)
  call ((void (*)(void *)) quat_reunit)($q)
  printf "output_bits\n"
  x/1gx $q
  x/3gx ($q + 32)
  kill
  quit
end
run
