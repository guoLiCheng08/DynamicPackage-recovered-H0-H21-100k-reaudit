set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  # Original ABI symbol under test: inv_CAL_M3 (two raw double[9] pointers).
  set $input=(double *)calloc(9,8)
  set $output=(double *)calloc(9,8)
  set $input[0]=4.0
  set $input[1]=1.0
  set $input[2]=2.0
  set $input[3]=0.0
  set $input[4]=3.0
  set $input[5]=-1.0
  set $input[6]=0.0
  set $input[7]=0.0
  set $input[8]=2.0
  call ((void (*)(void *,void *))inv_CAL_M3)($input,$output)
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/inv_cal_m3_gold.bin $output $output+9
  kill
  quit
end
run
