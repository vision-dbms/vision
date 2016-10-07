#!/bin/csh

###
#
# $LocalVisionRoot/adminScripts/numSpaces.cmd
#
# This script is used to get the number of ObjectSpaces in the default env.
#
# It does not take any arguments.
#
###

$DBbatchvision << EOD | sed 's/V> //g'
  (AdminTools spacesInNetwork - 1) asInteger
?g
EOD

