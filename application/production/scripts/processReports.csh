#!/bin/csh

##################################################
#  $LocalVisionRoot/production/admin/scripts/processReports
#
#  Script to run production reports, cycling throughout day
#     until the next day's production processing begins.
#
#  Inputs:
#    restartFlag       indicates if initialization steps should NOT be run;
#                      value of 'restart' implies do not run setup
#    useInitsFlag      indicates if the jobs' okay files should be used
#                      to determine if execution is ready to begin;
#                      value of no implies do not check okay files
#
#  The directory jobs can contain any number of vision jobs to execute.
#  A job is a file with the suffix .job .  During processing, a new
#  batchvision session is started for each job, the job file is loaded
#  into the batchvision session and executed.
#  Output is directed to the output directory with the suffix .out.
#
#  Jobs can have a corresponding .okay file which provides tests for
#  determining if it is okay to run the job.  If the .okay file is
#  not present, the job will run as soon as its turn comes.  If the
#  file is present, the tests in the file are executed and the .okay
#  file returns a 0 if it is time for the job to run, a 1 if the job
#  should be deferred for now, and a 2 if the job should be cancelled.
#  In this way, the .okay file can perform the set of tests needed to
#  determine if the database is ready to execute this job.  For instance,
#  this script can check on the existence of a DONE.Prices token to
#  determine if it is time to start.
#
#  Jobs can also have a corresponding .wrapup file which provides steps
#  to execute once the job has completed such as sending the output to
#  the printer or mailing a note to a user.
#
#  At startup, all the jobs in the jobs directory are copied to the
#  pending directory for processing.  If the restartFlag (arg1) is set to 
#  yes, the jobs are not copied and the processing will continue with
#  whatever jobs have been left in this directory (presumably because
#  the reporting cycle was stopped for some reason).
#
#  Execution:
#     Normally called by admin/scripts/runReports via daily.daemon
#
#     To restart without reloading jobs and purging output:
#         /localvision/production/admin/scripts/restartReports
#
#     To stop processing
#         /localvision/production/admin/scripts/stopReports
#
#  Environment Variables: (sourced by dbadmin)
#     $reportArea         /lv/production/Reports
#     $tokens             /lv/production/status/tokens
#     $statusArea         /lv/production/status
#     
#       
#  Script History:
#     Aug 31, 1994        original
#     Oct 02, 1998        - relocate top level from /lv/reports to
#                           /lv/production/Reports ; 
#                         - add restart option
#                         - synch with daemon cycle processDate
#                         - integrate with status directory
##################################################

#----------
#  processing variables
#----------

set pending = $statusArea/pending

#--  arguments
set runSetup = True
set useInits = True

if ($#argv > 0) then
   if ($1 == "restart") set runSetup = False
   if ($#argv > 1 ) set useInits = $2
endif

#--  processDate is current production cycle date
if (-f $tokens/daemon.processDate) then
   set processDate = `cat $tokens/daemon.processDate`
else if ( ! $?processDate ) then
   set processDate = `date '+%Y%m%d'`
endif

produceToken $tokens/reportDaemon.inProgress
echo "... Jobs In (`pwd`) asof $processDate" 
echo " "

cd $reportArea           #- /lv/production/Reports

#----------
#  Setup Initial jobs if this is not a restart
#----------
if ($runSetup == "True") then
   #--  Purge current output directory
   set nonomatch
   touch output/tmp
   "mv" output/* oldoutput

   #--  get jobs
   cd jobs
   set nonomatch
   set jobs = (`ls *.job`)

   #--  create tokens
   cd $pending
   foreach job ($jobs)
      touch report.$job:gr
   end
endif

#--------------------
#  Run each pending job.  If .okay file exists, then execute it first.
#  If conditions are not ready, skip job for now.
#--------------------

cd $pending
touch report.
set currentList = (`ls report.*`)
"rm" -rf report.
cd $reportArea

#---  Keep processing until end of cycle
while (! -f $pending/reports.Stop) 

#---  Process Each Job
   consumeToken $statusArea/active/reportDaemon.inTest
   foreach job ($currentList)

      if ($job == "report.") continue
      set baseJob = $job:e
      set okayJob = jobs/$baseJob.okay
      set wrapJob = jobs/$baseJob.wrapup
      set outFile = output/$baseJob.out

      set jobStatus = 0
#---  If there is an okay test, run it
      if (-f $okayJob && $useInits == True ) then
#!!! replace with csh command  chmod u+x $okayJob
         csh $okayJob
         set jobStatus = $status
      endif
   
#---  Based on status: execute, postpone, or cancel job
      if ($jobStatus == 0) then
         echo "---> Begin Job: $job ... (`date`)"
         produceToken $statusArea/active/$job.inProgress
         $DBbatchvision << @@@EOJ | sed 's/V> //g' >& output/tmp 
   newLine print ; 
   $processDate asDate evaluate: [
       "jobs/$baseJob.job" asFileContents evaluateIn: ^global ;
    ] ;
?g
@@@EOJ
        if (`grep "@@@REPORT" output/tmp | wc -l` != 0) then
            vformat.exe < output/tmp | paginate > $outFile         
            "rm" output/tmp
        else
            "mv" output/tmp $outFile
        endif
        consumeToken $pending/$job
        consumeToken $statusArea/active/$job.inProgress

        if (-x $wrapJob ) then
            $wrapJob
        endif
        echo "---> End Job: $job ... (`date`)"
         
      else if ($jobStatus == 2) then
         echo "---> Cancel Job $job  ... (`date`)" 
         consumeToken $pending/$job
      endif

     end

#---  Reset Current List
   if (-f $pending/reports.Stop) break

   produceToken $statusArea/active/reportDaemon.inSleep
   sleep 360       #-- give cancel/continue events a chance to show up
   consumeToken $statusArea/active/reportDaemon.inSleep
   produceToken $statusArea/active/reportDaemon.inTest

   cd $pending
   touch report.
   set currentList = (`ls report.*`)
   "rm" -rf report.
   cd $reportArea
   if (! -f $tokens/InProgress.$processDate && ! -f $tokens/Done.$processDate ) break

end     #-- of while

consumeToken $statusArea/active/reportDaemon.inTest
consumeToken $tokens/reportDaemon.inProgress
consumeToken $statusArea/pending/reports.Stop

echo 



