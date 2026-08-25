set pagination off
set confirm off
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run
p/x &main
p/x &init
p/x &SADA
p/x &WheelGroup
p/x ((char *)&main + (0x218ae0 - 0x1340))
x/8gx &init
x/8gx ((char *)&main + (0x218ae0 - 0x1340))
quit
