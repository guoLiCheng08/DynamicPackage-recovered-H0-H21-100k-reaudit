set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run

# dyn_init establishes WheelGroup, MTQ_Group and Thruster descriptor backings.
set $init = (char *)calloc(0xe8, 1)
call ((void (*)(void *))dyn_init)($init)

# Fixed nonzero source backings exercise every three-double copy region.
set $wg_h = *(double **)((char *)&WheelGroup + 0x08)
set $wg_t = *(double **)((char *)&WheelGroup + 0x30)
set $mtq = *(double **)((char *)&MTQ_Group + 0x08)
set $thr_f = *(double **)((char *)&Thruster + 0x68)
set $thr_t = *(double **)((char *)&Thruster + 0x90)
set $wg_h[0] = 1.125
set $wg_h[1] = -2.25
set $wg_h[2] = 3.5
set $wg_t[0] = -4.75
set $wg_t[1] = 5.875
set $wg_t[2] = -6.0
set $mtq[0] = 7.25
set $mtq[1] = -8.5
set $mtq[2] = 9.75
set $thr_f[0] = -10.125
set $thr_f[1] = 11.25
set $thr_f[2] = -12.5
set $thr_t[0] = 13.625
set $thr_t[1] = -14.75
set $thr_t[2] = 15.875
set {double}((char *)&SADA + 0x18) = 16.125
set {double}((char *)&SADA + 0x20) = -17.25

# A nonzero tail sentinel demonstrates that getSADAangle writes only two doubles.
set $out = (char *)calloc(0x90, 1)
call ((void *(*)(void *, int, unsigned long))memset)($out, 0xa5, 0x90)
call ((void (*)(void *))UpdateCoreDynInput)($out)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/target_closure/gold_update_core_dyn_input_p4_output.bin $out ($out + 0x90)
quit
