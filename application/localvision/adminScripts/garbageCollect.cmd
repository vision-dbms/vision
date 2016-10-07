#!/bin/csh

###
#  Script: garbageCollect
#	This script iterates over the object spaces and runs
#       cleanSpace in parallel, then the garbage collection
#       followed by the network check (onprof) in background
#
###

csh $osvProd/scripts/garbageCollect.csh
