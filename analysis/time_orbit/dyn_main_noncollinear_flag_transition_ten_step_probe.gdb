set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set $core_hits = 0
set $step = 0
define set_command
  call ((void *(*)(void *,int,unsigned long))memset)($cmd,0,0x78)
  if $step == 0
    set {unsigned int}($cmd+0x58) = 1
    set {double}($cmd+0x60) = 0.01
    set {double}($cmd+0x68) = -0.01
  end
  if $step == 1
    set {unsigned int}($cmd+0x58) = 1
    set {double}($cmd+0x60) = 0.01
    set {double}($cmd+0x68) = -0.01
  end
  if $step == 2
    set {double}($cmd+0x08) = 0.001
    set {unsigned int}($cmd+0x58) = 1
    set {double}($cmd+0x60) = 0.01
    set {double}($cmd+0x68) = -0.01
    set {unsigned int}($cmd+0x74) = 1
  end
  if $step == 3
    set {double}($cmd+0x28) = 0.01
    set {unsigned int}($cmd+0x58) = 1
    set {double}($cmd+0x60) = 0.01
    set {double}($cmd+0x68) = -0.01
    set {unsigned int}($cmd+0x74) = 1
  end
  if $step == 4
    set {unsigned int}($cmd+0x58) = 1
    set {double}($cmd+0x60) = -0.015
    set {double}($cmd+0x68) = 0.005
    set {unsigned int}($cmd+0x70) = 1
    set {unsigned int}($cmd+0x74) = 1
  end
  if $step == 5
    set {double}($cmd+0x08) = -0.001
    set {unsigned int}($cmd+0x58) = 1
    set {double}($cmd+0x60) = -0.015
    set {double}($cmd+0x68) = 0.005
  end
  if $step == 6
    set {double}($cmd+0x30) = -0.005
    set {unsigned int}($cmd+0x58) = 1
    set {double}($cmd+0x60) = -0.015
    set {double}($cmd+0x68) = 0.005
    set {unsigned int}($cmd+0x74) = 1
  end
  if $step == 7
    set {unsigned int}($cmd+0x58) = 1
    set {double}($cmd+0x60) = 0.005
    set {double}($cmd+0x68) = 0.012
    set {unsigned int}($cmd+0x70) = 1
    set {unsigned int}($cmd+0x74) = 1
  end
  if $step == 8
    set {double}($cmd+0x08) = 0.0005
  end
  if $step == 9
    set {unsigned int}($cmd+0x58) = 1
    set {double}($cmd+0x60) = 0.005
    set {double}($cmd+0x68) = 0.012
    set {unsigned int}($cmd+0x74) = 1
  end
end
break CoreDynamic
commands
  silent
  set $core_hits = $core_hits + 1
  continue
end
break *CoreDynamic+0x4a6
commands
  silent
  if $core_hits == 1
    set $s1_m4a8 = *(void **)((char *)&Sat + 0x4a8 + 0x10)
    set $s1_m9 = *(void **)((char *)&Sat + 0x9a0 + 0x10)
    dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_flag_transition_step1_m4a8.bin $s1_m4a8 ((char *)$s1_m4a8 + 72)
    dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_flag_transition_step1_m9.bin $s1_m9 ((char *)$s1_m9 + 72)
  end
  if $core_hits == 2
    set $s2_m4a8 = *(void **)((char *)&Sat + 0x4a8 + 0x10)
    set $s2_m9 = *(void **)((char *)&Sat + 0x9a0 + 0x10)
    dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_flag_transition_step2_m4a8.bin $s2_m4a8 ((char *)$s2_m4a8 + 72)
    dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_flag_transition_step2_m9.bin $s2_m9 ((char *)$s2_m9 + 72)
  end
  if $core_hits == 3
    set $s3_m4a8 = *(void **)((char *)&Sat + 0x4a8 + 0x10)
    set $s3_m9 = *(void **)((char *)&Sat + 0x9a0 + 0x10)
    dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_flag_transition_step3_m4a8.bin $s3_m4a8 ((char *)$s3_m4a8 + 72)
    dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_flag_transition_step3_m9.bin $s3_m9 ((char *)$s3_m9 + 72)
  end
  if $core_hits == 4
    dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_flag_transition_step4_pre_y.bin &y ((char *)&y + 264)
    dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_flag_transition_step4_precore_sada.bin &SADA ((char *)&SADA + 0x68)
    dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_flag_transition_step4_j.bin &J_c_B_mem ((char *)&J_c_B_mem + 72)
    dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_flag_transition_step4_jinv.bin &J_c_B_inv_mem ((char *)&J_c_B_inv_mem + 72)
    set $m4a8 = *(void **)((char *)&Sat + 0x4a8 + 0x10)
    set $m6 = *(void **)((char *)&Sat + 0x688 + 0x10)
    set $c = *(void **)((char *)&Sat + 0x898 + 0x10)
    set $m9 = *(void **)((char *)&Sat + 0x9a0 + 0x10)
    dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_flag_transition_step4_used_model.bin $m4a8 ((char *)$m4a8 + 72)
    append binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_flag_transition_step4_used_model.bin $m6 ((char *)$m6 + 240)
    append binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_flag_transition_step4_used_model.bin $c ((char *)$c + 240)
    append binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_flag_transition_step4_used_model.bin $m9 ((char *)$m9 + 72)
  end
  continue
end
break *dyn_main+0x7b
commands
  silent
  if $step == 0
    dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_flag_transition_ten_step_state.bin $state ($state+33)
    dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_flag_transition_ten_step_out.bin $out ($out+0x220)
  else
    append binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_flag_transition_ten_step_state.bin $state ($state+33)
    append binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_flag_transition_ten_step_out.bin $out ($out+0x220)
  end
  call ((void (*)(void *, void *))sendDynTele)(0,$out)
  if $step == 0
    dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_flag_transition_ten_step_ipc_payload.bin $payload ($shared+0xbf0)
  else
    append binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_flag_transition_ten_step_ipc_payload.bin $payload ($shared+0xbf0)
  end
  set $step = $step + 1
  if $step < 10
    set_command
    set $rdi = $out
    set $rsi = $state
    set $rdx = $cmd
    set $pc = (void *)dyn_main
    continue
  end
  kill
  quit
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
  call ((void (*)(unsigned int))srand)(1)
  set $state = (double *)calloc(33,8)
  set $state[0] = 0.5
  set $state[1] = 0.5
  set $state[2] = -0.5
  set $state[3] = 0.5
  set $state[4] = 0.001
  set $state[5] = -0.002
  set $state[6] = 0.003
  set $state[7] = 7000000.0
  set $state[11] = 7546.0
  set $out = (char *)calloc(0x220,1)
  set $cmd = (char *)calloc(0x78,1)
  set $shared = (char *)calloc(0xbf0,1)
  call ((int (*)(void *, void *))pthread_rwlock_init)($shared,0)
  set {void *}0x5555556184b0 = $shared
  set $payload = $shared + 0x38
  set_command
  set $rdi = $out
  set $rsi = $state
  set $rdx = $cmd
  set $pc = (void *)dyn_main
  continue
end
run
