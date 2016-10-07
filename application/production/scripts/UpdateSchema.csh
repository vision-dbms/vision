##
# $osvProd/scripts/UpdateSchema.csh
#
# This script will update schema and commit
#
# Input Options:
#   -n  <NDF file to validate>   {Default: $NDFPathName}
#
###

#-------------
# Setup
#-------------
set batchHome  = $LocalVisionRoot/production/Updates/jobs

#-------------
# Environment
#-------------
set ndfToUse    = $NDFPathName
set jobName     = UpdateSchema
set processLog  = $HOME/localvision/logs/$jobName.$processDate
set batchHome   = $LocalVisionRoot/production/Updates/jobs

#----------
#  get arguments
#----------
while ( $#argv > 0)
   switch ( $1 )
      case "-N":
      case "-n":
         shift
         set ndfToUse = $1
         shift
         breaksw
      default:
         shift
         breaksw
   endsw
end

$DBbatchvision -n $ndfToUse << EOD |& sed 's/V> //g' >>& $processLog

Schema processAllMessages;

Utility updateNetworkWithAnnotation: "Updating Schema for $processDate";

?g
?t

EOD

####################
#  Wrapup
####################
#--  Save job completion status
set okay = $status
if ($okay == 0) then
   csh $batchHome/CheckLog.cmd $processLog $jobName 1
   set okay = $status
endif

#----------
#  and exit
#----------
exit $okay
