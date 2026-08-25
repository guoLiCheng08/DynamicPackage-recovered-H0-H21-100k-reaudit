set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/getdyninput_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  call (int)init_shared()
  call (int)set_uint8_value(0, 9)
  call (int)set_uint8_value(1, 7)
  call (int)set_uint8_value(2, 5)
  call ((int (*)(int, float))set_float_value)(0, 1.25)
  call ((int (*)(int, float))set_float_value)(1, -2.5)
  call ((int (*)(int, float))set_float_value)(2, 3.75)
  call ((int (*)(int, float))set_float_value)(3, -4.125)
  call ((int (*)(int, float))set_float_value)(4, 0.125)
  call ((int (*)(int, float))set_float_value)(5, -0.25)
  call ((int (*)(int, float))set_float_value)(6, 0.5)
  call ((int (*)(int, float))set_float_value)(7, -1.0)
  call ((int (*)(int, float))set_float_value)(8, 2.0)
  call ((int (*)(int, float))set_float_value)(9, -3.0)
  call ((int (*)(int, float))set_float_value)(10, 0.0625)
  call ((int (*)(int, float))set_float_value)(11, -0.03125)
  set $out=(char *)malloc(120)
  call (void)memset($out, 0xa5, 120)
  call (void)getDynInput((void *)0x11111111, (void *)0x22222222, (void *)$out)
  printf "GETDYNINPUT_FRAME\n"
  x/15gx $out
  kill
  quit
end
run
