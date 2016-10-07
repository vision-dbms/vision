#!/bin/csh

####################
#  Script: restoreBootstrapDatabase
#
#  This script is used to move the current network to a backup area
#  and revert to the original bootstrap network.  We will need to
#  consider adding a step here that reapplies patches submitted after
#  this bootstrap network was created.
#
#  LocalVisionRoot defines the location of the bootnet and network
####################

cd $LocalVisionRoot

#--- backup the current network : only one version of backup is saved
"rm" -rf backupNetwork
mv network backupNetwork

#--- copy bootnet
cp -rp bootNet network

#-- install patch scripts

