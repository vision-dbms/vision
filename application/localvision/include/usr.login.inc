####################
#  usr.login.inc
####################

#  Add 'VisionBinaries' to the command search path
#set path=($path $VisionBinaries)
set path=($path `csh -f $osvAdmin/include/addToPath.csh $VisionBinaries`)

#  Source installation specific 'login' file
if ( -f $LocalVisionRoot/include/usr.login.loc ) then
   source $LocalVisionRoot/include/usr.login.loc
endif

