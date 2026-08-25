set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dyn_main_global_abi/original_capture_dyninit_m2e.log
set logging overwrite on
set logging enabled on
break *Elements2PosVel_M+0x43
commands
  silent
  printf "original_M2E_bits="
  p/x $xmm0.v2_int64[0]
  continue
end
break *Elements2PosVel_M+0xb6
commands
  silent
  printf "original_n_bits="
  p/x $xmm5.v2_int64[0]
  printf "original_denom_bits="
  p/x $xmm10.v2_int64[0]
  continue
end
break *Elements2PosVel_M+0x10c
commands
  silent
  printf "original_velocity_factor_bits="
  p/x $xmm3.v2_int64[0]
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
  set $rdi = $init
  set $rip = DynamicInit
  continue
end
run
