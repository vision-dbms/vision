"
###################
####  Integer  ####
###################
"
?g

##############################################################################
#	Three primitive messages to 'Integer' are defined during system
#	bootstrap: 'asDate', 'asFloat', and 'asPointerTo'.
#	They are repeated here for consistency.
##############################################################################

##############################################################################
#  Note:
#	The 'Integer' method 'asPrimitive' is defined in 'NameBuiltIns'.
##############################################################################

####################
#  printing and other standard messages
####################

Prototypical Integer
;

####################
#  Primitives and Converses
####################

Prototypical Integer

  define: 'asNumber'                 toBe: 11 asPrimitive .     #Return Current
  define: 'asInteger'                toBe: 11 asPrimitive .     #ReturnCurrent
  define: 'asReferenceTo:'           toBe: 30 asPrimitive .     #IntegerAsReferenceTo
  define: '_primitiveRefTo:'         toBe: 30 asPrimitive .     #IntegerAsReferenceTo
  define: 'asPointerTo:'             toBe: 31 asPrimitive .     #ValueAsPointerTo
  define: '_integerRefTo:'           toBe: 31 asPrimitive .     #ValueAsPointerTo
  define: 'instancesAdjacentTo:'     toBe: 106 asPrimitive .     #SelectedRowsInStore
  define: 'print'                    toBe: 121 asPrimitive .     #PrintCurrentAsPointer
  define: 'printOf:'                 toBe: 133 asPrimitive .     #IntegerPrintOf
  define: 'sprintOf:'                toBe: 135 asPrimitive .     #IntegerSPrintOf
  define: 'printWithCommasOf:'       toBe: 136 asPrimitive .     #IntegerPrintWithCommasOf
  define: 'sprintWithCommasOf:'      toBe: 139 asPrimitive .     #IntegerSPrintWithCommasOf
  define: 'canonicalizedForSort'     toBe: 214 asPrimitive .     #IntegerAsDouble
  define: 'orderUsingDictionary:'    toBe: 214 asPrimitive .     # IntegerAsDouble
  define: 'asDouble'                 toBe: 214 asPrimitive .     #IntegerAsDouble
  define: 'asFloat'                  toBe: 215 asPrimitive .     #IntegerAsFloat
  define: '+'                        toBe: 220 asPrimitive .     #IntegerAdd
  define: '-'                        toBe: 221 asPrimitive .     #IntegerSubtract
  define: 'subtractedFrom:'          toBe: 222 asPrimitive .     #IntegerSubtractFrom
  define: '*'                        toBe: 223 asPrimitive .     #IntegerMultiply
  define: '/'                        toBe: 224 asPrimitive .     #IntegerDivide
  define: 'dividedInto:'             toBe: 225 asPrimitive .     #IntegerDivideInto
  define: '<'                        toBe: 226 asPrimitive .     #IntegerLessThan
  define: '<='                       toBe: 227 asPrimitive .     #IntegerLessThanOrEqual
  define: '='                        toBe: 228 asPrimitive .     #IntegerEqual
  define: 'asDate'                   toBe: 300 asPrimitive .     #IntegerAsDate
  define: 'basicAsDate'              toBe: 300 asPrimitive .     #IntegerAsDate
  define: 'sequence'                 toBe: 165 asPrimitive .
  define: 'sequence0'                toBe: 164 asPrimitive .

defineMethod: [ | sequenceTo: end  |
  !first <- ^self ;
  !last <- end asInteger else: 0 ;
  first <= last
  ifTrue:
    [ (last - first + 1) sequence0 
         send: [ (^my first + ^self ) asInteger ] 
    ]
  ifFalse:
    [ (first - last + 1) sequence0
         send: [ (^my first - ^self ) asInteger ] 
    ] 
] .

defineMethod: 
[ | fillFromString: aString |
   aString fill: ^self asInteger
] .

defineMethod: 
[ | takeFromString: aString |
   aString take: ^self asInteger
] .

