"
###############################
####  DateOffset Protocol  ####
###############################
"
?g

####################
#  Print and Other Standard Messages
####################

;

####################
#  Primitives and Converses
####################
Prototypical DateOffset

defineMethod:
[ | print |
  ^self = ^self asInteger
      ifFalse: [ ^self asInteger print ; " " print ] ;
  ^self incrementName print ; 
  ^self
] .

  define: 'asInteger' toBe: Prototypical DateOffset .
  define: 'incrementName' toBe: "DateOffset" .
;

Prototypical DateOffset defineMethod: [ | evaluate: block | 
  ^date + ^self evaluate: block
] ;

Prototypical DateOffset defineMethod: [ | convertFrom: string |
  !prefix <- string prefixSpannedBy: " 0123456789" ;
  !increment <- string take: prefix . asNumber asInteger ;
  !offset <- string drop: prefix ;
  increment isInteger && [ 0 respondsTo: offset ]
      ifTrue: [ offset evaluateIn: increment ] else: [ NA ] 
] ;


#--------------------

Prototypical Days
   define: 'incrementName'      toBe: "Days" .
   define: 'asInteger'          toBe: 33 asPrimitive .      #CurrentPointerAsValue
   define: 'increment:'         toBe: 310 asPrimitive .    #ByDaysIncrementDate
   define: 'decrement:'         toBe: 330 asPrimitive .    #ByDaysDecrementDate
   define: 'from:to:evaluate:'  toBe: 50 asPrimitive .    #ByDaysForDatesEvaluate
;

Prototypical BusinessDays
   define: 'incrementName'      toBe: "BusinessDays" .
   define: 'asInteger'          toBe: 33 asPrimitive .     #CurrentPointerAsValue
   define: 'increment:'         toBe: 311 asPrimitive .    #ByBDaysIncrementDate
   define: 'decrement:'         toBe: 331 asPrimitive .    #ByBDaysDecrementDate
   define: 'from:to:evaluate:'  toBe: 51 asPrimitive  .   #ByBDaysForDatesEvaluate
;

Prototypical Months
   define: 'incrementName'      toBe: "Months" .
   define: 'asInteger'          toBe: 33 asPrimitive .     #CurrentPointerAsValue
   define: 'increment:'         toBe: 312 asPrimitive .     #ByMonthsIncrementDate
   define: 'decrement:'         toBe: 332 asPrimitive .    #ByMonthsDecrementDate
   define: 'from:to:evaluate:'  toBe: 52 asPrimitive  .   #ByMonthsForDatesEvaluate
;

Prototypical MonthBeginnings
    define: 'incrementName' toBe: "MonthBeginnings" .
   define: 'asInteger'      toBe: 33 asPrimitive .     #CurrentPointerAsValue
   define: 'increment:'      toBe: 313 asPrimitive .     #ByMonthBeginningsIncrementDate
   define: 'decrement:'      toBe: 333 asPrimitive .     #ByMonthBeginningsDecrementDate
   define: 'from:to:evaluate:' toBe: 53 asPrimitive .     #ByMonthBeginningsForDatesEvaluate
;

Prototypical MonthEnds
    define: 'incrementName' toBe: "MonthEnds" .
   define: 'asInteger'      toBe: 33 asPrimitive .     #CurrentPointerAsValue
   define: 'increment:'      toBe: 314 asPrimitive .     #ByMonthEndsIncrementDate
   define: 'decrement:'      toBe: 334 asPrimitive .     #ByMonthEndsDecrementDate
   define: 'from:to:evaluate:' toBe: 54 asPrimitive .     #ByMonthEndsForDatesEvaluate
;

Prototypical Quarters
    define: 'incrementName' toBe: "Quarters" .
   define: 'asInteger'      toBe: 33 asPrimitive .     #CurrentPointerAsValue
   define: 'increment:'      toBe: 315 asPrimitive .     #ByQuartersIncrementDate
   define: 'decrement:'      toBe: 335 asPrimitive .     #ByQuartersDecrementDate
   define: 'from:to:evaluate:' toBe: 55 asPrimitive .     #ByQuartersForDatesEvaluate
;

Prototypical QuarterBeginnings
    define: 'incrementName' toBe: "QuarterBeginnings" .
   define: 'asInteger'      toBe: 33 asPrimitive .     #CurrentPointerAsValue
   define: 'increment:'      toBe: 316 asPrimitive .     #ByQuarterBeginningsIncrementDate
   define: 'decrement:'      toBe: 336 asPrimitive .     #ByQuarterBeginningsDecrementDate
   define: 'from:to:evaluate:' toBe: 56 asPrimitive .     #ByQuarterBeginningsForDatesEvaluate
