set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/intergrator_show_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $ybase=(double *)&y
  set $ybase[0]=0.5
  set $ybase[1]=-0.5
  set $ybase[2]=0.5
  set $ybase[3]=-0.5
  set $ybase[4]=1.25
  set $ybase[5]=-2.5
  set $ybase[6]=3.75
  set $ybase[7]=7.0
  set $ybase[8]=8.0
  set $ybase[9]=9.0
  set $ybase[10]=10.0
  set $ybase[11]=11.0
  set $ybase[12]=12.0
  set $ybase[13]=13.0
  set $ybase[14]=14.0
  set $ybase[15]=15.0
  set $ybase[16]=16.0
  set $ybase[17]=17.0
  set $ybase[18]=18.0
  set $ybase[19]=19.0
  set $ybase[20]=20.0
  set $ybase[21]=21.0
  set $ybase[22]=22.0
  set $ybase[23]=23.0
  set $ybase[24]=24.0
  set $ybase[25]=25.0
  set $ybase[26]=26.0
  set $ybase[27]=27.0
  set $ybase[28]=28.0
  set $ybase[29]=29.0
  set $ybase[30]=30.0
  set $ybase[31]=31.0
  set $ybase[32]=32.0
  set *((double *)((char *)&y-0x20))=-6.5
  call (void)intergrator_show()
  printf "INTERGRATOR_SHOW_T\n"
  x/1gx (char *)&y-0x20
  printf "INTERGRATOR_SHOW_Y\n"
  x/33gx &y
  kill
  quit
end
run
