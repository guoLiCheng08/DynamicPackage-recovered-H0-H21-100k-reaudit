set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/update_device_control_composite_probe.log
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
  call ((void (*)(void)) Thruster_Init)()
  set $in = (char *)calloc(1, 0x78)
  set *(double *)($in + 0x08) = 0.02
  set *(double *)($in + 0x10) = -0.04
  set *(double *)($in + 0x18) = 0.06
  set *(double *)($in + 0x20) = -0.08
  set *(double *)($in + 0x28) = 0.1
  set *(double *)($in + 0x30) = -0.2
  set *(double *)($in + 0x38) = 0.3
  set *(double *)($in + 0x40) = -0.4
  set *(double *)($in + 0x48) = 0.5
  set *(double *)($in + 0x50) = -0.6
  set *(unsigned int *)($in + 0x58) = 7
  set *(double *)($in + 0x60) = 0.3
  set *(double *)($in + 0x68) = -0.4
  set *(unsigned int *)($in + 0x70) = 1
  set *(unsigned int *)($in + 0x74) = 0
  call ((void (*)(void *, double)) UpdateDeviceControl)($in, 0.1)
  printf "rw_acc_omega_h_bits\n"
  x/3gx ((char *)&RWheel + 0x30)
  x/3gx ((char *)&RWheel + 0xa8)
  x/3gx ((char *)&RWheel + 0x120)
  x/3gx ((char *)&RWheel + 0x198)
  printf "mtq_actual_bits\n"
  x/1gx (char *)&MTQ
  x/1gx ((char *)&MTQ + 0x38)
  x/1gx ((char *)&MTQ + 0x70)
  x/1gx ((char *)&MTQ + 0xa8)
  x/1gx ((char *)&MTQ + 0xe0)
  x/1gx ((char *)&MTQ + 0x118)
  printf "thruster_outputs_bits\n"
  x/3gx *(void **)((char *)&Thruster + 0x68)
  x/3gx *(void **)((char *)&Thruster + 0x90)
  printf "sada_prefix_bits\n"
  x/1wx (char *)&SADA
  x/4gx ((char *)&SADA + 0x08)
  kill
  quit
end
run
