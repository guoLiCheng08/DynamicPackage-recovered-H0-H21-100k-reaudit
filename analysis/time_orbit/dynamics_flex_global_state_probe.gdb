set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/dynamics_flex_global_state_probe.log
set logging overwrite on
set logging enabled on
break dynamics_flex
commands
  silent
  printf "SADA bytes and key Sat descriptors at dynamics_flex entry\n"
  x/12gx &SADA
  x/3gx &J_c_B
  x/3gx &J_c_B_inv
  x/3gx &H_w_B
  x/3gx &L_c_B
  printf "J/Jinv/H/L backing\n"
  x/9gx *(void **)((char *)&J_c_B + 0x10)
  x/9gx *(void **)((char *)&J_c_B_inv + 0x10)
  x/3gx *(void **)((char *)&H_w_B + 0x08)
  x/3gx *(void **)((char *)&L_c_B + 0x08)
  x/3gx ((char *)&Sat + 0x448)
  x/3gx ((char *)&Sat + 0x4a8)
  x/3gx ((char *)&Sat + 0x688)
  x/3gx ((char *)&Sat + 0x898)
  x/3gx ((char *)&Sat + 0x9a0)
  x/3gx ((char *)&Sat + 0xa00)
  x/3gx ((char *)&Sat + 0xd38)
  printf "Sat flexible backing samples\n"
  x/30gx *(void **)((char *)&Sat + 0x898 + 0x10)
  x/9gx *(void **)((char *)&Sat + 0x448 + 0x10)
  x/9gx *(void **)((char *)&Sat + 0x4a8 + 0x10)
  x/30gx *(void **)((char *)&Sat + 0x688 + 0x10)
  x/9gx *(void **)((char *)&Sat + 0x9a0 + 0x10)
  x/100gx *(void **)((char *)&Sat + 0xa00 + 0x10)
  x/100gx *(void **)((char *)&Sat + 0xd38 + 0x10)
  kill
  quit
end
run
