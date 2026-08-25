set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run

# The original image is PIE. Derive its load base from main to avoid GDB's
# resolution of the libc `init` symbol instead of the ELF's local global `init`.
set $image_base = (char *)&main - 0x1340
set $init_global = $image_base + 0x218ae0
set $simu_global = $image_base + 0x215020
set $sada_global = $image_base + 0x216980
set $mtq_group_global = $image_base + 0x217040
set $thruster_global = $image_base + 0x217c60
set $wheel_group_global = $image_base + 0x217d20

# main calls DynamicDllInit before init_shared. This reproduces its fresh-process
# shared-input-unavailable startup branch and its resulting default initialization.
call ((void (*)(void))DynamicDllInit)()

# Raw blocks document descriptor layout; pointer-bearing blocks are not used for
# cross-process direct memcmp. Backing data below is address independent.
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/target_closure/gold_dynamic_dll_init_p0_init.bin $init_global ($init_global + 0xe8)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/target_closure/gold_dynamic_dll_init_p0_simu.bin $simu_global ($simu_global + 0x48)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/target_closure/gold_dynamic_dll_init_p0_sada.bin $sada_global ($sada_global + 0x68)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/target_closure/gold_dynamic_dll_init_p0_wheelgroup.bin $wheel_group_global ($wheel_group_global + 0xc8)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/target_closure/gold_dynamic_dll_init_p0_mtqgroup.bin $mtq_group_global ($mtq_group_global + 0x110)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/target_closure/gold_dynamic_dll_init_p0_thruster.bin $thruster_global ($thruster_global + 0xb0)

set $wg_h = *(double **)($wheel_group_global + 0x08)
set $wg_t = *(double **)($wheel_group_global + 0x30)
set $mtq = *(double **)($mtq_group_global + 0x08)
set $thr_f = *(double **)($thruster_global + 0x68)
set $thr_t = *(double **)($thruster_global + 0x90)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/target_closure/gold_dynamic_dll_init_p0_wheelgroup_h.bin $wg_h ($wg_h + 3)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/target_closure/gold_dynamic_dll_init_p0_wheelgroup_t.bin $wg_t ($wg_t + 3)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/target_closure/gold_dynamic_dll_init_p0_mtqgroup_moment.bin $mtq ($mtq + 3)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/target_closure/gold_dynamic_dll_init_p0_thruster_force.bin $thr_f ($thr_f + 3)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/target_closure/gold_dynamic_dll_init_p0_thruster_torque.bin $thr_t ($thr_t + 3)
quit
