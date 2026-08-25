set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/get_orbit_rv_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $s=(char *)&Sat
  set {double}((char *)&y+56)=1.25
  set {double}((char *)&y+64)=-2.5
  set {double}((char *)&y+72)=3.75
  set {double}((char *)&y+80)=-4.125
  set {double}((char *)&y+88)=5.5
  set {double}((char *)&y+96)=-6.625
  call (void)Get_Orbit_RV((void *)($s+0x100), (void *)($s+0x120))
  printf "ORBIT_RV\n"
  x/3gx ($s+0x100)
  x/3gx ($s+0x120)
  kill
  quit
end
run
