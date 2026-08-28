set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $base = (char *)&main - 0x1340
  set $g_data_slot = (void **)($base + 0x2184b0)
  set $data = (char *)calloc(1, 0xbf0)
  call ((int (*)(void *, void *))pthread_rwlock_init)($data, 0)
  set *$g_data_slot = $data
  set $payload = $data + 0x38
  call ((void (*)(void))DynamicDllInit)()
  set $initial = $base + 0x218ae0
  # H35: high finite elliptic eccentricity with 7,000 km perigee.
  set {double}($initial + 0x68) = 700000000.0
  set {double}($initial + 0x70) = 0.99
  set {double}($initial + 0x78) = 0.85
  set {double}($initial + 0x80) = 2.30
  set {double}($initial + 0x88) = 1.10
  set {double}($initial + 0x90) = 0.37
  call ((void (*)(void *))dyn_init)($initial)
  call ((void (*)(unsigned int))srand)(1)
  set $out = (char *)calloc(0x220, 1)
  set $core = (char *)calloc(0x148, 1)
  set $cmd = (char *)calloc(0x78, 1)
  set $step = 0
  while $step < 100
    call ((void (*)(void *, void *, const void *))dyn_main)($out, $core, $cmd)
    call ((void (*)(void *, void *))sendDynTele)(0, $out)
    if $step == 0
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h35_near_parabolic_hundred_first_state.bin $core ($core + 0x108)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h35_near_parabolic_hundred_first_global_y.bin &y ((char *)&y + 0x108)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h35_near_parabolic_hundred_first_out.bin $out ($out + 0x220)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h35_near_parabolic_hundred_first_ipc_payload.bin $payload ($data + 0xbf0)
    else
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h35_near_parabolic_hundred_first_state.bin $core ($core + 0x108)
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h35_near_parabolic_hundred_first_global_y.bin &y ((char *)&y + 0x108)
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h35_near_parabolic_hundred_first_out.bin $out ($out + 0x220)
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_h35_near_parabolic_hundred_first_ipc_payload.bin $payload ($data + 0xbf0)
    end
    set $step = $step + 1
  end
  quit
end
run
