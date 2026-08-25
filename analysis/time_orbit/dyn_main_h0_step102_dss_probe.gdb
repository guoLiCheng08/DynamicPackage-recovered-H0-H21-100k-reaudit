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
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step102_dss.bin &DSS ((char *)&DSS+0x2b0)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step102_core.bin $state ($state+0x108)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step102_main.bin $out ($out+0x220)
  set $dydt=(double *)calloc(33,8)
  call ((void (*)(void *,void *))differential_equation)($dydt,&y)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step102_precore_dydt.bin $dydt ($dydt+33)
  set $pos=(char *)calloc(0x10,1)
  set $acc=(char *)calloc(0x10,1)
  set $acc_data=(double *)calloc(3,8)
  set {int}$pos=3
  set {void *}($pos+8)=((char *)&y+0x38)
  set {int}$acc=3
  set {void *}($acc+8)=$acc_data
  set $calendar=(double *)calloc(6,8)
  set $ecef_data=(double *)calloc(3,8)
  set $ecef=(char *)calloc(0x10,1)
  set {int}$ecef=3
  set {void *}($ecef+8)=$ecef_data
  call ((void (*)(void *))TimeArrayGet)($calendar)
  call ((void (*)(void *,void *,void *))GCI2ECEF)($ecef,$pos,$calendar)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step102_ecef_position.bin $ecef_data ($ecef_data+3)
  tbreak *(orbit_dynamic+0x4b5)
  commands
    silent
    dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step102_ecef_acc.bin $rsp+0x170 $rsp+0x188
    continue
  end
  call ((void (*)(void *,void *))orbit_dynamic)($acc,$pos)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h0_step102_orbit_acc.bin $acc_data ($acc_data+3)
  kill
  quit
end
run
