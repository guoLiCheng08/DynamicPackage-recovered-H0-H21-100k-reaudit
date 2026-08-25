set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/rk4_function_probe.log
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
  printf "pre t=%.17g y0=%.17g y1=%.17g y2=%.17g y3=%.17g p=(%.17g,%.17g,%.17g) v=(%.17g,%.17g,%.17g) bits=(%016llx,%016llx,%016llx,%016llx,%016llx,%016llx)\n", *(double *)&t, ((double *)&y)[0], ((double *)&y)[1], ((double *)&y)[2], ((double *)&y)[3], ((double *)&y)[7], ((double *)&y)[8], ((double *)&y)[9], ((double *)&y)[10], ((double *)&y)[11], ((double *)&y)[12], *(unsigned long long *)((double *)&y + 7), *(unsigned long long *)((double *)&y + 8), *(unsigned long long *)((double *)&y + 9), *(unsigned long long *)((double *)&y + 10), *(unsigned long long *)((double *)&y + 11), *(unsigned long long *)((double *)&y + 12)
  call ((void (*)(double)) RK4_Intergrator)(0.1)
  printf "post t=%.17g y0=%.17g y1=%.17g y2=%.17g y3=%.17g p=(%.17g,%.17g,%.17g) v=(%.17g,%.17g,%.17g) bits=(%016llx,%016llx,%016llx,%016llx,%016llx,%016llx)\n", *(double *)&t, ((double *)&y)[0], ((double *)&y)[1], ((double *)&y)[2], ((double *)&y)[3], ((double *)&y)[7], ((double *)&y)[8], ((double *)&y)[9], ((double *)&y)[10], ((double *)&y)[11], ((double *)&y)[12], *(unsigned long long *)((double *)&y + 7), *(unsigned long long *)((double *)&y + 8), *(unsigned long long *)((double *)&y + 9), *(unsigned long long *)((double *)&y + 10), *(unsigned long long *)((double *)&y + 11), *(unsigned long long *)((double *)&y + 12)
  kill
  quit
end
run
