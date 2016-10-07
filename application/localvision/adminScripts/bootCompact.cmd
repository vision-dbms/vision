#!/bin/csh

###
#  Script: fullCompact
#       This script performs the full compaction and relocates the
#	segments to the various space '.dsegs' directories.  
#       It assumes OSDPathName indicates root of network directory.
###


echo "...  version history prior to compact" 
$VisionBin/ndftool -v 
echo ""

echo "...  running full compaction" 
$DBbatchvision -a -U3 << @@@EOD
Utility fullCompact ;
?g

@@@EOD

#--  If Error, Report and Abort
if ($status != 0 ) then
    echo ">>>  Full Compaction Failed  <<<"
    exit 1
endif

#--------------------
#  move segments
#--------------------
#--   Make 'ls' safe
unalias ls

#---  Define the network root ...
set networkRoot = $OSDPathName
cd $networkRoot

#---  ... and perform the cleanup
foreach dir (`ls`)
    echo Considering: $dir
    if ( -f $dir/MSS ) then
	cd $dir
	set mss = `cat MSS`
	echo Processing: $dir, MSS: $mss

	###  Make a script to move the deleted segments, ...
	if ( -f .mvscript ) then
	    "rm" -f .mvscript
	endif
	echo mv MSS .dsegs > .mvscript
	ls | $VisionBin/makemvscript $mss >> .mvscript
	chmod u+x .mvscript

	###  ... prepare the target directory for the move, ...
	if ( ! -d .dsegs ) then
	    mkdir .dsegs
	endif
	if ( -f .dsegs/MSS ) then
	    "rm" -f .dsegs/MSS
	endif

	###  ... and execute the script.
	./.mvscript

	cd $networkRoot
    endif
end
