#!/bin/csh

####################
#  Script: moveFile
#	This script moves file in arg1 from dir in arg2 to dir in arg3
####################

if ( $#argv != 3) then
    echo "usage: $argv[0] fileName sourceDir targetDir"
    exit 1
endif

set fileName = $1
set sourceDir = $2
set targetDir = $3

if (-d $sourceDir && -d $targetDir && -f $sourceDir/$fileName ) then
   mv $sourceDir/$fileName $targetDir
   echo "...  $fileName moved from $sourceDir "
   echo "                       to $targetDir"
else
   echo ">>> Bad inputs.  $fileName not moved from $sourceDir"
   echo "                                        to $targetDir"
   echo "    moveFile fileName sourceDir targetDir "
endif