defineMethod: 
[ | dropFromString: aString|
   aString drop: ^self asInteger
] .

defineMethod: 
[ | prefixWith: aString |
   aString concat: ^self asInteger
] .

defineMethod: 
[ | days | 
  ^self asPointerTo: ^my Prototypical Days 
] .

defineMethod:
[ | businessDays | 
  ^self asPointerTo: ^my Prototypical BusinessDays
] .

defineMethod:
[ | months|
  ^self asPointerTo: ^my Prototypical Months
] .

defineMethod:
[ | monthBeginnings |
  ^self asPointerTo: ^my Prototypical MonthBeginnings
] .

defineMethod:
[ | monthEnds |
  ^self asPointerTo: ^my Prototypical MonthEnds
] .

defineMethod:
[ | quarters |
  ^self asPointerTo: ^my Prototypical Quarters
] .

defineMethod:
[ | quarterBeginnings |
  ^self asPointerTo: ^my Prototypical QuarterBeginnings
] .

defineMethod:
[ | quarterEnds |
  ^self asPointerTo: ^my Prototypical QuarterEnds
] .

defineMethod:
[ | years |
  ^self asPointerTo: ^my Prototypical Years
] .

defineMethod:
[ | yearBeginnings |
  ^self asPointerTo: ^my Prototypical YearBeginnings
] .

defineMethod:
[ | yearEnds |
  ^self asPointerTo: ^my Prototypical YearEnds
] .

defineMethod:
[ | base1CellPositionOf: aList |
  aList base1CellAtPosition: super
] .

defineMethod:
[ | base0CellPositionOf: aList |
    aList base0CellAtPosition: super
] .

;

?g

####################
#  Computations: Integer only
####################

Prototypical Integer

defineMethod: 
[ | mod: aNumber |
  ^self >= 0 && aNumber > 0 
  ifTrue: [ (^self - (^self / aNumber) asInteger * aNumber ) asInteger]
] .

defineMethod:
[ | adjustedMod: aNumber |
  ^self > 1 
     ifTrue: [ ( (^self - 1 ) asInteger mod: aNumber . + 1 ) asInteger ]
    ifFalse: [ 1 ] 
] .

#----------

defineMethod:
[ | numberOfDigits |
^self = 0
   ifTrue: [ 1 ]
  ifFalse:
     [ ^self < 0
         ifTrue: [ ( ^self absoluteValue asInteger numberOfDigits + 1 ) 
                                                                asInteger 
                 ]
        ifFalse: [ (^self log: 10 . + 1.000000000001 ) asInteger ] 
     ]

] ;     # numberOfDigits

#--------------------
#  methods to increment and decrement, preserving integer form
#--------------------

Prototypical Integer defineMethod: [ | increment | ^self increment: 1 ] ;
Prototypical Integer defineMethod: [ | increment: n | (^self + n) asInteger ] ;
Prototypical Integer defineMethod: [ | decrement | ^self decrement: 1 ] ;
Prototypical Integer defineMethod: [ | decrement: n | (^self - n) asInteger ] ;

####################
#  date conversion: integer only
####################

Prototypical Integer 

#----------
#  redefine asDate to be non-primitive
#----------
defineMethod: [ | asDate |
!date <- basicAsDate ;
!digits <- numberOfDigits; 

#-- 10101 is default date ; 20010101 needs to be in yyyymmdd format
date != earliestPossibleDate && [ ^self >= 0 ] && [ digits != 8 ]
ifTrue:
  [ 
  digits <= 2     # YY format
  ifTrue: [ :date <- ^self asDateFromYearForMonth: 12 andDay: 31 ; ] .
  elseIf: [ digits <= 4 ]     # YYMM format
    then: [ !mm <- ^self mod: 100 ;
            !yy <- ((^self - mm) / 100 ) asInteger ;
            :date <- yy asDateFromYearForMonth: mm andDay: 1 .
                 + 0 monthEnds ;
          ] .
  elseIf: [ digits <= 6 ]     # YYMMDD format
    then: [ !dd <- ^self mod: 100 ;
            !mm <- ((^self - dd) / 100 ) asInteger mod: 100 ;
            !yy <- (( ^self - mm * 100 - dd) / 10000 ) asInteger ;
            :date <- yy asDateFromYearForMonth: mm andDay: dd ;
          ] ;
    ] ;
date
] .

