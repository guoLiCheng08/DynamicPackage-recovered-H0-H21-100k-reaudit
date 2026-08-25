set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break dynamics_flex
commands
  silent
  set $p448 = *(char **)((char *)&Sat + 0x448 + 0x10)
  set $p4a8 = *(char **)((char *)&Sat + 0x4a8 + 0x10)
  set $p688 = *(char **)((char *)&Sat + 0x688 + 0x10)
  set $p898 = *(char **)((char *)&Sat + 0x898 + 0x10)
  set $p9a0 = *(char **)((char *)&Sat + 0x9a0 + 0x10)
  set $pa00 = *(char **)((char *)&Sat + 0xa00 + 0x10)
  set $pd38 = *(char **)((char *)&Sat + 0xd38 + 0x10)
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/dynamics_flex_sat_448.bin $p448 $p448+72
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/dynamics_flex_sat_4a8.bin $p4a8 $p4a8+72
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/dynamics_flex_sat_688.bin $p688 $p688+240
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/dynamics_flex_sat_898.bin $p898 $p898+240
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/dynamics_flex_sat_9a0.bin $p9a0 $p9a0+72
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/dynamics_flex_sat_a00.bin $pa00 $pa00+800
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/dynamics_flex_sat_d38.bin $pd38 $pd38+800
  kill
  quit
end
run
