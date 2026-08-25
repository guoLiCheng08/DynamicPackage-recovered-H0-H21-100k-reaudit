set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/original_capture.log
set logging overwrite on
set logging enabled on

break *DynamicInit
commands
  silent
  set $init_entry = $rdi
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/pre_init_e8.bin $init_entry ($init_entry+0xe8)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/pre_sada_104.bin &SADA ((char *)&SADA+104)
  printf "dynamic_init_entry=%p step=%.17g mass=%.17g\n", $init_entry, *(double *)$init_entry, *(double *)($init_entry+0xe0)
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
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/post_y_33.bin $y_begin ($y_begin+264)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/post_t.bin &t ((char *)&t+8)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/post_step_time.bin &step_time ((char *)&step_time+8)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/post_time_array_6.bin $time_array ($time_array+48)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/post_sat_1078.bin &Sat ((char *)&Sat+0x1078)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/post_sat_torque_168.bin &SatTorque ((char *)&SatTorque+0x168)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/post_j_c_b_9.bin $jdata ((char *)$jdata+72)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/post_j_c_b_inv_9.bin $jidata ((char *)$jidata+72)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/post_h_w_b_3.bin $hdata ((char *)$hdata+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/post_l_c_b_3.bin $ldata ((char *)$ldata+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/post_f_i_external_3.bin $fdata ((char *)$fdata+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/post_spacecraft_mass.bin &SpacecraftMass ((char *)&SpacecraftMass+8)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dynamic_init_global_abi/post_sada_104.bin &SADA ((char *)&SADA+104)
  printf "dynamic_init_exit t=%.17g step=%.17g y0=%.17g mass=%.17g\n", *(double *)&t, *(double *)&step_time, ((double *)&y)[0], *(double *)&SpacecraftMass
  kill
  quit
end

run
