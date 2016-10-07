####################
#  dba.cshrc.inc
####################

#--------------------
#  Set core variables if not set
#--------------------
if (! $?osvAdmin) then
    setenv osvAdmin ~osvadmin/dbadmin
endif

source $osvAdmin/include/setupRoots.env

#--  run non-dba setup
source $osvAdmin/include/usr.cshrc.inc

#--------------------
#  Vision Production Controls
#--------------------

#-- turn off VisionAdm and UserOSI (unix vision editor to prevent saving)
alias vision "(unsetenv VisionAdm; $VisionBinaries/vision -DF$VisionBinaries/batchvision \!*)"

#-- Define VisionProduction
setenv VisionProduction On

#-- other aliases needed by vadmin (when .login not read in)
alias produceToken touch
alias consumeToken /bin/rm -f
alias diskUsage du -sk $LocalVisionRoot/network/.

#-- Source installation-specific 'cshrc' file
if ( -f $LocalVisionRoot/include/dba.cshrc.loc ) then
   source $LocalVisionRoot/include/dba.cshrc.loc
endif

#-- VCA Logging 
setenv LogFilePath $LocalVisionRoot/logs/batchvisionVca.log
setenv LogFileBackups 1
setenv LogSwitchOn
