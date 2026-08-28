set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
commands
  silent
  set $base = (char *)&main - 0x1340
  set $g_data_slot = (void **)($base + 0x2184b0)
  set $data = (char *)calloc(1, 0xbf0)
  call ((int (*)(void *, void *))pthread_rwlock_init)($data, 0)
  set *$g_data_slot = $data
  call ((void (*)(void))DynamicDllInit)()
  set $step = 0
  while $step < 1000
    set {float}($data + 0x38 + 64 * 4) = $step + 0.25
    set {float}($data + 0x38 + 65 * 4) = -$step - 0.5
    set {float}($data + 0x38 + 66 * 4) = $step * 0.125 + 1.0
    set $i = 0
    while $i < 17
      set {unsigned char}($data + 0x998 + 20 + $i) = ($step * 17 + $i + 0x80) & 0xff
      set $i = $i + 1
    end
    call ((void (*)(void))getDeskCommand)()
    set $flags = $base + 0x218000
    set $init_tail = $base + 0x218b08
    set $drc = $base + 0x215500
    if $step == 0
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_command_family/gold_get_desk_command_h29_thousand_first_flags.bin $flags ($flags + 4)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_command_family/gold_get_desk_command_h29_thousand_first_init_tail.bin $init_tail ($init_tail + 12)
      dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_command_family/gold_get_desk_command_h29_thousand_first_drc.bin $drc ($drc + 0x70)
    else
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_command_family/gold_get_desk_command_h29_thousand_first_flags.bin $flags ($flags + 4)
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_command_family/gold_get_desk_command_h29_thousand_first_init_tail.bin $init_tail ($init_tail + 12)
      append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_command_family/gold_get_desk_command_h29_thousand_first_drc.bin $drc ($drc + 0x70)
    end
    set $step = $step + 1
  end
  quit
end
run
