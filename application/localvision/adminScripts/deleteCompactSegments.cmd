#!/bin/csh

####################
#  Script: deleteCompactSegments
#	This script deletes the segments relocated by moveCompactSegments.
#       It assumes OSDPathName indicates root of network directory.
####################

#---  Remove any potentially harmful aliases ...
unalias ls

#---  Define the network root ...
set networkRoot = $OSDPathName
cd $networkRoot

#---  ... and delete any files contained in the '.dsegs' sub-directories.
cd $networkRoot

foreach dir (`ls`)
    if ( ! -d $dir ) continue

    cd $dir
    if ( -d .dsegs ) then
	cd .dsegs
	echo Processing: $dir/.dsegs
	set nonomatch
	sh -c "rm -f *"
	unset nonomatch
    endif

    cd $networkRoot
end
