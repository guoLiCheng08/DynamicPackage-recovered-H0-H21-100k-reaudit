set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run

# g_data is a raw mapped frame whose rwlock occupies +0x00..+0x37, floats start
# at +0x38 and command bytes start at +0x998.
set $base = (char *)&main - 0x1340
set $g_data_slot = (void **)($base + 0x2184b0)
set $data = (char *)calloc(1, 0xbf0)
call ((int (*)(void *, void *))pthread_rwlock_init)($data, 0)
set *$g_data_slot = $data

# float indices 64/65/66
set {float}($data + 0x38 + 64 * 4) = 1.25
set {float}($data + 0x38 + 65 * 4) = -2.5
set {float}($data + 0x38 + 66 * 4) = 3.75
# byte indices 20..36, all distinct.
set {unsigned char}($data + 0x998 + 20) = 0x81
set {unsigned char}($data + 0x998 + 21) = 0x82
set {unsigned char}($data + 0x998 + 22) = 0x83
set {unsigned char}($data + 0x998 + 23) = 0x84
set {unsigned char}($data + 0x998 + 24) = 0x85
set {unsigned char}($data + 0x998 + 25) = 0x86
set {unsigned char}($data + 0x998 + 26) = 0x87
set {unsigned char}($data + 0x998 + 27) = 0x88
set {unsigned char}($data + 0x998 + 28) = 0x89
set {unsigned char}($data + 0x998 + 29) = 0x8a
set {unsigned char}($data + 0x998 + 30) = 0x8b
set {unsigned char}($data + 0x998 + 31) = 0x8c
set {unsigned char}($data + 0x998 + 32) = 0x8d
set {unsigned char}($data + 0x998 + 33) = 0x8e
set {unsigned char}($data + 0x998 + 34) = 0x8f
set {unsigned char}($data + 0x998 + 35) = 0x90
set {unsigned char}($data + 0x998 + 36) = 0x91

call ((void (*)(void))getDeskCommand)()
set $flags = $base + 0x218000
set $init_tail = $base + 0x218b08
set $drc = $base + 0x215500
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_command_family/gold_get_desk_command_p0_flags.bin $flags ($flags + 4)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_command_family/gold_get_desk_command_p0_init_float_tail.bin $init_tail ($init_tail + 12)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_command_family/gold_get_desk_command_p0_drc.bin $drc ($drc + 0x70)
quit
