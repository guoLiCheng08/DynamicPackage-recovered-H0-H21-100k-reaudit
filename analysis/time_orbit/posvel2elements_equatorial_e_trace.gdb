set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/posvel2elements_equatorial_e_trace.log
set logging overwrite on
set logging enabled on
dprintf *PosVel2Elements_M+0x1d1, "e-pre-sqrt xmm0=%.17g xmm2=%.17g xmm3=%.17g xmm4=%.17g xmm5=%.17g stack08=%.17g stack10=%.17g stack98=%.17g\n", $xmm0.v2_double[0], $xmm2.v2_double[0], $xmm3.v2_double[0], $xmm4.v2_double[0], $xmm5.v2_double[0], *(double *)($rsp+0x8), *(double *)($rsp+0x10), *(double *)($rsp+0x98)
break main
commands
  silent
  set $o = (double *) malloc(64)
  set $position = (double *) malloc(24)
  set $velocity = (double *) malloc(24)
  set $position[0] = -6058112.4461465934
  set $position[1] = 4070232.2512063403
  set $position[2] = 0.0
  set $velocity[0] = -4559.3926008976832
  set $velocity[1] = -5612.2779225600771
  set $velocity[2] = -0.0
  call ((void (*)(void *, void *, void *, void *, void *, void *, void *, void *, void *, void *)) PosVel2Elements_M)($o, $o+1, $o+2, $o+3, $o+4, $o+5, $o+6, $o+7, $position, $velocity)
  printf "outputs e=%.17g argp=%.17g nu=%.17g\n", $o[1], $o[4], $o[5]
  kill
  quit
end
run
