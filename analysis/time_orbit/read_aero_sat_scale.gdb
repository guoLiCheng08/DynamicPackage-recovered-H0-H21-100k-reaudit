set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
commands
 silent
 call (void)SatParaInit()
 printf "SAT_AERO_SCALE\n"
 x/1gx (char *)&Sat+0x1070
 printf "AERO_CONST\n"
 x/1gx 0x555555411ed0
 kill
 quit
end
run
