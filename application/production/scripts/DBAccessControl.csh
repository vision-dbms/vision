#!/bin/csh 

####################
#  $osvProd/scripts/DBAccessControl.csh
#
#  This script is used to freeze/unfreeze a Vision network (database). 
#  The following optional arguments may be supplied:
#
#     -a        {action: freeze|thaw|unfreeze|status}
#     -n        network
#     -info     info : echo status information
#
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
      case "-A":
      case "-a":
         shift
         set action = $1
         if ($action == 'unfreeze') set action = 'thaw'
         shift
         breaksw
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
   echo "*  Running DBAccessControl at `date`"
   echo "*      Action: $action"
   echo "*     Network: $network"
   echo "************************"
endif

#----------
#  freeze
#----------
if ($action == 'freeze')  then
   # no NDF.RELEASED or it is empty
   if (! -f $network/NDF.RELEASED || -z $network/NDF.RELEASED) then

$DBbatchvision << EOD |& sed 's/V> //g' >& /dev/null

Utility ndfPathName concat: ".RELEASED" . substituteOutputOf:
  [ Utility accessedNetworkVersion asString printNL ; ] ;
?g

EOD
      set rc = $status
      if ($rc != 0) then
         echo ">>> Error running $DBbatchvision: $rc"
         exit 1
      endif

      if (! -f $network/NDF.RELEASED) then
        echo ">>> Error creating token $network/NDF.RELEASED"
        exit 1
      endif

      echo "Network Frozen on Version `cat $network/NDF.RELEASED`"
   else
      echo ">>> Network Already Frozen. No action taken"
      exit 1
   endif

else if ($action == 'thaw') then
   if (-f $network/NDF.RELEASED && ! -z $network/NDF.RELEASED) then
      \rm $network/NDF.RELEASED
      if (-f $network/NDF.RELEASED) then
        echo "Could not remove released token: $network/NDF.RELEASED"
        exit 1
      endif
      echo "Network Thawed"
   else
      echo ">>> Network Not Frozen. No action taken"
   endif
else if ($action == 'status') then
   echo "************************"
   if (-f $network/NDF.RELEASED && ! -z $network/NDF.RELEASED) then
     echo "Network Frozen on Version `cat $network/NDF.RELEASED`"
   else
     echo "Network Not Frozen"
   endif
   echo "************************"
else
   echo 'Usage: $OSVProd/scripts/DBAccessControl.csh -a {status|freeze|thaw} {-info} {-n network}'
endif
