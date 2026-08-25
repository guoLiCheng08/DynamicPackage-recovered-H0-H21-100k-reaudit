set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/get_inertial_mag_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $s=(char *)&Sat
  set $b=(char *)&B_I_static
  set {int}$b=3
  set {int}($b+4)=0
  set *(void **)($b+8)=$s+0x100
  set {double}($s+0x100)=1.25
  set {double}($s+0x108)=-2.5
  set {double}($s+0x110)=3.75
  set {int}($s+0x200)=3
  set {int}($s+0x204)=0
  set *(void **)($s+0x208)=$s+0x220
  call (void)GetInertialMag((void *)($s+0x200))
  printf "INERTIAL_MAG\n"
  x/3gx ($s+0x220)
  p/x *(int *)($s+0x200)
  p/x (long)*(void **)($s+0x208)-(long)$s
  kill
  quit
end
run
