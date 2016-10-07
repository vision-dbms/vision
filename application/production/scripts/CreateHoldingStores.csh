#!/bin/csh
###
#  job:  CreateHoldingStores.csh
#
#  Description: 
#     Creates Holding Object Space and Structures.  
#      - This script can take YYYY argument(s) or 
#        default to the upcomming year.
#      - to execute:
#           csh CreateHoldingStores.csh
#        or csh CreateHoldingStores.csh "2011 2012"  
#        or csh CreateHoldingStores.csh 2011 
#
###

#----------
#  Setup
#----------

if ($#argv > 0) then
   set years = "$*"
else
   @ currentYear = `date '+%Y'`
   @ currentYear++ 
   set years = $currentYear
endif

foreach year ( $years )

set startDate = {$year}0101

set jobName = CreateHoldingStores-{$year}

set processLog = $LocalVisionRoot/production/Updates/logs/$jobName.$processDate

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
 [
  !startDate <- $startDate asInteger asDate ;

  (Holding :storeXRef on: startDate . isNA)
  ifTrue: 
    [ Environment MaintenanceTools
      setupSpaceFor: ("Holdings" concat: startDate asInteger)
      as: "DB"
      description: ("Holdings Data As Of " concat: startDate) ;

      Holding createNewStoreAsOf: startDate ;
      Utility createSpaceInNetworkWithAnnotation: "Holding Space Setup $year" ;
    ]
  ifFalse:
    [ ">>> Invalid Year $year - Holding Structure already Created <<<" printNL;];
 ]value; 

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

end

#----------
#  and exit
#----------

if ($status == 0) then
   echo " `/bin/date` " > $tokens/Done.$jobName
else
   echo " `/bin/date` " > $tokens/Error.$jobName
endif

exit $status
