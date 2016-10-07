#!/bin/csh

####################
#  Script: garbageCollect
#	This script iterates over the object spaces and runs
#       cleanSpace in parallel, then the garbage collection
#       followed by the network check (onprof) in background
#
####################

echo  "...  Starting Network Cleanup and Garbage Collection"
date

$DBbatchvision -a -U3 << EOD | sed 's/V> //g' 
   newLine print;
   "...  Begin Garbage Collection " print;
   Utility UnixSeconds currentTime printNL;
   Utility collectGarbage
       ifTrue: [ Utility updateNetworkWithAnnotation: "Garbage Collection" ]
      ifFalse: [">>>  Garbage Collection Failed  <<<" printNL];
   "...  End Garbage Collection " print;
   Utility UnixSeconds currentTime printNL;
   newLine print ;
?g
?t
EOD

#--  If Error, Report and Abort
if ($? != 0) then
    echo ">>>  Garbage Collection Failed  <<<"
    exit 1
endif

set numSpaces = `csh $AdminTools/numSpaces.cmd`
bash $AdminTools/parallel.cmd 2 2 "csh $AdminTools/cleanSpace.cmd" "csh $AdminTools/filteredSpaces.cmd 2 $numSpaces"

#--  If Error, Report and Abort
if ($? != 0) then
    echo ">>>  Network Cleanup Failed  <<<"
    exit 1
endif

#--  Log to NDF Journal
echo "===> Maintenance: GarbageCollect `date`" >> $NDFJournal

#--  Run onprof in background
csh $AdminTools/networkCheck.cmd &

