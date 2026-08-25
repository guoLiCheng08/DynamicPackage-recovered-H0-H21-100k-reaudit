set pagination off
set confirm off
set print elements 64
set print repeats 0
set disable-randomization on
set $flex_seen = 0
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/dynamics_flex_entry_exit.log
set logging overwrite on
set logging enabled on

break dynamics_flex
commands
  silent
  set $flex_seen = $flex_seen + 1
  if $flex_seen == 1
    printf "=== dynamics_flex entry ===\n"
    info registers rdi rsi rdx rcx r8 r9 rsp rbp rip
    set $flex_arg0 = $rdi
    set $flex_arg1 = $rsi
    set $flex_arg2 = $rdx
    set $flex_arg3 = $rcx
    set $flex_out = $r8
    set $flex_arg5 = $r9
    printf "-- six vector/matrix descriptors --\n"
    x/4gx $flex_arg0
    x/4gx $flex_arg1
    x/4gx $flex_arg2
    x/4gx $flex_arg3
    x/4gx $flex_out
    x/4gx $flex_arg5
    printf "-- first data vectors (through descriptor +8) --\n"
    x/12fg *(void **)($flex_arg0 + 8)
    x/12fg *(void **)($flex_arg1 + 8)
    x/12fg *(void **)($flex_arg2 + 8)
    x/12fg *(void **)($flex_arg3 + 8)
  end
  continue
end

break *dynamics_flex+0xb43
commands
  silent
  if $flex_seen == 1
    printf "=== dynamics_flex exit after final vector_memcpy ===\n"
    printf "-- output descriptor saved from entry r8 --\n"
    x/4gx $flex_out
    printf "-- output data --\n"
    x/12fg *(void **)($flex_out + 8)
    printf "-- effective inertia and key coupled state globals --\n"
    x/9fg &J_c_B_mem
    x/9fg &J_c_B_inv_mem
    x/12fg ((char *)&Sat + 0x898 + 0x18)
  end
  kill
  quit
end

run
