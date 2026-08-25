set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/update_sat_inertia_flag0_default_probe.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $initial = (double *)calloc(1, 232)
  set $initial[0] = 0.1
  set $initial[7] = 2020.0
  set $initial[8] = 1.0
  set $initial[9] = 2.0
  set $initial[10] = 3.0
  set $initial[11] = 4.0
  set $initial[12] = 5.0
  set $initial[13] = 7000000.0
  set $initial[14] = 0.001
  set $initial[15] = 0.1
  set $initial[16] = 0.2
  set $initial[17] = 0.3
  set $initial[18] = 0.4
  set $initial[19] = 10.0
  set $initial[23] = 11.0
  set $initial[27] = 12.0
  set $initial[28] = 100.0
  call ((void (*)(void *)) DynamicInit)($initial)
  call ((void (*)(void)) Wheel_Init)()
  call ((void (*)(int)) Update_sat_inertia_xw)(0)
  printf "flag0_jc_mem_bits\n"
  x/9gx &J_c_B_mem
  printf "flag0_jc_inv_mem_bits\n"
  x/9gx &J_c_B_inv_mem
  printf "flag0_sat_m3_e8_bits\n"
  x/9gx *(void **)((char *)&Sat + 0x3e8 + 0x10)
  printf "flag0_sat_m3_448_bits\n"
  x/9gx *(void **)((char *)&Sat + 0x448 + 0x10)
  printf "flag0_sat_m3_4a8_bits\n"
  x/9gx *(void **)((char *)&Sat + 0x4a8 + 0x10)
  printf "flag0_sat_m3_508_bits\n"
  x/9gx *(void **)((char *)&Sat + 0x508 + 0x10)
  printf "flag0_sat_m3_568_bits\n"
  x/9gx *(void **)((char *)&Sat + 0x568 + 0x10)
  printf "flag0_sat_m6_bits\n"
  x/30gx *(void **)((char *)&Sat + 0x688 + 0x10)
  printf "flag0_sat_c_bits\n"
  x/30gx *(void **)((char *)&Sat + 0x898 + 0x10)
  printf "flag0_sat_m9_bits\n"
  x/9gx *(void **)((char *)&Sat + 0x9a0 + 0x10)
  printf "flag0_sat_ma_bits\n"
  x/100gx *(void **)((char *)&Sat + 0xa00 + 0x10)
  printf "flag0_sat_md_bits\n"
  x/100gx *(void **)((char *)&Sat + 0xd38 + 0x10)
  kill
  quit
end
run
