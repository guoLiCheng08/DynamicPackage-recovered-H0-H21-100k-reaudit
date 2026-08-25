set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/update_device_control_controlled_gold_probe.log
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
  set $mgo[0] = 0.0
  set $mgo[1] = 0.0
  set $mgo[2] = 0.0
  set *(double *)($th + 0x00) = 2.5
  set $lever = *(double **)($th + 0x18)
  set $force = *(double **)($th + 0x40)
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
  set *(unsigned int *)($in + 0x74) = 1
  call ((void (*)(void *, double)) UpdateDeviceControl)($in, 0.1)
  printf "controlled_update_device_wheel_omega_h_torque_bits\n"
  x/3gx ($rw + 0x30)
  x/3gx ($rw + 0xa8)
  x/3gx ($rw + 0x120)
  x/3gx ($rw + 0x198)
  printf "controlled_update_device_wheel_group_bits\n"
  x/3gx $wgh
  x/3gx $wgt
  printf "controlled_update_device_mtq_group_bits\n"
  x/6gx *(void **)($mg + 0x30)
  x/3gx $mgo
  printf "controlled_update_device_thruster_bits\n"
  x/3gx *(void **)($th + 0x68)
  x/3gx *(void **)($th + 0x90)
  printf "controlled_update_device_sada_bits\n"
  x/6gx ((char *)&SADA + 0x18)
  kill
  quit
end
run
