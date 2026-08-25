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
  set {double}($init+0x48)=30.0
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
  set {double}((char *)&y+0x08)=-0.5
  set {double}((char *)&y+0x10)=0.5
  set {double}((char *)&y+0x18)=0.5
  set {double}((char *)&y+0x38)=-12000000.0
  set {double}((char *)&y+0x40)=65000000.0
  set {double}((char *)&y+0x48)=28000000.0
  set {double}((char *)&y+0x50)=0.0
  set {double}((char *)&y+0x58)=2000.0
  set {double}((char *)&y+0x60)=0.0
  set {double}((char *)&y+0x68)=2.0
  set {double}((char *)&y+0x70)=-1.0
  set {double}((char *)&y+0x78)=0.5
  set {double}((char *)&y+0x80)=-3.0
  set {double}((char *)&y+0x88)=1.5
  set {double}((char *)&y+0x90)=-0.75
  set {double}((char *)&y+0x98)=0.375
  set {double}((char *)&y+0xa0)=-0.1875
  set {double}((char *)&y+0xa8)=0.09375
  set {double}((char *)&y+0xb0)=-0.046875
  set {double}((char *)&y+0xb8)=0.2
  set {double}((char *)&y+0xc0)=-0.4
  set {double}((char *)&y+0xc8)=0.6
  set {double}((char *)&y+0xd0)=-0.8
  set {double}((char *)&y+0xd8)=0.4
  set {double}((char *)&y+0xe0)=-0.2
  set {double}((char *)&y+0xe8)=0.1
  set {double}((char *)&y+0xf0)=-0.05
  set {double}((char *)&y+0xf8)=0.025
  set {double}((char *)&y+0x100)=-0.0125
  call ((void (*)(unsigned int))srand)(12345)
  set $state=(char *)calloc(0x148,1)
  set {double}($state+0x00)=0.5
  set {double}($state+0x08)=-0.5
  set {double}($state+0x10)=0.5
  set {double}($state+0x18)=0.5
  set {double}($state+0x20)=0.0125
  set {double}($state+0x28)=-0.00875
  set {double}($state+0x30)=0.00425
  set {double}($state+0x38)=-12000000.0
  set {double}($state+0x40)=65000000.0
  set {double}($state+0x48)=28000000.0
  set {double}($state+0x50)=0.0
  set {double}($state+0x58)=2000.0
  set {double}($state+0x60)=0.0
  set $out=(char *)calloc(0x220,1)
  set $cmd=(char *)calloc(0x78,1)
  set $shared=(char *)calloc(0xbf0,1)
  call ((int (*)(void *,void *))pthread_rwlock_init)($shared,0)
  set {void *}0x5555556184b0=$shared
  set $step=0
  while $step<102
    call ((void (*)(void *,void *,void *))dyn_main)($out,$state,$cmd)
    set $step=$step+1
  end
  tbreak *(dynamics_flex+0x4fc)
  commands
    silent
    set $cross_left_ptr = *(char **)($rdi+0x8)
    set $cross_right_ptr = *(char **)($rsi+0x8)
    set $rhs_pre_cross_ptr = *(char **)($rsp+0x88)
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_flex_rhs_pre_cross.bin $rhs_pre_cross_ptr ($rhs_pre_cross_ptr+0x18)
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_cross_left.bin $cross_left_ptr ($cross_left_ptr+0x18)
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_cross_right.bin $cross_right_ptr ($cross_right_ptr+0x18)
    continue
  end
  tbreak *(dynamics_flex+0x558)
  commands
    silent
    set $base_rhs_ptr = *(char **)($rsp+0x88)
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_flex_rhs_base.bin $base_rhs_ptr ($base_rhs_ptr+0x18)
    continue
  end
  tbreak *(dynamics_flex+0x899)
  commands
    silent
    set $final_projection_output = *(char **)($r12+0x8)
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_final_projection_output.bin $final_projection_output ($final_projection_output+0x50)
    continue
  end
  tbreak *(dynamics_flex+0xb26)
  commands
    silent
    set $final_projection_input = *(char **)($rsi+0x8)
    set $final_projection_matrix = *(char **)($rdi+0x10)
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_final_projection_matrix.bin $final_projection_matrix ($final_projection_matrix+0x320)
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_final_projection_input.bin $final_projection_input ($final_projection_input+0x50)
    continue
  end
  tbreak *(dynamics_flex+0x911)
  commands
    silent
    set $rhs_combine_base = *(char **)($rsp+0x88)
    set $rhs_combine_sub = *(char **)($rsp+0x108)
    set $rhs_combine_add = *(char **)($rsp+0x148)
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_rhs_combine_base.bin $rhs_combine_base ($rhs_combine_base+0x18)
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_rhs_combine_sub.bin $rhs_combine_sub ($rhs_combine_sub+0x18)
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_rhs_combine_scalevec.bin $rsp+0x3e0 $rsp+0x3f8
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_rhs_combine_add.bin $rhs_combine_add ($rhs_combine_add+0x18)
    continue
  end
  tbreak *(dynamics_flex+0x9ad)
  commands
    silent
    set $eff_ptr = *(char **)($rsp+0x1d0)
    set $rhs_ptr = *(char **)($rsp+0x88)
    set $accel_ptr = *(char **)($r12+0x8)
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_effective_inertia.bin $eff_ptr ($eff_ptr+0x48)
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_flex_rhs.bin $rhs_ptr ($rhs_ptr+0x18)
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_effective_inertia_inv.bin $rsp+0x580 $rsp+0x5c8
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_angular_acceleration_internal.bin $accel_ptr ($accel_ptr+0x18)
    continue
  end
  tbreak *(differential_equation+0x42d)
  commands
    silent
    set $ang_acc_ptr = *(void **)($rsp+0x38)
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_angular_acceleration.bin $ang_acc_ptr ((char *)$ang_acc_ptr+0x18)
    continue
  end
  break *(RK4_Intergrator+0x1a7)
  commands
    silent
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_hk1.bin $rsp+0x30 $rsp+0x138
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_trial1.bin $rsp+0x580 $rsp+0x688
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_h_w_b.bin &H_w_B_mem ((char *)&H_w_B_mem+0x18)
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_l_c_b.bin &L_c_B_mem ((char *)&L_c_B_mem+0x18)
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_f_i_external.bin &F_I_external_mem ((char *)&F_I_external_mem+0x18)
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_j_c_b.bin &J_c_B_mem ((char *)&J_c_B_mem+0x48)
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_j_c_b_inv.bin &J_c_B_inv_mem ((char *)&J_c_B_inv_mem+0x48)
    continue
  end
  break *(RK4_Intergrator+0x272)
  commands
    silent
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_hk2.bin $rsp+0x140 $rsp+0x248
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_trial2.bin $rsp+0x580 $rsp+0x688
    continue
  end
  break *(RK4_Intergrator+0x31e)
  commands
    silent
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_hk3.bin $rsp+0x250 $rsp+0x358
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_trial3.bin $rsp+0x580 $rsp+0x688
    continue
  end
  break *(RK4_Intergrator+0x363)
  commands
    silent
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step103_hk4.bin $rsp+0x360 $rsp+0x468
    kill
    quit
  end
  # 堆上跳板：sub rsp,8; movabs rax,dyn_main; call rax; add rsp,8; int3。
  set $tramp=(char *)mmap(0,4096,7,0x22,-1,0)
  set {unsigned char}($tramp+0)=0x48
  set {unsigned char}($tramp+1)=0x83
  set {unsigned char}($tramp+2)=0xec
  set {unsigned char}($tramp+3)=0x08
  set {unsigned char}($tramp+4)=0x48
  set {unsigned char}($tramp+5)=0xb8
  set {void *}($tramp+6)=dyn_main
  set {unsigned char}($tramp+14)=0xff
  set {unsigned char}($tramp+15)=0xd0
  set {unsigned char}($tramp+16)=0x48
  set {unsigned char}($tramp+17)=0x83
  set {unsigned char}($tramp+18)=0xc4
  set {unsigned char}($tramp+19)=0x08
  set {unsigned char}($tramp+20)=0xcc
  set $rdi=$out
  set $rsi=$state
  set $rdx=$cmd
  set $rip=$tramp
  continue
end
run
