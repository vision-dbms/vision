"
###################
####  Closure  ####
###################
"
?g

##############################################################################
#	Four primitive messages to 'Closure's are defined during system
#	bootstrap: 'selector', 'asMethod', 'value', and 'environment'.
#	They are repeated here for consistency.
##############################################################################

####################
#  Print and Other Standard Messages
####################
Prototypical Closure
;


####################
#  Primitives and Converses
####################
Prototypical Closure

   define: 'value' toBe: 40 asPrimitive .     #EvaluateForValue
   define: 'value:' toBe: 40 asPrimitive .    #EvaluateForValue
   define: 'valueWith:' toBe: 40 asPrimitive . 
   define: 'valueWith:and:' toBe: 40 asPrimitive .
   define: 'valueWith:and:and:' toBe: 40 asPrimitive .
   define: 'valueWith:and:and:and:' toBe: 40 asPrimitive . 
   define: 'valueWith:and:and:and:and:' toBe: 40 asPrimitive .
   define: 'valueWith:and:and:and:and:and:' toBe: 40 asPrimitive .

   define: 'environment' toBe: 41 asPrimitive . #EvaluateForEnvironment
   define: 'environment:' toBe: 41 asPrimitive .
   define: 'environmentWith:' toBe: 41 asPrimitive . 
   define: 'environmentWith:and:' toBe: 41 asPrimitive . 
   define: 'environmentWith:and:and:' toBe: 41 asPrimitive . 
   define: 'environmentWith:and:and:and:' toBe: 41 asPrimitive . 
   define: 'environmentWith:and:and:and:and:' toBe: 41 asPrimitive . 
   define: 'environmentWith:and:and:and:and:and:' toBe: 41 asPrimitive . 

   define: 'selector'      toBe: 70 asPrimitive .   #ClosureSelector
   define: 'asMethod'      toBe: 72 asPrimitive .   #ClosureAsMethod
   define: 'print'         toBe: 128 asPrimitive .  #PrintClosure
   define: 'sprint'        toBe: 279 asPrimitive .  #SPrintClosure
   define: 'divertOutput'  toBe: 87  asPrimitive .  #
   define: 'divertDelimitedOutput' toBe: 84 asPrimitive .
   define: 'whileTrue:'    toBe: 45 asPrimitive .
   define: 'self'          toBe: 74 asPrimitive .
   define: 'current'       toBe: 75 asPrimitive .
   define: 'my'	           toBe: 76 asPrimitive .

defineMethod:
[ | asClosure: object |
  ^self asMethod asClosure: object
] .

defineMethod: [ | convertFrom: string | 
  string asBlock
] .

defineMethod:
[ | primitiveIndex |
    !method <- ^self asMethod;
    method isPrimitive ifTrue: [method index]
].

defineMethod:
[ | primitiveControlValue |
    !method <- ^self asMethod;
    method isPrimitive ifTrue: [method controlValue]
].

;

?g

##################################################
#     Closure Additional Protocol
#
#   1)  Closure Only: Iteration
#   2)  Relative Dates
#   3)  Stats
#   4)  Display
#   5)  Update
#
##################################################

#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#    NOTE:  super vs ^self usage
#
#  The evaluate message as currently defined (release 3.0) only works
#  with 'pure' timeseries and closures (i.e., extensions return ^self
#  without any evaluation being done.  Therefore any methods implemented
#  here that send the evaluate: (or asOf: which calls evaluate: ) should
#  be sent to super not ^self so that situations which create extensions
#  to the initial time series structures will work consistently.  For
#  example, consider the following:
#
#       ts defineMethod: [ | latest1 | super asOf: ^today ] .
#       ts defineMethod: [ | latest2 | ^self asOf: ^today ] ;
#  1. ts latest1               -->   correct response
#  2. ts latest2               -->   correct response
#  3. ts send: [ latest1 ]     -->   correct response
#  4. ts send: [ latest2 ]     -->   incorrect response
#  5. ts send: [ ^self latest2 ] --> correct response
#
#  The fourth case breaks because ^current is an extension to the timeseries
#  and this structure is the ^self in the latest2 message.  Since super
#  always returns the structure where the message was found and the
#  messages are defined at the 'pure' time series, this approach is safe.
#  There may be problems in the future when extensions to time series
#  provide more functionality.
#
#  Although a future release will automatically send the value message
#  for non-standard types, this methods assume that super will be a
#  better choice in most cases than self.
#
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

