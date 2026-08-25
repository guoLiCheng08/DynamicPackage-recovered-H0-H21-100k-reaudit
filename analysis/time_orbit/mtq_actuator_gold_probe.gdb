set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/mtq_actuator_gold_probe.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $initial = (double *)calloc(1, 232)
  set $initial[0] = 0.1
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
  set $initial[23] = 11.0
  set $initial[27] = 12.0
  set $initial[28] = 100.0
  call ((void (*)(void *)) DynamicInit)($initial)
  call ((void (*)(void)) MagTorque_Init)()
  set $m0 = (char *)&MTQ
  set $cmd = (double *)calloc(6, 8)
  set *(double *)($m0 + 0x08) = 0.5
  set *(double *)($m0 + 0x40) = 1.0
  set *(double *)($m0 + 0x78) = 2.0
  set *(double *)($m0 + 0xb0) = 3.0
  set *(double *)($m0 + 0xe8) = 4.0
  set *(double *)($m0 + 0x120) = 5.0
  set $cmd[0] = 0.25
  set $cmd[1] = -2.0
  set $cmd[2] = 3.0
  set $cmd[3] = -4.0
  set $cmd[4] = 0.0
  set $cmd[5] = 6.0
  call ((void (*)(void *)) SetMomentMTQ)($cmd)
  printf "mtq_actual_moment_bits\n"
  x/1gx ($m0 + 0x00)
  x/1gx ($m0 + 0x38)
  x/1gx ($m0 + 0x70)
  x/1gx ($m0 + 0xa8)
  x/1gx ($m0 + 0xe0)
  x/1gx ($m0 + 0x118)
  kill
  quit
end
run
