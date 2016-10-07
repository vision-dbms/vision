#!/usr/bin/tcsh

#---------------------------------------------------------------------------
# AnyDataTest.csh:
# This test is for testing the lowPriority option set when sending queries
# i.e. tests whether the low priority query is processed immediately by 
# any worker.
# In a single iteration, this script updates a variable (TestVar) in
# UpdatePool. This will automatically restart the querypool. A query 
# is then made to get the value of this variable from the QueryPool and
# since the test is run in lowPriority (AnyData) mode, the returned 
# value can be old or new depending on the worker available, but since
# this is a controlled test, there should be workers immediately 
# available to service the request. Hence the time taken should be
# very less
# To have an environment where outside updations, this script also
# runs a ExternalInfiniteUpdator.csh as background process to have simultaneous
# updations (TestVar increments) to the querypool from outside
#---------------------------------------------------------------------------  

#-------------------------------
# Variables
#-------------------------------

set TestDir = `pwd` 
set LogFile = $TestDir/LOGS/current
set Loader  = ExternalUpdator.csh

#-------------------------------
# Input Argument Check
#-------------------------------

if ($#argv != 4) then
   echo "Usage: ${0} <Type> <UpdatePoolSocket> <QueryPoolSocket> <NumIterations>"
   echo "Type: File | Raw"
   echo "Socket: SocketFilePath | SocketAddress"
   exit
else if (${1} != "File" && ${1} != "Raw") then
   echo "Invalid Type in Usage"
   exit
else if (${1} == "File") then
   if (! -e ${2} || ! -e ${3}) then
	echo "Invalid Socket File specified"
	exit
   endif
endif



# Check for Tools Env variable
if (! $?Tools) then
   echo "Tools Environment variable needs to be set"
   exit
endif


set Type = ${1}
set UpdatePoolSocket = ${2}
set QueryPoolSocket = ${3}
set NumIterations = ${4}

if ($Type == "File") then
   set updatePoolServerArg = "-serverFile=$UpdatePoolSocket"
   set queryPoolServerArg = "-serverFile=$QueryPoolSocket"
else
   set updatePoolServerArg = "-server=$UpdatePoolSocket"
   set queryPoolServerArg = "-server=$QueryPoolSocket"
endif 

#---------------------------------
# Test
#---------------------------------

echo "-------------------------------------------------" >> $LogFile
echo "UpdatePool     : $UpdatePoolSocket" >> $LogFile
echo "QueryPool      : $QueryPoolSocket"  >> $LogFile

echo "TestDate : `date`" >> $LogFile
echo -n "LatestData Test         :" >> $LogFile
echo -n "LatestData Test         :"

set TestVar = 1
set InitQuery = "\!TestVar<-$TestVar"
$Tools/vpooladmin $updatePoolServerArg -ping="$InitQuery"


#-----------------------------------------
# Start the ExternalUpdator script in BG 
#-----------------------------------------

$TestDir/$Loader $Type $UpdatePoolSocket $QueryPoolSocket $NumIterations & 
 
while ($NumIterations > 0)

   #----------------------------------------------------------------
   # Update TestVar in UpdatePool and Commit/Restart the QueryPool
   #----------------------------------------------------------------
      set RetrQuery = "TestVar asInteger printNL;"
      set TestVar = `$Tools/vpooladmin $updatePoolServerArg -ping="$RetrQuery"`

      set IncrQuery = ":TestVar<-TestVar+1"
      set CommitRestartQuery="ProcessControlTools commitToDatabaseAndRestartQueryPool: 'RestartingQueryPool'"
      @ TestVar = $TestVar + 1

      $Tools/vpooladmin $updatePoolServerArg -ping="$IncrQuery" >> /dev/null
      $Tools/vpooladmin $updatePoolServerArg -ping="$CommitRestartQuery" >> /dev/null

   #---------------------------------------------
   # Check for the Latest data in the QueryPool
   #---------------------------------------------
     set CheckQuery = "TestVar asInteger printNL;"
   
     # Have a TimeOut of 10 to detect problems with not getting immediate service
     set Result = `$Tools/vpooladmin $queryPoolServerArg -ping="$CheckQuery" -timeOut=10 -lowPriority`  

     echo "Expecting >= $TestVar Obtained $Result" 	
     echo "Expecting >= $TestVar Obtained $Result" >> $LogFile 	

     echo $Result | grep Error >> /dev/null
     if ($status == 0) then
	set TestResult = "Failed"
     endif

     @ NumIterations = $NumIterations - 1;
end

if ($?TestResult) then
   echo "FAILED"
   echo "FAILED" >> $LogFile
else
   echo "PASSED"
   echo "PASSED" >> $LogFile
endif
