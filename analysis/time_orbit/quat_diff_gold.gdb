set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/quat_diff_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $q_xyz = (double *)malloc(24)
  set $q_xyz[0] = 0.1
  set $q_xyz[1] = -0.2
  set $q_xyz[2] = 0.3
  set $q = (char *)malloc(32)
  set *(double *)$q = 0.8
  set *(int *)($q + 8) = 3
  set *(int *)($q + 12) = 0
  set *(double **)($q + 16) = $q_xyz
  set $rate_data = (double *)malloc(24)
  set $rate_data[0] = 0.01
  set $rate_data[1] = -0.02
  set $rate_data[2] = 0.03
  set $rate = (char *)malloc(16)
  set *(int *)$rate = 3
  set *(int *)($rate + 4) = 0
  set *(double **)($rate + 8) = $rate_data
  set $out_xyz = (double *)malloc(24)
  set $out = (char *)malloc(32)
  set *(double *)$out = 0.0
  set *(int *)($out + 8) = 3
  set *(int *)($out + 12) = 0
  set *(double **)($out + 16) = $out_xyz
  call ((void (*)(void *, void *, void *)) quat_diff)($out, $q, $rate)
  printf "out=(w=%.17g,x=%.17g,y=%.17g,z=%.17g) bits=(%016llx,%016llx,%016llx,%016llx)\n", *(double *)$out, $out_xyz[0], $out_xyz[1], $out_xyz[2], *(unsigned long long *)$out, *(unsigned long long *)$out_xyz, *(unsigned long long *)($out_xyz+1), *(unsigned long long *)($out_xyz+2)
  kill
  quit
end
run
