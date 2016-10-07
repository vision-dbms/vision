##################
####  String  ####
##################

# $File: //depot/vision/applications/core/builtin/mainline/String.bi $
# $DateTime: 2013/11/26 15:30:03 $
# $Revision: #9 $
# $Author: klynch $

####################
#  Primitives and Converses
####################

#--------------------
#  this changes makes String '=' another object return FALSE, not NA
#--------------------
Prototypical String

  define: '_equals:' toBe: 362 asPrimitive .

defineMethod:
[ | = s2 |
  s2 isString ifTrue: [ ^self _equals: s2 ] ifFalse: [ FALSE ]
] .

;

#--------------------
#  Strings Manipulations
#--------------------

Prototypical String

#-- replace old method
define:'stripLeading:'  toBePrimitive: 386 .

#--  Define new primitives
define:'stripTrailing:' toBePrimitive: 386 withControlValue: 1 .
define:'stripBounding:' toBePrimitive: 386 withControlValue: 2 .
define:'stripLeadingAlwaysCopy:' toBePrimitive: 386 withControlValue: 3 . 
define:'stripTrailingAlwaysCopy:' toBePrimitive: 386 withControlValue: 4 .
define:'stripBoundingAlwaysCopy:' toBePrimitive: 386 withControlValue: 5 .

#-- and their binary converses ....
defineMethod: [ | stripFromFrontOf: aString |
  aString asString stripLeading: super
] .
defineMethod: [ | stripFromEndOf: aString |
  aString asString stripTrailing: super
] .
defineMethod: [ | stripFromBothEndsOf: aString |
  aString asString stripBounding: super
] .
defineMethod:[ | stripFromFrontOfAlwaysCopy: aString |
   aString asString stripLeadingAlwaysCopy: super
] .
defineMethod:[ | stripFromEndOfAlwaysCopy: aString |
   aString asString stripTrailingAlwaysCopy: super
] .
defineMethod:[ | stripFromBothEndsOfAlwaysCopy: aString |
   aString asString stripBoundingAlwaysCopy: super
] .

#-  rebind current covers to use primitives
defineMethod: [ | stripLeadingBlanks |
  ^self stripLeading: " "
] .

defineMethod: [ | stripBoundingBlanks |
  ^self stripBounding: " "
] .

defineMethod: [ | stripTrailingBlanks |
  ^self stripTrailing: " "
] .

#- Add White Space support
defineMethod: [ | stripLeadingWhiteSpace |
  !whiteSpace <- newLine concat: "	 " ;
  ^self stripLeading: whiteSpace
] .

defineMethod: [ | stripTrailingWhiteSpace |
  !whiteSpace <- newLine concat: "	 " ;
  ^self stripTrailing: whiteSpace
] .

defineMethod: [ | stripWhiteSpace |
  !whiteSpace <- newLine concat: "	 ";
  ^self stripBounding: whiteSpace
] .
;

#--------------------
#  Modify String asBlock so evaluate does not generate output
#--------------------
String defineMethod: [ | asBlock | 
  !string <- ("[ " concat: ^self .concat: " ] ") ;
  !block <- string
    evaluateWithCompilationErrorHandler:
       [ | :string :errorLine :errorCharacter | ] in: CoreWorkspace ;
  block isBlock ifTrue: [ block ] ifFalse: [ [ NoValue ] ] 
] ;

#--------------------
#  New String Method to strip trailing 0s after decimal
#--------------------

String defineMethod: [ | stripExtra0s |
  !s <- ^self stripBoundingBlanks ;
  !padding <- 0 ;
  s containsSubstring: "." . 
    ifTrue: [ :padding <- s reverse prefixSpannedBy: "0" ] ;
  s drop: padding negated
] ;

