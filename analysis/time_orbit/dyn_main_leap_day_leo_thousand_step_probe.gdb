set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $init=(char *)calloc(0xe8,1)
  set {double}($init+0x00)=0.1
  set {double}($init+0x08)=43210.0
  set {float}($init+0x28)=-0.006
  set {float}($init+0x2c)=0.011
  set {float}($init+0x30)=-0.009
  set {double}($init+0x38)=2024.0
  set {double}($init+0x40)=2.0
  set {double}($init+0x48)=29.0
  set {double}($init+0x50)=12.0
  set {double}($init+0x58)=34.0
  set {double}($init+0x60)=56.0
  set {double}($init+0x68)=7078137.0
  set {double}($init+0x70)=0.05
  set {double}($init+0x78)=0.9
  set {double}($init+0x80)=1.2
  set {double}($init+0x88)=0.7
  set {double}($init+0x90)=4.2
  set {double}($init+0x98)=120.0
  set {double}($init+0xc0)=100.0
  set {double}($init+0xd8)=80.0
  set {double}($init+0xe0)=1000.0
  call ((void (*)(void *))dyn_init)($init)
  set $sun_data=(double *)calloc(3,8)
  set $sun_vec=(char *)calloc(0x10,1)
  set {unsigned int}($sun_vec)=3
  set {void *}($sun_vec+8)=$sun_data
  call ((void (*)(void *,void *))GetSunVector)($sun_vec,$init+0x38)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_leap_day_leo_sun_gci.bin $sun_data ($sun_data+3)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_leap_day_leo_pre_dss.bin &DSS ((char *)&DSS+0x2b0)
  call ((void (*)(unsigned int))srand)(12345)
  set $state=(double *)calloc(33,8)
  set $state_byte=(char *)$state
  set $state[0]=0.5
  set $state[1]=0.5
  set $state[2]=-0.5
  set $state[3]=0.5
  set $state[4]=-0.006
  set $state[5]=0.011
  set $state[6]=-0.009
  set $state[7]=6500000.0
  set $state[8]=-2200000.0
  set $state[9]=3500000.0
  set $state[10]=2000.0
  set $state[11]=6200.0
  set $state[12]=3000.0
  set $out=(char *)calloc(0x220,1)
  set $cmd=(char *)calloc(0x78,1)
  set $shared=(char *)calloc(0xbf0,1)
  call ((int (*)(void *,void *))pthread_rwlock_init)($shared,0)
  set {void *}0x5555556184b0=$shared
  set $payload=$shared+0x38
  set $step=0
  while $step<1000
    call ((void (*)(void *,void *,void *))dyn_main)($out,$state,$cmd)
    if $step==0
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_leap_day_leo_step1_dss_sun_input.bin ($state_byte+0x50) ($state_byte+0x68)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_leap_day_leo_step1_dss_position_input.bin ($state_byte+0x98) ($state_byte+0xb0)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_leap_day_leo_step1_dss.bin &DSS ((char *)&DSS+0x2b0)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_leap_day_leo_thousand_step_state.bin $state ($state+33)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_leap_day_leo_thousand_step_out.bin $out ($out+0x220)
    else
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_leap_day_leo_thousand_step_state.bin $state ($state+33)
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_leap_day_leo_thousand_step_out.bin $out ($out+0x220)
    end
    call ((void (*)(void *,void *))sendDynTele)(0,$out)
    if $step==0
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_leap_day_leo_thousand_step_ipc_payload.bin $payload ($shared+0xbf0)
    else
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_leap_day_leo_thousand_step_ipc_payload.bin $payload ($shared+0xbf0)
    end
    set $step=$step+1
  end
  kill
  quit
end
run
