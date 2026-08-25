set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/wheel_actuator_gold_probe.log
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
  set $w0 = (char *)&RWheel
  set $w1 = $w0 + 0x78
  set $w2 = $w0 + 0xf0
  set $w3 = $w0 + 0x168
  set $cmd = (double *)calloc(4, 8)

  set *(unsigned int *)($w0 + 0x4) = 0
  set *(unsigned int *)($w1 + 0x4) = 0
  set *(unsigned int *)($w2 + 0x4) = 0
  set *(unsigned int *)($w3 + 0x4) = 0
  set *(double *)($w0 + 0x18) = 1.0
  set *(double *)($w1 + 0x18) = 1.0
  set *(double *)($w2 + 0x18) = 1.0
  set *(double *)($w3 + 0x18) = 1.0
  set *(double *)($w0 + 0x20) = 0.05
  set *(double *)($w1 + 0x20) = 0.05
  set *(double *)($w2 + 0x20) = 0.05
  set *(double *)($w3 + 0x20) = 0.05
  set *(double *)($w0 + 0x28) = 0.1
  set *(double *)($w1 + 0x28) = 0.1
  set *(double *)($w2 + 0x28) = 0.1
  set *(double *)($w3 + 0x28) = 0.1
  set *(double *)($w0 + 0x30) = 0.0
  set *(double *)($w1 + 0x30) = 0.0
  set *(double *)($w2 + 0x30) = 1.0
  set *(double *)($w3 + 0x30) = -1.0
  set $cmd[0] = 0.02
  set $cmd[1] = -0.04
  set $cmd[2] = 0.10
  set $cmd[3] = -0.10
  call ((void (*)(void *)) SetWheelAcc)($cmd)
  printf "set_acc_case1_bits\n"
  x/1gx ($w0 + 0x38)
  x/1gx ($w1 + 0x38)
  x/1gx ($w2 + 0x38)
  x/1gx ($w3 + 0x38)

  set $cmd[2] = -0.02
  set $cmd[3] = 0.02
  call ((void (*)(void *)) SetWheelAcc)($cmd)
  printf "set_acc_case2_bits\n"
  x/1gx ($w0 + 0x38)
  x/1gx ($w1 + 0x38)
  x/1gx ($w2 + 0x38)
  x/1gx ($w3 + 0x38)

  set *(double *)($w0 + 0x30) = 0.3
  set *(double *)($w1 + 0x30) = -0.4
  set *(double *)($w2 + 0x30) = 0.5
  set *(double *)($w3 + 0x30) = -0.6
  set *(double *)($w0 + 0x38) = 0.2
  set *(double *)($w1 + 0x38) = -0.4
  set *(double *)($w2 + 0x38) = 0.5
  set *(double *)($w3 + 0x38) = -0.5
  call ((void (*)(double)) UpdateWheel)(0.2)
  printf "update_wheel_fields_bits\n"
  x/3gx ($w0 + 0x30)
  x/3gx ($w1 + 0x30)
  x/3gx ($w2 + 0x30)
  x/3gx ($w3 + 0x30)
  kill
  quit
end
run
