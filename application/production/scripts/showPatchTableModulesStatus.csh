#!/bin/csh

###
# Status of PatchTable in database.
##

$DBbatchvision -U3 << @@@EOC@@@ |& sed 's/V> //g'
">>> Status of PatchTable modules. " print; Utility UnixSeconds currentTime printNL; 
newLine print;
Utility osdPathName printNL;
Utility ndfPathName printNL; 
PatchTable displayStatus;
"=" fill: 70 . printNL;
?g
@@@EOC@@@

