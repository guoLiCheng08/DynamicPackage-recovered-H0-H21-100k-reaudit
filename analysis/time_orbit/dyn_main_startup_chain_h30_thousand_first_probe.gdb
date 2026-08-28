set pagination off
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
  set {float}($data + 0x38 + 64 * 4) = 1.25
  set {float}($data + 0x38 + 65 * 4) = -2.5
  set {float}($data + 0x38 + 66 * 4) = 3.75
  set $i = 0
  while $i < 17
    set {unsigned char}($data + 0x998 + 20 + $i) = 0x80 + $i
    set $i = $i + 1
  end
  call ((void (*)(void))DynamicDllInit)()
  call ((void (*)(void))getDeskCommand)()
  set $initial = $base + 0x218ae0
  call ((void (*)(void *))dyn_init)($initial)
  set $out = (char *)calloc(0x220, 1)
  set $core = (char *)calloc(0x148, 1)
  set $cmd = (char *)calloc(0x78, 1)
  set $step = 0
  while $step < 1000
    call ((void (*)(void *, void *, const void *))dyn_main)($out, $core, $cmd)
    call ((void (*)(void *, void *))sendDynTele)(0, $out)
    if $step == 0
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_startup_chain_h30_thousand_first_state.bin $core ($core + 0x108)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_startup_chain_h30_thousand_first_global_y.bin &y ((char *)&y + 0x108)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_startup_chain_h30_thousand_first_out.bin $out ($out + 0x220)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_startup_chain_h30_thousand_first_ipc_payload.bin $payload ($data + 0xbf0)
    else
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_startup_chain_h30_thousand_first_state.bin $core ($core + 0x108)
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_startup_chain_h30_thousand_first_global_y.bin &y ((char *)&y + 0x108)
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_startup_chain_h30_thousand_first_out.bin $out ($out + 0x220)
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_startup_chain_h30_thousand_first_ipc_payload.bin $payload ($data + 0xbf0)
    end
    set $step = $step + 1
  end
  quit
end
run
