set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
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
  set {double}((char *)&y+0x00) = 0.5
  set {double}((char *)&y+0x08) = -0.5
  set {double}((char *)&y+0x10) = 0.5
  set {double}((char *)&y+0x18) = 0.5
  set {double}((char *)&y+0x68) = 2.0
  set {double}((char *)&y+0x70) = -1.0
  set {double}((char *)&y+0x78) = 0.5
  set {double}((char *)&y+0x80) = -3.0
  set {double}((char *)&y+0x88) = 1.5
  set {double}((char *)&y+0x90) = -0.75
  set {double}((char *)&y+0x98) = 0.375
  set {double}((char *)&y+0xa0) = -0.1875
  set {double}((char *)&y+0xa8) = 0.09375
  set {double}((char *)&y+0xb0) = -0.046875
  set {double}((char *)&y+0xb8) = 0.2
  set {double}((char *)&y+0xc0) = -0.4
  set {double}((char *)&y+0xc8) = 0.6
  set {double}((char *)&y+0xd0) = -0.8
  set {double}((char *)&y+0xd8) = 0.4
  set {double}((char *)&y+0xe0) = -0.2
  set {double}((char *)&y+0xe8) = 0.1
  set {double}((char *)&y+0xf0) = -0.05
  set {double}((char *)&y+0xf8) = 0.025
  set {double}((char *)&y+0x100) = -0.0125
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_altq_flex_amplitude_pre_dyn_main_y.bin &y ((char *)&y+0x108)
  call ((void (*)(unsigned int))srand)(1)
  set $state = (double *)calloc(33,8)
  set $state[0] = 0.5
  set $state[1] = -0.5
  set $state[2] = 0.5
  set $state[3] = 0.5
  set $state[4] = 0.001
  set $state[5] = -0.002
  set $state[6] = 0.003
  set $state[7] = 7000000.0
  set $state[11] = 7546.0
  set $out = (char *)calloc(0x220,1)
  set $cmd = (char *)calloc(0x78,1)
  set {unsigned int}($cmd+0x58) = 1
  set {double}($cmd+0x60) = 0.01
  set {double}($cmd+0x68) = -0.01
  set {unsigned int}($cmd+0x74) = 1
  set $shared = (char *)calloc(0xbf0,1)
  call ((int (*)(void *, void *))pthread_rwlock_init)($shared,0)
  set {void *}0x5555556184b0 = $shared
  set $payload = $shared + 0x38
  set $step = 0
  while $step < 100
    call ((void (*)(void *,void *,void *))dyn_main)($out,$state,$cmd)
    if $step == 0
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_altq_flex_amplitude_post_step1_y.bin &y ((char *)&y+0x108)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_altq_flex_amplitude_hundred_step_global_y.bin &y ((char *)&y+0x108)
    else
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_altq_flex_amplitude_hundred_step_global_y.bin &y ((char *)&y+0x108)
    end
    if $step == 0
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_altq_flex_amplitude_hundred_step_state.bin $state ($state+33)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_altq_flex_amplitude_hundred_step_out.bin $out ($out+0x220)
    else
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_altq_flex_amplitude_hundred_step_state.bin $state ($state+33)
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_altq_flex_amplitude_hundred_step_out.bin $out ($out+0x220)
    end
    call ((void (*)(void *, void *))sendDynTele)(0,$out)
    if $step == 0
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_altq_flex_amplitude_hundred_step_ipc_payload.bin $payload ($shared+0xbf0)
    else
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_altq_flex_amplitude_hundred_step_ipc_payload.bin $payload ($shared+0xbf0)
    end
    set $step = $step + 1
  end
  kill
  quit
end
run
