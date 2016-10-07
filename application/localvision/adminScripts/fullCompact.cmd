#!/bin/csh

###
#  Script: fullCompact
#       This script performs the full compaction and optionally moves
#	the dsegs to the various space '.dsegs' directories.  
#       It assumes OSDPathName indicates root of network directory.
###

echo "...  version history prior to compact" 
$VisionBin/ndftool -v 
echo

echo "...  running full compaction" 
set numSpaces = `csh $AdminTools/numSpaces.cmd`
bash $AdminTools/parallel.cmd 4 2 "csh $AdminTools/compactSpace.cmd" "csh $AdminTools/filteredSpaces.cmd 3 $numSpaces"

set parallelExit = $?

$DBbatchvision -U2 << EOD | sed 's/V> //g'
    !att <- Utility SessionAttribute;
    att updateFlagOfSpace: 2 .<- TRUE;
    att traceCompaction <- TRUE;
    att compactOnUpdate <- TRUE;
    att makeBaseVersionOnUpdate <- TRUE;
    att updateAnnotation <- "Completing Compaction (Space 2)";
    !result <- Utility updateNetwork;
    att traceCompaction <- FALSE;
    att compactOnUpdate <- FALSE;
    att makeBaseVersionOnUpdate <- FALSE;
    result
?g
EOD

#--  If Error, Report and Abort
if ($? != 0 || $parallelExit != 0) then
    echo ">>>  Full Compaction Failed  <<<"
    exit 1
endif

#--------------------
#  move segments
#--------------------
if ($# == 0) then
   moveSegs.cmd
endif

