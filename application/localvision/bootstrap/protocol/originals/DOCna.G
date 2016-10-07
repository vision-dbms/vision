Undefined getMessage: "&&".
setSelectorTo: "&& aBoolean" .
setDescriptionTo: 
 "Returns FALSE if parameter evaluates to FALSE, NA otherwise." .
setReturnObjectTypeTo: NA .
setParameter: 1 typeTo: Boolean.
;

Undefined getMessage: "*".
setSelectorTo: "* aValue" .
setDescriptionTo: 
"Since no value can meaningfully be multiplied by NA, the value NA is always 
returned.".
setReturnObjectTypeTo: NA.
setParameter: 1 typeTo: Number .
;

Undefined getMessage: "+".
setSelectorTo: "+ aValue" .
setDescriptionTo: 
"Since the value of NA cannot meaningfully be added to any value, the value 
NA is always returned.".
setReturnObjectTypeTo: NA.
setParameter: 1 typeTo: Number .
;

Undefined getMessage: "-".
setSelectorTo: "- aValue" .
setDescriptionTo: 
"Since no value can meaningfully be subtracted from NA, the value NA is always 
returned.".
setReturnObjectTypeTo: NA.
setParameter: 1 typeTo: Number.
;

Undefined getMessage: "/".
setSelectorTo: "/ aValue" .
setDescriptionTo: 
"Since no value can meaningfully be divided into NA, the NA is always 
returned.".
setReturnObjectTypeTo: NA.
setParameter: 1 typeTo: Number.
;

Undefined getMessage: "<".
setSelectorTo: "< aValue" .
setDescriptionTo: 
"Since the value of NA cannot meaningfully be compared to any value, the value 
NA is always returned.".
setReturnObjectTypeTo: NA.
setParameter: 1 typeTo: Number.
;

Undefined getMessage: "<=".
setSelectorTo: "<= aValue" .
setDescriptionTo: 
"Since the value of NA cannot meaningfully be compared toany value, the value 
NA is always returned.".
setReturnObjectTypeTo: NA.
setParameter: 1 typeTo: Number.
;

Undefined getMessage: ">".
setSelectorTo: "> aValue" .
setDescriptionTo: 
"Since the value of NA cannot meaningfully be compared to any value, the value 
NA is always returned.".
setReturnObjectTypeTo: NA.
setParameter: 1 typeTo: Number.
;

Undefined getMessage: ">=".
setSelectorTo: ">= aValue" .
setDescriptionTo: 
"Since the value of NA cannot meaningfully be compared to any value, the value 
NA is always returned.".
setReturnObjectTypeTo: NA.
setParameter: 1 typeTo: Number.
;

Undefined getMessage: "absoluteValue".
setDescriptionTo: "Returns an NA value.".
setReturnObjectTypeTo: NA.
;

Undefined getMessage: "asAPowerOf:".
setSelectorTo: "asAPowerOf: aValue" .
setDescriptionTo: 
"Since no value can meaningfully be raised to the NA power, the value NA is 
always returned.".
setReturnObjectTypeTo: NA.
setParameter: 1 typeTo: Number.
;

Undefined getMessage: "asDate".
setDescriptionTo: "Returns the value NA.".
setReturnObjectTypeTo: NA.
;

Undefined getMessage: "asDouble".
setDescriptionTo: "Returns the value NA".
setReturnObjectTypeTo: NA.
;

Undefined getMessage: "asFloat".
setDescriptionTo: "Returns the value NA".
setReturnObjectTypeTo: NA.
;

Undefined getMessage: "asInteger".
setDescriptionTo: "Returns the value NA".
setReturnObjectTypeTo: NA.
setFunctionTypeTo: "Conversion".
;

Undefined getMessage: "asNumber".
setDescriptionTo: "Returns the value NA".
setReturnObjectTypeTo: Undefined.
;

Undefined getMessage: "asPointerTo:".
setLevelTo: "Internal" .
;

Undefined getMessage: "asSelf".
setDescriptionTo: "Returns the value NA".
setReturnObjectTypeTo: NA .
;

Undefined getMessage: "basicAsDate".
setDescriptionTo: "Returns the value NA".
;

Undefined getMessage: "between:and:".
setSelectorTo: "between: lowerBoundary and: upperBoundary" .
setDescriptionTo: "Returns the value NA".
;

Undefined getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
;

Undefined getMessage: "contains:".
setDescriptionTo: "Returns the value NA".
setReturnObjectTypeTo: NA .
;

Undefined getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: Undefined.
;

Undefined getMessage: "dividedInto:".
setDescriptionTo: "Returns the value NA".
setReturnObjectTypeTo: NA .
setLevelTo: "Internal" .
;

Object getMessage: "else:". 
setSelectorTo: "else: object" .
setDescriptionTo: "When sent to NA, evalutes supplied object." .
setFunctionTypeTo: "Evaluation" .
setParameter: 1 typeTo: Object .
;

