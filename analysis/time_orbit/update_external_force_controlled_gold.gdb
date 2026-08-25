set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/update_external_force_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $s=(char *)&Sat
  set $f=(char *)&F_I_external
  set {int}($s+0x100)=3
  set {int}($s+0x104)=0
  set *(void **)($s+0x108)=$s+0x120
  set {double}($s+0x120)=1.25
  set {double}($s+0x128)=-2.5
  set {double}($s+0x130)=3.75
  set {int}$f=3
  set {int}($f+4)=0
  set *(void **)($f+8)=$s+0x200
  call (void)UpdateExternalForce((void *)($s+0x100))
  printf "EXTERNAL_FORCE\n"
  x/3gx ($s+0x200)
  p/x *(int *)$f
  p/x (long)*(void **)($f+8)-(long)$s
  kill
  quit
end
run
