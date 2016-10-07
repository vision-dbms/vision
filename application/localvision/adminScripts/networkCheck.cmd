###
# $LocalVisionRoot/adminScripts/networkCheck.cmd
#
# This script is a wrapper for onprof so that it can be run in the background.
#
###

set log = $globalLogs/onprof.log
echo "***************************************************" >> $log
echo "*  Object Network Consistency Check" >> $log
echo "*  Start Time: `date` " >> $log
echo "***************************************************" >> $log
echo "" >> $log
$VisionRoot/tools/onprof $NDFPathName >>& $log

set archivedLog = $LocalVisionRoot/logs/onprof.log.$processDate
touch $archivedLog
cat $log >> $archivedLog

