#!/bin/csh

####################
#  Script: showProcesses.cmd
#	This script shows the process status for the dbadmin users
####################

echo "Process Status for Vision Administrator Users"
date
echo ""
"ps" -ef | fgrep -f $OSDPathName/NDF.GURL | grep -v showProcess | grep -v ps | grep -v grep | grep -v sort | sort -r -n -k 2