#--  this one is useful if YY or YYM is all 0 since the
#--  default interpretation of 00MMDD or 000MDD will be YYMM
defineMethod: [ | asDateFromYYMMDD |
  !dd <- ^self mod: 100 ;
  !mm <- ((^self - dd) / 100 ) asInteger mod: 100 ;
  !yy <- (( ^self - mm * 100 - dd) / 10000 ) asInteger ;
  yy asDateFromYearForMonth: mm andDay: dd 
] .

defineMethod: [ | asDateFromYYMM |
!mm <- ^self mod: 100 ;
!yy <- ((^self - mm) / 100 ) asInteger mod: 100 ;
yy asDateFromYearForMonth: mm andDay: 1 . + 0 monthEnds
] .

#----------

defineMethod:
[ | asDateFromYYYYMM |
  ((^self * 100) + 1 ) asInteger basicAsDate + 0 monthEnds
] .

#----------

defineMethod: 
[ | asDateFromMMDDYY |

#-- smallest value is 010100 -> 1/1/2001
#-- largest values is 123199 -> 12/31/1999
  ^self >= 10100 && ^self <= 123199
  ifTrue:
    [
    !string <- ^self asString ;
    !mm <- string drop: -4 . asNumber ;
    !dd <- string drop: -2 . take: -2 . asNumber ;
    !yy <- string take: -2 . asNumber ;
    yy asDateFromYearForMonth: mm andDay: dd
    ]

] .

#----------

defineMethod:
[ | asDateFromMMDD |
  !year <- ^today year ;
  (year * 10000 + ^self ) asInteger basicAsDate
] .

#----------

defineMethod:
[ | asDateFromMMDDYYYY |
  !YYYY <- ^self mod: 10000 ;
  !MMDD <- (^self / 10000 ) asInteger ; 
  (YYYY * 10000 + MMDD ) asInteger basicAsDate
] .

#----------

defineMethod: [ | asDateFromMMYY | 
  !yy <- ^self mod: 100; 
  !mm <- (^self - yy) / 100; 
  yy asDateFromYearForMonth: mm andDay: 1 . + 0 monthEnds
] .

#----------

defineMethod: 
[ | asMonthEndInYear: yyyy |

yyyy asInteger asDateFromYearForMonth: ^self andDay: 1 . + 0 monthEnds
]  .

defineMethod: [ | asDateFromYearForMonth: mm andDay: dd | 
!cutoff <- 25; 
^self >= 0 && [ mm between: 1 and: 12 ] && [ dd between: 1 and: 31 ] 
ifTrue: 
  [
  !yyyy <- ^self < 100 
    ifTrue: [ ^self < cutoff 
                  ifTrue: [2000 + ^self] ifFalse: [1900 + ^self]
            ]
   ifFalse: [^self] ;
   ( yyyy * 10000 + mm * 100 + dd) asInteger basicAsDate
  ]
] .

#----------

defineMethod: [ | asMonthEnd | ^self asMonthEndInYear: ^today year ] ;

#------------------------------

####################
#  date evaluation
####################

Prototypical Integer
defineMethod:
[ | evaluate: aBlock |
  ^self asDate send: [ isDate ifTrue: [ ^self evaluate: ^my aBlock ] ]
] .

#----------

defineMethod:
[ | to: aDate by: anIncrement |
 ^self asDate send: [ isDate
                      ifTrue: [ ^self to: ^my aDate by: ^my anIncrement ] 
                    ]
] ;

?g
