#!/bin/csh

###
#  Network Maintenance Script
#	Designed to be invoked by /localvision/production/admin/daily.daemon
#	  as part of the nightly Vision Update Cycle
###


####################
#  Set up Environment
####################

set homedir  = $cwd
set tools    = $LocalVisionRoot/adminScripts
set logs     = $LocalVisionRoot/logs

set mainttag = $LocalVisionRoot/network/NDF.MAINT

set curr_version  = `csh $osvProd/scripts/DBVersion.csh`

if (-f $mainttag) then
   set maint_version = `cat $mainttag`
else
   set maint_version =  'not set' 
endif

echo " " 
echo "*** Begin Network Maintenance"
date

echo "          Last Maintainance $maint_version               " 
echo "          Current           $curr_version                " 
echo " " 

# check if network maintenance is even necessary and if
# not necessary exit 

if (-f $mainttag) then
   if ($maint_version == $curr_version) then
       set log = $globalLogs/maintainOk.$processDate
       echo "***************************************************" >> $log
       echo "*  Bypassing Network Maintenance - Versions Match  " >> $log
       echo "*  Start Time: `date`                              " >> $log
       echo "***************************************************" >> $log
       echo "" >>& $log
       echo "Bypassing Network Maintenance " 
       consumeToken $tokens/dsegs 
       consumeToken $tokens/compact 
       consumeToken $tokens/collect 

       cp $log $updateArea/logs
       exit 0
   endif
endif

#--------------------
#  Cleanup old Segments, if Scheduled
#--------------------
if ( -f $tokens/dsegs ) then
    produceToken $statusArea/active/DeleteSegs.InProgress
    echo "--> Begin: Network Cleanup"
    consumeToken $tokens/dsegs
    $tools/moveSegs.cmd                   #- prior day's
    $tools/deleteCompactSegments.cmd      #- 2 days old
    echo "--> End  : Network Cleanup"
    echo " "
    consumeToken $statusArea/active/DeleteSegs.InProgress
endif

#--------------------
#  Network Garbage Collection
#--------------------
if ( -f $tokens/collect ) then
    produceToken $statusArea/active/GarbageCollection.InProgress
    echo "--> Begin: Network Garbage Collection"
    date

    consumeToken $tokens/collect
    set log = $globalLogs/garbageCollect.log
    echo "***************************************************" >> $log
    echo "*  runNetworkMaintenance: Network Garbage Collection" >> $log
    echo "*  Start Time: `date` " >> $log
    echo "***************************************************" >> $log
    echo "" >>& $log
    $tools/garbageCollect.cmd >>& $log
    if ($status != 0 ) then
       echo ">>>  Garbage Collect Failed. Process Halted"
       consumeToken $statusArea/active/GarbageCollection.InProgress
       exit 1
    endif

    set log = $logs/gcollect.log.$processDate
    touch $log
    cat $globalLogs/garbageCollect.log >> $log

    echo "--> End  : Network Garbage Collection"
    date
    echo " "
    consumeToken $statusArea/active/GarbageCollection.InProgress
endif

#--------------------
#  Network Compaction
#--------------------
if ( -f $tokens/compact ) then
    produceToken $statusArea/active/Compaction.InProgress
    echo "--> Begin: Network Compaction"
    date

    consumeToken $tokens/compact
    set log = $globalLogs/compact.log
    echo "***************************************************" >> $log
    echo "*  runNetworkMaintenance: Compaction" >> $log
    echo "*  Start Time: `date` " >> $log
    echo "***************************************************" >> $log
    echo "" >>& $log

    echo "" >>& $log
    $tools/fullCompact.cmd nomove >>& $log
    if ($status != 0 ) then
       echo ">>>  Compaction Failed. Process Halted"
       consumeToken $statusArea/active/Compaction.InProgress
       exit 1
    endif

    set log = $logs/compact.log.$processDate
    touch $log
    cat $globalLogs/compact.log >> $log

    echo "--> End  : Network Compaction"
    date 
    echo " "
    consumeToken $statusArea/active/Compaction.InProgress
endif

echo "...   Total Disk Available"
diskUsage

echo "...   Setting Network Maintenance Version"
csh $osvProd/scripts/DBVersion.csh > $mainttag
cat $mainttag
echo " "       

echo " "
echo "*** End Network Maintenance"
date
echo " "


