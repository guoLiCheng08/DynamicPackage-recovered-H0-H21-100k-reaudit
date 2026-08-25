set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/sts_valid_gold_probe.log
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
  call ((void (*)(void)) STS_Init)()
  set $pos_data = (double *)calloc(3, 8)
  set $pos_data[0] = 7000000.0
  set $pos_data[1] = 0.0
  set $pos_data[2] = 0.0
  set $pos = (char *)calloc(1, 16)
  set *(int *)$pos = 3
  set *(void **)($pos + 8) = $pos_data
  set $sun_data = (double *)calloc(3, 8)
  set $sun_data[0] = 1.0
  set $sun_data[1] = 0.0
  set $sun_data[2] = 0.0
  set $sun = (char *)calloc(1, 16)
  set *(int *)$sun = 3
  set *(void **)($sun + 8) = $sun_data
  set $q_xyz = (double *)calloc(3, 8)
  set $q = (char *)calloc(1, 24)
  set *(double *)$q = 1.0
  set *(int *)($q + 8) = 3
  set *(void **)($q + 16) = $q_xyz
  set $sts = (char *)&STS
  printf "sts_threshold_distance_and_matrix_bits\n"
  set $i = 0
  while $i < 3
    set $base = $sts + $i*0x170
    printf "sts_index=%d threshold_distance\n", $i
    x/2gx ($base + 0x50)
    set $matrix = *(void **)($base + 0x70)
    x/9gx $matrix
    set $i = $i + 1
  end
  call ((void (*)(void *, void *, void *)) Update_STS_ValidFlag)($q, $pos, $sun)
  printf "sts_valid_flags_sun_pos_x\n"
  x/1wx ($sts + 0x138)
  x/1wx ($sts + 0x2a8)
  x/1wx ($sts + 0x418)
  set $sun_data[0] = 0.0
  set $sun_data[1] = 1.0
  set $sun_data[2] = 0.0
  call ((void (*)(void *, void *, void *)) Update_STS_ValidFlag)($q, $pos, $sun)
  printf "sts_valid_flags_sun_pos_y\n"
  x/1wx ($sts + 0x138)
  x/1wx ($sts + 0x2a8)
  x/1wx ($sts + 0x418)
  set $sun_data[0] = 0.0
  set $sun_data[1] = 0.0
  set $sun_data[2] = 1.0
  call ((void (*)(void *, void *, void *)) Update_STS_ValidFlag)($q, $pos, $sun)
  printf "sts_valid_flags_sun_pos_z\n"
  x/1wx ($sts + 0x138)
  x/1wx ($sts + 0x2a8)
  x/1wx ($sts + 0x418)
  set $sun_data[0] = -1.0
  set $sun_data[1] = 0.0
  set $sun_data[2] = 0.0
  call ((void (*)(void *, void *, void *)) Update_STS_ValidFlag)($q, $pos, $sun)
  printf "sts_valid_flags_sun_neg_x\n"
  x/1wx ($sts + 0x138)
  x/1wx ($sts + 0x2a8)
  x/1wx ($sts + 0x418)
  kill
  quit
end
run
