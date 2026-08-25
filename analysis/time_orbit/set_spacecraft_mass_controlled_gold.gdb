set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/set_spacecraft_mass_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  call (void)SetSpacecraftMass(123.75)
  printf "MASS_A\n"
  x/1gx &SpacecraftMass
  call (void)SetSpacecraftMass(-0.0)
  printf "MASS_B\n"
  x/1gx &SpacecraftMass
  kill
  quit
end
run
