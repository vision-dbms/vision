#!/bin/csh

set gurl = $OSDPathName/NDF.GURL

if (! -f $gurl) then
   touch $gurl
   chmod 666 $gurl
endif
  
set user = $LOGNAME
if ( `grep $user $gurl | wc -l` == 0) then
   echo $user >> $gurl 
   echo "...  $user added"
else
   echo "...  $user already in GURL"
endif


