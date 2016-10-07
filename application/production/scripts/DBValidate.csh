#!/bin/csh 
#

####################
# $osvProd/scripts/DBValidate.csh
#
# This script can be used to validate database consistancy using
# a basic query 'Utility collectGarbage' that will step through
# the entire database. If the query fails or it takes an excesive 
# duration to run, we will exit with a Non-Zero exit status.
#
# Input Options:  
#   -n  <NDF file to validate>   {Default: $NDFPathName}
#   -t  <Timeout in Seconds>     {Default: 1200}
#
# Usage Notes: 
#   1) This script will produce some "noise" that you may chose to ignore
#      by sending the interum output to /dev/null. 
#   2) Be careful when setting an aggressive TimeOut, -t,  value. If the system
#      is under load or ther are other variables that will throw off your results, 
#      you could get a False Negative with the results failing the timeout only.
#      The Default of 20 minutes "should" be sufficient. If the query finishes
#      sooner, control will be returned to the caller.
#
#   (e.g.)
#     csh $osvProd/scripts/DBValidate.csh -n $NDFPathName -t 60 >>& /dev/null
#     set dbValid = $status
#     if ($dbValid == 1) then
#        ... Database Validation Failed!
#     endif
#
# Exit Codes:
#   0 = Test Passed
#   1 = Test Failed
#              
####################

#-------------
# Environment
#-------------
set ndfToUse    = $NDFPathName
set timeOutSecs = 1200
set poolOut     = /tmp/dbc.$$.poolOut
set serverFile  = /tmp/dbc.$$.serverFile
set debugOn     = ""

if (! $?tools)         set tools = $VisionRoot/bin
if (! $?DBbatchvision) set DBbatchvision = $tools/batchvision
if (! $?VPoolArea)     set VPoolArea = $osvAdmin/tmp

#----------
#  get arguments
#----------
while ( $#argv > 0)
   switch ( $1 )
      case "-N":
      case "-n":
         shift
         set ndfToUse = $1
         shift
         breaksw
      case "-T":
      case "-t":
         shift
         set timeOutSecs = $1
         shift
         breaksw
      default:
         shift
         breaksw
   endsw
end

#-- Start temporary vpool to manage the batchvision session.
setenv WorkerSource "$DBbatchvision -n $ndfToUse"
setenv WorkerSpawnErrorLmt  1
setenv EvaluationAttempts  0
setenv VisionExitOnError 1
"rm" -f $serverFile # Make sure we delete the serverFile if it already exists.
$tools/vpool -serverFile=$serverFile $VPoolArea/sockets/ epool &

#-- Make sure we wait until the serverFile has been created, or 60 seconds, whichever comes first.
set maxtries = 60
while ( ! -f $serverFile )
  if ( $maxtries == 0 ) exit 1
  @ maxtries--
  sleep 1
end

#-- Submit Test Query with timeout and check for "Error" in results
$tools/vpooladmin -serverFile=$serverFile -timeOut=$timeOutSecs -ping="Utility collectGarbage" >& $poolOut
cat $poolOut
grep Error $poolOut >> /dev/null
set okay = $status

#-- We are checking for a Negative so we want to Inverse the result for the caller.
if ($okay == 0) then
  set okay = 1
else
  set okay = 0
endif

#-- Stop temporary vpool and Cleanup
$tools/vpooladmin -serverFile=$serverFile -hardStop 
"rm" -f $serverFile
"rm" -f $poolOut

exit $okay
