#!/bin/csh

echo "Using osvAdmin in $osvAdmin"
echo "Using localvision in $LocalVisionRoot"
echo "Using batchvision in $DBbatchvision"
echo -n "Okay to proceed [Yes]? "
set input = $<
if ($input == "No" || $input == "NO" || $input == "no" ) exit

setenv bootPath $LocalVisionRoot/bootstrap
setenv bootSource $osvAdmin/localvision/bootstrap

echo -n "Create Basic or Invest Bootstrap Network [B]? "
set input = $<
if ($input == "I" || $input == "i") then
   set version = invest
   setenv OSDPathName $bootPath/investNet
else
   set version = basic
   setenv OSDPathName $bootPath/basicNet
endif

if (! -d $OSDPathName) then
   echo "Creating network in $OSDPathName"
   mkdir $OSDPathName
else
   echo $OSDPathName already exists.  Aborting.
   exit 
endif

echo -n "Generate Schema Documentation [Y]? "
set input = $<
if ($input == "No" || $input == "no" || $input == "NO") then
   set createDoc = "no"
else
   set createDoc = "yes"
endif

#----------------
#  Set other environment variables...
#----------------

setenv AdminTools       $osvAdmin/localvision/adminScripts
setenv NDFPathName	$OSDPathName/NDF
setenv NDFJournal	$NDFPathName.JOURNAL

#----------------
#  ... run the build scripts
#----------------

#-- Step 1: Basic - always run this step
echo "+++ Building Basic Network in $OSDPathName"
csh -f $bootSource/scripts/BUILDboot.basic >& $OSDPathName/boot.log

#-- Step 2: Invest - only run this if version = invest
if ($version == "invest") then
   echo "+++ Adding Invest Network in $OSDPathName"
   csh -f $bootSource/scripts/BUILDboot.invest >>& $OSDPathName/boot.log
   echo "+++ Adding Upgrades"
   csh -f $bootSource/scripts/InstallUpgrades.bi >>& $OSDPathName/boot.log
endif

#-- Step 3: Create schema cross-reference documentation if desired
if ($createDoc == "yes") then
   echo "+++ Creating Schema Cross Reference Documentation"
   csh -f $bootSource/scripts/BUILDschemaXRef.core >>& $OSDPathName/boot.log
endif
