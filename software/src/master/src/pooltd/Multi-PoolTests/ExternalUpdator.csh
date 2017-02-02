#!/usr/bin/tcsh

#-------------------------------
# Variables
#-------------------------------

set TestDir = `pwd` 
set LogFile = $TestDir/LOGS/current

# Number of times update is done is latest data is checked for in query Pool

#-------------------------------
# Input Argument Check
#-------------------------------

if ($#argv != 4) then
   echo "Usage: ${0} <Type> <UpdatePoolSocket> <QueryPoolSocket> <NumberOfUpdates>"
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
set NumUpdates = ${4}

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

while ($NumUpdates > 0)

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

      @ NumUpdates = $NumUpdates - 1
end
