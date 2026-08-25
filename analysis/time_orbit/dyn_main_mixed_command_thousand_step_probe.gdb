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
  call ((void (*)(unsigned int))srand)(1)
  set $state = (double *)calloc(33,8)
  set $state[0] = 1.0
  set $state[4] = 0.001
  set $state[5] = -0.002
  set $state[6] = 0.003
  set $state[7] = 7000000.0
  set $state[11] = 7546.0
  set $out = (char *)calloc(0x220,1)
  set $cmd = (char *)calloc(0x78,1)
  set $shared = (char *)calloc(0xbf0,1)
  call ((int (*)(void *, void *))pthread_rwlock_init)($shared,0)
  set {void **}&g_data = $shared
  set $payload = $shared + 0x38
  set $step = 0
  while $step < 1000
    call ((void *(*)(void *,int,unsigned long))memset)($cmd,0,0x78)
    if $step == 1
      set {double}($cmd+0x08) = 0.001
    end
    if $step == 2
      set {double}($cmd+0x28) = 0.01
    end
    if $step == 3
      set {unsigned int}($cmd+0x58) = 1
      set {double}($cmd+0x60) = 0.01
      set {double}($cmd+0x68) = -0.01
    end
    if $step == 4
      set {unsigned int}($cmd+0x70) = 1
      set {unsigned int}($cmd+0x74) = 1
    end
    if $step == 5
      set {double}($cmd+0x08) = -0.001
    end
    if $step == 6
      set {unsigned int}($cmd+0x58) = 1
      set {double}($cmd+0x60) = -0.015
      set {double}($cmd+0x68) = 0.005
    end
    if $step == 7
      set {double}($cmd+0x30) = -0.005
    end
    if $step == 8
      set {unsigned int}($cmd+0x70) = 1
    end
    call ((void (*)(void *,void *,void *))dyn_main)($out,$state,$cmd)
    if $step == 4
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step5_sada.bin &SADA ((char *)&SADA+0x68)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step5_rwheel.bin &RWheel ((char *)&RWheel+0x1e0)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step5_mtq.bin &MTQ ((char *)&MTQ+0x150)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step5_j.bin &J_c_B_mem ((char *)&J_c_B_mem+72)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step5_jinv.bin &J_c_B_inv_mem ((char *)&J_c_B_inv_mem+72)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step5_h.bin &H_w_B_mem ((char *)&H_w_B_mem+24)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step5_l.bin &L_c_B_mem ((char *)&L_c_B_mem+24)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step5_f.bin &F_I_external_mem ((char *)&F_I_external_mem+24)
    end
    if $step == 5
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step6_sada.bin &SADA ((char *)&SADA+0x68)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step6_rwheel.bin &RWheel ((char *)&RWheel+0x1e0)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step6_mtq.bin &MTQ ((char *)&MTQ+0x150)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step6_j.bin &J_c_B_mem ((char *)&J_c_B_mem+72)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step6_jinv.bin &J_c_B_inv_mem ((char *)&J_c_B_inv_mem+72)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step6_h.bin &H_w_B_mem ((char *)&H_w_B_mem+24)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step6_l.bin &L_c_B_mem ((char *)&L_c_B_mem+24)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step6_f.bin &F_I_external_mem ((char *)&F_I_external_mem+24)
    end
    if $step == 0
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_command_thousand_step_state.bin $state ($state+33)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_command_thousand_step_out.bin $out ($out+0x220)
    else
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_command_thousand_step_state.bin $state ($state+33)
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_command_thousand_step_out.bin $out ($out+0x220)
    end
    if $step == 0
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_command_thousand_step_global_y.bin &y ((char *)&y+264)
    else
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_command_thousand_step_global_y.bin &y ((char *)&y+264)
    end
    call ((void (*)(void *, void *))sendDynTele)(0,$out)
    if $step == 0
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_command_thousand_step_ipc_payload.bin $payload ($shared+0xbf0)
    else
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_command_thousand_step_ipc_payload.bin $payload ($shared+0xbf0)
    end
    set $step = $step + 1
  end
  kill
  quit
end
run
