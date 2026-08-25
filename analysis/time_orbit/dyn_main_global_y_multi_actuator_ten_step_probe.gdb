set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $init=(char *)calloc(0xe8,1)
  set {double}($init+0x00)=0.1
  set {double}($init+0x08)=1.0
  set {float}($init+0x28)=0.001
  set {float}($init+0x2c)=-0.002
  set {float}($init+0x30)=0.003
  set {double}($init+0x38)=2025.0
  set {double}($init+0x40)=1.0
  set {double}($init+0x48)=2.0
  set {double}($init+0x50)=3.0
  set {double}($init+0x58)=4.0
  set {double}($init+0x60)=5.0
  set {double}($init+0x68)=7000000.0
  set {double}($init+0x70)=0.01
  set {double}($init+0x78)=0.5
  set {double}($init+0x80)=0.2
  set {double}($init+0x88)=0.3
  set {double}($init+0x90)=0.4
  set {double}($init+0x98)=120.0
  set {double}($init+0xc0)=100.0
  set {double}($init+0xd8)=80.0
  set {double}($init+0xe0)=1000.0
  call ((void (*)(void *))dyn_init)($init)
  set {double}((char *)&y+0x00)=0.5
  set {double}((char *)&y+0x08)=0.5
  set {double}((char *)&y+0x10)=-0.5
  set {double}((char *)&y+0x18)=0.5
  call ((void (*)(unsigned int))srand)(1)
  set {double}((char *)&RWheel+0x30)=628.4185307179587
  set {double}((char *)&RWheel+0x40)=7.981915339118876
  set $state=(double *)calloc(33,8)
  set $state[0]=0.5
  set $state[1]=0.5
  set $state[2]=-0.5
  set $state[3]=0.5
  set $state[4]=0.001
  set $state[5]=-0.002
  set $state[6]=0.003
  set $state[7]=7000000.0
  set $state[11]=7546.0
  set $out=(char *)calloc(0x220,1)
  set $cmd=(char *)calloc(0x78,1)
  set $shared=(char *)calloc(0xbf0,1)
  call ((int (*)(void *,void *))pthread_rwlock_init)($shared,0)
  set {void *}0x5555556184b0=$shared
  set $payload=$shared+0x38
  set $step=0
  while $step<10
    call ((void *(*)(void *,int,unsigned long))memset)($cmd,0,0x78)
    if $step==0
      set {double}($cmd+0x08)=0.01
      set {double}($cmd+0x10)=-0.005
    end
    if $step==1
      set {double}($cmd+0x08)=-0.01
      set {double}($cmd+0x18)=0.007
      set {double}($cmd+0x28)=0.01
      set {double}($cmd+0x30)=-0.008
    end
    if $step==2
      set {unsigned int}($cmd+0x70)=1
    end
    if $step==3
      set {double}($cmd+0x20)=-0.006
      set {double}($cmd+0x38)=0.012
      set {double}($cmd+0x40)=-0.009
    end
    if $step==4
      set {unsigned int}($cmd+0x70)=1
      set {unsigned int}($cmd+0x74)=1
    end
    if $step==5
      set {double}($cmd+0x08)=0.01
      set {double}($cmd+0x18)=-0.007
    end
    if $step==6
      set {double}($cmd+0x08)=-0.01
      set {double}($cmd+0x48)=0.02
      set {double}($cmd+0x50)=-0.02
    end
    if $step==7
      set {unsigned int}($cmd+0x70)=1
    end
    if $step==8
      set {double}($cmd+0x10)=0.004
      set {double}($cmd+0x20)=-0.004
    end
    call ((void (*)(void *,void *,void *))dyn_main)($out,$state,$cmd)
    if $step==0
      dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_global_y_multi_actuator_ten_step_state.bin $state ($state+33)
      dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_global_y_multi_actuator_ten_step_out.bin $out ($out+0x220)
    else
      append binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_global_y_multi_actuator_ten_step_state.bin $state ($state+33)
      append binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_global_y_multi_actuator_ten_step_out.bin $out ($out+0x220)
    end
    call ((void (*)(void *,void *))sendDynTele)(0,$out)
    if $step==0
      dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_global_y_multi_actuator_ten_step_ipc_payload.bin $payload ($shared+0xbf0)
    else
      append binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_global_y_multi_actuator_ten_step_ipc_payload.bin $payload ($shared+0xbf0)
    end
    set $step=$step+1
  end
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_global_y_multi_actuator_final_rwheel.bin &RWheel ((char *)&RWheel+0x1e0)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_global_y_multi_actuator_final_mtq.bin &MTQ ((char *)&MTQ+0x150)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_global_y_multi_actuator_final_thruster.bin &Thruster ((char *)&Thruster+0xb0)
  set $thr_lever = *(double **)((char *)&Thruster+0x18)
  set $thr_input = *(double **)((char *)&Thruster+0x40)
  set $thr_force = *(double **)((char *)&Thruster+0x68)
  set $thr_torque = *(double **)((char *)&Thruster+0x90)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_global_y_multi_actuator_final_thruster_lever.bin $thr_lever ($thr_lever+3)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_global_y_multi_actuator_final_thruster_input.bin $thr_input ($thr_input+3)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_global_y_multi_actuator_final_thruster_force.bin $thr_force ($thr_force+3)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_global_y_multi_actuator_final_thruster_torque.bin $thr_torque ($thr_torque+3)
  kill
  quit
end
run
