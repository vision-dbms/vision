#!/bin/csh

####################
#  Script: getPendingFeeds
#
#  This script is used to display the files in the pending feeds directory.
#  This directory is defined by the environment variable $feedFilePath.
####################

#---  change to correct directory
cd $feedFilePath

#---  and echo all the files
set nonomatch
set joblist = (`ls`)
foreach job ($joblist)
   if (! -d $job) echo $job
end
