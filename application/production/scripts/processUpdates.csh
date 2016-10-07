#!/bin/csh

####################
#  processJobs
#
####################

set pendingJobs =   $statusArea/pending
set submitScript =  $adminArea/scripts/processSubmits
set startWaiting = $statusArea/active/daemon.WaitingToStart

#--- Run job until new daemon has started or this one has been stopped
while (! -f $startWaiting && ! -f $pendingJobs/updates.Stop)

#--  get current pending jobs
   cd $pendingJobs
   set nonomatch
   set joblist = (`ls -rt`)
   cd $updateArea

#--------------------
#  for each job still pending
#--------------------
   consumeToken $statusArea/active/updateDaemon.inTest
   set anyJobsDone = "no"
   foreach job ($joblist)

#--  if a 'report', skip it; if a 'submit' job, skip until end
   if (`echo $job | grep report. | wc -l` == 1) continue
   if (`echo $job | grep submit | wc -l` == 1) continue

   if (-d $updateArea/cycles/`echo $job | cut -d"." -f1`) continue

#--  if 'cycle', run processCycles
   if (`echo $job | grep "^cycle" | wc -l` == 1) then
      set cycleName = `echo $job | cut -d "." -f2`
      set okayToRun = 0
      if (-f jobs/cycle.${cycleName}.okay) then
        csh jobs/cycle.${cycleName}.okay
        set okayToRun = $status
      endif
      #don't need okay dependencies if cycle is in the middle of processing
      if (`ls -1 $statusArea/pending | grep "$cycleName\." | wc -l` > 0) then
        set okayToRun = 0
      endif
      if ($okayToRun == 0) then
        csh $osvProd/scripts/processCycles.csh $cycleName
      endif
      continue
   endif

#--  if not cancelled check if okay to run job
   set cancelFlag = Cancel.$job
   if ( ! -f $tokens/$cancelFlag && ! -f $tokens/$processDate.$cancelFlag) then
      set okayToRun = 0
      if (-f jobs/$job.okay) then
#!!! replace with csh command         chmod u+x jobs/$job.okay
         csh jobs/$job.okay
         set okayToRun = $status
      endif

      if ($okayToRun == 2) then                #- conditional cancel
         produceToken $tokens/Cancel.$job
      else if ($okayToRun == 0 ) then          #- okay to run job
         produceToken $statusArea/active/$job.inProgress
         echo ""
         echo "Begin Job: $job at `date` "
         if ( ! -f jobs/$job.job) then
            echo ">>> Job File $job.job Not Found." >> $errorLog
         endif
#!!! replace with csh command          chmod u+x jobs/$job.job
         csh jobs/$job.job                         #--  run the job
         set jobStatus = $status

         consumeToken $statusArea/active/$job.inProgress
         produceToken $tokens/Done.$job
         consumeToken $tokens/Error.$job
         if (! -f $tokens/Retry.$job) then
            consumeToken $pendingJobs/$job
            set anyJobsDone = "yes"
         endif
         if ($jobStatus == 1) then 
            produceToken $tokens/Error.$job
            consumeToken $tokens/Done.$job
         else if ($jobStatus == 2) then
            produceToken $tokens/Warning.$job
         endif
         echo ""
         echo "End Job: $job at `date` "
      endif
   else
      echo ""
      echo "Cancel Job: $job at `date` "
      produceToken $tokens/Cancel.$job      #-- create for date-based cancel
      produceToken $tokens/Done.$job
      consumeToken $pendingJobs/$job 
      set anyJobsDone = "yes"
   endif  

   end

#-- run any pending ad-hoc submits
   csh $submitScript >>& $submitLog

#--  if no jobs were completed, wait 5 minutes and try again 
   produceToken $statusArea/active/updateDaemon.inSleep
   if ($anyJobsDone == "no") sleep 300 
   consumeToken $statusArea/active/updateDaemon.inSleep
   produceToken $statusArea/active/updateDaemon.inTest
   while (-f $tokens/spaceInstall.inProgress)
      sleep 30
   end
end

consumeToken $statusArea/active/updateDaemon.inTest
echo "===> Daemon Stopped At `date` " 
echo " Pending Jobs: `ls $pendingJobs`" 

if ( -f $startWaiting ) then
   exit 0
else      #-- explicit stop token was created
  consumeToken $tokens/daemon.stop
  exit 2
endif

