#!/bin/csh


if (-f $tokens/Cancel.newDaemon) exit 2

if (`ps -u $USER -o user:12,cmd | grep -i processUpdates | grep -v grep |  grep $LOGNAME |  wc -l` >= 1) then
  exit 1
endif

exit 0

