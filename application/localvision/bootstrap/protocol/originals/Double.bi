"
##################
####  Double  ####
##################
"
?g

####################
#  printing and standard messages
####################

####################
#  Primitives and Converses
####################

Prototypical Double

  define: 'print'                toBe: 121 asPrimitive .      #PrintCurrentAsPointer
  define: 'printOf:'             toBe: 134 asPrimitive .      #DoublePrintOf
  define: 'printWithCommasOf:'   toBe: 137 asPrimitive .      #DoublePrintWithCommasOf
  define: 'sprintOf:'            toBe: 138 asPrimitive .      #DoubleSPrintOf
  define: 'sprintWithCommasOf:'  toBe: 276 asPrimitive .      #DoubleSPrintWithCommasOf
  define: 'canonicalizedForSort' toBe: 11 asPrimitive .      #ReturnCurrent
  define: 'orderUsingDictionary:' toBe: 11 asPrimitive .      # 'asDouble'
  define: 'asNumber'              toBe: 11 asPrimitive .      #Return Current
  define: 'asDouble'              toBe: 11 asPrimitive .      #ReturnCurrent
  define: 'asFloat'               toBe: 210 asPrimitive .      #DoubleAsFloat
  define: 'asInteger'             toBe: 211 asPrimitive .      #DoubleAsInteger
  define: 'asPointerTo:'          toBe: 31 asPrimitive .      #ValueAsPointerTo
  define: '+'                     toBe: 260 asPrimitive .      #DoubleAdd
  define: '-'                     toBe: 261 asPrimitive .      #DoubleSubtract
  define: 'subtractedFrom:'       toBe: 262 asPrimitive .      #DoubleSubtractFrom
  define: '*'                     toBe: 263 asPrimitive .      #DoubleMultiply
  define: '/'                     toBe: 264 asPrimitive .      #DoubleDivide
  define: 'dividedInto:'          toBe: 265 asPrimitive .      #DoubleDivideInto
  define: 'exp'                   toBe: 269 asPrimitive .      #DoubleExp
  define: 'log'                   toBe: 270 asPrimitive .      #DoubleLog
  define: 'sqrt'                  toBe: 272 asPrimitive .      #DoubleSqrt
  define: 'toThe:'                toBe: 271 asPrimitive .      #DoublePow
  define: 'asAPowerOf:'           toBe: 273 asPrimitive .      #DoubleWop
  define: '<'                     toBe: 266 asPrimitive .      #DoubleLessThan
  define: '<='                    toBe: 267 asPrimitive .      #DoubleLessThanOrEqual
  define: '='                     toBe: 268 asPrimitive .      #DoubleEqual
  define: 'sine'                  toBe: 230 asPrimitive .
  define: 'cosine'                toBe: 231 asPrimitive .
  define: 'tangent'               toBe: 232 asPrimitive .
  define: 'arcSine'               toBe: 233 asPrimitive .
  define: 'arcCosine'             toBe: 234 asPrimitive .
  define: 'arcTangent'            toBe: 235 asPrimitive .
  define: 'hyperbolicSine'        toBe: 236 asPrimitive .
  define: 'hyperbolicCosine'      toBe: 237 asPrimitive .
  define: 'hyperbolicTangent'     toBe: 238 asPrimitive . 

;

?g


