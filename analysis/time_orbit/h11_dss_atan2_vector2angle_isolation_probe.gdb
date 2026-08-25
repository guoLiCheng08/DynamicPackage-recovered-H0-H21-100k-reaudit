set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $measure=(double *)calloc(3,8)
  set $zero=(double *)calloc(3,8)
  set $angles=(double *)calloc(3,8)
  set $measure_vec=(char *)calloc(0x10,1)
  set $zero_vec=(char *)calloc(0x10,1)
  set {unsigned int}($measure_vec)=3
  set {void *}($measure_vec+8)=$measure
  set {unsigned int}($zero_vec)=3
  set {void *}($zero_vec+8)=$zero

  # H11 step 68, second DSS: x=0.83362441794983411, y=-1.1195354132019451, z=1.
  set $measure[0]=0.83362441794983411
  set $measure[1]=-1.1195354132019451
  set $measure[2]=1.0
  call atan2($measure[1],$measure[2])
  set $angles[0]=((double (*)(void *,void *))vector2angle)($measure_vec,$zero_vec)

  # H11 step 73, second DSS: x=-1.6195474134279633, y=0.22740143153761325, z=1.
  set $measure[0]=-1.6195474134279633
  set $measure[1]=0.22740143153761325
  set $measure[2]=1.0
  call atan2($measure[1],$measure[2])
  set $angles[1]=((double (*)(void *,void *))vector2angle)($measure_vec,$zero_vec)

  # H11 step 85, second DSS: x=1.052115513916005, y=-1.295553286738208, z=1.
  set $measure[0]=1.052115513916005
  set $measure[1]=-1.295553286738208
  set $measure[2]=1.0
  call atan2($measure[1],$measure[2])
  set $angles[2]=((double (*)(void *,void *))vector2angle)($measure_vec,$zero_vec)

  dump binary memory /tmp/h11_dss_atan2_vector2angle_isolation.bin $angles ($angles+3)
  kill
  quit
end
run
