set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dyn_main_global_abi/original_capture_dyn_init_devices.log
set logging overwrite on
set logging enabled on
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
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dyn_main_global_abi/pre_y_33.bin &y ((char *)&y+264)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dyn_main_global_abi/pre_t.bin &t ((char *)&t+8)
  set $rw = (char *)&RWheel
  set $mtq = (char *)&MTQ
  set $wg = (char *)&WheelGroup
  set $mg = (char *)&MTQ_Group
  set $th = (char *)&Thruster
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dyn_main_global_abi/pre_rwheel_4.bin $rw ($rw+0x1e0)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dyn_main_global_abi/pre_mtq_6.bin $mtq ($mtq+0x150)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dyn_main_global_abi/pre_sada_104.bin &SADA ((char *)&SADA+104)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dyn_main_global_abi/pre_wheel_group_c8.bin $wg ($wg+0xc8)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dyn_main_global_abi/pre_mtq_group_110.bin $mg ($mg+0x110)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dyn_main_global_abi/pre_thruster_b0.bin $th ($th+0xb0)
  set $wgh = *(double **)($wg+0x08)
  set $wgt = *(double **)($wg+0x30)
  set $wgm = *(double **)($wg+0x60)
  set $mgm = *(double **)($mg+0x08)
  set $mgc = *(double **)($mg+0x30)
  set $mgmap = *(double **)($mg+0x78)
  set $thlever = *(double **)($th+0x18)
  set $thforcein = *(double **)($th+0x40)
  set $thforceout = *(double **)($th+0x68)
  set $thtorqueout = *(double **)($th+0x90)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dyn_main_global_abi/pre_wheel_h_3.bin $wgh ((char *)$wgh+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dyn_main_global_abi/pre_wheel_torque_3.bin $wgt ((char *)$wgt+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dyn_main_global_abi/pre_wheel_map_12.bin $wgm ((char *)$wgm+96)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dyn_main_global_abi/pre_mtq_group_3.bin $mgm ((char *)$mgm+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dyn_main_global_abi/pre_mtq_channel_6.bin $mgc ((char *)$mgc+48)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dyn_main_global_abi/pre_mtq_map_18.bin $mgmap ((char *)$mgmap+144)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dyn_main_global_abi/pre_thruster_lever_3.bin $thlever ((char *)$thlever+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dyn_main_global_abi/pre_thruster_force_input_3.bin $thforcein ((char *)$thforcein+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dyn_main_global_abi/pre_thruster_force_output_3.bin $thforceout ((char *)$thforceout+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/dyn_main_global_abi/pre_thruster_torque_output_3.bin $thtorqueout ((char *)$thtorqueout+24)
  printf "dyn_init device payload captured\n"
  kill
  quit
end
run
