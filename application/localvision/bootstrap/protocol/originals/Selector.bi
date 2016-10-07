"
####################
####  Selector  ####
####################
"
?g

##############################################################################
#	Two primitive messages to 'Selector's are defined during system
#	bootstrap: 'print' and 'locateOrAddInDictionaryOf'.
#	They are repeated here for consistency.
##############################################################################

####################
#  Print and Other Standard Messages
####################

Prototypical Selector
;

####################
#  Primitives and Converses
####################

Prototypical Selector
  define: 'asString'                    toBe:  65 asPrimitive .   #SelectorToString
  define: 'locateOrAddInDictionaryOf:'  toBe: 111 asPrimitive .   #LocateOrAddEntryInDictionaryOf
  define: 'locateInDictionaryOf:'       toBe: 112 asPrimitive .   #LocateEntryInDictionaryOf
  define: 'deleteFromDictionaryOf:'     toBe: 113 asPrimitive .   #DeleteFromDictionaryOf
  define: 'print'                       toBe: 124 asPrimitive .   #PrintSelector

  defineMethod: [ | new | ^self ] .
;

Prototypical Selector defineMethod: [ | asSelector | ^self ] ;

?g
