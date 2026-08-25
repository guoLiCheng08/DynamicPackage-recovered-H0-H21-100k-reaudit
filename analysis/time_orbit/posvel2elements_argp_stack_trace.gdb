set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/posvel2elements_argp_stack_trace.log
set logging overwrite on
set logging enabled on
dprintf *PosVel2Elements_M+0x266, "argp-scale-source xmm1=%.17g xmm4=%.17g xmm5=%.17g xmm8=%.17g stack08=%.17g stack10=%.17g\n", $xmm1.v2_double[0], $xmm4.v2_double[0], $xmm5.v2_double[0], $xmm8.v2_double[0], *(double *)($rsp+0x8), *(double *)($rsp+0x10)
dprintf *PosVel2Elements_M+0x3f3, "argp-pre rsp=%p xmm2=%.17g xmm3=%.17g xmm4=%.17g xmm6=%.17g xmm7=%.17g xmm8=%.17g stack08=%.17g stack10=%.17g stack48=%.17g stack40=%.17g stack58=%.17g stack60=%.17g stack80=%.17g stack88=%.17g\n", $rsp, $xmm2.v2_double[0], $xmm3.v2_double[0], $xmm4.v2_double[0], $xmm6.v2_double[0], $xmm7.v2_double[0], $xmm8.v2_double[0], *(double *)($rsp+0x8), *(double *)($rsp+0x10), *(double *)($rsp+0x48), *(double *)($rsp+0x40), *(double *)($rsp+0x58), *(double *)($rsp+0x60), *(double *)($rsp+0x80), *(double *)($rsp+0x88)
break main
commands
  silent
  set $a = (double *) malloc(8)
  set $e = (double *) malloc(8)
  set $i = (double *) malloc(8)
  set $raan = (double *) malloc(8)
  set $argp = (double *) malloc(8)
  set $true_anomaly = (double *) malloc(8)
  set $mean_anomaly = (double *) malloc(8)
  set $argument_of_latitude = (double *) malloc(8)
  set $position = (double *) malloc(24)
  set $velocity = (double *) malloc(24)
  set $position[0] = -4073306.892903815
  set $position[1] = -5851722.1589736138
  set $position[2] = 1987412.5991544391
  set $velocity[0] = 3643.5044963902051
  set $velocity[1] = -4634.4388877243673
  set $velocity[2] = -4001.236670030642
  call ((void (*)(void *, void *, void *, void *, void *, void *, void *, void *, void *, void *)) PosVel2Elements_M)($a, $e, $i, $raan, $argp, $true_anomaly, $mean_anomaly, $argument_of_latitude, $position, $velocity)
  printf "outputs argp=%.17g nu=%.17g M=%.17g u=%.17g\n", $argp[0], $true_anomaly[0], $mean_anomaly[0], $argument_of_latitude[0]
  kill
  quit
end
run
