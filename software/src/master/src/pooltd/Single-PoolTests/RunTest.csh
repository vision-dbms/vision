#!/bin/csh

#-------------------------------
# Variables
#-------------------------------

set TestDir = `pwd` 
set LogDir = $TestDir/LOGS/
# Environment Variable "Tools" needs to be set

#-------------------------------
# Input Argument Check
#-------------------------------

if ($#argv != 3) then
   echo "Usage: ${0} <Type> <Socket> <Iterations>"
   echo "Options:"
   echo "     Type    : File | Raw"
   echo "               (File-UnixDomainSocketFile, Address-SocketAddress)"
   echo "     Socket  : SocketFilePath | SocketAddress"
   exit
else if (${1} != "File" && ${1} != "Raw") then
   echo "Invalid Type Option (Type-->File|Address)""
   exit
else if (${1} == "File") then
   if (! -e ${2}) then
	echo "Invalid UnixDomain SocketFile Path "${2} " specified"
	exit
   endif
endif

set Type = ${1}
set Socket = ${2}
set Iterations = ${3}

#------------------------------
# Log File 
#------------------------------
 
# create new log file and link it to current
set LogFile = $LogDir/`date '+%y%m%d%H%M%S'`
echo > $LogFile
ln $LogFile $LogDir/current


#------------------------------
# Test
#------------------------------

while ($Iterations > 0)

    echo "Iteration $Iterations"

    # Invoke .csh Tests
    foreach Test (*.csh)
       if ($Test != ${0}) then 
           $Test $Type $Socket
       endif 
    end

    # Recursively traverse through subdirectories    
    foreach Directory (`find ./* -type d -prune`)
       cd $Directory
       if (-e ${0}) then
          echo 
          echo $Directory...
          ${0} $Type $Socket 1
       endif
       cd ..
     end

  @ Iterations --
end
