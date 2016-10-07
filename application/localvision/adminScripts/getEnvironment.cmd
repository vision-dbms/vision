#!/bin/csh

###
# getEnvironment
#
###

echo feedFilePath $feedFilePath
echo scriptFilePath $scriptFilePath
echo updateLogPath $updateLogPath
# use something other than UNIX for non Unix hosts
echo visionServerType UNIX
if ( $?VisionProduction) then
   setenv productionPath $LocalVisionRoot/production
   echo productionPath $productionPath
   echo vscriptCleanupPath $productionPath/Updates/workArea/Cleanup/scripts/
   echo visionSubmit $productionPath/admin/scripts/submitUpdate
endif



