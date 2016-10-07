#!/bin/csh

###
# $LocalVisionRoot/adminScripts/filteredSpaces.cmd
#
# This script gets a subset of the ObjectSpaces after removing those found
#   within $adminArea/helpers/NetMaintOSExcludeList.
#
# It takes a start and end parameter specifying the sequence range.
#
###

if ($# != 2) then
    echo "Usage: $0 start end"
    echo "cleans the given object space"
    exit
endif

set excludedSpaces = "0"  # fake space used when not excluding
set excludeList = $adminArea/helpers/NetMaintOSExcludeList

# ensure that exclude file exists and is not empty
if (-e $excludeList) then
    if (-s $excludeList) then
        # change newlines into pipes for regex alternation
        set excludedSpaces = `sed ':a;N;$\\!ba;s/\n/|/g' < $excludeList`
    endif
else
    touch $excludeList
endif

set seqScript = 'BEGIN {for (i=ARGV[1]; i<=ARGV[2]; i++) print i; exit}'
awk "$seqScript" $1 $2 | egrep --invert-match --line-regexp $excludedSpaces

