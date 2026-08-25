set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/sts_valid_internal_probe.log
set logging overwrite on
set logging enabled on
break *isStarTrackerValid+0x160
commands
  silent
  printf "at_first_blas_gemv\n"
  printf "matrix=%p source_vector=%p dest_vector=%p\n", $rdi, $rsi, $rdx
  printf "source_descriptor count=%d data=%p\n", {int}$rsi, *(void **)($rsi+8)
  x/3gx *(void **)($rsi+8)
  printf "dest_descriptor count=%d data=%p\n", {int}$rdx, *(void **)($rdx+8)
  x/3gx *(void **)($rdx+8)
  disable 1
  continue
end
break main
commands
  silent
  set $xyz = (double *)calloc(3,8)
  set $quat = (char *)calloc(24,1)
  set {double}$quat = 1.0
  set {int}($quat+8) = 3
  set {void *}($quat+16) = $xyz
  set $posdata = (double *)calloc(3,8)
  set $posdata[0] = 7000000.0
  set $posdata[1] = 1000.0
  set $posdata[2] = -500.0
  set $pos = (char *)calloc(16,1)
  set {int}$pos = 3
  set {void *}($pos+8) = $posdata
  set $sundata = (double *)calloc(3,8)
  set $sundata[0] = 1.0
  set $sun = (char *)calloc(16,1)
  set {int}$sun = 3
  set {void *}($sun+8) = $sundata
  set $mdata = (double *)calloc(9,8)
  set $mdata[0] = 1.0
  set $mdata[4] = 1.0
  set $mdata[8] = 1.0
  set $mat = (char *)calloc(24,1)
  set {int}$mat = 3
  set {int}($mat+4) = 3
  set {int}($mat+8) = 3
  set {void *}($mat+16) = $mdata
  set $rdi = $quat
  set $rsi = $pos
  set $rdx = $sun
  set $rcx = $mat
  set $pc = (void *)isStarTrackerValid
  continue
end
run
