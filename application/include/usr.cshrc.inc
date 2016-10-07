###################
#  usr.cshrc.inc
####################

#--------------------
#  Set core varibles if not set
#--------------------

if ( ! $?VisionRoot ) then
   setenv VisionRoot 		/vision
endif
setenv VisionBinaries      $VisionRoot/bin
setenv VisionLibrary       $VisionRoot/lib

limit coredumpsize 102400  #-- limit coredump files to 100 Mb

if (! $?LocalVisionRoot) then
   setenv LocalVisionRoot /localvision
endif

# Set-up global aliases:
alias vision "(unsetenv VisionAdm; $VisionBinaries/vision -DF$VisionBinaries/batchvision \!*)"
alias batchvision "$VisionBinaries/batchvision"
alias visionSubmit $LocalVisionRoot/production/admin/scripts/submitUpdate

# Set up common shortcuts

set scripts = $osvAdmin/include

#--------------------
#  Other Useful Aliases and Sets
#--------------------

#-- Make sure HOST is set.
    if (! $?HOST) setenv HOST `uname -n`

#-- Make 'rm' cautious
    alias rm rm -i

#-- Put 'pwd' in prompt
    set prompt="$cwd [\!]% "
    alias cd 'cd \!*; set prompt="$cwd [\\!]% "; '
    alias pushd 'pushd \!*; set prompt="$cwd [\\!]% "; '
    alias popd 'popd \!*; set prompt="$cwd [\\!]% "; '

#-- Turn off autologout
    unset autologout

#-- Enable history
    set history=20
    alias h history

#-- Turn on job completion notification
    set notify

#-- Other aliases
    alias lo logout
    alias lt 'll -t |more '
    alias ls ls -x
    alias ll ls -l
    alias lsr ls -R
    alias psme 'ps -fu $user'
#  Source installation specific 'cshrc' file
if ( -f $LocalVisionRoot/include/usr.cshrc.loc ) then
   source $LocalVisionRoot/include/usr.cshrc.loc
endif

