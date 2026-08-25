set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $left_data = (double *)calloc(3,8)
  set $right_data = (double *)calloc(3,8)
  set $left_data[0] = 1.0
  set $left_data[1] = -2.0
  set $left_data[2] = 3.5
  set $right_data[0] = 4.0
  set $right_data[1] = 5.0
  set $right_data[2] = -6.0
  set $left = (char *)calloc(16,1)
  set $right = (char *)calloc(16,1)
  set {int}($left+0) = 2
  set {int}($right+0) = 3
  set {void *}($left+8) = $left_data
  set {void *}($right+8) = $right_data
  set $result = (int *)calloc(3,4)
  set $result[0] = ((int (*)(void *,void *))vector_add)($left,$right)
  set {int}($left+0) = 0
  set {int}($right+0) = 0
  set $result[1] = ((int (*)(void *,void *))vector_add)($left,$right)
  set {int}($left+0) = 3
  set {int}($right+0) = 3
  set $result[2] = ((int (*)(void *,void *))vector_add)($left,$right)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_vector_add_nonfatal_p3_result.bin $result ($result+3)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_vector_add_nonfatal_p3_left.bin $left_data ($left_data+3)
  kill
  quit
end
run
