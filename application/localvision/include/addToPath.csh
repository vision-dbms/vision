#!/bin/csh
# Only returns text if not already on the PATH.
#
# Usage:
#
#   set path=($path `csh -f $osvAdmin/include/addToPath.csh $VisionRoot/bin`)
#
set fixpath = `echo $PATH | sed -e 's/^/:/' -e 's/$/:/'`
set newpath = ''
foreach arg ($*)
  set there = `echo $fixpath | grep -c :${arg}:`
  if ($there == 0) then
    set newpath="$newpath $arg"
  endif
end
echo $newpath
