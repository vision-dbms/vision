#!/bin/tcsh
#---------------------------------------------------------------------------
# file: IntegerTimestamp.csh
#
# This utility script is designed to return the timestamp of a file
# expressed as in integer number YYYYMMDDHHMM. We utilize the integer timestamp
# in a number of production scripts so converting it from the default Unix expression
# is important.
#
# Limitations:
#   If the file is more than a year old then the Hour and Minute will default to "00".
#
# Input Options:  
#   -f <file to get value for>
#   -debug
#
# Output:
#  Integer representation of the file timestamp.
#
#---------------------------------------------------------------------------

#  Variables
set debugOn = ""

#----------
#  get arguments 
#----------
while ( $#argv > 0)
   switch ( $1 )
      case "-F":
      case "-f":
         shift
         set inFile = $1
         shift
         breaksw
      case "-debug":
         set debugOn = "yes"
         shift
         breaksw
      default:
         shift
         breaksw
   endsw
end

#-- Defaults if needed
if (! $?inFile) then
  set intTStamp = `date '+%Y%m%d%H%M'`
  if ($debugOn != "") echo ">>> No file passed via '-f'. Default Current $intTStamp"
  echo $intTStamp
  exit 1
endif

if (! -f $inFile) then 
  set intTStamp = `date '+%Y%m%d%H%M'`
  if ($debugOn != "") echo ">>> File $inFile does not exist. Default Current $intTStamp"
  echo $intTStamp
  exit 1
endif

"ls" -l $inFile | awk '{ \
  cY = substr(curTS, 1, 4); cMon = substr(curTS, 5, 2); cD = substr(curTS, 7, 2); \
  if ($6 == "Jan") fMon = "01"; if ($6 == "Feb") fMon = "02"; if ($6 == "Mar") fMon = "03"; \
  if ($6 == "Apr") fMon = "04"; if ($6 == "May") fMon = "05"; if ($6 == "Jun") fMon = "06"; \
  if ($6 == "Jul") fMon = "07"; if ($6 == "Aug") fMon = "08"; if ($6 == "Sep") fMon = "09"; \
  if ($6 == "Oct") fMon = "10"; if ($6 == "Nov") fMon = "11"; if ($6 == "Dec") fMon = "12"; \
  if ($7 < 10) fD = "0"$7; if ($7 >= 10) fD = $7; \
  fnbr = split($8,hm,":"); \
  if (fnbr > 1) { \
    fHr = hm[1]; fMin = hm[2]; \
    if (cMon >= fMon) \
      {fY = cY;} \
    else \
      {fY = cY - 1;} \
    } \
  else  \
    {fY = $8; fHr = "00"; fMin = "00";}; \
  intTS = fY fMon fD fHr fMin; \
  if (debug == "yes") { \
    print "Current Time: "curTS; \
    print "File TStamp: "intTS; \
  };\
}; END {print intTS;}' curTS=`date '+%Y%m%d%H%M'` debug=$debugOn -