Object getMessage: "elseIf:then:". 
setSelectorTo: "elseIf: boolean then: object" .
setDescriptionTo: 
 "Evalutes supplied object if boolean is TRUE, otherwise returns NA." .
setFunctionTypeTo: "Evaluation" .
setParameter: 1 typeTo: Boolean .
setParameter: 2 typeTo: Object .
;

Undefined getMessage: "exp".
setDescriptionTo: "Returns the value NA".
setReturnObjectTypeTo: NA.
;

Undefined getMessage: "isNA".
setDescriptionTo: "Always returns TRUE if recipient is an NA.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

Undefined getMessage: "isDefault".
setDescriptionTo: "Always returns TRUE if recipient is an NA.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

Undefined getMessage: "isUndefined".
setDescriptionTo: "Returns a TRUE value when sent to any Undefined.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

Undefined getMessage: "isntNA".
setDescriptionTo: "Always returns FALSE if recipient is an NA.".
setReturnObjectTypeTo: FALSE.
setFunctionTypeTo: "Inquiry".
;

Undefined getMessage: "log".
setDescriptionTo: "Returns an NA value.".
setReturnObjectTypeTo: NA.
;

Undefined getMessage: "log:".
setDescriptionTo: "Returns an NA value.".
setReturnObjectTypeTo: NA.
;

Undefined getMessage: "matchesString:".
setDescriptionTo: "Returns an NA value.".
setReturnObjectTypeTo: NA .
;

Undefined getMessage: "max:".
setDescriptionTo: "Returns an NA value.".
setReturnObjectTypeTo: NA.
;

Undefined getMessage: "min:".
setDescriptionTo: "Returns an NA value.".
setReturnObjectTypeTo: NA.
;

Undefined getMessage: "mod:".
setDescriptionTo: "Returns an NA value.".
setReturnObjectTypeTo: NA.
;

Undefined getMessage: "negated".
setDescriptionTo: "Returns an NA value.".
setReturnObjectTypeTo: NA.
;

Undefined getMessage: "new" .
setDescriptionTo: "Undefines the 'new' message" .
setReturnObjectTypeTo: Undefined.
;

Undefined getMessage: "not".
setDescriptionTo: "Returns an NA value.".
setReturnObjectTypeTo: NA.
;

Undefined getMessage: "notBetween:and:".
setDescriptionTo: "Returns an NA value.".
setReturnObjectTypeTo: NA .
;

Undefined getMessage: "numberOfDigits".
setDescriptionTo: "Returns an NA value.".
setReturnObjectTypeTo: NA.
;

Undefined getMessage: "pctChangeTo:".
setDescriptionTo: "Returns an NA value.".
;

Undefined getMessage: "print".
setDescriptionTo: 
"Redefines the standard print message to print the string 'NA'.".
setReturnObjectTypeTo: Undefined.
;

Undefined getMessage: "print:".
setSelectorTo: "print: aNumber" .
setDescriptionTo: 
"Prints the string 'NA' formatted to the number of characters provided in 
supplied format.  NA is always right-justified in a field followed by one blank.".
setReturnObjectTypeTo: Undefined.
setParameter: 1 typeTo: Number.
;

Undefined getMessage: "round".
setDescriptionTo: "Returns an NA value.".
setReturnObjectTypeTo: NA.
;

Undefined getMessage: "round:".
setDescriptionTo: "Returns an NA value.".
setReturnObjectTypeTo: NA.
;

Undefined getMessage: "sprint:".
setSelectorTo: "sprint: aNumber" .
setDescriptionTo: 
"Returns the value NA as a string formatted to the number of characters 
specified in supplied format.".
setReturnObjectTypeTo: String.
setParameter: 1 typeTo: Number.
;

Undefined getMessage: "sqrt".
setDescriptionTo: "Returns an NA value.".
setReturnObjectTypeTo: NA.
;

Undefined getMessage: "subtractedFrom:".
setDescriptionTo: "Returns an NA value.".
setReturnObjectTypeTo: NA .
setLevelTo: "Internal" .
;

Undefined getMessage: "toThe:".
setDescriptionTo: "Returns an NA value.".
setReturnObjectTypeTo: NA.
;

Undefined getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'undefined'.".
setReturnObjectTypeTo: String.
;

Undefined getMessage: "within:percentOf:".
setDescriptionTo: "Returns an NA value.".
setReturnObjectTypeTo: NA.
;

Undefined getMessage: "||".
setSelectorTo: "|| aBoolean" .
setDescriptionTo: 
  "If the supplied boolean evaluates to TRUE, then TRUE, NA otherwise.".
setReturnObjectTypeTo: Boolean.
setParameter: 1 typeTo: Boolean.
;



