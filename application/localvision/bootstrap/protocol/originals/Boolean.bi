"
###################
####  Boolean  ####
###################
"
?g

####################
#  Print and Other Standard Messages
####################

Prototypical Boolean
 defineMethod: [ | new | ^self ] .
 defineMethod: [ | print: f | ^self asString print: f ] .
;

Prototypical Boolean defineMethod: [ | convertFrom: string | 
  !id <- string stripBoundingBlanks toUpper take: 1 ;
  id = "T" || id = "Y" ifTrue: [ TRUE ].
     elseIf: [ id = "F" || id = "N" ] then: [ FALSE ] 
] ;
  
################
####  TRUE  ####
################

TRUE
  defineMethod: [ | && x | x value ] .
  defineMethod: [ | || x | TRUE ] .

   define: 'ifTrue:'  toBe: 42 asPrimitive .    #EvaluateUltimateParameter
   define: 'ifFalse:' toBe: NA .
   define: 'ifTrue:ifFalse:'                    #EvaluatePenultimateParameter
     toBe: 43 asPrimitive . 

   define: 'not' toBe: FALSE .
   define: 'isTrue' toBe: TRUE .

defineMethod:
[ | ifTrue: object1 else: object2 | 
  object1 value
] .

defineMethod:
[ | ifTrue: object1 ifFalse: object2 else: object3 | 
  object1 value
] .

;


#################
####  FALSE  ####
#################

FALSE
    defineMethod: [ | && x | FALSE ] .
    defineMethod: [ | || x | x value ] .

   define: 'ifTrue:'  toBe: NA .
   define: 'ifFalse:' toBe: 42 asPrimitive .   #EvaluateUltimateParameter
   define: 'ifTrue:ifFalse:'                   #EvaluateUltimateParameter
      toBe: 42 asPrimitive .
   define: 'not' toBe: TRUE .
   define: 'isFalse' toBe: TRUE .
defineMethod:
[ | ifFalse: object1 else: object2 | 
  object1 value
] .

defineMethod:
[ | ifTrue: object1 ifFalse: object2 else: object3 | 
  object2 value
] .
;


?g
