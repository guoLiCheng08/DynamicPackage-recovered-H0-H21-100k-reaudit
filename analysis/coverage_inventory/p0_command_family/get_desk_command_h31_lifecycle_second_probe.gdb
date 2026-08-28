set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  call ((int (*)(const char *))shm_unlink)("/sharedvars_example")
  call ((int (*)(void))init_shared)()
  set $base = (char *)&main - 0x1340
  set $g_data_slot = (void **)($base + 0x2184b0)
  set $data = (char *)*$g_data_slot
  set {float}($data + 0x38 + 64 * 4) = 6.25
  set {float}($data + 0x38 + 65 * 4) = -7.5
  set {float}($data + 0x38 + 66 * 4) = 8.75
  set $i = 0
  while $i < 17
    set {unsigned char}($data + 0x998 + 20 + $i) = 0xa0 + $i
    set $i = $i + 1
  end
  call ((void (*)(void))DynamicDllInit)()
  call ((void (*)(void))getDeskCommand)()
  set $flags = $base + 0x218000
  set $init_tail = $base + 0x218b08
  set $drc = $base + 0x215500
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_command_family/gold_get_desk_command_h31_lifecycle_second_after_open_flags.bin $flags ($flags + 4)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_command_family/gold_get_desk_command_h31_lifecycle_second_after_open_init_tail.bin $init_tail ($init_tail + 12)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_command_family/gold_get_desk_command_h31_lifecycle_second_after_open_drc.bin $drc ($drc + 0x70)
  call ((void (*)(void))close_shared)()
  call ((int (*)(void))init_shared)()
  call ((void (*)(void))getDeskCommand)()
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_command_family/gold_get_desk_command_h31_lifecycle_second_after_reopen_flags.bin $flags ($flags + 4)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_command_family/gold_get_desk_command_h31_lifecycle_second_after_reopen_init_tail.bin $init_tail ($init_tail + 12)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_command_family/gold_get_desk_command_h31_lifecycle_second_after_reopen_drc.bin $drc ($drc + 0x70)
  call ((void (*)(void))close_shared)()
  call ((int (*)(const char *))shm_unlink)("/sharedvars_example")
  quit
end
run
