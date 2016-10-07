#!/bin/csh

####################
#  Script: getPendingScripts
#
#  This script is used to display the files in the pending scripts directory.
#  This directory is defined by the environment variable $scriptFilePath.
####################

#---  change to correct directory
cd $scriptFilePath

#---  and echo all the files
set nonomatch
set joblist = (`ls`)
foreach job ($joblist)
   if (! -d $job) echo $job
end