####################
# iteration
####################

Prototypical Closure

defineMethod:
[ | convergeTo: aValue within: tolerance initialGuess: guess |

!formula <- ^self;

!low <- 0;
!high <- 1;
!maxCount <- 40;
!count <- 0;
!tempValue <- 0;

### Iterate

[ ( aValue - tempValue ) absoluteValue > tolerance && count <= maxCount ]
whileTrue:
   [
   :tempValue <- formula value: guess;
    
   aValue > tempValue
   ifTrue:                    # make tempValue bigger - decrease guess
     [
     :high <- guess;
     :guess <- (guess + low) / 2;
     ]
   ifFalse:                    # make tempValue smaller - increase guess
     [
     :low <- guess;
     :guess <- (guess + high) / 2;
     ];
   :count <- count + 1;
   
   ];        # end of while true
"number of iterations: " print; count printNL: 10;
guess

] ;

#------------------------------

##################################################
#  Closure / Time Series Routines
#   defined here separately instead of at Function
#     so that simple displayMessages will work
##################################################

####################
#  relative dates: Closure
####################

Prototypical Closure

defineMethod: [ | asOf: aDate| aDate evaluate: super ] .
defineMethod: [ | latest | super asOf: ^today ] .
defineMethod: [ | lag: anOffset | super asOf: ^date - anOffset ] .

defineMethod: 
[ | changeLag: anOffset | 
(super value) - (super lag: anOffset) 
] .

defineMethod:
[ | pctChangeLag: anOffset |
(super value) pctChangeTo: (super lag: anOffset)
] .

defineMethod: [ | lead: anOffset | super asOf: ^date + anOffset ] .

defineMethod: 
[ | changeLead: anOffset |
(super lead: anOffset) - (super value) 
] .

defineMethod:
[ | pctChangeLead: anOffset |
(super lead: anOffset) pctChangeTo: (super value)
] ;


#------------------------------

####################
#  stats : Closure
####################

Prototypical Closure

defineMethod:
[ | extractForDateRange: dr |

!ts <- ^global Prototypical TimeSeries new;
dr evaluate: [ ts put: value ];
ts

] .

#----------

defineMethod:
[ | aveForDateRange: dr |
  ^self extractForDateRange: dr . average 
] .

#----------

defineMethod:
[ | totalForDateRange: dr |
  ^self extractForDateRange: dr . total 
] .

#----------

defineMethod:
[ | maxForDateRange: dr |
  ^self extractForDateRange: dr . max 
] .

#----------

defineMethod:
[ | minForDateRange: dr |
  ^self extractForDateRange: dr . min 
] .

#----------

defineMethod:
[ | productForDateRange: dr |
  ^self extractForDateRange: dr . product 
] .

#----------

defineMethod:
[ | stdDevForDateRange: dr |
  ^self extractForDateRange: dr . stdDev
] .

#----------

defineMethod:
[ | medianForDateRange: dr |
  ^self extractForDateRange: dr . median
] .

#----------

defineMethod:
[ | cGrowForDateRange: dr |
  ^self extractForDateRange: dr . cGrow
] .

#----------

defineMethod:
[ | gMeanForDateRange: dr |
  ^self extractForDateRange: dr . gMean 
] .

#----------

defineMethod:
[ | lsGrowForDateRange: dr |
  ^self extractForDateRange: dr . lsGrow 
] .

;


####################
#  display
####################

Prototypical Closure
defineMethod:
[ | displayAll |
"  ***  Message Is Not A Time Series  ***
Defined As:
===> " print;
printNL;
] .

#----------

defineMethod:
[ | asUndelimitedString |
  ^self asString stripChar: newLine . drop: 1 . drop: -1
] .

defineMethod:
[ | formatForExcel | 
  ^self divertDelimitedOutput formatForExcel
] .

defineMethod:
[ | formatForPRN | 
  ^self divertDelimitedOutput formatForPRN
] .

;

Prototypical Closure defineMethod: [ | asRowsColumns |
!delimiter <- "ÿ" ;      #- vision delimiter
delimiter concat: (^self divertDelimitedOutput drop: -1) . asLines
   send: [ ^self drop: 1 ] .   #- drop leading delimiter character
  extendBy: [ !columns <- ^self breakOn: ^my delimiter . numberElements ]
] ;

?g
