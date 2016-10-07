"
################
####  Date  ####
################
"
?g

####################
#  Print and Other Standard Messages
####################
Prototypical Date 
;

####################
#  Primitives and Converses
####################


Prototypical Date

  define: 'asIDate7'         toBe: 33 asPrimitive .    #CurrentPointerAsValue
  define: 'evaluate:'        toBe: 44 asPrimitive .    #ForDateEvaluateUltimateParameter
  define: 'print'            toBe: 129 asPrimitive .   #PrintDate
  define: 'asInteger'        toBe: 301 asPrimitive .   #DateAsInteger
  define: 'year'             toBe: 302 asPrimitive .   #DateYearAsInteger
  define: 'month'            toBe: 303 asPrimitive .   #DateMonthAsInteger
  define: 'day'              toBe: 304 asPrimitive .   #DateDayAsInteger

   defineAsMessage: 'asDate' .
   defineMethod: [| + anIncrement | anIncrement increment: super] .
   defineMethod: [| - anIncrement | anIncrement decrement: super] .

### Add the two converses for the 'By...ForDatesEvaluate' primitives:
defineMethod:
[ | by: increment to: date evaluate: block |
  increment from: super to: date evaluate: block
] .

defineMethod:
[ | by: increment from: date evaluate: block |
  increment from: date to: super evaluate: block
] .

;

?g

##################################################
#    Date Definitions
#
#   1) print
#   2) relational ops
#   3) is 
#   4) conversion
#   5) date range
#   6) ytm calc support
#   7) formatting
#
##################################################

####################
#  print
####################

Prototypical Date

defineMethod:
[ | print: aWidth|      # use divertOutput so only one string is printed
  !yyyy <- year asString; 
  !mm <- month < 10 
      ifTrue: ["0" concat: month asString ] ifFalse: [ month asString ] ;
  !dd <- day < 10 
      ifTrue: ["0" concat: day asString ] ifFalse: [ day asString ] ;
  !width <- aWidth absoluteValue < 5 
     ifTrue:  [ 5 ] 
    ifFalse:  [ aWidth absoluteValue asInteger ] ; 
  !dateString <-  width >= 10 
      ifTrue: [ mm concat: "/" . concat:  dd . concat:  "/" .  concat:  yyyy ] 
      ifFalse:
         [ !yy <- yyyy drop: 2 ;
           width >= 8 
             ifTrue: [ mm concat: "/" . concat: dd . concat: "/" . concat: yy ]
            ifFalse: [ mm concat: "/" . concat: yy ]
         ]; 
   !printWidth <- aWidth < 0 
       ifTrue: [ width * -1 ] ifFalse: [ width ] ;
   dateString print:  printWidth ;
  ^self
] .

defineMethod:
[ | formatUsingMMDD | 
  [ month < 10 ifTrue: [ "0" print; month print: 1 ]
              ifFalse: [ month print: 2 ] ;
    "/" print ;
    day < 10   ifTrue: [ "0" print; day print: 1 ]
              ifFalse: [ day print: 2 ]
   ] divertOutput 
] .

defineMethod:
[ | formatUsingMMDDYY |
  !yy <- year mod: 100 ;
  !format <- yy < 10 
     ifTrue: [ "0" concat: yy ] ifFalse: [ yy asString ] ;
  formatUsingMMDD concat: "/" . concat: format
] .

defineMethod:
[ | formatUsingMMDDYYYY |
  formatUsingMMDD concat: "/" . concat: (year sprint: 4)
] .

;

#------------------------------

####################
#  relational ops
####################

Prototypical Date

defineMethod:
[ | = aDate |
  aDate isDate && [ ^self asIDate7 = aDate asIDate7 ]
] .

defineMethod:
[ | != aDate |
  aDate isDate
    ifTrue: [ ^self asIDate7 != aDate asIDate7 ]
] .

defineMethod:
[ | < aDate |
  aDate isDate
    ifTrue: [ ^self asIDate7 < aDate asIDate7 ]
] .

defineMethod:
[ | <= aDate |
  aDate isDate
    ifTrue: [ ^self asIDate7 <= aDate asIDate7 ]
] .

defineMethod:
[ | > aDate |
  aDate isDate
    ifTrue: [ ^self asIDate7 > aDate asIDate7 ]
] .

defineMethod:
[ | >= aDate |
  aDate isDate
    ifTrue: [ ^self asIDate7 >= aDate asIDate7 ]
] .

;


#------------------------------

####################
#  'is' messages
####################

Prototypical Date

defineMethod: [ | isDefault | ^self asSelf = earliestPossibleDate ] .

defineMethod: [ | isValid | asIDate7 > 0 ] .     # catches 1/1/1 - 1 days

defineMethod: [ | isntValid | asIDate7 <= 0 ] .

defineMethod: [ | isMonthEnd | (^self + 0 monthEnds) = ^self ] .

defineMethod: 
[ | isLastBusinessDayOfMonth |
(^self + 0 monthEnds + 0 businessDays) = ^self
] .

defineMethod:
[ | isInitialDate |
  ^self = 10101 basicAsDate - 1 days
] ;

#------------------------------

####################
#  date conversion methods
####################

Prototypical Date

defineMethod:
[ | inputExpression |
  ^self asInteger
] .

