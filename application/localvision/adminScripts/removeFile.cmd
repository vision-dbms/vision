#!/bin/csh

####################
#  Script: removeFile
#	This script removes file in arg1 from dir in arg2
####################

if ( $#argv != 2) then
    echo ">>> Bad Inputs Supplied."
    echo "usage: $argv[0] fileName sourceDir"
    exit 1
endif

set fileName = $1
set sourceDir = $2

if (! -d $sourceDir) then
  echo ">>> Bad Source Directory: $sourceDir"
  exit 1
endif

if (! -f $sourceDir/$fileName) then
  echo ">>> File Not Found: $fileName in $sourceDir"
  exit 1
endif

"rm" -f $sourceDir/$fileName 
echo "...  $fileName removed from $sourceDir"
