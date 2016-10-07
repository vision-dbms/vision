Selector getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: Selector.
setFunctionTypeTo: "Conversion".
;

Selector getMessage: "asString".
setDescriptionTo: "Converts recipient selector into a string.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Conversion".
;

Selector getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

Selector getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: Selector.
setFunctionTypeTo: "Schema" .
;

Selector getMessage: "deleteFromDictionaryOf:".
setSelectorTo: "deleteFromDictionaryOf: anObject" .
setDescriptionTo: 
"Removes the recipient selector and the protocol it implements from the 
dictionary associatd with the supplied object.".
# setReturnObjectTypeTo: .
setFunctionTypeTo: "Messages".
setParameter: 1 typeTo: Object.
;

Selector getMessage: "isSelector".
setDescriptionTo: "Returns a TRUE value when sent to any Selector.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

Selector getMessage: "locateInDictionaryOf:".
setSelectorTo: "locateInDictionaryOf: anObject" .
setDescriptionTo: 
"Returns the intensional form of the value accessed by the recipient 
selector in the dictionary of the supplied object.  Send the 'value'
message to evaluate the object returned.".
setFunctionTypeTo: "Access".
setLevelTo: "Advanced" .
setParameter: 1 typeTo: Object.
;

Selector getMessage: "locateOrAddInDictionaryOf:".
setSelectorTo: "locateOrAddInDictionaryOf: anObject" .
setDescriptionTo: 
"Looks up recipient selector in the supplied object's dictionary and returns 
the intensional form of the value if present.  Otherwise the selector is added 
to the dictionary and the intensional form is returned.  Send the 
'value' message to intensional form to evaluate it. Intensional form allows 
immediate assignment of new value to object returned.".
setFunctionTypeTo: "Access".
setLevelTo: "Advanced" .
setParameter: 1 typeTo: Object.
;

Selector getMessage: "new" .
setDescriptionTo: "Undefines the 'new' message." .
setFunctionTypeTo:  "Creation" .
;

Selector getMessage: "print".
setDescriptionTo: 
"Redefines the standard print message to indicate that object is a selector.".
setReturnObjectTypeTo: Selector.
setFunctionTypeTo: "Display".
;

Selector getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'selector'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

