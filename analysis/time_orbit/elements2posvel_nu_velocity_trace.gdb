set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/elements2posvel_nu_velocity_trace.log
set logging overwrite on
set logging enabled on
dprintf *Elements2PosVel_nu+0x1b1, "nu-velocity-pre xmm0=%.17g xmm1=%.17g xmm2=%.17g xmm3=%.17g xmm4=%.17g xmm5=%.17g xmm7=%.17g xmm8=%.17g stack08=%.17g stack10=%.17g stack18=%.17g stack20=%.17g stack28=%.17g stack30=%.17g stack38=%.17g stack40=%.17g stack48=%.17g stack50=%.17g stack58=%.17g\n", $xmm0.v2_double[0], $xmm1.v2_double[0], $xmm2.v2_double[0], $xmm3.v2_double[0], $xmm4.v2_double[0], $xmm5.v2_double[0], $xmm7.v2_double[0], $xmm8.v2_double[0], *(double *)($rsp+0x8), *(double *)($rsp+0x10), *(double *)($rsp+0x18), *(double *)($rsp+0x20), *(double *)($rsp+0x28), *(double *)($rsp+0x30), *(double *)($rsp+0x38), *(double *)($rsp+0x40), *(double *)($rsp+0x48), *(double *)($rsp+0x50), *(double *)($rsp+0x58)
dprintf *Elements2PosVel_nu+0x230, "nu-velocity-final xmm0=%.17g xmm1=%.17g xmm2=%.17g xmm3=%.17g xmm4=%.17g xmm5=%.17g xmm7=%.17g xmm8=%.17g\n", $xmm0.v2_double[0], $xmm1.v2_double[0], $xmm2.v2_double[0], $xmm3.v2_double[0], $xmm4.v2_double[0], $xmm5.v2_double[0], $xmm7.v2_double[0], $xmm8.v2_double[0]
break main
commands
  silent
  set $position = (double *) malloc(24)
  set $velocity = (double *) malloc(24)
  call ((void (*)(void *, void *, double, double, double, double, double, double)) Elements2PosVel_nu)($position, $velocity, 7000000.0, 0.1, 0.7, 1.3, 0.45, 2.2616708994930574)
  printf "outputs v=%.17g %.17g %.17g\n", $velocity[0], $velocity[1], $velocity[2]
  kill
  quit
end
run
