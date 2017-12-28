#!/bin/csh
echo "JOBSTART `/bin/date` "
####################
#  job:  CreateHoldingStructure-2016.csh
#
#  Create the Holding Object Space and Structures for
####################

#----------
#  Setup
#----------

set batchHome   = $LocalVisionRoot/production/batchUnix
if (! -d $batchHome) then
   set batchHome = $updateArea/jobs
endif

if ( -f $batchHome/batchEnv.cmd ) then
    source $batchHome/batchEnv.cmd
endif

set year = 2016

set startDate = {$year}0101

set jobName = CreateHoldingStructure-{$year}

set batchLogs = $batchHome/logs
if (! -d $batchLogs) then
   set batchLogs = $updateLogArea
endif

set processLog = $batchLogs/$jobName.$processDate


#-- make sure no space is preset for this user code
unsetenv UserOSI

echo "" > $processLog
echo "********************************************************" >> $processLog
echo "*  Processing Holdings Structure Creation $year"          >> $processLog
echo "*  Start Time: `date` "                                   >> $processLog
echo "*  Process Date:  $processDate "                          >> $processLog
echo "********************************************************" >> $processLog
echo "" >> $processLog

$DBbatchvision << EOD |& sed 's/V> //g' >>& $processLog
 !startDate <- $startDate asInteger asDate ;

  Environment MaintenanceTools
     setupSpaceFor: ("Holdings" concat: startDate asInteger)
     as: "DB"
     description: ("Holdings Data As Of " concat: startDate) ;

  Holding createNewStoreAsOf: startDate ;

  Utility createSpaceInNetworkWithAnnotation: "Holding Space Setup $year" ;
?g
EOD

#-----
# Test
#-----
$DBbatchvision << EOD |& sed 's/V> //g' >>& $processLog
        Holding :storeXRef
                do: [ ^self objectSpace asObjectSpace spaceName print; " " print;
              ^self objectSpace print:12; " " print;
              ^self instanceList count print; " " print;
              ^self creationDate print:12; " " print;
              ^self displayPOP;
                ] . count printNL;
?g
?t
EOD

#------
# logs
#------

if ( $?globalLogs) then
    cp -p $processLog $globalLogs/
endif

if ( $?updateArea ) then
    cp -p $processLog $updateArea/submits/logs/
endif

#----------
#  and exit
#----------
exit $status
