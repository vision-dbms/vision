"
###################
####  Ordinal  ####
###################
"
?g

####################
#  Print and Other Standard Messages
####################

Prototypical Ordinal defineMethod: [ | displayInfo | ^self printNL ] ;


####################
#  Primitives and Converses
####################

Prototypical Ordinal

defineMethod: [ | new | ^self ] .

defineMethod: [| < anOrdinal | ^self subClassResponsibility: '<'] .
defineMethod: [| <= anOrdinal | ^self < anOrdinal || ^self = anOrdinal] .
defineMethod: [| >= anOrdinal | (^self < anOrdinal) not] .
defineMethod: [| > anOrdinal | (^self <= anOrdinal) not] .
;

?g


##################################################
#    Ordinal Definitions
#
##################################################

Prototypical Ordinal

defineMethod:
[ | min: val |
  ^self < val 
    ifTrue: [ ^self ] 
   ifFalse: [ val ] 
      else: [ NA ]
] .
defineMethod:
[ | max: val | 
  ^self > val 
    ifTrue: [ ^self ] 
   ifFalse: [ val ] 
      else: [ NA ]
] .

defineMethod:
[ | between: lower and: upper |
  lower isntNA && upper isntNA
  ifTrue: [ lower <= ^self && ^self <= upper ] 
  ifFalse:
    [
    lower isntNA ifTrue: [ ^self >= lower ] 
                ifFalse: [ ^self <= upper ]
    ]
] .

defineMethod:
[ | notBetween: lower and: upper |
  lower isntNA && upper isntNA
  ifTrue: [ ^self < lower || ^self > upper ] 
  ifFalse:
    [
    lower isntNA ifTrue: [ ^self < lower ] 
                ifFalse: [ ^self > upper ]
    ]
] .

defineMethod:
[ | inSet: list |
  list includesElement: ^self
] .

defineMethod:
[ | inRange: list |
  !point1 <- list at: 1 ;
  !point2 <- list at: 2 ;
  (point1 <= ^self )  && (^self <= point2)
] .

;

?g
