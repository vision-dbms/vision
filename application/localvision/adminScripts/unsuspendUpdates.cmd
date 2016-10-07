#!/bin/csh

####################
#  Script: unsupsendUpdates.cmd
#	This script "unschedules" the Suspend production job 
####################

"rm" -f $statusArea/pending/Suspend
echo "--- Production Jobs Restarted --- "
