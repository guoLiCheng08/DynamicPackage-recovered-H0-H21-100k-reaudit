set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/dss_gold_probe.log
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
  call ((void (*)(void)) DSS_Init)()
  set $dss = (char *)&DSS
  set $mat0 = *(double **)($dss + 0x68)
  set $mat1 = *(double **)($dss + 0x1c0)
  set $i = 0
  while $i < 9
    set $mat0[$i] = 0.0
    set $mat1[$i] = 0.0
    set $i = $i + 1
  end
  set $mat0[0] = 1.0
  set $mat0[4] = 1.0
  set $mat0[8] = 1.0
  set $mat1[0] = 0.0
  set $mat1[1] = 1.0
  set $mat1[2] = 0.0
  set $mat1[3] = 0.0
  set $mat1[4] = 0.0
  set $mat1[5] = 1.0
  set $mat1[6] = 1.0
  set $mat1[7] = 0.0
  set $mat1[8] = 0.0
  set *(unsigned int *)($dss + 0x18) = 0
  set *(unsigned int *)($dss + 0x170) = 0
  set $sun_data = (double *)calloc(3, 8)
  set $sun_data[0] = 3.0
  set $sun_data[1] = 4.0
  set $sun_data[2] = 5.0
  set $sun_vec = (char *)calloc(1, 16)
  set *(int *)$sun_vec = 3
  set *(void **)($sun_vec + 8) = $sun_data
  call ((void (*)(void *)) UpdateDSS)($sun_vec)
  printf "dss_normalized_vectors_bits\n"
  x/3gx *(void **)($dss + 0x118)
  x/3gx *(void **)($dss + 0x270)
  printf "dss_angle_projection_bits\n"
  x/4gx ($dss + 0x138)
  x/4gx ($dss + 0x290)
  kill
  quit
end
run
