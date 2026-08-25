set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/mag_update_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $s=(char *)&Sat
  set $state=(char *)&y
  set $b=(char *)&B_I_static
  set {double}($state+0x38)=7000000.0
  set {double}($state+0x40)=-1200000.0
  set {double}($state+0x48)=2300000.0
  set {double}($state+0x50)=1200.0
  set {double}($state+0x58)=7300.0
  set {double}($state+0x60)=-1800.0
  set {int}$b=3
  set {int}($b+4)=0
  set *(void **)($b+8)=$s+0x100
  set {double}($s+0x300)=2024.0
  set {double}($s+0x308)=2.0
  set {double}($s+0x310)=29.0
  set {double}($s+0x318)=23.0
  set {double}($s+0x320)=59.0
  set {double}($s+0x328)=59.5
  call (void)MagUpdate((void *)($s+0x300))
  printf "MAG_UPDATE\n"
  x/3gx ($s+0x100)
  p/x *(int *)$b
  p/x (long)*(void **)($b+8)-(long)$s
  kill
  quit
end
run
