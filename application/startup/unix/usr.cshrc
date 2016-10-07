setenv osvAdmin ~{$USER}/dbadmin
# Set the process date if not already set
if ( ! $?processDate ) then
  setenv processDate    `date '+%Y%m%d'`
endif
source $osvAdmin/include/dba.cshrc.inc 