#--------------------
#  new asDateRange from string support relative date options
#  General form:
#      startDate:toDate:freq
#    where   startDate is    0            current date
#                            YYYYMMDD     full date
#                            YYMM         month end in year
#                            YY           year end in year
#                            -n           n periods ago (based on freq)
#            endDate   is same rules as startDate
#            freq      is    nD            n days
#                            nB            n business days
#                            nM nMB  nME   n months (or month begins or month ends)
#                            nQ nQB  nQE   n qtrs (or qtr begins or qtr ends)
#                            nY nYB  nYE   n years (or yr begins or yr ends)
#                            
#  Samples:
#        "0301:0306"  asDateRange printNL ;     #- 1/31/2003 - 6/30/2003 monthly
#        "0:-5:b" asDateRange printNL ;         #- last 5 business days
#        "0:0101:100b" asDateRange printNL ;    #- today - 1/31/2001 by 100 bdays
#        "-1:-12:me" asDateRange printNL ;      #- last 12 month ends
#        "0:-3me:b" asDateRange printNL ;       #- last 3 months of bday
#--------------------

#--  add TD freq as a variation on business days

String defineMethod: [|asDateRange|
!dict <- CoreWorkspace Dictionary basicSpecialized ;
dict at: "D" put: [ days ] ;
dict at: "B" put: [ businessDays ] ;
dict at: "TD" put: [ businessDays ] ;
dict at: "M" put: [ months ] ;
dict at: "MB" put: [ monthBeginnings ] ;
dict at: "ME" put: [ monthEnds ] ;
dict at: "Q" put: [ quarters ] ;
dict at: "QB" put: [ quarterBeginnings ] ;
dict at: "QE" put: [ quarterEnds ] ;
dict at: "Y" put: [ years ] ;
dict at: "YB" put: [ yearBeginnings ] ;
dict at: "YE" put: [ yearEnds ] ;
dict at: "AW" put: [ 7 days ] ;
dict at: "W" put: [ 7 days ] ;

!parts <- ^self breakOn: ":,"; 
!p1 <- parts at: 1 . else: "" . stripBoundingBlanks toUpper ;
!p2 <- parts at: 2 . else: "" . stripBoundingBlanks toUpper ;
!p3 <- parts at: 3 . else: "" . stripBoundingBlanks toUpper ;
!interval <- p3 convertToNumber else: 1 ;
!offset <- p3 drop: (p3 prefixSpannedBy: "0123456789") ;
!freq <- dict at: offset . else: [ [ monthEnds ] ] ;
!incr <- interval send: freq ;
!start <- p1 isDefault || [ p1 asNumber = 0 ] ifTrue: [ ^date ] .
   elseIf: [ p1 contains: "^[-+]" ] 
     then: [ !interval <- p1 convertToNumber else: 0 ;
             !offset <- p1 drop: (p1 prefixSpannedBy: "+-0123456789")  ;
             !f <- dict at: offset . else: [ freq ]  ;
             p1 convertToNumber else: 0 . send: f. asDate 
           ] .
     else: [ p1 asDate ] . else: [ ^date ] ;
!end <- p2 isDefault || [ p2 asNumber = 0 ] ifTrue: [ ^date ] .
   elseIf: [ p2 contains: "^[-+]" ] 
     then: [ !interval <- p2 convertToNumber else: 0 ;
             !offset <- p2 drop: (p2 prefixSpannedBy: "+-0123456789") ;
             !f <- dict at: offset . else: [ freq ] ;
             p2 convertToNumber else: 0 . send: f. asDate 
           ] .
     else: [ p2 asDate ] . else: [ ^date ] ;
p3 = "W"
ifTrue:[
start < end 
ifTrue:
[
:start <- start asFriday;
:end <-  end asFriday - 7 days
]
else:
[
:start <- start asFriday - 7 days;
:end <-  end asFriday
]
];
start to: end by: incr
] ;

#--------------------
#  New Date formatters
#  patch_125.vis
#--------------------

String defineMethod: [ | asDateFromMMYYYY |
!fields <- ^self breakOn: "-/" ;
!mm <- fields at: 1 . asNumber asInteger ;
!yyyy <- fields at: 2 . asNumber asInteger ;
mm isInteger && yyyy isInteger
   ifTrue: [ mm asMonthEndInYear: yyyy ] 
] ;

#####-----
# patch_127.vis
#####-----

