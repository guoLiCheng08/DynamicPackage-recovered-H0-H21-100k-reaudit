set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/differential_equation_probe.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $initial = (double *)malloc(232)
  set $initial[0] = 0.1
  set $initial[1] = 0.0
  set $initial[2] = 0.0
  set $initial[3] = 0.0
  set $initial[4] = 0.0
  set $initial[5] = 0.0
  set $initial[6] = 0.0
  set $initial[7] = 2020.0
  set $initial[8] = 1.0
  set $initial[9] = 2.0
  set $initial[10] = 3.0
  set $initial[11] = 4.0
  set $initial[12] = 5.0
  set $initial[13] = 7000000.0
  set $initial[14] = 0.001
  set $initial[15] = 0.1
  set $initial[16] = 0.2
  set $initial[17] = 0.3
  set $initial[18] = 0.4
  set $initial[19] = 10.0
  set $initial[20] = 0.0
  set $initial[21] = 0.0
  set $initial[22] = 0.0
  set $initial[23] = 11.0
  set $initial[24] = 0.0
  set $initial[25] = 0.0
  set $initial[26] = 0.0
  set $initial[27] = 12.0
  set $initial[28] = 100.0
  call ((void (*)(void *)) DynamicInit)($initial)
  set ((double *)&y)[0] = 1.0
  set ((double *)&y)[1] = 0.0
  set ((double *)&y)[2] = 0.0
  set ((double *)&y)[3] = 0.0
  set $out = (double *)malloc(264)
  call ((void (*)(void *, void *, double)) differential_equation)($out, (double *)&y, 0.0)
  printf "dydt q=(%.17g,%.17g,%.17g,%.17g) rate=(%.17g,%.17g,%.17g) posdot=(%.17g,%.17g,%.17g) veldot=(%.17g,%.17g,%.17g) flex0=(%.17g,%.17g,%.17g)\n", $out[0], $out[1], $out[2], $out[3], $out[4], $out[5], $out[6], $out[7], $out[8], $out[9], $out[10], $out[11], $out[12], $out[13], $out[14], $out[15]
  kill
  quit
end
run