defineMethod:
[ | dayNumberOfWeek |     # Monday = 1 - Sunday = 7
( (asIDate7 - 1 ) asInteger mod: 7 . + 1 ) asInteger
] .

defineMethod: [ | asBDay | ^self + 0 businessDays ] .

defineMethod:
[ | asBDayMonday | 
  dayNumberOfWeek < 6 
  ifTrue: [ ^self ] ifFalse: [ ^self + 1 businessDays ] 
] .
   
defineMethod: [ | asMonthEnd | ^self + 0 monthEnds ] .
   
defineMethod: [ | asMostRecentMonthEnd | ^self + 1 days - 1 monthEnds ] .

defineMethod: 
[ | asMostRecentBusinessMonthEnd |
 asMostRecentMonthEnd asBDay
] .

defineMethod:
[ | asCurrentYearMMDD |
  !baseDate <- (^today year * 10000 + month * 100 + day ) asInteger asDate ;
  baseDate isNA ifTrue: [ :baseDate <- month asMonthEnd ] ; # in current year
  baseDate
] .

  defineMethod: [ | quarterNumber | ((month - 1)/3 + 1) asInteger ] .
  defineMethod: [ | asQuarterEnd | ^self + 0 quarterEnds ] .
  defineMethod:  [ | asYearEnd | ^self + 0 yearEnds ] .
  defineMethod: [ | asYearBeginning | ^self + 0 yearBeginnings ] .

defineMethod: [ | convertFrom: string | 
  string asDate
] .

;


#------------------------------

####################
#  Date Range: at date
####################

Prototypical Date
defineMethod:
[| to: date2 by: increment| 
!d2 <- date2 asDate ;
!endDate <- d2 isDate                # use date1 if date2 is bad
            ifTrue: [ d2 ] ifFalse: [ ^self ] ;
!incr <- increment asInteger != 0    # avoid adding 0 anythings as inc
            ifTrue: [ increment ] ifFalse: [ 1 days ] ;

^my Prototypical DateRange from: ^self to: endDate by: incr
] ;
 
Prototypical Date
defineMethod:
[ | extract: block for: object | 
  ^self evaluate: [ object send: block ] 
] ;

#------------------------------

####################
#  ytm calc support
####################

Prototypical Date

defineMethod:
[ | countDaysTo: aDate |
  (^self asIDate7 - aDate asIDate7) absoluteValue
] .

defineMethod:
[ | countBDaysTo: date | 
  !date1 <- ^self asBDay; 
  !date2 <- date asBDay; 
  !weeks <- (date1 countDaysTo: date2 ./ 7) asInteger; 
  !extraDays <- 
     ((5 + date2 dayNumberOfWeek - date1 dayNumberOfWeek) asInteger mod: 5); 
  (weeks * 5 + extraDays) asInteger
] .

defineMethod:
[ | countYearsTo: aDate | 
(^self countDaysTo: aDate ) / 365.25
] .

defineMethod:
[ | countMonthsTo: aDate |
  !initialDate <- ^self + 0 monthEnds ;
  !finalDate <- aDate asDate + 0 monthEnds ;
  initialDate > finalDate
    ifTrue: [ :initialDate <- aDate; :finalDate <- ^self ];
  initialDate = finalDate 
     ifTrue: [ 0 ]
    ifFalse: [ ( initialDate countFullMonthsTo: finalDate . + 1) asInteger ]
] .

defineMethod:
[ | countFullMonthsTo: aDate |

###   assumes ^self < aDate
    
!initialMonth <- ^self month;
!initialYear <- ^self year;
!finalMonth <- aDate month;
!finalYear <- aDate year;

(initialYear = finalYear)
ifTrue: 
  [
  finalMonth - initialMonth < 2
  ifTrue: [ 0 ]
  ifFalse: [ finalMonth - initialMonth - 1 ]
  ]
ifFalse:
  [
  !fullYears <- finalYear - initialYear - 1;
  (12 - initialMonth) + fullYears * 12 + (finalMonth - 1)
  ]
    
] .

defineMethod:
[ | count30DayDaysTo: aDate |
  (^self countFullMonthsTo: aDate) * 30 +
           (30 - ( 30 min: ^self day) ) +
           (30 min: aDate day)
] .

defineMethod:
[ | count360DayYearsTo: aDate |
  (^self count30DayDaysTo: aDate) / 360
] .

defineMethod:
[ | countIntervalsOf: offset to: aDate |

!tempDate <- ^self;
!count <- 0;

[ tempDate <= aDate ]
whileTrue: 
  [
  :tempDate <- tempDate + offset;
  :count <- count + 1;
  ];

count - 1
   
] .

defineMethod:
[ | count6monthIntervalsTo: aDate |
# should be generalized - assumes end point is always included

 !initialMonth <- ^self month;
 !initialYear <- ^self year;
 !finalMonth <- aDate month;
 !finalYear <- aDate year;

 initialYear = finalYear
 ifTrue: [ 1 ]                  # just the last one
 ifFalse:
   [
   !initialCount <- 
      initialMonth <= 6
      ifTrue: [ 1 ] ifFalse: [ 0 ];
   !fullYearCount <-
   (finalYear - initialYear - 1) * 2;
   !finalCount <- 
      finalMonth <= 6
      ifTrue: [ 1 ] ifFalse: [ 2 ];
   initialCount + fullYearCount + finalCount
   ]

] ;
?g



