set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break *CoreDynamic+0x4a6
commands
  silent
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step6_j.bin &J_c_B_mem ((char *)&J_c_B_mem+72)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step6_jinv.bin &J_c_B_inv_mem ((char *)&J_c_B_inv_mem+72)
  set $m3_e8 = *(void **)((char *)&Sat+0x3e8+0x10)
  set $m448 = *(void **)((char *)&Sat+0x448+0x10)
  set $m4a8 = *(void **)((char *)&Sat+0x4a8+0x10)
  set $m508 = *(void **)((char *)&Sat+0x508+0x10)
  set $m568 = *(void **)((char *)&Sat+0x568+0x10)
  set $m6 = *(void **)((char *)&Sat+0x688+0x10)
  set $coupling = *(void **)((char *)&Sat+0x898+0x10)
  set $m9 = *(void **)((char *)&Sat+0x9a0+0x10)
  set $ma = *(void **)((char *)&Sat+0xa00+0x10)
  set $md = *(void **)((char *)&Sat+0xd38+0x10)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step6_model.bin $m3_e8 ((char *)$m3_e8+72)
  append binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step6_model.bin $m448 ((char *)$m448+72)
  append binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step6_model.bin $m4a8 ((char *)$m4a8+72)
  append binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step6_model.bin $m508 ((char *)$m508+72)
  append binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step6_model.bin $m568 ((char *)$m568+72)
  append binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step6_model.bin $m6 ((char *)$m6+240)
  append binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step6_model.bin $coupling ((char *)$coupling+240)
  append binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step6_model.bin $m9 ((char *)$m9+72)
  append binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step6_model.bin $ma ((char *)$ma+800)
  append binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_mixed_step6_model.bin $md ((char *)$md+800)
  kill
  quit
end
break main
commands
  silent
  disable 1
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
  call ((void (*)(unsigned int))srand)(1)
  set $state = (double *)calloc(33,8)
  set $state[0] = 1.0
  set $state[4] = 0.001
  set $state[5] = -0.002
  set $state[6] = 0.003
  set $state[7] = 7000000.0
  set $state[11] = 7546.0
  set $out = (char *)calloc(0x220,1)
  set $cmd = (char *)calloc(0x78,1)
  call ((void (*)(void *,void *,void *))dyn_main)($out,$state,$cmd)
  call ((void *(*)(void *,int))memset)($cmd,0,0x78)
  set {double}($cmd+0x08) = 0.001
  call ((void (*)(void *,void *,void *))dyn_main)($out,$state,$cmd)
  call ((void *(*)(void *,int))memset)($cmd,0,0x78)
  set {double}($cmd+0x28) = 0.01
  call ((void (*)(void *,void *,void *))dyn_main)($out,$state,$cmd)
  call ((void *(*)(void *,int))memset)($cmd,0,0x78)
  set {unsigned int}($cmd+0x58) = 1
  set {double}($cmd+0x60) = 0.01
  set {double}($cmd+0x68) = -0.01
  call ((void (*)(void *,void *,void *))dyn_main)($out,$state,$cmd)
  call ((void *(*)(void *,int))memset)($cmd,0,0x78)
  set {unsigned int}($cmd+0x70) = 1
  set {unsigned int}($cmd+0x74) = 1
  call ((void (*)(void *,void *,void *))dyn_main)($out,$state,$cmd)
  call ((void *(*)(void *,int))memset)($cmd,0,0x78)
  set {double}($cmd+0x08) = -0.001
  enable 1
  set $rdi = $out
  set $rsi = $state
  set $rdx = $cmd
  set $pc = (void *)dyn_main
  continue
end
run