############################################################
#
# Description:
#    String 'from: pos for: count' 
#      . Revised method 
#        - Fixed bug where a count of 0 returned 2 characters; 
#          a count of 0 will now return an empty string
#        - Tightened method such that the 'from: pos' parameter
#          cannot be <= 0 (an unexpected value); if a parameter
#          value is passed in which is <= 0, the parameter value
#          is reset to 1. This may introduce behavior 
#          differences in cases where applications might pass
#          in a negative or 0 'from: pos' parameter value as well 
#          as behavior differences where the 'from: pos' is 
#          negative or 0 and the 'for: count' paramter value is 
#          larger than the string size. Examples below.
#
#    Examples					Before			     After
#    "abcdefg" from: 0 for: 0  . print;      -> "a"      		     ""
#    "abcdefg" from: 0 for: 1  . print;      -> "a"      		     "a" 
#    "abcdefg" from: 0 for: -1 . print;      -> "a"      		     "a"
#    "abcdefg" from: 1 for: 0  . print;      -> "ab"     		     ""
#    "abcdefg" from: 1 for: 1  . print;      -> "a"      		     "a"
#    "abcdefg" from: 1 for: -1  .print;      -> "a"      		     "a"
#    "abcdefg" from: 1 for: -2 . print;      -> "a"     		     "a"
#    "abcdefg" from: 2 for: 0  . print;      -> "bc"     		     ""
#    "abcdefg" from: 2 for: 1  . print;      -> "b"      		     "b"
#    "abcdefg" from: 2 for: -1 . print;      -> "b"      		     "b"
#    "abcdefg" from: 2 for: -2 . print;      -> "ab"     		     "ab"
#    "abcdefg" from: 2 for: -3 . print;      -> "ab"     		     "ab"
#    "abcdefg" from: 2 for: 20 . print;      -> "bcdefg" (right blank filled to 20)  "bcdefg" (right blank filled to 20)
#    "abcdefg" from: 2 for: -20 . print;     -> "ab"                         "ab"
#    "abcdefg" from: -2 for: 20 . print;     -> "bcdefg" (right blank filled to 17)  "abcdefg" (right blank filled to 20)
#    "abcdefg" from: -3 for: 1 . print;     ->  "a"                          "a" 
#    "abcdefg" from: -3 for: 2 . print;     ->  "a"                          "ab"
#    "abcdefg" from: -3 for: 6 . print;     ->  "ab"                         "abcdef"
############################################################

String defineMethod: [ | from: pos for: count |
count = 0 ifTrue: [ "" ]
else: [
      pos <= 0
      ifTrue: [ :pos <- 1 ];
      !start <- pos;
      !end <- count > 0
              ifTrue: [ pos + count - 1 ]
              else: [ pos + count + 1 ];

      start < end
        ifTrue: [ ^self from: start to: end ]
        ifFalse: [ ^self from: end to: start ]
      ]
];

##################################################
# From patch_tssize.bridge
#  New string ->date conversion method for:
#     core/pending/convertToDate.bi
##################################################

#-- Convert from date in DD/MM/YY DD-MM-YY DD/MM/YYYY or DD-MM-YYYY formats
String defineMethod:[ | asDateFromDDMMYYYY |
!fields <- ^self breakOn: "-/" ;
!dd <- fields at: 1 .asNumber asInteger;
!mm <- fields at: 2 . asNumber asInteger ;
!yyyy <- fields at: 3 . asNumber asInteger ;
mm isInteger && [dd isInteger]&& [yyyy isInteger]
    ifTrue: [yyyy asDateFromYearForMonth: mm andDay: dd]
] ;

####
# From patch_132.vis
####

String defineMethod: [ | asFormulaList |
!string <- asString stripBoundingBlanks ;
string at: 1 . = ","  ifTrue: [ :string <- " " concat: string ; ] ;
!list <-  string toList ;
!rlist <- list runningTotal:
    [ ^self = "(" ifTrue: 1 .
         elseIf: [ ^self = ")" ] then: -1 .
         else: 0
    ] .
 runningTotal: [ runningTotal = 0 && ^self = "," ifTrue: 1 ifFalse: 0 ] ;
#--  concat the extra , and blank so all strings wind up in the same cluster
#--   after stripLeading: "," and blanks
rlist groupedBy: [ runningTotal ] .
    send: [ "," concat:
            [ groupList do: [ print ]
            ] divertOutput  . concat: " " .
            stripLeading: "," . stripBoundingBlanks
          ] .
] ;

###
# From patch_133.vis
###

String defineMethod: [ | isExtendedAscii | 
!xascii <-"€";
^self >= xascii
] ;
