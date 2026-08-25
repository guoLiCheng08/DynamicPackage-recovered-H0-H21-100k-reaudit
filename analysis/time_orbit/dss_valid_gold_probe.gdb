set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/dss_valid_gold_probe.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $position_data = (double *)calloc(3,8)
  set $sun_data = (double *)calloc(3,8)
  set $position_data[0] = 7000000.0
  set $position = (char *)calloc(16,1)
  set $sun = (char *)calloc(16,1)
  set {int}$position = 3
  set {void *}($position+8) = $position_data
  set {int}$sun = 3
  set {void *}($sun+8) = $sun_data
  set $sun_data[0] = 1.0
  call ((void (*)(void *, void *))UpdateSunSensorValidFlag)($sun,$position)
  printf "dss_valid_sun_pos_x\n"
  x/1wx ((char *)&DSS + 0x108)
  x/1wx ((char *)&DSS + 0x260)
  set $sun_data[0] = -1.0
  set $sun_data[1] = 0.0
  call ((void (*)(void *, void *))UpdateSunSensorValidFlag)($sun,$position)
  printf "dss_valid_sun_neg_x\n"
  x/1wx ((char *)&DSS + 0x108)
  x/1wx ((char *)&DSS + 0x260)
  set $sun_data[0] = 0.0
  set $sun_data[1] = 1.0
  call ((void (*)(void *, void *))UpdateSunSensorValidFlag)($sun,$position)
  printf "dss_valid_sun_pos_y\n"
  x/1wx ((char *)&DSS + 0x108)
  x/1wx ((char *)&DSS + 0x260)
  kill
  quit
end
run