;

Prototypical QuarterEnds
    define: 'incrementName' toBe: "QuarterEnds" .
   define: 'asInteger'      toBe: 33 asPrimitive .     #CurrentPointerAsValue
   define: 'increment:'      toBe: 317 asPrimitive .     #ByQuarterEndsIncrementDate
   define: 'decrement:'      toBe: 337 asPrimitive .     #ByQuarterEndsDecrementDate
   define: 'from:to:evaluate:' toBe: 57 asPrimitive .     #ByQuarterEndsForDatesEvaluate
;

Prototypical Years
    define: 'incrementName' toBe: "Years" .
   define: 'asInteger'      toBe: 33 asPrimitive .     #CurrentPointerAsValue
   define: 'increment:'      toBe: 318 asPrimitive .     #ByYearsIncrementDate
   define: 'decrement:'      toBe: 338 asPrimitive .     #ByYearsDecrementDate
   define: 'from:to:evaluate:' toBe: 58 asPrimitive .     #ByYearsForDatesEvaluate
;

Prototypical YearBeginnings
    define: 'incrementName' toBe: "YearBeginnings" .
   define: 'asInteger'      toBe: 33 asPrimitive .     #CurrentPointerAsValue
   define: 'increment:'      toBe: 319 asPrimitive .     #ByYearBeginningsIncrementDate
   define: 'decrement:'      toBe: 339 asPrimitive .     #ByYearBeginningsDecrementDate
   define: 'from:to:evaluate:' toBe: 59 asPrimitive .     #ByYearBeginningsForDatesEvaluate
;

Prototypical YearEnds
    define: 'incrementName' toBe: "YearEnds" .
   define: 'asInteger'      toBe: 33 asPrimitive .     #CurrentPointerAsValue
   define: 'increment:'      toBe: 320 asPrimitive .     #ByYearEndsIncrementDate
   define: 'decrement:'      toBe: 340 asPrimitive .     #ByYearEndsDecrementDate
   define: 'from:to:evaluate:' toBe: 60 asPrimitive .     #ByYearEndsForDatesEvaluate
;

?g


##################################################
#    DateOffset Definitions
#
#   1) base routines
#   2) arithmetic
#   3) asDateOffset
#
##################################################

####################
#  base routines
####################

Prototypical DateOffset

defineMethod:
[ | evaluate: aBlock from: date1 to: date2 |

date1 <= date2
ifTrue: 
   [
      [ date1 <= date2 ] whileTrue:
          [
          date1 evaluate: aBlock;
          :date1 <- ^self increment: date1
          ]
   ]
ifFalse:
   [
      [ date1 >= date2 ] whileTrue: 
          [
          date1 evaluate: aBlock;
	  :date1 <- ^self decrement: date1
	  ]
   ]
] .

defineMethod:
[ | print |
  ^self = ^self asInteger
    ifFalse: [ ^self asInteger print ; " " print ] ;
  ^self incrementName print; 
  ^self
] ;


#------------------------------

####################
#  arithmetic
####################

Prototypical DateOffset

define: '*' toBe:
[ | :aNumber |
        (^self asInteger * aNumber) asInteger asPointerTo: ^self asDateOffset
] asMethod .

#----------

defineMethod: [ | + aValue | aValue asDate + ^self] .

defineMethod: [ | subtractedFrom: aValue | aValue asDate - ^self] .

defineMethod: [ | asDate | ^date + ^self ] ;

#------------------------------

####################
#  asDateOffset
####################

Prototypical BusinessDays defineAsMessage: 'asDateOffset' ;

Prototypical Days defineAsMessage: 'asDateOffset' ;

Prototypical MonthBeginnings defineAsMessage: 'asDateOffset' ;

Prototypical MonthEnds defineAsMessage: 'asDateOffset' ;

Prototypical Months defineAsMessage: 'asDateOffset' ;

Prototypical QuarterBeginnings defineAsMessage: 'asDateOffset' ;

Prototypical QuarterEnds defineAsMessage: 'asDateOffset' ;

Prototypical Quarters defineAsMessage: 'asDateOffset' ;

Prototypical YearBeginnings defineAsMessage: 'asDateOffset' ;

Prototypical YearEnds defineAsMessage: 'asDateOffset' ;

Prototypical Years defineAsMessage: 'asDateOffset' ;

?g


