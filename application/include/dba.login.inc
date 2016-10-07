###
# dba.login.inc
###

#--------------------
# Set up the standard VISION login environment
#--------------------
set scripts = $osvAdmin/include

source $scripts/usr.login.inc
source $scripts/DBAVision.env

# Set the process date if not already set
if ( ! $?processDate ) then
  setenv processDate	`date '+%Y%m%d'`
endif

# Other shared update directories, files, and variables
setenv adminArea	$LocalVisionRoot/production/admin
setenv updateArea	$LocalVisionRoot/production/Updates
setenv updateLogArea	$updateArea/logs
setenv data		$updateArea/data
setenv reportArea	$LocalVisionRoot/production/Reports
setenv statusArea	$LocalVisionRoot/production/status
setenv tokens		$statusArea/tokens
setenv statusFile       $statusArea/active/status.out
setenv globalLogs       $statusArea/logs
setenv updateLog        $globalLogs/update.log
setenv errorLog         $globalLogs/errors.log
setenv warningLog       $globalLogs/warnings.log
setenv reportLog        $globalLogs/report.log
setenv submitLog        $globalLogs/submit.log
setenv listenerLog      $globalLogs/listener.log
setenv NDFJournal	$OSDPathName/NDF.JOURNAL
setenv exportData       $updateArea/exportdata
setenv rawData          $updateArea/rawdata

#  Source installation specific 'login' file
if ( -f $LocalVisionRoot/include/dba.login.loc ) then
	source $LocalVisionRoot/include/dba.login.loc
endif


