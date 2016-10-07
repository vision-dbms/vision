#!/bin/tcsh
#---------------------------------------------------------------------------
# file: ElapsedMinsFrom.csh
#
# This utility script is designed to return the number of minutes
# between the two timestamp values. The input strings are presumed 
# to be in the format of 'YYYYMMDDHHMM'. 
#
# Limitations:
#    This will only calculate using the YYYYMMDDHHMM values provided. If the -t
# value is prior to the -f (from value) the year will be ignored and results are 
# not verified.
#
# Month used to augment day count (fmd & tmd).
# Month  | Offset to add to current day
#  01      Begin =  0 days
#  02      Jan  += 31 days
#  03      Feb  += 28 days
#  04      Mar  += 31 days
#  05      Apr  += 30 days
#  06      May  += 31 days
#  07      Jun  += 30 days
#  08      Jul  += 31 days
#  09      Aug  += 31 days
#  10      Sep  += 30 days
#  11      Oct  += 31 days
#  12      Nov  += 30 days
#
# Input Options:  
#   -f <YYYYMMDDHHMM>  From Timestamp  {Default Current Time}
#   -t <YYYYMMDDHHMM>  To Timestamp    {Default Current Time}
#   -debug
#
# Output:
#  Number of minutes between inputs.
#


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
         set fromTime = $1
         shift
         breaksw
      case "-T":
      case "-t":
         shift
         set toTime = $1
         shift
         breaksw
      case "-debug":
         set debugOn = "y"
         shift
         breaksw
      default:
         shift
         breaksw
   endsw
end


#-- Defaults if needed
if (! $?fromTime) set fromTime = `date '+%Y%m%d%H%M'`
if (! $?toTime)   set toTime = `date '+%Y%m%d%H%M'`

#-- Note: The input string for this awk program is close to being too large for CShell (the default calling environment). 
#         Additions/modifications will require an increasing parsimonious use of syntax.
echo $fromTime | awk '{ \
fmd=0;tmd=0;y=0; \
fY = substr($0, 1, 4);fm = substr($0, 5, 2);fD = substr($0, 7, 2);fH = substr($0, 9, 2);fM = substr($0, 11, 2);\
if (fm ~ /02/) fmd=31;if (fm ~ /03/) fmd=59;if (fm ~ /04/) fmd=90;if (fm ~ /05/) fmd=120;\
if (fm ~ /06/) fmd=151;if (fm ~ /07/) fmd=181;if (fm ~ /08/) fmd=212;if (fm ~ /09/) fmd=243;\
if (fm ~ /10/) fmd=273;if (fm ~ /11/) fmd=304;if (fm ~ /12/) fmd=334;\
tY = substr(tstp, 1, 4);tm = substr(tstp, 5, 2);tD = substr(tstp, 7, 2);tH = substr(tstp, 9, 2);tM = substr(tstp, 11, 2);\
if (tm ~ /02/) tmd=31;if (tm ~ /03/) tmd=59;if (tm ~ /04/) tmd=90;if (tm ~ /05/) tmd=120;\
if (tm ~ /06/) tmd = 151;if (tm ~ /07/) tmd = 181;if (tm ~ /08/) tmd = 212;if (tm ~ /09/) tmd = 243;\
if (tm ~ /10/) tmd = 273;if (tm ~ /11/) tmd = 304;if (tm ~ /12/) tmd = 334;\
if (tY > fY) y = tY - fY;\
ftm = ((fmd+fD)*1440)+(fH*60)+fM;\
ttm = ((tmd+tD)*1440)+(tH*60)+tM;\
el = ((y*365*1440) + ttm) - ftm;\
if (db == "y") {print "Fr:"ftm;print "To:"ttm;print "El:"el;};\
}; END {print el;}' tstp=$toTime db=$debugOn -

