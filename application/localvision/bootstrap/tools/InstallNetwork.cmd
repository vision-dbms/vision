#!/bin/csh

#------------------------
# installNetwork.csh
#------------------------
#
#  Script to copy new localvision database into target directory.
#
###

if ($#argv > 0) then
	set source = $argv[1]
else
	set source = $OSDPathName
endif

if ($#argv > 1) then
	set target = $argv[2]
else
	set target = $LocalVisionRoot/network
endif

echo "Installing Vision Database"
echo "Source: $source "
echo "Target: $target "
echo -n "Okay to Proceeed [Y]: "
set input = $<
if ($input == "n" || $input == "N" || $input == "NO" || $input == "no" || $input == "No") exit

if (! -d $source) then
   echo "Source $source directory not found.  Aborting."
   exit
endif
if (-d $target) then
   echo "Target $target directory already exists.  Aborting."
   exit
endif

echo "Creating target $target"
mkdir -p $target

echo "Copying new private database..."
echo "Source: $source"
echo "Target: $target"

cp -rp $source/* $target

#--  localize the localvision
cd $target
"pwd"  > NDF.OSDPATH
echo $USER > NDF.GURL

#--  create the NDF.UUID file
touch NDF.UUID; "rm" NDF.UUID 
batchvision -n $target/NDF << EOJ |& sed 's/V> //g'
EOJ

echo "...   $target installed."

#--  initialize ProcessControlTools environment variables 
set installenv =  $LocalVisionRoot/adminScripts/install.env

if (-f $installenv ) then
 batchvision -a -U3 -n $target/NDF < $installenv > $target/install.env.log
echo "...   ProcessControlTools installed."

endif

#-- PatchTable modules.

batchvision -n $target/NDF << EOJ |& sed 's/V> //g'

# update core/builtin to latest release
  PatchTable updateAllModules ;

Schema processAllMessages ;
?g
EOJ

echo ""
echo "...   PatchTable modules updated."
 

