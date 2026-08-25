set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run
set $base = (char *)&main - 0x1340
set $tc = $base + 0x218420
set $cmd = (unsigned char *)calloc(0x90, 1)

# Scenario A: opcode 0x77 follows the tc_cmd+0x88/+0x8a branch.
call ((void *(*)(void *, int, unsigned long))memset)($cmd, 0xa5, 0x90)
set {unsigned char}($cmd + 4) = 0x11
set {unsigned char}($cmd + 5) = 0x22
set {unsigned char}($cmd + 6) = 0x33
set {unsigned char}($cmd + 7) = 0x44
set {unsigned char}($cmd + 8) = 0x77
set {unsigned char}($cmd + 9) = 0x34
set {unsigned char}($cmd + 10) = 0x12
call ((void *(*)(void *, int, unsigned long))memset)($tc, 0, 0x90)
call ((void (*)(void *))Analyze_Command)($cmd)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_command_family/gold_analyze_command_p0_opcode77_frame.bin $cmd ($cmd + 0x90)
call ((void (*)(void *))Command_Execute)($cmd)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_command_family/gold_command_execute_p0_opcode77_tc.bin $tc ($tc + 0x90)

# Scenario B: opcode 0x66 follows tc_cmd+0x8c/+0x8e.
call ((void *(*)(void *, int, unsigned long))memset)($cmd, 0xa5, 0x90)
set {unsigned char}($cmd + 4) = 0xaa
set {unsigned char}($cmd + 5) = 0xbb
set {unsigned char}($cmd + 6) = 0xcc
set {unsigned char}($cmd + 7) = 0xdd
set {unsigned char}($cmd + 8) = 0x66
set {unsigned char}($cmd + 9) = 0x5e
set {unsigned char}($cmd + 10) = 0x7f
call ((void *(*)(void *, int, unsigned long))memset)($tc, 0, 0x90)
call ((void (*)(void *))Analyze_Command)($cmd)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_command_family/gold_analyze_command_p0_opcode66_frame.bin $cmd ($cmd + 0x90)
call ((void (*)(void *))Command_Execute)($cmd)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_command_family/gold_command_execute_p0_opcode66_tc.bin $tc ($tc + 0x90)

# Scenario C: nonzero opcode other than 0x66/0x77 leaves tc_cmd unchanged.
call ((void *(*)(void *, int, unsigned long))memset)($cmd, 0xa5, 0x90)
set {unsigned char}($cmd + 8) = 0x01
set {unsigned char}($cmd + 9) = 0xee
call ((void *(*)(void *, int, unsigned long))memset)($tc, 0x3c, 0x90)
call ((void (*)(void *))Analyze_Command)($cmd)
call ((void (*)(void *))Command_Execute)($cmd)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_command_family/gold_command_execute_p0_opcode01_tc.bin $tc ($tc + 0x90)
quit
