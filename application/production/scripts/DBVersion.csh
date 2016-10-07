#!/bin/csh 

####################
#  $osvProd/scripts/DBVersion.csh
#
#
#     script returns the vdb network version at the time it executes 
#     -n        network
#     -info     info : echo status information
#
####################

#-------------
# Environment
#-------------

set action = 'status'
set info = 'no'

#----------
#  get arguments
#----------
while ( $#argv > 0)
   switch ( $1 )
      case "-n":
      case "-N":
         shift
         set network = $1
         shift
         breaksw
      case "-info":
      case "-Info":
         set info = "yes"
         shift
         breaksw
      default:
         set extraArgs = "$extraArgs $1"
         shift
         breaksw
   endsw
end

if (! $?network) then
   if (! $?LocalVisionRoot) then
      echo '>>> $LocalVisionRoot is not set.'
      exit 1
   else
      set network = "$LocalVisionRoot/network"
   endif
endif

if (! -d $network) then
   echo ">>> netork directory $network is not available."
   exit 1
endif

if (! $?DBbatchvision) then
   if (! $?VisionRoot) then
      echo '>>> $VisionRoot is not set.'
      exit 1
   else
      set DBbatchvision = $VisionRoot/bin/batchvision
   endif
endif

if (! -x $DBbatchvision) then
   echo ">>> $DBbatchvision is not executable."
   exit 1
endif

if ($info == "yes") then
   echo "************************"
   echo "*  Running DBVersion at `date`"
   echo "*      Action: $action"
   echo "*     Network: $network"
   echo "************************"
endif

#ndftool -r1 | grep "Newest Version" | cut -c35-40
$DBbatchvision << EOD |& sed 's/V> //g' 

   Utility accessedNetworkVersion asString 
?g

EOD

