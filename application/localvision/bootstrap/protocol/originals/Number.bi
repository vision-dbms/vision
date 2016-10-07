"
##################
####  Number  ####
##################
"
?g

####################
#  Printing and Standard Messages
####################

Prototypical Number

defineMethod: [ | print: aFormat | aFormat printOf: ^self asNumber] .
defineMethod: [ | sprint: aFormat | aFormat sprintOf: ^self asNumber] .

defineMethod:
[ | printWithCommas: aFormat |
  aFormat printWithCommasOf: ^self asNumber
] .

defineMethod:
[ | sprintWithCommas: aFormat |
  aFormat sprintWithCommasOf: ^self asNumber
] .

defineMethod: [ | printWithLeadingZeros: aWidth | 
aWidth < 0 
  ifTrue: [ ^self print: aWidth ]
 ifFalse: 
   [ !string <- [ ^self print: aWidth ] divertOutput stripBoundingBlanks;
     "0" fill: aWidth . concat: string . 
          take: aWidth negated . print ; 
   ] ;
^self
] .

defineMethod: [ | printWithLeadingZerosNL: aWidth | 
  ^self printWithLeadingZeros: aWidth ;
  newLine print;
  ^self
] .

;

####################
#  Primitives and Converses
####################

Prototypical Number

  defineMethod: [ | sequence  | ^self asInteger sequence ].
  defineMethod: [ | sequence0 | ^self asInteger sequence0].

defineMethod:
[ | base1CellPositionOf: aList |
    aList base1CellAtPosition: ^self asInteger
].

defineMethod:
[ | base0CellPositionOf: aList |
    aList base0CellAtPosition: ^self asInteger
].

;
?g

#--------------------------------------------------

##################################################
#    Number Definitions
#
#   -  rel ops
#   -  computations
#   -  date evaluation
#   -  misc
#
##################################################


####################
#  rel ops
####################

Prototypical Number

defineMethod: [ | != aNumber | (^self = aNumber) not] .
defineMethod: [ | negated | 0 - ^self ] .

defineMethod:
[ | > aNumber | 
  aNumber isNumber
     ifTrue: [ aNumber < ^self asNumber ]
] .

defineMethod:
[ | >= aNumber | 
  aNumber isNumber
     ifTrue: [ aNumber <= ^self asNumber ]
] .
;

####################
#  computations
####################

Prototypical Number

  defineMethod: [ | log | ^self asDouble log ] .
  defineMethod: [ | exp | ^self asDouble exp ] .
  defineMethod: [ | sqrt | ^self asDouble sqrt ] .
  defineMethod: [ | toThe: x | ^self asDouble toThe: x ] .
  defineMethod: [ | asAPowerOf: x | ^self asDouble asAPowerOf: x ] .
  defineMethod: [ | sine               | ^self asDouble sine] .
  defineMethod: [ | cosine          | ^self asDouble cosine] .
  defineMethod: [ | tangent          | ^self asDouble tangent] .
  defineMethod: [ | arcSine          | ^self asDouble arcSine] .
  defineMethod: [ | arcCosine          | ^self asDouble arcCosine] .
  defineMethod: [ | arcTangent          | ^self asDouble arcTangent] .
  defineMethod: [ | hyperbolicSine     | ^self asDouble hyperbolicSine] .
  defineMethod: [ | hyperbolicCosine     | ^self asDouble hyperbolicCosine] .
  defineMethod: [ | hyperbolicTangent     | ^self asDouble hyperbolicTangent] .

defineMethod:
[ | pctChangeTo: n | 
!curr <- ^self ;
!base <- n else: 0 ;
  (curr > 0 && base > 0 ) || (curr < 0 && base < 0 )
     ifTrue: [ (curr - base) / base absoluteValue * 100 ]
       else: [ NA ]
] .

#----------

defineMethod:
[ | within: pc percentOf: y |

!x <- ^self;
!fraction <- pc / 100;

y < 0 ifTrue: [ x >= y * (1 + fraction)  &&
                x <= y * (1 - fraction)
              ]
     ifFalse: [ x <= y * (1 + fraction)  &&
                x >= y * (1 - fraction)
              ]
] .

#----------

defineMethod:
[ | round |
  ^self < 0
     ifTrue: [ ^self - .5] ifFalse: [ ^self + .5] . asInteger
] .

#----------

defineMethod: [ | round: decimalPlaces |
!factor <- 10 toThe: decimalPlaces;
^self < 0
   ifTrue: [ (^self * factor - .5 ) asInteger / factor ]
   ifFalse: [(^self * factor + .5 ) asInteger / factor ]
] .

#----------

defineMethod: 
[ | absoluteValue |
^self < 0 
  ifTrue: [ ^self negated] ifFalse: [ ^self ]
] .

#----------

defineMethod: [ | log: base |  ^self log / base log ] .

#----------

defineMethod:
[ | sign | 
  ^self < 0 ifTrue: [ -1 ] .
     elseIf: [ ^self = 0 ] then: [ 0 ] .
     else: [ 1 ] 
] .
;

#------------------------------

####################
#  date conversion: 
####################

Prototypical Number

defineMethod:
[ | asDate |
  ^self asInteger asDate
] .

;

#--------------------
#  misc
#--------------------

Prototypical Number
defineMethod:
[ | inputExpression |
  ^self
] ;


Prototypical Integer

defineMethod:
[ | asString | 
  ^self sprint: ^self numberOfDigits
] .
;

Prototypical Number defineMethod: [ | convertFrom: string | 
  string convertToNumber
] ;

Prototypical Float defineMethod: [ | convertFrom: string | 
  string asNumber asFloat
] ;

Prototypical Integer defineMethod: [ | convertFrom: string | 
  string asNumber asInteger
] ;

Prototypical Double defineMethod: [ | convertFrom: string | 
  string asNumber asDouble
] ;

?g

