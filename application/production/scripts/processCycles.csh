#!/bin/csh

####################
#  processCycles
####################



set cycleName   =   $1
set requiredJobs =   $updateArea/cycles/$cycleName
if (! -d $requiredJobs) then
  echo ">>> Token directory for cycle $cycleName not found"
  echo ">>> Expecting $requiredJobs"
  produceToken $tokens/Error.$cycleName
  exit 1
endif

#don't run if the cycle has errored out
if (`ls -1 $tokens | grep "Error.$cycleName" | wc -l` > 0) then
  exit 0
else
  #start the cycle from where it left off if tokens are still in pending
  if (`ls -1 $statusArea/pending | grep "^$cycleName\." | wc -l` > 0) then
     echo "Resuming mini-cycle $cycleName"
  else
     #start the cycle from scratch
     cd $tokens
     "rm" -rf Done.$cycleName* >& /dev/null
     foreach job (`ls $requiredJobs`)
       cp $requiredJobs/$job $statusArea/pending/$cycleName.$job
     end
  endif
endif

set pendingJobs  = $statusArea/pending

#--  get current pending jobs
cd $updateArea

produceToken $statusArea/active/cycle.$cycleName.inProgress

set anyJobsDone = "yes"
set cycleHeader = 1
while (`ls -1 $pendingJobs | wc -l` > 0 && $anyJobsDone == "yes")
set anyJobsDone = "no"

#--------------------
#  for each job still pending
#--------------------
  foreach job (`ls -1 $pendingJobs | grep "^$cycleName\."`)
   set jobFull = `basename $job`
   set jobBase = `echo $jobFull | sed s/$cycleName.//`

#--  if not cancelled check if okay to run job
   set cancelFlag = Cancel.$jobFull
   if ( ! -f $tokens/$cancelFlag && ! -f $tokens/processDate.$cancelFlag) then
      set okayToRun = 0
      # halt the cycle if error tokens exist
      if (`ls -1 $tokens | grep "Error.$cycleName" | wc -l` == 0) then

      if (-f jobs/$jobBase.okay) then
         csh jobs/$jobBase.okay
         set okayToRun = $status
      endif

      if ($okayToRun != 1 && $cycleHeader == 1) then
	echo ""
	echo "Begin Cycle: $cycleName at `date` "
        set cycleHeader = 0
      endif

      if ($okayToRun == 2) then                #- conditional cancel
         produceToken $tokens/Cancel.$jobFull
         set anyJobsDone = "yes"
      else if ($okayToRun == 0 ) then          #- okay to run job
         produceToken $statusArea/active/$jobFull.inProgress
         echo ""
         echo "Begin Job: $jobBase at `date` "
  	 if ( ! -f jobs/$jobBase.job) then
            echo ">>> Job File $jobBase.job Not Found." >> $errorLog
         endif
         csh jobs/$jobBase.job                         #--  run the job
         set jobStatus = $status

         consumeToken $statusArea/active/$jobFull.inProgress
         produceToken $tokens/Done.$jobFull
         consumeToken $tokens/Error.$jobFull
         if ($jobStatus == 1) then 
	    produceToken $tokens/Error.$jobFull
            consumeToken $tokens/Done.$jobFull
         else if ($jobStatus == 2) then
            produceToken $tokens/Warning.$jobFull
         endif
         echo ""
         echo "End Job: $jobBase at `date` "
         consumeToken $pendingJobs/$jobFull
         set anyJobsDone = "yes"
      endif
 
      endif
   else
      echo ""
      echo "Cancel Job: $jobBase at `date` "
      produceToken $tokens/Cancel.$jobFull      #-- create for date-based cancel
      produceToken $tokens/Done.$jobFull
      consumeToken $pendingJobs/$jobFull
      set anyJobsDone = "yes"
   endif  

  end
end

#if the cycle isn't on retry and there's no errors and no more pending tokens, consume pending token and write out done token
if (! -f $tokens/Retry.cycle.$cycleName && `ls -1 $tokens | grep "Error.$cycleName" | wc -l` == 0 && `ls -1 $statusArea/pending | grep "$cycleName\." | wc -l` == 0) then
   consumeToken $statusArea/pending/cycle.$cycleName
   produceToken $tokens/Done.$cycleName
endif

if ($cycleHeader == 0) then
   echo ""
   echo "End Cycle: $cycleName at `date` "
endif

consumeToken $statusArea/active/cycle.$cycleName.inProgress
