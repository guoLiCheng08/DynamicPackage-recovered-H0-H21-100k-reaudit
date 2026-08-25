set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run

# rdi is a 30-double legacy initialization array. Use all-nonzero values to
# expose every copied field; dyn_init_array must not write this caller buffer.
set $input = (double *)calloc(30, 8)
set $i = 0
while $i < 30
  set $input[$i] = ($i + 1) * 1.125
  set $i = $i + 1
end
call ((void (*)(const double *))dyn_init_array)($input)

dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_dyn_init_array/gold_dyn_init_array_p0_input_after.bin $input ($input + 30)
set $wheel = (char *)&WheelGroup
set $mtq_group = (char *)&MTQ_Group
set $thruster = (char *)&Thruster
set $wg_h = *(double **)($wheel + 0x08)
set $wg_t = *(double **)($wheel + 0x30)
set $mtq = *(double **)($mtq_group + 0x08)
set $thr_f = *(double **)($thruster + 0x68)
set $thr_t = *(double **)($thruster + 0x90)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_dyn_init_array/gold_dyn_init_array_p0_wheel_h.bin $wg_h ($wg_h + 3)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_dyn_init_array/gold_dyn_init_array_p0_wheel_t.bin $wg_t ($wg_t + 3)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_dyn_init_array/gold_dyn_init_array_p0_mtq.bin $mtq ($mtq + 3)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_dyn_init_array/gold_dyn_init_array_p0_thr_force.bin $thr_f ($thr_f + 3)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_dyn_init_array/gold_dyn_init_array_p0_thr_torque.bin $thr_t ($thr_t + 3)
quit
