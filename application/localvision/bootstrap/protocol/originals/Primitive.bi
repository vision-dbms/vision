"
#####################
####  Primitive  ####
#####################
"
?g

####################
#  Print and Other Standard Messages
####################
Prototypical Primitive
;

####################
#  Primitives and Converses
####################
Prototypical Primitive
  define: 'print'   toBe: 126 asPrimitive .     #PrintPrimitive
  define: 'sprint'  toBe: 277 asPrimitive .     #SPrintPrimitive

define: 'asPointerTo:'  toBe: 31 asPrimitive.

defineMethod:
[ | index |
    ^self asPointerTo: ^global Integer. mod: 65536 
].

defineMethod:
[ | controlValue |
    (^self asPointerTo: ^global Integer. / 65536) asInteger
].
;

?g
