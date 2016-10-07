#!/bin/csh

####################
#  Script: showProductionStatus.cmd 
#	This script lists the files in arg1
####################

if ( $#argv != 1) then
    echo "usage: $argv[0] path name"
    exit 1
endif

set targetDir = $1
#ls -ogL $targetDir
ls -1 $targetDir

