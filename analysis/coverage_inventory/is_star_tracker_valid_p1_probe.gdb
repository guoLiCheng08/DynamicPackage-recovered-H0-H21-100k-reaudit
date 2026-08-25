set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run
set $att = (char *)calloc(24, 1)
set $qxyz = (double *)calloc(3, 8)
set $pos = (char *)calloc(16, 1)
set $posdata = (double *)calloc(3, 8)
set $sun = (char *)calloc(16, 1)
set $sundata = (double *)calloc(3, 8)
set $mat = (char *)calloc(32, 1)
set $matdata = (double *)calloc(9, 8)
set $out = (int *)calloc(2, 4)
# DpQuatAbi w + DpVector xyz.
set {double}$att = 1.0
set {int}($att + 8) = 3
set {void **}($att + 16) = $qxyz
# position/sun DpVector.
set {int}$pos = 3
set {void **}($pos + 8) = $posdata
set {int}$sun = 3
set {void **}($sun + 8) = $sundata
set $posdata[0] = 7000000.0
# DpMatrix rows/cols/stride/reserved/data, identity installation.
set {int}($mat + 0) = 3
set {int}($mat + 4) = 3
set {int}($mat + 8) = 3
set {void **}($mat + 16) = $matdata
set $matdata[0] = 1.0
set $matdata[4] = 1.0
set $matdata[8] = 1.0
# Sun aligned with boresight: angle 0 <= threshold, return valid.
set $sundata[2] = 1.0
set $out[0] = ((int (*)(void *, void *, void *, void *, double, double))isStarTrackerValid)($att, $pos, $sun, $mat, 0.2, 0.1)
# Sun anti-aligned: angle pi > threshold; Earth-out returns 1, hence invalid.
set $sundata[2] = -1.0
set $out[1] = ((int (*)(void *, void *, void *, void *, double, double))isStarTrackerValid)($att, $pos, $sun, $mat, 0.2, 0.1)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_is_star_tracker_valid_p1_returns.bin $out $out+2
quit
