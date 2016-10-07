#!/bin/csh

####################
#  Script: batchCleanup.cmd
#     Run the 3 standard cleanup scripts and log to logs/batchCleanup.yyyymmdd
####################

#--------------------
#  setup environment
#--------------------
set tools       = $LocalVisionRoot/adminScripts
set processDate =  `date '+%Y%m%d'`
set logFile     = $LocalVisionRoot/logs/batchCleanup.$processDate

#--------------------
#  create / append to log file
#--------------------
touch $logFile       #- create if it doesn't exist
echo "" >> $logFile
echo "===> Begin Batch Cleanup: `date`" >> $logFile
echo "===> Begin Batch Cleanup: `date`" 

echo ""
echo "...  Starting Old Segment Deletion"
date
$tools/deleteCompactSegments.cmd >> $logFile

echo ""
echo "...  Starting Network Cleanup and Garbage Collection"
date
$tools/garbageCollect.cmd >> $logFile

echo ""
echo "...  Starting Compaction"
date
$tools/fullCompact.cmd >> $logFile

echo "" >> $logFile
echo "===> End Batch Cleanup: `date`" >> $logFile
echo ""
echo "===> End Batch Cleanup: `date`" 
