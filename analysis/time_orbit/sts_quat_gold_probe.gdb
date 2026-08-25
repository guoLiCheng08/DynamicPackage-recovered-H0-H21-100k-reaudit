set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/sts_quat_gold_probe.log
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
  set $sts = (char *)&STS
  set $i = 0
  while $i < 3
    set $base = $sts + $i*0x170
    set *(double *)($base + 0x08) = 1.0
    set *(double *)($base + 0x130) = 0.0
    set *(double *)($base + 0xf8) = 1.0
    set *(int *)($base + 0x100) = 3
    set $err_xyz = *(double **)($base + 0x108)
    set $err_xyz[0] = 0.0
    set $err_xyz[1] = 0.0
    set $err_xyz[2] = 0.0
    set $i = $i + 1
  end
  set $q_xyz = (double *)calloc(3, 8)
  set $q_xyz[0] = 0.1
  set $q_xyz[1] = -0.2
  set $q_xyz[2] = 0.3
  set $q = (char *)calloc(1, 24)
  set *(double *)$q = 1.0
  set *(int *)($q + 8) = 3
  set *(void **)($q + 16) = $q_xyz
  call ((void (*)(void *, double)) Update_STS_Quat)($q, 0.1)
  printf "sts_quat_out_bits\n"
  x/1gx ($sts + 0x140)
  x/3gx *(void **)($sts + 0x150)
  x/1gx ($sts + 0x2b0)
  x/3gx *(void **)($sts + 0x2c0)
  x/1gx ($sts + 0x420)
  x/3gx *(void **)($sts + 0x430)
  printf "sts_time_candidate_bits\n"
  x/1gx ($sts + 0x130)
  x/1gx ($sts + 0x2a0)
  x/1gx ($sts + 0x410)
  kill
  quit
end
run
