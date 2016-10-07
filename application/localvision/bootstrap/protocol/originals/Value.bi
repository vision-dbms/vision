"
#################
####  Value  ####
#################
"
?g

####################
#  Print and Other Standard Messages
####################
Prototypical Value

defineMethod:
[ | print |
  ":[" print;
  ^self value print;
  "]" print;
  ^self
] .
;

####################
#  Primitives and Converses
####################

Prototypical Value
 define: 'value' toBe: 40 asPrimitive . #EvaluateForValue
 define: '<-'    toBe: 80 asPrimitive . #Assign
 defineMethod: [| asOf: aDate | ^self value ] .

;

####################
#  implement equivalent of += and -=
#     :score incrementBy: 1   replaces  :score <- (score + 1 ) asInteger
####################


Prototypical Value
defineMethod: 
[ | incrementBy: counter |
   ^self <- (^self value + counter ) asInteger
] .

defineMethod:
[ | increment |
  ^self incrementBy: 1
] .

defineMethod: 
[ | decrementBy: counter |
   ^self <- (^self value - counter ) asInteger
] .

defineMethod:
[ | decrement |
  ^self decrementBy: 1
] .

defineMethod: [ | changeTo: newValue | 
!changed <- FALSE ;
!currentValue <- ^self value ;
currentValue !== newValue
   ifTrue: [ :changed <- TRUE ; ^self <- newValue ] ;
changed
] .

;

?g
