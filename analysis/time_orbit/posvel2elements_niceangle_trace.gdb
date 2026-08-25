set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/posvel2elements_niceangle_trace.log
set logging overwrite on
set logging enabled on
set $nice_count = 0
dprintf NiceAngle, "NiceAngle ret=%p x=%.17g y=%.17g\n", *(void **)$rsp, $xmm0.v2_double[0], $xmm1.v2_double[0]
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
