#!/usr/bin/tcsh

#-------------------------------
# Variables
#-------------------------------

set TestDir = `pwd` 
#set Query = "showMessages"
set Query = "Schema MID instanceList do:[message printNL]"
set NumPings = 5 
set PingTimeOut = 20 #seconds

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

# Check for Tools Env Variable
if (! $?Tools) then
   echo "Tools Environment variable needs to be set"
   exit
endif

set Type = ${1}
set Socket = ${2}

#---------------------------------
# Test
#---------------------------------

if ($Type == "File") then
   set serverArg = "-serverFile=$Socket"
else
   set serverArg = "-server=$Socket"
endif 

echo -n "SuccessivePing Test     :"
while ($NumPings >= 0) 
   set PingResult = `$Tools/vpooladmin $serverArg -ping=$Query -timeOut=$PingTimeOut`
   echo $PingResult | grep Error  >> /dev/null
   if ($status == 0) then
      echo  "FAILED"
      exit
   endif
   @ NumPings --
end
echo "PASSED"
