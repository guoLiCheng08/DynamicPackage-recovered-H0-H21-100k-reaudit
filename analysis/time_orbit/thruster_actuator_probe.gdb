set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/thruster_actuator_probe.log
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
  call ((void (*)(void)) Thruster_Init)()
  printf "thruster_descriptors\n"
  printf "v10 n=%d p=%p v38 n=%d p=%p v60 n=%d p=%p v88 n=%d p=%p\n", *(int *)((char *)&Thruster+0x10), *(void **)((char *)&Thruster+0x18), *(int *)((char *)&Thruster+0x38), *(void **)((char *)&Thruster+0x40), *(int *)((char *)&Thruster+0x60), *(void **)((char *)&Thruster+0x68), *(int *)((char *)&Thruster+0x88), *(void **)((char *)&Thruster+0x90)
  set $v10 = *(double **)((char *)&Thruster+0x18)
  set $v38 = *(double **)((char *)&Thruster+0x40)
  set $v60 = *(double **)((char *)&Thruster+0x68)
  set $v88 = *(double **)((char *)&Thruster+0x90)
  set *(double *)&Thruster = 2.5
  set $v10[0] = 1.0
  set $v10[1] = 2.0
  set $v10[2] = 3.0
  set $v60[0] = -0.5
  set $v60[1] = 4.0
  set $v60[2] = 1.5
  set $v38[0] = 9.0
  set $v38[1] = 8.0
  set $v38[2] = 7.0
  set $v88[0] = 6.0
  set $v88[1] = 5.0
  set $v88[2] = 4.0
  call ((void (*)(int)) SetThrusterWorkStatus)(0)
  call ((void (*)(void)) UpdateThruster)()
  printf "disabled_v38_v60_v88_bits\n"
  x/3gx $v38
  x/3gx $v60
  x/3gx $v88
  set $v60[0] = -0.5
  set $v60[1] = 4.0
  set $v60[2] = 1.5
  call ((void (*)(int)) SetThrusterWorkStatus)(1)
  call ((void (*)(void)) UpdateThruster)()
  printf "enabled_v38_v60_v88_bits\n"
  x/3gx $v38
  x/3gx $v60
  x/3gx $v88
  kill
  quit
end
run
