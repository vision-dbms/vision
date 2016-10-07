#!/bin/csh

####################
#  GetLocalTokenInfo.csh
#
#  This will return a formatted list of the Tokens for the current processDate.
#  We will look for files that start with 'E' or 'D' or 'C'.
#

#  Defaults:
#    Directory:                  $tokens
#    Local ProcessDate:          $tokens/deamon.processDate
#
#  Format:
#    Name  | Status | Timestamp | Process Date
#
#  e.g.
#    CompactMap  | Done  | 20071031.2211 | 20071031
#
####################

if (! $?tokens)   set tokens   =  $LocalVisionRoot/production/status/tokens

#--  processDate is current production cycle date
if (-f $tokens/daemon.processDate) then
   set localPDate = `cat $tokens/daemon.processDate`
else
   set localPDate = ""
endif

pushd $tokens >/dev/null

set nonomatch
set clientTokens = `ls -1 |egrep -e "^[DEC]"`
popd >/dev/null

foreach rec ($clientTokens)
  if (! $?tokens) continue
  if (! -f $tokens/$rec) continue
  set token           = $rec
  set tstamp  = `csh $osvProd/scripts/IntegerTimestamp.csh -f $tokens/${rec}`
  set fileTimeDate    = `echo $tstamp| cut -c1-8`
  set fileTimeHour    = `echo $tstamp| cut -c9-12`
  set tName           = `echo $token | awk -F. '{print $2}'`
  set tStatus         = `echo $token | awk -F. '{print $1}'`
  echo "$tName | $tStatus  | ${fileTimeDate}.${fileTimeHour} | $localPDate"
end


exit

