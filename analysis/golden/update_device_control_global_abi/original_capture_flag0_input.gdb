set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/original_capture.log
set logging overwrite on
set logging enabled on

break main
commands
  silent
  set $initial = (double *)calloc(1, 232)
  set $initial[0] = 0.1
  set $initial[7] = 2020.0
  set $initial[8] = 1.0
  set $initial[9] = 2.0
  set $initial[10] = 3.0
  set $initial[11] = 4.0
  set $initial[12] = 5.0
  set $initial[13] = 7000000.0
  set $initial[14] = 0.001
  set $initial[15] = 0.1
  set $initial[16] = 0.2
  set $initial[17] = 0.3
  set $initial[18] = 0.4
  set $initial[19] = 10.0
  set $initial[23] = 11.0
  set $initial[27] = 12.0
  set $initial[28] = 100.0
  call ((void (*)(void *)) DynamicInit)($initial)
  call ((void (*)(void)) Wheel_Init)()
  call ((void (*)(void)) MagTorque_Init)()
  call ((void (*)(void)) Thruster_Init)()
  set $rw = (char *)&RWheel
  set $wg = (char *)&WheelGroup
  set $mtq = (char *)&MTQ
  set $mg = (char *)&MTQ_Group
  set $th = (char *)&Thruster
  set $swm = *(double **)($wg + 0x60)
  set $smm = *(double **)($mg + 0x78)
  set $i = 0
  while $i < 4
    set *(unsigned int *)($rw + $i*0x78 + 0x04) = 0
    set *(double *)($rw + $i*0x78 + 0x18) = 1.0
    set *(double *)($rw + $i*0x78 + 0x20) = 0.05
    set *(double *)($rw + $i*0x78 + 0x28) = 0.1
    set *(double *)($rw + $i*0x78 + 0x30) = 0.0
    set $i = $i + 1
  end
  set $i = 0
  while $i < 12
    set $swm[$i] = 0.0
    set $i = $i + 1
  end
  set $swm[0] = 1.0
  set $swm[5] = 1.0
  set $swm[10] = 1.0
  set $wgh = *(double **)($wg + 0x08)
  set $wgt = *(double **)($wg + 0x30)
  set $wgh[0] = 0.0
  set $wgh[1] = 0.0
  set $wgh[2] = 0.0
  set $wgt[0] = 0.0
  set $wgt[1] = 0.0
  set $wgt[2] = 0.0
  set *(double *)($mtq + 0x08) = 0.5
  set *(double *)($mtq + 0x40) = 1.0
  set *(double *)($mtq + 0x78) = 2.0
  set *(double *)($mtq + 0xb0) = 3.0
  set *(double *)($mtq + 0xe8) = 4.0
  set *(double *)($mtq + 0x120) = 5.0
  set $i = 0
  while $i < 18
    set $smm[$i] = 0.0
    set $i = $i + 1
  end
  set $smm[0] = 1.0
  set $smm[7] = 1.0
  set $smm[14] = 1.0
  set $mgo = *(double **)($mg + 0x08)
  set $mtq_channel = *(double **)($mg + 0x30)
  set $mgo[0] = 0.0
  set $mgo[1] = 0.0
  set $mgo[2] = 0.0
  set *(double *)($th + 0x00) = 2.5
  set $lever = *(double **)($th + 0x18)
  set $force = *(double **)($th + 0x40)
  set $thruster_force_out = *(double **)($th + 0x68)
  set $thruster_torque_out = *(double **)($th + 0x90)
  set $lever[0] = 1.0
  set $lever[1] = 2.0
  set $lever[2] = 3.0
  set $force[0] = 9.0
  set $force[1] = 8.0
  set $force[2] = 7.0
  set *(unsigned int *)((char *)&SADA + 0x00) = 0
  set *(double *)((char *)&SADA + 0x18) = 0.0
  set *(double *)((char *)&SADA + 0x20) = 0.0
  set *(double *)((char *)&SADA + 0x28) = 0.0
  set *(double *)((char *)&SADA + 0x30) = 0.0
  set *(double *)((char *)&SADA + 0x38) = 0.0
  set *(double *)((char *)&SADA + 0x40) = 0.0
  set *(double *)((char *)&SADA + 0x48) = 1.0
  set *(double *)((char *)&SADA + 0x50) = 1.0
  set *(double *)((char *)&SADA + 0x58) = 0.2
  set *(double *)((char *)&SADA + 0x60) = 0.2
  set $in = (char *)calloc(1, 0x78)
  set *(double *)($in + 0x08) = 0.02
  set *(double *)($in + 0x10) = -0.04
  set *(double *)($in + 0x18) = 0.06
  set *(double *)($in + 0x20) = -0.08
  set *(double *)($in + 0x28) = 0.25
  set *(double *)($in + 0x30) = -2.0
  set *(double *)($in + 0x38) = 3.0
  set *(double *)($in + 0x40) = -4.0
  set *(double *)($in + 0x48) = 0.0
  set *(double *)($in + 0x50) = 6.0
  set *(unsigned int *)($in + 0x58) = 42
  set *(double *)($in + 0x60) = 0.3
  set *(double *)($in + 0x68) = -0.4
  set *(unsigned int *)($in + 0x70) = 1
  set *(unsigned int *)($in + 0x74) = 0
  set $jdata = *(void **)((char *)&J_c_B+16)
  set $jidata = *(void **)((char *)&J_c_B_inv+16)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/pre_input_78.bin $in ($in+0x78)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/pre_rwheel_4.bin $rw ($rw+0x1e0)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/pre_mtq_6.bin $mtq ($mtq+0x150)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/pre_sada_104.bin &SADA ((char *)&SADA+104)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/pre_wheel_h_3.bin $wgh ((char *)$wgh+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/pre_wheel_torque_3.bin $wgt ((char *)$wgt+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/pre_wheel_map_12.bin $swm ((char *)$swm+96)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/pre_mtq_group_3.bin $mgo ((char *)$mgo+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/pre_mtq_channel_6.bin $mtq_channel ((char *)$mtq_channel+48)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/pre_mtq_map_18.bin $smm ((char *)$smm+144)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/pre_thruster_scale.bin $th ($th+8)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/pre_thruster_lever_3.bin $lever ((char *)$lever+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/pre_thruster_force_input_3.bin $force ((char *)$force+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/pre_j_c_b_9.bin $jdata ((char *)$jdata+72)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/pre_j_c_b_inv_9.bin $jidata ((char *)$jidata+72)
  call ((void (*)(void *, double)) UpdateDeviceControl)($in, 0.1)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/post_rwheel_4.bin $rw ($rw+0x1e0)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/post_mtq_6.bin $mtq ($mtq+0x150)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/post_sada_104.bin &SADA ((char *)&SADA+104)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/post_wheel_h_3.bin $wgh ((char *)$wgh+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/post_wheel_torque_3.bin $wgt ((char *)$wgt+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/post_mtq_group_3.bin $mgo ((char *)$mgo+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/post_mtq_channel_6.bin $mtq_channel ((char *)$mtq_channel+48)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/post_thruster_force_3.bin $thruster_force_out ((char *)$thruster_force_out+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/post_thruster_torque_3.bin $thruster_torque_out ((char *)$thruster_torque_out+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/post_j_c_b_9.bin $jdata ((char *)$jdata+72)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/update_device_control_global_abi/flag0_input/post_j_c_b_inv_9.bin $jidata ((char *)$jidata+72)
  printf "update_device_control_controlled captured\n"
  kill
  quit
end

run
