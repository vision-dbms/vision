"
#####################
####  Undefined  ####
#####################
"
?g

####################
#  Printing and other standard messages
####################

Prototypical Undefined

defineMethod: [ | print |  "      NA " print; ^self ] .

defineMethod:
[ | print: aWidth | 
"NA " print: aWidth absoluteValue negated; ^self
] .
                       
defineMethod:
[ | sprint: aWidth | 
"NA " sprint: aWidth absoluteValue negated; ^self 
] .
;

NA defineMethod: [ | printWithLeadingZeros: f | ^self print: f ] ;
NA defineMethod: [ | printWithLeadingZerosNL: f | ^self printNL: f ] ;

####################
#  Primitives and Converses
####################

Prototypical Undefined

    defineMethod: [ | new | ^self ] .
    define: 'isNA'              toBe: TRUE .
    define: 'isntNA'            toBe: FALSE .
    define: 'isDefault'         toBe: TRUE .
    define: 'negated'           toBe: NA .
    define: '+'                 toBe: NA .
    define: '-'                 toBe: NA .
    define: 'subtractedFrom:'   toBe: NA .
    define: '*'                 toBe: NA .
    define: '/'                 toBe: NA .
    define: 'dividedInto:'      toBe: NA .
    define: 'log'               toBe: NA .
    define: 'exp'               toBe: NA .
    define: 'sqrt'              toBe: NA .
    define: 'toThe:'            toBe: NA .
    define: 'asAPowerOf:'       toBe: NA .
    define: '<'                 toBe: NA .
    define: '<='                toBe: NA .
    define: '>='                toBe: NA .
    define: '>'                 toBe: NA .
    define: 'not'               toBe: NA .
    define: 'asPointerTo:'      toBe:  31 asPrimitive .     #ValueAsPointerTo
    define: 'contains:'         toBe: NA .
    define: 'matchesString:'    toBe: NA .

;

?g

##################################################
#    Undefined (NA) Definitions
#
#   2) boolean logic
#   3) numeric equivs
#   4) other equivs
#
##################################################



####################
#  Fix && and || logic at NA
####################

Prototypical Undefined

defineMethod:
[ | && x |
  x value = FALSE ifTrue: [ FALSE ] ifFalse: [ NA ]
] .

defineMethod:
[ | || x |
  x value = TRUE ifTrue: [ TRUE ] ifFalse: [ NA ]
] .

defineMethod: 
[ | else: object |
  object value
] .

defineMethod:
[ | elseIf: condition then: object |
  condition value ifTrue: object else: NA
] .

;

#------------------------------

####################
#  numeric equivs
####################

Prototypical Undefined

defineMethod: [| pctChangeTo: aNumber |  NA ] .

defineMethod: [ | within: pc percentOf: y |  NA  ] .

define: 'round' toBe: NA  .

defineMethod: [ | round: decimalPlaces | NA ] .

define: 'absoluteValue' toBe: NA .

defineMethod: [ | min: aNumber | NA ] .

defineMethod: [ | max: aNumber | NA ] .

defineMethod: [ | mod: aNumber | NA ] .

defineMethod: [ | toThe: aNumber | NA ] .

defineMethod: [ | log: base | NA ] .

define: 'numberOfDigits' toBe: NA .

defineMethod:
[ | between: lower and: upper |
  NA
] .
defineMethod:
[ | notBetween: lower and: upper | 
  NA
] .
;

#------------------------------

####################
#  other equivs
####################
Prototypical Undefined 
  define: 'asInteger' toBe: NA    .
  define: 'asDouble'  toBe: NA .
  define: 'asFloat'   toBe: NA .
  define: 'asNumber'  toBe: NA .
  define: 'asDate'    toBe: NA .
  define: 'asCurrency' toBe: NA .
  define: 'basicAsDate' toBe: NA .
  defineMethod: [ | asDateFromYearForMonth: mm andDay: dd | NA ] .
;

NA 
defineMethod: 
[ | as: type | 
  type isString && [ type isntDefault ] 
     ifTrue: [ type evaluate ] ifFalse: [ type ] .
  defaultInstance 
] ;
NA defineMethod: [ | convertFrom: object | NA ] ;

####################
#  Redefine else: and elseIf:then: messages at the NoValue class
#    to use the versions from object; only the explict NA should trigger
#    condition
#
# To test use:
#  TRUE
#    ifTrue: [ "abc" printNL ; ] .       #- only this should print
#    elseIf: [ TRUE] then: [ "def" printNL ; ] .
#    else: [ "foo" printNL ; ] ;
#
####################

[;] value
defineMethod: [ | else: object | ^self ] .
defineMethod: [ | elseIf: condition then: object | ^self ] .

;



?g



