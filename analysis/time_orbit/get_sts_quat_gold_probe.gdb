set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/get_sts_quat_gold_probe.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  call ((void (*)(void))STS_Init)()
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_get_sts_quat_init_sts.bin &STS ((char *)&STS + 0x450)
  set $out = (double *)calloc(12,8)
  set {int}((char *)&STS + 0x18) = 0
  call ((void (*)(void *,int))GetStarTrackerQuat)($out,0)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_get_sts_quat_no_noise.bin $out ($out+4)
  set {int}((char *)&STS + 0x18) = 1
  call ((void (*)(unsigned int))srand)(1)
  call ((void (*)(void *,int))GetStarTrackerQuat)($out,0)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_get_sts_quat_noise_0.bin $out ($out+4)
  call ((void (*)(void *,int))GetStarTrackerQuat)($out,1)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_get_sts_quat_noise_1.bin $out ($out+4)
  call ((void (*)(void *,int))GetStarTrackerQuat)($out,2)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_get_sts_quat_noise_2.bin $out ($out+4)
  x/4gx $out
  kill
  quit
end
run
