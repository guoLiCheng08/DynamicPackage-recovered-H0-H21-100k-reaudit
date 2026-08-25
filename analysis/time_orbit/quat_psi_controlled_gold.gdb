set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/quat_psi_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $s=(char *)&Sat
  set {int}($s+0x100)=4
  set {int}($s+0x104)=3
  set {int}($s+0x108)=3
  set {int}($s+0x10c)=0
  set *(void **)($s+0x110)=$s+0x120
  set {double}($s+0x200)=1.25
  set {int}($s+0x208)=3
  set {int}($s+0x20c)=0
  set *(void **)($s+0x210)=$s+0x220
  set {double}($s+0x220)=-2.5
  set {double}($s+0x228)=4.125
  set {double}($s+0x230)=3.75
  call (void)quat_psi((void *)($s+0x100), (void *)($s+0x200))
  printf "QUAT_PSI\n"
  x/12gx ($s+0x120)
  kill
  quit
end
run
