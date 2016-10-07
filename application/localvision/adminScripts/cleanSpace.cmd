#!/bin/csh

###
# $LocalVisionRoot/adminScripts/cleanSpace.cmd
#
# This script is used to clean a single ObjectSpace.
#
# It takes one argument: the number of the ObjectSpace to clean.
#
###

unsetenv VisionAdm

if ($# != 1) then
    echo "Usage: $0 objectSpace"
    echo "cleans the given object space"
    exit
endif

$DBbatchvision -U$1 << EOD | sed 's/V> //g'
    newLine print;
    "...  Begin Cleanup of Space $1 " print;
    Utility UnixSeconds currentTime printNL;
    newLine print;
    GlobalWorkspace AdminTools setAllocationThresholdTo: 800000000;
    $1 asObjectSpace cleanSpace;
    newLine print;
    Utility updateNetworkWithAnnotation: "Cleanup of Space $1";
    "...  End Cleanup of Space $1 " print;
    Utility UnixSeconds currentTime printNL;
    newLine print;
?g
?t
EOD

#--  If Error, Report and Abort
if ($? != 0) then
    echo ">>>  Cleanup of Space $1 Failed  <<<"
    exit 1
endif

