set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  call ((int (*)(const char *))shm_unlink)("/sharedvars_example")
  set $returns = (char *)calloc(8, 4)
  set $invalid_float_out = (float *)calloc(1, 4)
  set $reopen_float_out = (float *)calloc(1, 4)
  set $u8_out = (unsigned char *)calloc(1, 1)
  set *$invalid_float_out = 123.5
  set *$reopen_float_out = 77.25
  set *$u8_out = 0x5a
  set {int}($returns + 0) = ((int (*)(void))init_shared)()
  set {int}($returns + 4) = ((int (*)(int, float *))get_float_value)(-1, $invalid_float_out)
  set {int}($returns + 8) = ((int (*)(int, float *))get_float_value)(0x258, $invalid_float_out)
  set {int}($returns + 12) = ((int (*)(int, float *))get_float_value)(0, 0)
  set {int}($returns + 16) = ((int (*)(int, unsigned char *))get_uint8_value)(-1, $u8_out)
  set {int}($returns + 20) = ((int (*)(int, unsigned char *))get_uint8_value)(0x258, $u8_out)
  set {int}($returns + 24) = ((int (*)(int, unsigned char *))get_uint8_value)(0, 0)
  call ((void (*)(void))close_shared)()
  set {int}($returns + 28) = ((int (*)(int, float *))get_float_value)(0, $reopen_float_out)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_command_family/gold_ipc_getter_h33_invalid_second_returns.bin $returns ($returns + 32)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_command_family/gold_ipc_getter_h33_invalid_second_invalid_float_out.bin $invalid_float_out ($invalid_float_out + 1)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_command_family/gold_ipc_getter_h33_invalid_second_reopen_float_out.bin $reopen_float_out ($reopen_float_out + 1)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_command_family/gold_ipc_getter_h33_invalid_second_u8_out.bin $u8_out ($u8_out + 1)
  call ((void (*)(void))close_shared)()
  call ((int (*)(const char *))shm_unlink)("/sharedvars_example")
  quit
end
run
