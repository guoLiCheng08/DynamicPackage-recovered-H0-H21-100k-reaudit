set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/intergrator_init_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $q=(char *)malloc(24)
  set $qxyz=(double *)malloc(24)
  set $rate=(char *)malloc(16)
  set $ratedat=(double *)malloc(24)
  set $pos=(char *)malloc(16)
  set $posdat=(double *)malloc(24)
  set $vel=(char *)malloc(16)
  set $veldat=(double *)malloc(24)
  set *((double *)$q)=0.125
  set $qxyz[0]=-0.5
  set $qxyz[1]=2.25
  set $qxyz[2]=-3.75
  set *((void **)($q+16))=$qxyz
  set $ratedat[0]=-1.25
  set $ratedat[1]=0.0
  set $ratedat[2]=1.5
  set *((void **)($rate+8))=$ratedat
  set $posdat[0]=7000000.0
  set $posdat[1]=-12.5
  set $posdat[2]=42.25
  set *((void **)($pos+8))=$posdat
  set $veldat[0]=-7546.0
  set $veldat[1]=0.0625
  set $veldat[2]=8.0
  set *((void **)($vel+8))=$veldat
  call (void)memset(&y, 0xa5, 264)
  call (void)intergrator_init((void *)$q, (void *)$rate, (void *)$posdat, (void *)$veldat, 1234.5)
  printf "INTERGRATOR_INIT_T\n"
  x/1gx (char *)&y-0x20
  printf "INTERGRATOR_INIT_Y\n"
  x/33gx &y
  kill
  quit
end
run
