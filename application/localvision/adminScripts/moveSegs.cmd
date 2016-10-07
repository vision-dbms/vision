###
#  moveSegs.cmd
#
# replacing with shared moveSegs
###

csh $osvProd/scripts/moveSegs.csh
if ($status != 0) then
   echo ">>> Error - moveSegs.cmd "
   exit 1
endif


