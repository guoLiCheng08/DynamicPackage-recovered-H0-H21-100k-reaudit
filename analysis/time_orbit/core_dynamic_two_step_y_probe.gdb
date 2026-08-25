set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set $core_pre_hits = 0
break *CoreDynamic+0x49e
commands
  silent
  set $core_pre_hits = $core_pre_hits + 1
  if $core_pre_hits == 1
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_core_dynamic_torque_step1.bin &SatTorque ((char *)&SatTorque + 0x168)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_core_dynamic_input_step1.bin $rbp ($rbp+0x90)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_core_dynamic_sat_step1.bin &Sat ((char *)&Sat + 0x1078)
    continue
  end
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_core_dynamic_torque_step2.bin &SatTorque ((char *)&SatTorque + 0x168)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_core_dynamic_input_step2.bin $rbp ($rbp+0x90)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_core_dynamic_sat_step2.bin &Sat ((char *)&Sat + 0x1078)
  continue
end
set $core_hits = 0
break *CoreDynamic+0x4ab
commands
  silent
  set $core_hits = $core_hits + 1
  set $state_d = (double *)&y
  if $core_hits == 1
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_core_dynamic_y_step1.bin $state_d ($state_d+33)
    continue
  end
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_core_dynamic_y_step2.bin $state_d ($state_d+33)
  kill
  quit
end
set $dyn_round = 0
break *dyn_main+0x7b
commands
  silent
  if $dyn_round == 0
    set $dyn_round = 1
    set $rdi = $out
    set $rsi = $state
    set $rdx = $cmd
    set $pc = (void *)dyn_main
    continue
  end
  continue
end
break main
commands
  silent
  set $init = (char *)calloc(0xe8,1)
  set {double}($init+0x00) = 0.1
  set {double}($init+0x08) = 1.0
  set {float}($init+0x28) = 0.001
  set {float}($init+0x2c) = -0.002
  set {float}($init+0x30) = 0.003
  set {double}($init+0x38) = 2025.0
  set {double}($init+0x40) = 1.0
  set {double}($init+0x48) = 2.0
  set {double}($init+0x50) = 3.0
  set {double}($init+0x58) = 4.0
  set {double}($init+0x60) = 5.0
  set {double}($init+0x68) = 7000000.0
  set {double}($init+0x70) = 0.01
  set {double}($init+0x78) = 0.5
  set {double}($init+0x80) = 0.2
  set {double}($init+0x88) = 0.3
  set {double}($init+0x90) = 0.4
  set {double}($init+0x98) = 120.0
  set {double}($init+0xc0) = 100.0
  set {double}($init+0xd8) = 80.0
  set {double}($init+0xe0) = 1000.0
  call ((void (*)(void *))dyn_init)($init)
  set $out = (char *)calloc(0x220,1)
  set $state = (double *)calloc(33,8)
  set $state[0] = 1.0
  set $state[4] = 0.001
  set $state[5] = -0.002
  set $state[6] = 0.003
  set $state[7] = 7000000.0
  set $state[11] = 7546.0
  set $cmd = (char *)calloc(0x78,1)
  set $rdi = $out
  set $rsi = $state
  set $rdx = $cmd
  set $pc = (void *)dyn_main
  continue
end
run
