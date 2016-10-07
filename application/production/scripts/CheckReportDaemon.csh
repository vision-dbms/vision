#!/bin/csh

if (`ps -u $USER -o user:12,cmd | grep -i processReports | grep -v grep |  grep $LOGNAME | wc -l` >= 1) then
  exit 1
endif

exit 0

