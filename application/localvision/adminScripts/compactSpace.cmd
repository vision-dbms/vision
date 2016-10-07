#!/bin/csh

###
#
# $LocalVisionRoot/adminScripts/compactSpace.cmd
#
# This script is used to compact a single ObjectSpace.
#
# It takes one argument: the number of the ObjectSpace to clean.
#
###

unsetenv VisionAdm

if ($# != 1) then
    echo "Usage: $0 objectSpace"
    echo "compacts the given object space"
    exit
endif

$DBbatchvision -U$1 << EOD | sed 's/V> //g'
    !att <- Utility SessionAttribute;
    att updateFlagOfSpace: $1 .<- TRUE;
    att traceCompaction <- TRUE;
    att compactOnUpdate <- TRUE;
    att updateAnnotation <- "Compaction of Space $1";
    !result <- Utility updateNetwork;
    att traceCompaction <- FALSE;
    att compactOnUpdate <- FALSE;
    result
?g
?t
EOD

#--  If Error, Report and Abort
if ($? != 0) then
    echo ">>>  Compaction of Space $1 Failed  <<<"
    exit 1
endif

