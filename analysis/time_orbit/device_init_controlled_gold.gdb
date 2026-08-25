set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/device_init_controlled_gold.log
set logging overwrite on
set logging enabled on
break *Gyro_Init
commands
  silent
  set $g=(char *)&Gyro
  set {double}($g+0x80)=1.25
  set {double}($g+0x88)=-2.5
  set {double}($g+0x90)=3.75
  set {double}($g+0x98)=-4.125
  set {double}($g+0xa0)=5.5
  set {double}($g+0xa8)=-6.625
  set {double}($g+0xb0)=7.875
  set {double}($g+0xb8)=-8.25
  set {double}($g+0xc0)=9.5
  set $g2=$g+0x320
  set {double}($g2+0x80)=-1.5
  set {double}($g2+0x88)=2.25
  set {double}($g2+0x90)=-3.0
  set {double}($g2+0x98)=4.75
  set {double}($g2+0xa0)=-5.25
  set {double}($g2+0xa8)=6.5
  set {double}($g2+0xb0)=-7.75
  set {double}($g2+0xb8)=8.125
  set {double}($g2+0xc0)=-9.875
  set $gyro_return=*(void **)$rsp
  tbreak *$gyro_return
  commands $bpnum
    silent
    printf "GYRO\n"
    p/x (long)*(void **)((char *)&Gyro+0x60)-(long)&Gyro
    p/x (long)*(void **)((char *)&Gyro+0x78)-(long)&Gyro
    p/x (long)*(void **)((char *)&Gyro+0x120)-(long)&Gyro
    x/9gx ((char *)&Gyro+0xc8)
    x/9gx ((char *)&Gyro+0x320+0xc8)
    set $gyro_gold=(char *)&Gyro+0xc8
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gyro_init_gold.bin $gyro_gold $gyro_gold+72
    continue
  end
  continue
end
break *MagMeter_Init
commands
  silent
  set $m=(char *)&MagMeter
  set {double}($m+0x78)=1.25
  set {double}($m+0x80)=-2.5
  set {double}($m+0x88)=3.75
  set {double}($m+0x90)=-4.125
  set {double}($m+0x98)=5.5
  set {double}($m+0xa0)=-6.625
  set {double}($m+0xa8)=7.875
  set {double}($m+0xb0)=-8.25
  set {double}($m+0xb8)=9.5
  set $m2=$m+0x138
  set {double}($m2+0x78)=-1.5
  set {double}($m2+0x80)=2.25
  set {double}($m2+0x88)=-3.0
  set {double}($m2+0x90)=4.75
  set {double}($m2+0x98)=-5.25
  set {double}($m2+0xa0)=6.5
  set {double}($m2+0xa8)=-7.75
  set {double}($m2+0xb0)=-8.125
  set {double}($m2+0xb8)=9.875
  set $mag_return=*(void **)$rsp
  tbreak *$mag_return
  commands $bpnum
    silent
    printf "MAG\n"
    p/x (long)*(void **)((char *)&MagMeter+0x58)-(long)&MagMeter
    p/x (long)*(void **)((char *)&MagMeter+0x70)-(long)&MagMeter
    p/x (long)*(void **)((char *)&MagMeter+0x118)-(long)&MagMeter
    x/9gx ((char *)&MagMeter+0xc0)
    x/9gx ((char *)&MagMeter+0x138+0xc0)
    set $mag_gold=(char *)&MagMeter+0xc0
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/magmeter_init_gold.bin $mag_gold $mag_gold+72
    kill
    quit
  end
  continue
end
run
