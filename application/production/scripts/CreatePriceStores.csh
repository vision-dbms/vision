#!/bin/csh
####################
#  job:  CreatePriceStores.csh
#
#  Description: 
#    Create the Pricing Object Space and Structures.
#      - This script can take YYYY argument(s) or
#        default to the upcomming year.
#      - to execute:
#           csh CreatePriceStores.csh
#        or csh CreatePriceStores.csh "2011 2012"
#        or csh CreatePricetores.csh 2011
#
#
##################

#----------
#  Setup
#----------

#-- make sure no space is preset for this user code
unsetenv UserOSI
setenv VisionImplicitClean 0

#-- Obtain price year (YYYY)
#   Default is next year

if ($#argv > 0) then
   set years = "$*"  ##-- specific YYYY as command line arg

else ## default to next year
   @ currentYear = `date '+%Y'`
   @ currentYear++ ##-- get next year
   set years = $currentYear

endif


foreach year ( $years )

 set jobName = CreatePriceStores-{$year}
 set processLog  = $LocalVisionRoot/production/Updates/logs/{$jobName}.$processDate

 set priceDate = {$year}0101

 echo "" > $processLog
 echo "********************************************************" >> $processLog
 echo "*  Processing Price Structure Creation $year"             >> $processLog
 echo "*  Start Time: `date` "                                   >> $processLog
 echo "*  Process Date:  $processDate "                          >> $processLog
 echo "********************************************************" >> $processLog
 echo "" >> $processLog

$DBbatchvision << EOD |& sed 's/V> //g' >>& $processLog
 [ !priceDate <- "$priceDate" asInteger asDate;
    PriceTools :tsStoreXRef effectiveDateAsOf: $priceDate . = $priceDate 
      ifTrue: 
       [ ">>> Invalid Year $year - Price Structure already Created <<<" printNL;]
      ifFalse: 
       [ priceDate isDate
          ifTrue: 
           [ PriceTools setupYear: $priceDate ;
             Utility createSpaceInNetworkWithAnnotation: "Price Space Setup $year" 
           ]
          ifFalse: 
           [ ">>> Invalid Year $year - Price Structure Not Created <<<" printNL ];
       ];
 ] value; 
?g
EOD

#----------
# test
#----------

$DBbatchvision << EOD |& sed 's/V> //g' >>& $processLog

$processDate evaluate:
 [ "PriceRecord storeXRef references: " printNL;
    ^global PriceRecord storeXRef
     do: [ ^self objectSpace asObjectSpace spaceName print; " " print;
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
   cp $processLog $globalLogs/
endif

if ( $?updateArea ) then
   cp $processLog $updateArea/submits/logs/
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
