set pagination off
set confirm off
set print elements 64
set print repeats 0
set disable-randomization on
set $core_hit = 0
set $flex_hit = 0
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/core_dynamics_snapshot.log
set logging overwrite on
set logging enabled on

break CoreDynamic
commands
  silent
  set $core_hit = $core_hit + 1
  if $core_hit == 1
    printf "\n=== CoreDynamic first entry ===\n"
    info registers rdi rsi rdx rcx r8 r9 rsp rbp rip
    bt 12
    printf "-- ABI registers / stack --\n"
    x/24gx $rsp
    printf "-- DynSat (first 128 bytes) --\n"
    x/16gx &DynSat
    printf "-- Sat dynamic region +0x4a8 --\n"
    x/16gx ((char *)&Sat + 0x4a8)
    printf "-- Sat coupling region +0x898 --\n"
    x/16gx ((char *)&Sat + 0x898)
    printf "-- torque and wheel momentum --\n"
    x/12gx &SatTorque
    x/4gx &H_w_B
  end
  continue
end

break dynamics_flex
commands
  silent
  set $flex_hit = $flex_hit + 1
  if $flex_hit == 1
    printf "\n=== dynamics_flex first entry ===\n"
    info registers rdi rsi rdx rcx r8 r9 rsp rbp rip
    bt 16
    printf "-- first six ABI arguments and stack arguments --\n"
    x/32gx $rsp
    printf "-- Sat+0x4a8 / Sat+0x688 / Sat+0x898 / Sat+0x9a0 / Sat+0xa00 / Sat+0xd38 --\n"
    x/12gx ((char *)&Sat + 0x4a8)
    x/12gx ((char *)&Sat + 0x688)
    x/12gx ((char *)&Sat + 0x898)
    x/12gx ((char *)&Sat + 0x9a0)
    x/12gx ((char *)&Sat + 0xa00)
    x/12gx ((char *)&Sat + 0xd38)
    printf "-- SADA / H_w_B / L_c_B / J_c_B --\n"
    x/16gx &SADA
    x/4gx &H_w_B
    x/4gx &L_c_B
    x/9gx &J_c_B_mem
  end
  kill
  quit
end

run
