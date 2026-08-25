set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set $mag_hits = 0
set $magnetic_vector_hits = 0
set $core_dynamic_hits = 0
break CoreDynamic
commands
  silent
  set $core_dynamic_hits = $core_dynamic_hits + 1
  continue
end
break *CoreDynamic+0x700
commands
  silent
  if $core_dynamic_hits == 2
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_core_dynamic_environment_step2.bin $rbx ($rbx+0x148)
  end
  continue
end
break MagneticVector
commands
  silent
  set $magnetic_vector_hits = $magnetic_vector_hits + 1
  if $magnetic_vector_hits == 2
    set $magnetic_vector_values = (double *)calloc(7,8)
    set {double}($magnetic_vector_values+0) = $xmm0.v2_double[0]
    set {double}($magnetic_vector_values+1) = $xmm1.v2_double[0]
    set {double}($magnetic_vector_values+2) = $xmm2.v2_double[0]
    set {double}($magnetic_vector_values+3) = $xmm3.v2_double[0]
  end
  continue
end
break *MagneticVector+0x2ec
commands
  silent
  if $magnetic_vector_hits == 2
    set $magnetic_vector_out = (double *)$r14
    set {double}($magnetic_vector_values+4) = $magnetic_vector_out[0]
    set {double}($magnetic_vector_values+5) = $magnetic_vector_out[1]
    set {double}($magnetic_vector_values+6) = $magnetic_vector_out[2]
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_magnetic_vector_step2.bin $magnetic_vector_values ($magnetic_vector_values+7)
  end
  continue
end
break Calc_InertialMagneticVector
commands
  silent
  set $mag_hits = $mag_hits + 1
  if $mag_hits == 2
    set $mag_values = (double *)calloc(15,8)
    set $mag_time = (double *)$rcx
    set $mag_pos = *(double **)($rsi+0x8)
    set $mag_vel = *(double **)($rdx+0x8)
    set {double}($mag_values+0) = $mag_time[0]
    set {double}($mag_values+1) = $mag_time[1]
    set {double}($mag_values+2) = $mag_time[2]
    set {double}($mag_values+3) = $mag_time[3]
    set {double}($mag_values+4) = $mag_time[4]
    set {double}($mag_values+5) = $mag_time[5]
    set {double}($mag_values+6) = $mag_pos[0]
    set {double}($mag_values+7) = $mag_pos[1]
    set {double}($mag_values+8) = $mag_pos[2]
    set {double}($mag_values+9) = $mag_vel[0]
    set {double}($mag_values+10) = $mag_vel[1]
    set {double}($mag_values+11) = $mag_vel[2]
  end
  continue
end
break *Calc_InertialMagneticVector+0x2cd
commands
  silent
  if $mag_hits == 2
    set $mag_out = *(double **)($r12+0x8)
    set {double}($mag_values+12) = $mag_out[0]
    set {double}($mag_values+13) = $mag_out[1]
    set {double}($mag_values+14) = $mag_out[2]
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_inertial_magnetic_step2.bin $mag_values ($mag_values+15)
  end
  continue
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
