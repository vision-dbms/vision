#!/bin/csh
echo "JOBSTART `/bin/date` "
####################
#  job:  CreatePricingStructure.csh
#
#  Create the Pricing Object Space and Structures for
#
#
####################

#----------
#  Setup
#----------
set batchHome   = $LocalVisionRoot/production/batchUnix

if (! -d $batchHome) then
   set batchHome = $updateArea/jobs
endif

if (-f $batchHome/batchEnv.cmd ) then
        source $batchHome/batchEnv.cmd
endif

set year = 2016

set startDate = {$year}0101

set jobName = CreatePricingStructure-{$year}

set batchLogs = $batchHome/logs

if (! -d $batchLogs) then
        set batchLogs = $updateLogArea
endif

set processLog = $batchLogs/$jobName.$processDate

#-- make sure no space is preset for this user code
unsetenv UserOSI
setenv VisionImplicitClean 0

echo "" > $processLog
echo "********************************************************" >> $processLog
echo "*  Processing Price Structure Creation $year"             >> $processLog
echo "*  Start Time: `date` "                                   >> $processLog
echo "*  Process Date:  $processDate "                          >> $processLog
echo "********************************************************" >> $processLog
echo "" >> $processLog

$DBbatchvision << EOD |& sed 's/V> //g' >>& $processLog
  #'visionImplicitPropertyCleanup' implemented in batchvision 7.0
  (Utility SessionAttribute getMessage: "visionImplicitPropertyCleanup" . isntNA)
     ifTrue: [ Utility SessionAttribute visionImplicitPropertyCleanup <- FALSE; ];

  PriceTools setupYear: $startDate;

  Utility createSpaceInNetworkWithAnnotation: "Price Space Setup $year" ;
?g
EOD


#----------
# test
#----------

$DBbatchvision << EOD |& sed 's/V> //g' >>& $processLog

$processDate evaluate:
 [ "PriceRecord storeXRef references: " printNL;
    ^global PriceRecord storeXRef
    do: [  ^self objectSpace asObjectSpace spaceName print; " " print;
           ^self objectSpace print:12; " " print;
           ^self instanceList count print; " " print;
           ^self creationDate print:12; " " print;
           ^self displayPOP;
         ] . count printNL;
     newLine print;
 ];
?g
?t
EOD

#-----
# logs
#-----

if ( $?globalLogs) then
        cp -p $processLog $globalLogs
endif

if ( $?updateArea ) then
        cp -p $processLog $updateArea/submits/logs
endif

#----------
#  and exit
#----------
exit $status
