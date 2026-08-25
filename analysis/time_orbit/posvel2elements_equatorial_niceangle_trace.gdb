set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/posvel2elements_equatorial_niceangle_trace.log
set logging overwrite on
set logging enabled on
dprintf NiceAngle, "NiceAngle ret=%p x=%.17g y=%.17g\n", *(void **)$rsp, $xmm0.v2_double[0], $xmm1.v2_double[0]
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
  printf "outputs %.17g %.17g %.17g %.17g\n", $o[4], $o[5], $o[6], $o[7]
  kill
  quit
end
run
