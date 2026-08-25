set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/julian_centuries_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set {double}((char *)&Sat+0x200)=(double)JulianCenturies(2000.0, 1.0, 1.0, 12.0, 0.0, 0.0)
  set {double}((char *)&Sat+0x208)=(double)JulianCenturies(2024.0, 2.0, 29.0, 23.0, 59.0, 59.5)
  printf "JULIAN\n"
  x/2gx ((char *)&Sat+0x200)
  kill
  quit
end
run
