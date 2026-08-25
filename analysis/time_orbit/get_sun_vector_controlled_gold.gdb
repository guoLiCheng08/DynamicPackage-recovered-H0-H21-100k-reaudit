set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/get_sun_vector_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $s=(char *)&Sat
  set {int}$s=3
  set {int}($s+4)=0
  set *(void **)($s+8)=$s+0x20
  set {double}($s+0x100)=2024.0
  set {double}($s+0x108)=2.0
  set {double}($s+0x110)=29.0
  set {double}($s+0x118)=23.0
  set {double}($s+0x120)=59.0
  set {double}($s+0x128)=59.5
  call (void)GetSunVector((void *)$s, (void *)($s+0x100))
  printf "SUN\n"
  x/3gx ($s+0x20)
  kill
  quit
end
run
