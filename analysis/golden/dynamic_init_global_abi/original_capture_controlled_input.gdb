set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/controlled_input/original_capture.log
set logging overwrite on
set logging enabled on

break *DynamicInit
commands
  silent
  set $init_entry = $rdi
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/controlled_input/pre_init_e8.bin $init_entry ($init_entry+0xe8)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/controlled_input/pre_sada_104.bin &SADA ((char *)&SADA+104)
  printf "dynamic_init_controlled_entry=%p step=%.17g mass=%.17g\n", $init_entry, *(double *)$init_entry, *(double *)($init_entry+0xe0)
  continue
end

break *(DynamicInit+0x1ec)
commands
  silent
  set $y_begin = (char *)&y
  set $hdata = *(void **)((char *)&H_w_B+8)
  set $ldata = *(void **)((char *)&L_c_B+8)
  set $jdata = *(void **)((char *)&J_c_B+16)
  set $jidata = *(void **)((char *)&J_c_B_inv+16)
  set $fdata = *(void **)((char *)&F_I_external+8)
  set $time_array = (char *)calloc(48, 1)
  call (void)TimeArrayGet($time_array)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/controlled_input/post_y_33.bin $y_begin ($y_begin+264)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/controlled_input/post_t.bin &t ((char *)&t+8)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/controlled_input/post_step_time.bin &step_time ((char *)&step_time+8)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/controlled_input/post_time_array_6.bin $time_array ($time_array+48)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/controlled_input/post_j_c_b_9.bin $jdata ((char *)$jdata+72)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/controlled_input/post_j_c_b_inv_9.bin $jidata ((char *)$jidata+72)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/controlled_input/post_h_w_b_3.bin $hdata ((char *)$hdata+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/controlled_input/post_l_c_b_3.bin $ldata ((char *)$ldata+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/controlled_input/post_f_i_external_3.bin $fdata ((char *)$fdata+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/controlled_input/post_spacecraft_mass.bin &SpacecraftMass ((char *)&SpacecraftMass+8)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/controlled_input/post_sada_104.bin &SADA ((char *)&SADA+104)
  printf "dynamic_init_controlled_exit t=%.17g step=%.17g y0=%.17g mass=%.17g\n", *(double *)&t, *(double *)&step_time, ((double *)&y)[0], *(double *)&SpacecraftMass
  kill
  quit
end

break main
commands
  silent
  set $init = (char *)calloc(0xe8, 1)
  set {double}($init+0x00) = 0.125
  set {double}($init+0x08) = 0.9
  set {double}($init+0x10) = 0.1
  set {double}($init+0x18) = -0.2
  set {double}($init+0x20) = 0.3
  set {float}($init+0x28) = 0.0125
  set {float}($init+0x2c) = -0.025
  set {float}($init+0x30) = 0.0375
  set {double}($init+0x38) = 2026.0
  set {double}($init+0x40) = 2.0
  set {double}($init+0x48) = 27.0
  set {double}($init+0x50) = 6.0
  set {double}($init+0x58) = 7.0
  set {double}($init+0x60) = 8.0
  set {double}($init+0x68) = 7200000.0
  set {double}($init+0x70) = 0.123
  set {double}($init+0x78) = 1.0
  set {double}($init+0x80) = 0.7
  set {double}($init+0x88) = -0.4
  set {double}($init+0x90) = 0.9
  set {double}($init+0x98) = 173.0
  set {double}($init+0xa0) = -3.5
  set {double}($init+0xa8) = 2.75
  set {double}($init+0xb0) = -3.5
  set {double}($init+0xb8) = 181.0
  set {double}($init+0xc0) = 4.25
  set {double}($init+0xc8) = 2.75
  set {double}($init+0xd0) = 4.25
  set {double}($init+0xd8) = 195.0
  set {double}($init+0xe0) = 880.0
  set $rdi = $init
  set $pc = (void *)dyn_init
  continue
end

run
