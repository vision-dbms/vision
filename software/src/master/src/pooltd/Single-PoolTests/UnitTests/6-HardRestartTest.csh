#!/usr/bin/tcsh

#-------------------------------
# Variables
#-------------------------------

set TestDir = `pwd` 
set LogFile = $TestDir/LOGS/current

#-------------------------------
# Input Argument Check
#-------------------------------

if ($#argv != 2) then
   echo "Usage: ${0} <Type> <Socket>"
   echo "Type: File | Raw"
   echo "Socket: SocketFilePath | SocketAddress"
   exit
else if (${1} != "File" && ${1} != "Raw") then
   echo "Invalid Type in Usage"
   exit
else if (${1} == "File") then
   if (! -e ${2}) then
	echo "Invalid File specified"
	exit
   endif
endif

# Check for Tools Env variable
if (! $?Tools) then
   echo "Tools Environment variable needs to be set"
   exit
endif

set Type = ${1}
set Socket = ${2}

if ($Type == "File") then
   set serverArg = "-serverFile=$Socket"
else
   set serverArg = "-server=$Socket"
endif 

#---------------------------------
# Test
#---------------------------------

echo "-------------------------------------------------" >> $LogFile
echo "Pool     : $Socket" >> $LogFile
echo "TestDate : `date`" >> $LogFile

echo -n "HardRestart Test        :"
echo -n "HardRestart Test        :" >> $LogFile

set HardRestartResult = `$Tools/vpooladmin $serverArg -hardRestart`
echo $HardRestartResult  | grep Error  >> /dev/null

if ($status != 0) then 
    $Tools/vpooladmin $serverArg -ping=3+3 >> /dev/null
    set TotalWorkersResult = `$Tools/vpooladmin $serverArg -totalWorkers`
    set CurGenTotalWorkers = `$Tools/vpooladmin $serverArg -currentGenerationTotalWorkers`
    if ($TotalWorkersResult == $CurGenTotalWorkers) then
       echo "PASSED"
       echo "PASSED" >> $LogFile
       exit
    else
       echo "FAILED"
       echo "FAILED" >> $LogFile
       echo "All Workers are not current" >> $LogFile
    endif
else
    echo "FAILED"
    echo "FAILED" >> $LogFile
    echo $HardRestartResult >> $LogFile
endif
