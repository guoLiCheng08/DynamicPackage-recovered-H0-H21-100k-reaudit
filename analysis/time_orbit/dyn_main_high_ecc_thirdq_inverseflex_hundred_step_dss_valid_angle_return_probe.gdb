set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $init=(char *)calloc(0xe8,1)
  set {double}($init+0x00)=0.1
  set {double}($init+0x08)=86400.0
  set {float}($init+0x28)=0.0125
  set {float}($init+0x2c)=-0.00875
  set {float}($init+0x30)=0.00425
  set {double}($init+0x38)=2031.0
  set {double}($init+0x40)=12.0
  set {double}($init+0x48)=31.0
  set {double}($init+0x50)=23.0
  set {double}($init+0x58)=59.0
  set {double}($init+0x60)=50.0
  set {double}($init+0x68)=26560000.0
  set {double}($init+0x70)=0.65
  set {double}($init+0x78)=1.1
  set {double}($init+0x80)=1.7
  set {double}($init+0x88)=2.4
  set {double}($init+0x90)=0.9
  set {double}($init+0x98)=120.0
  set {double}($init+0xc0)=100.0
  set {double}($init+0xd8)=80.0
  set {double}($init+0xe0)=1000.0
  call ((void (*)(void *))dyn_init)($init)
  set {double}((char *)&y+0x00)=0.5
  set {double}((char *)&y+0x08)=0.5
  set {double}((char *)&y+0x10)=0.5
  set {double}((char *)&y+0x18)=-0.5
  set {double}((char *)&y+0x68)=-2.0
  set {double}((char *)&y+0x70)=1.0
  set {double}((char *)&y+0x78)=-0.5
  set {double}((char *)&y+0x80)=3.0
  set {double}((char *)&y+0x88)=-1.5
  set {double}((char *)&y+0x90)=0.75
  set {double}((char *)&y+0x98)=-0.375
  set {double}((char *)&y+0xa0)=0.1875
  set {double}((char *)&y+0xa8)=-0.09375
  set {double}((char *)&y+0xb0)=0.046875
  set {double}((char *)&y+0xb8)=-0.2
  set {double}((char *)&y+0xc0)=0.4
  set {double}((char *)&y+0xc8)=-0.6
  set {double}((char *)&y+0xd0)=0.8
  set {double}((char *)&y+0xd8)=-0.4
  set {double}((char *)&y+0xe0)=0.2
  set {double}((char *)&y+0xe8)=-0.1
  set {double}((char *)&y+0xf0)=0.05
  set {double}((char *)&y+0xf8)=-0.025
  set {double}((char *)&y+0x100)=0.0125
  set $sun_data=(double *)calloc(3,8)
  set $sun_vec=(char *)calloc(0x10,1)
  set {unsigned int}($sun_vec)=3
  set {void *}($sun_vec+8)=$sun_data
  call ((void (*)(void *,void *))GetSunVector)($sun_vec,$init+0x38)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_high_ecc_thirdq_inverseflex_sun_gci.bin $sun_data ($sun_data+3)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_high_ecc_thirdq_inverseflex_pre_dss.bin &DSS ((char *)&DSS+0x2b0)
  call ((void (*)(unsigned int))srand)(12345)
  set $state=(double *)calloc(33,8)
  set $state_byte=(char *)$state
  set $state[0]=0.5
  set $state[1]=0.5
  set $state[2]=0.5
  set $state[3]=-0.5
  set $state[4]=0.0125
  set $state[5]=-0.00875
  set $state[6]=0.00425
  set $state[7]=15000000.0
  set $state[8]=-90000000.0
  set $state[9]=-39000000.0
  set $state[10]=-2500.0
  set $state[11]=5500.0
  set $state[12]=1000.0
  set $out=(char *)calloc(0x220,1)
  set $cmd=(char *)calloc(0x78,1)
  set $shared=(char *)calloc(0xbf0,1)
  call ((int (*)(void *,void *))pthread_rwlock_init)($shared,0)
  set {void *}0x5555556184b0=$shared
  set $payload=$shared+0x38
  # 仅在调试副本内改写 UpdateSunSensorValidFlag 的两条 call 指令。
  # 每个跳板调用原 vector2angle，保存 XMM0 并 ret 至原调用点返回地址。
  set $angles=(double *)calloc(2,8)
  set $call0=(char *)UpdateSunSensorValidFlag+0x64
  set $call1=(char *)UpdateSunSensorValidFlag+0xb9
  set $v2a=(char *)vector2angle
  set $cave0=(char *)vector_print
  set $cave1=$cave0+0x20
  set {unsigned char}($cave0+0)=0xe8
  set {int}($cave0+1)=(int)($v2a-($cave0+5))
  set {unsigned char}($cave0+5)=0xf2
  set {unsigned char}($cave0+6)=0x0f
  set {unsigned char}($cave0+7)=0x11
  set {unsigned char}($cave0+8)=0x05
  set {int}($cave0+9)=(int)((char *)$angles-($cave0+13))
  set {unsigned char}($cave0+13)=0xc3
  set {unsigned char}($cave1+0)=0xe8
  set {int}($cave1+1)=(int)($v2a-($cave1+5))
  set {unsigned char}($cave1+5)=0xf2
  set {unsigned char}($cave1+6)=0x0f
  set {unsigned char}($cave1+7)=0x11
  set {unsigned char}($cave1+8)=0x05
  set {int}($cave1+9)=(int)(((char *)$angles+8)-($cave1+13))
  set {unsigned char}($cave1+13)=0xc3
  set {unsigned char}($call0+0)=0xe8
  set {int}($call0+1)=(int)($cave0-($call0+5))
  set {unsigned char}($call1+0)=0xe8
  set {int}($call1+1)=(int)($cave1-($call1+5))
  set $step=0
  while $step<100
    call ((void (*)(void *,void *,void *))dyn_main)($out,$state,$cmd)
    if $step==0
      dump binary memory /tmp/h11_original_dss_valid_angle_return_trace.bin $angles ($angles+2)
    else
      append binary memory /tmp/h11_original_dss_valid_angle_return_trace.bin $angles ($angles+2)
    end
    if $step==0
      dump binary memory /tmp/h11_original_dss_trace.bin &DSS ((char *)&DSS+0x2b0)
    else
      append binary memory /tmp/h11_original_dss_trace.bin &DSS ((char *)&DSS+0x2b0)
    end
    if $step==0
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_high_ecc_thirdq_inverseflex_hundred_step_global_y.bin &y ((char *)&y+0x108)
    else
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_high_ecc_thirdq_inverseflex_hundred_step_global_y.bin &y ((char *)&y+0x108)
    end
    if $step==0
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_high_ecc_thirdq_inverseflex_step1_dss_sun_input.bin ($state_byte+0x50) ($state_byte+0x68)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_high_ecc_thirdq_inverseflex_step1_dss_position_input.bin ($state_byte+0x98) ($state_byte+0xb0)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_high_ecc_thirdq_inverseflex_step1_dss.bin &DSS ((char *)&DSS+0x2b0)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_high_ecc_thirdq_inverseflex_hundred_step_state.bin $state ($state+33)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_high_ecc_thirdq_inverseflex_hundred_step_out.bin $out ($out+0x220)
    else
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_high_ecc_thirdq_inverseflex_hundred_step_state.bin $state ($state+33)
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_high_ecc_thirdq_inverseflex_hundred_step_out.bin $out ($out+0x220)
    end
    call ((void (*)(void *,void *))sendDynTele)(0,$out)
    if $step==0
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_high_ecc_thirdq_inverseflex_hundred_step_ipc_payload.bin $payload ($shared+0xbf0)
    else
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_high_ecc_thirdq_inverseflex_hundred_step_ipc_payload.bin $payload ($shared+0xbf0)
    end
    set $step=$step+1
  end
  kill
  quit
end
run
