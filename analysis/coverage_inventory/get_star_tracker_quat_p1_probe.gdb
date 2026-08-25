set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run
set $base = (char *)&main - 0x1340
set $sts = $base + 0x217540
set $xyz = (double *)calloc(3, 8)
set $out = (double *)calloc(4, 8)
set $xyz[0] = -0.25
set $xyz[1] = 0.375
set $xyz[2] = -0.5
# STS[0]: noise +0x18; Euler means +0x20; sigma +0x38; measure_quat w +0x140; xyz DpVector +0x148.
set {int}($sts + 0x18) = 0
set {double}($sts + 0x140) = 0.75
set {int}($sts + 0x148) = 3
set {void **}($sts + 0x150) = $xyz
call ((void (*)(double *, int))GetStarTrackerQuat)($out, 0)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_get_star_tracker_quat_p1_no_noise.bin $out $out+4
set {int}($sts + 0x18) = 1
set {double}($sts + 0x20) = 0.1
set {double}($sts + 0x28) = 0.2
set {double}($sts + 0x30) = -0.3
set {double}($sts + 0x38) = 0.04
set {double}($sts + 0x40) = 0.05
set {double}($sts + 0x48) = 0.06
call srand(1)
call ((void (*)(double *, int))GetStarTrackerQuat)($out, 0)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_get_star_tracker_quat_p1_seed1_noise.bin $out $out+4
quit
