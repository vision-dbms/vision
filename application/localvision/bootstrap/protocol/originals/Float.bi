"
#################
####  Float  ####
#################
"
?g

####################
#  printing and other standard messages
####################

Prototypical Float
  defineMethod: [ | printOf: anObject | anObject print: asDouble ] .

defineMethod:
[ | printWithCommasOf: anObject | 
  anObject printWithCommas: asDouble
] .

defineMethod: [ | sprintOf: anObject | anObject sprint: asDouble ] .

defineMethod: 
[ | sprintWithCommasOf: anObject | anObject sprintWithCommas: asDouble ] .
;

####################
#  primitives and converses
####################

Prototypical Float
  define: 'print'                 toBe: 121 asPrimitive .     #PrintCurrentAsPointer
  define: 'canonicalizedForSort'  toBe: 212 asPrimitive .     #FloatAsDouble
  define: 'orderUsingDictionary:' toBe: 212 asPrimitive .     # FloatAsDouble
  define: 'asNumber'              toBe: 11 asPrimitive .     #Return Current
  define: 'asDouble'              toBe: 212 asPrimitive .     #FloatAsDouble
  define: 'asFloat'               toBe: 11 asPrimitive .     #ReturnCurrent
  define: 'asInteger'             toBe: 213 asPrimitive .     #FloatAsInteger
  define: 'asPointerTo:'          toBe: 31 asPrimitive .     #ValueAsPointerTo
  define: '+'                     toBe: 240 asPrimitive .     #FloatAdd
  define: '-'                     toBe: 241 asPrimitive .     #FloatSubtract
  define: 'subtractedFrom:'       toBe: 242 asPrimitive .     #FloatSubtractFrom
  define: '*'                     toBe: 243 asPrimitive .     #FloatMultiply
  define: '/'                     toBe: 244 asPrimitive .     #FloatDivide
  define: 'dividedInto:'          toBe: 245 asPrimitive .     #FloatDivideInto
  define: '<'                     toBe: 246 asPrimitive .     #FloatLessThan
  define: '<='                    toBe: 247 asPrimitive .     #FloatLessThanOrEqual
  define: '='                     toBe: 248 asPrimitive .     #FloatEqual

;

?g
