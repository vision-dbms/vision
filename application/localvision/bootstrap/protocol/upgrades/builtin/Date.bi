################
####  Date  ####
################

# $File: //depot/vision/applications/core/builtin/mainline/Date.bi $
# $DateTime: 2009/05/04 09:50:46 $
# $Revision: #6 $
# $Author: sglee $

####################
#  Primitives and Converses
####################

Prototypical Date
defineMethod:
[ | localizeValue |
  ^self _inContextOf: GlobalWorkspace
] .

;

Date defineMethod: [ | asFriday |
!daysToAdd <- 
   ((7 - dayNumberOfWeek) asInteger mod: 7 . + 5 ) asInteger mod: 7 ;
^self + daysToAdd days
] ;

#
# Add simple iterate: to Date for completeness
#        so that daterange/date iterate: both work
# patch_125.vis
#

Date defineMethod: [ | iterate: block |
  ^self evaluate: block
] ;


##### -----
# patch_126.vis
##### -----


############################################################
# Description:
#    Date
#      . New methods introduced:
#        - Date 'formatUsingMMYY'
#        - Date 'formatUsingMMYYYY'
#        - Date 'formatUsingYYMMDD'
#        - Date 'formatUsingYYYYMM'
#        - Date 'formatUsingYYYYMMDD'
#      . Revised methods:
#        - Date 'formatUsingYYMM' (patch_114.vis)
#          NOTE: The method was NOT revised to introduce "/" between 
#                YY and MM because of possible impact to applications.
#          Revised for presentation of earliestPossibleDate (behavior change)
#        - Date 'formatUsingMMDDYY'
#          Revised for presentation of earliestPossibleDate (behavior change)
#
# Note: Wherever YY or YYYY is part of the date format, the earliestPossibleDate 
#        year (1) is expected to be right justified in order to differentiate it 
#        from 2001 year (01 or 2001).
#
# earliestPossibleDate formatUsingMMYY      -> "01/ 1"
# 20010101 asDate formatUsingMMYY           -> "01/01"
#
# earliestPossibleDate formatUsingMMDDYY    -> "01/01/ 1"
# 20010101 asDate formatUsingMMDDYY         -> "01/01/01"
#
# earliestPossibleDate formatUsingMMDDYYYY  -> "01/01/   1"
# 20010101 asDate formatUsingMMDDYYYY       -> "01/01/2001"
#
# earliestPossibleDate formatUsingMMYYYY    -> "01/   1"
# 20010101 asDate formatUsingMMYYYY         -> "01/2001"
#
# earliestPossibleDate formatUsingYYMM      -> " 101"
# 20010101 asDate formatUsingYYMM           -> "0101"
#
# earliestPossibleDate formatUsingYYYYMM    -> "   1/01"
# 20010101 asDate formatUsingYYYYMM         -> "2001/01"
#
# earliestPossibleDate formatUsingYYYYMMDD  -> "   1/01/01"
# 20010101 asDate formatUsingYYYYMMDD       -> "2001/01/01"
#
#
####################################################


Date defineMethod: [ | formatUsingMMYY | 
!yy <- year mod: 100;
[ month < 10 
  ifTrue: [ "0" print; month print: 1 ] 
  ifFalse: [ month print: 2 ]; "/" print;
  yy < 10 && [ ^self != earliestPossibleDate ] 
  ifTrue: [ "0" print; yy print: 1 ] 
  ifFalse: [ yy print: 2 ];
] divertOutput
];


Date defineMethod: [ | formatUsingMMYYYY |
 [ month < 10 ifTrue: [ "0" print; month print: 1 ] 
   ifFalse: [ month print: 2 ];
  "/" print; year print: 4;
 ] divertOutput
];


Date defineMethod: [ | formatUsingYYMMDD |
!yy <- year mod: 100;
[
  yy < 10 && [ ^self != earliestPossibleDate ]
  ifTrue: [ "0" print; yy print: 1 ]
  ifFalse: [ yy print: 2 ];
  "/" print;  ^self formatUsingMMDD print;
] divertOutput
];


Date defineMethod: [ | formatUsingYYYYMM |
 [ year print:4 ; "/" print;
   month < 10 
   ifTrue: [ "0" print; month print: 1 ] 
   ifFalse: [ month print: 2 ];
 ] divertOutput
];


Date defineMethod: [ | formatUsingYYYYMMDD |
!d <- day < 10 ifTrue: [ "0" concat: day  ]ifFalse: [ day asString ]; ;
^self formatUsingYYYYMM concat: "/" . concat: d 
];


Date defineMethod: [ | formatUsingYYMM |
!yy <- year mod: 100;
[
  yy < 10 && [ ^self != earliestPossibleDate ]
  ifTrue: [ "0" print; yy print: 1 ]
  ifFalse: [ yy print: 2 ]; 
  month < 10 ifTrue: ["0" print; month print: 1 ] ifFalse: [ month print: 2 ];
] divertOutput
];


Date defineMethod: [ | formatUsingMMDDYY |
!yy <- year mod: 100;
[
  ^self formatUsingMMDD print;
  "/" print;
  yy < 10 && [ ^self != earliestPossibleDate ]
  ifTrue: [ "0" print; yy print: 1 ]
  ifFalse: [ yy print: 2 ];
] divertOutput
];




