#!/bin/csh

##################################################
# runDailySetup.csh 
#

###
#  Designed to be invoked by /localvision/production/admin/daily.daemon
#   as part of the nightly Vision Update Cycle
#
#  This script is responsible for scheduling the jobs that are
#     required to run as part of this cycle based on the day of
#     the week and holiday status
#
###

echo "*** Begin Daily Setup"
date
echo " "

cd $adminArea
#---  Load current day's setup
source helpers/setup.$dayOfWeek

#--- Check if Holiday
if (-f $adminArea/holidays/$processDate) then
  produceToken $tokens/Run.Holiday

  #--- If a RunHoliday job exists, run it
  if (-f $updateArea/jobs/RunHoliday.job) then
    touch $statusArea/pending/RunHoliday
  endif

endif

#--- Display Global Tokens
ls $tokens

#---  Create update, error, warning, and report log files
touch $updateLog $errorLog $warningLog $reportLog $submitLog $listenerLog

echo " "
echo "*** End Daily Setup"
date
echo " "

