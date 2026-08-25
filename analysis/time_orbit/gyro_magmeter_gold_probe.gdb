set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gyro_magmeter_gold_probe.log
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
  call ((void (*)(void)) Gyro_Init)()
  call ((void (*)(void)) MagMeter_Init)()
  set $gyro = (char *)&Gyro
  set $mag = (char *)&MagMeter
  printf "gyro_desc_m68=(%d,%d,%d,%p) v118=(%d,%p) raw_m50=(%d,%d,%d,%p)\n", *(int *)($gyro+0x68), *(int *)($gyro+0x6c), *(int *)($gyro+0x70), *(void **)($gyro+0x78), *(int *)($gyro+0x118), *(void **)($gyro+0x120), *(int *)($gyro+0x50), *(int *)($gyro+0x54), *(int *)($gyro+0x58), *(void **)($gyro+0x60)
  printf "gyro2_desc_m68=(%d,%d,%d,%p) v118=(%d,%p)\n", *(int *)($gyro+0x388), *(int *)($gyro+0x38c), *(int *)($gyro+0x390), *(void **)($gyro+0x398), *(int *)($gyro+0x438), *(void **)($gyro+0x440)
  printf "mag_desc_m60=(%d,%d,%d,%p) v110=(%d,%p) raw_m48=(%d,%d,%d,%p)\n", *(int *)($mag+0x60), *(int *)($mag+0x64), *(int *)($mag+0x68), *(void **)($mag+0x70), *(int *)($mag+0x110), *(void **)($mag+0x118), *(int *)($mag+0x48), *(int *)($mag+0x4c), *(int *)($mag+0x50), *(void **)($mag+0x58)
  set $gmat0 = *(double **)($gyro + 0x78)
  set $gmat1 = *(double **)($gyro + 0x398)
  set $mmat0 = *(double **)($mag + 0x70)
  set $mmat1 = *(double **)($mag + 0x1a8)
  set $i = 0
  while $i < 9
    set $gmat0[$i] = 0.0
    set $gmat1[$i] = 0.0
    set $mmat0[$i] = 0.0
    set $mmat1[$i] = 0.0
    set $i = $i + 1
  end
  set $gmat0[0] = 1.0
  set $gmat0[4] = 1.0
  set $gmat0[8] = 1.0
  set $gmat1[0] = 0.0
  set $gmat1[1] = 1.0
  set $gmat1[3] = 0.0
  set $gmat1[4] = 0.0
  set $gmat1[5] = 1.0
  set $gmat1[6] = 1.0
  set *(unsigned int *)($gyro + 0x18) = 0
  set *(unsigned int *)($gyro + 0x338) = 0
  set $rate = (double *)calloc(3, 8)
  set $rate[0] = 1.25
  set $rate[1] = -2.5
  set $rate[2] = 3.75
  set $rate_vec = (char *)calloc(1, 16)
  set *(int *)$rate_vec = 3
  set *(void **)($rate_vec + 8) = $rate
  call ((void (*)(void *)) UpdateGyro)($rate_vec)
  printf "gyro_measure_bits\n"
  x/3gx *(void **)($gyro + 0x120)
  x/3gx *(void **)($gyro + 0x440)
  set $mmat0[0] = 1.0
  set $mmat0[4] = 1.0
  set $mmat0[8] = 1.0
  set $mmat1[0] = 0.0
  set $mmat1[1] = 0.0
  set $mmat1[2] = 1.0
  set $mmat1[3] = 1.0
  set $mmat1[4] = 0.0
  set $mmat1[5] = 0.0
  set $mmat1[6] = 0.0
  set $mmat1[7] = 1.0
  set $mmat1[8] = 0.0
  set *(unsigned int *)($mag + 0x10) = 0
  set *(unsigned int *)($mag + 0x148) = 0
  set $field = (double *)calloc(3, 8)
  set $field[0] = 4.0
  set $field[1] = -5.0
  set $field[2] = 6.0
  set $field_vec = (char *)calloc(1, 16)
  set *(int *)$field_vec = 3
  set *(void **)($field_vec + 8) = $field
  call ((void (*)(void *)) UpdateMagMeter)($field_vec)
  printf "magmeter_measure_bits\n"
  x/3gx *(void **)($mag + 0x118)
  x/3gx *(void **)($mag + 0x250)
  kill
  quit
end
run
