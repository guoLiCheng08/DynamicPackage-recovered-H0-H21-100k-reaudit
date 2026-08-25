set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/update_mag_moment_gold_probe.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  call ((void (*)(void)) MagTorque_Init)()
  printf "mtq_group_descriptors\n"
  printf "out n=%d p=%p channel n=%d p=%p matrix=(%d,%d,%d) p=%p\n", *(int *)((char *)&MTQ_Group+0x00), *(void **)((char *)&MTQ_Group+0x08), *(int *)((char *)&MTQ_Group+0x28), *(void **)((char *)&MTQ_Group+0x30), *(int *)((char *)&MTQ_Group+0x68), *(int *)((char *)&MTQ_Group+0x6c), *(int *)((char *)&MTQ_Group+0x70), *(void **)((char *)&MTQ_Group+0x78)
  set $matrix = *(double **)((char *)&MTQ_Group+0x78)
  set $mtq = (char *)&MTQ
  set $matrix[0] = 1.0
  set $matrix[1] = 0.0
  set $matrix[2] = 0.0
  set $matrix[3] = 0.0
  set $matrix[4] = 0.0
  set $matrix[5] = 0.0
  set $matrix[6] = 0.0
  set $matrix[7] = 1.0
  set $matrix[8] = 0.0
  set $matrix[9] = 0.0
  set $matrix[10] = 0.0
  set $matrix[11] = 0.0
  set $matrix[12] = 0.0
  set $matrix[13] = 0.0
  set $matrix[14] = 1.0
  set $matrix[15] = 0.0
  set $matrix[16] = 0.0
  set $matrix[17] = 0.0
  set *(double *)($mtq + 0x00) = 0.1
  set *(double *)($mtq + 0x38) = -0.2
  set *(double *)($mtq + 0x70) = 0.3
  set *(double *)($mtq + 0xa8) = -0.4
  set *(double *)($mtq + 0xe0) = 0.5
  set *(double *)($mtq + 0x118) = -0.6
  call ((void (*)(void)) UpdateMagMoment)()
  printf "mtq_group_channel_and_output_bits\n"
  x/6gx *(void **)((char *)&MTQ_Group+0x30)
  x/3gx *(void **)((char *)&MTQ_Group+0x08)
  kill
  quit
end
run
