#--------------------
#  Includes:
#    Boolean
#       FALSE
#       TRUE
#--------------------


Boolean getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Conversion".
;

Boolean getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information is
stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

Boolean getMessage: "defaultInstance".
setDescriptionTo: "Returns the default instance for the recipient's class." .
setReturnObjectTypeTo: "Boolean" .
setFunctionTypeTo: "Schema".
;

Boolean getMessage: "isBoolean".
setDescriptionTo: "Returns a TRUE value when sent to any boolean.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

Boolean getMessage: "new" .
setDescriptionTo: 
"Invalidates the 'new' message' which is not valid for this class" .
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Creation" ;
;

Boolean getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'Boolean'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

#--------------------

FALSE getMessage: "&&" .
setSelectorTo: "&& aBoolean" .
setDescriptionTo: "Returns FALSE independent of parameter" .
setReturnObjectTypeTo: FALSE .
setFunctionTypeTo: "Inquiry" .
setParameter: 1 typeTo: Boolean .
;

FALSE getMessage: "asSelf" .
setDescriptionTo: "Returns the original object." .
setReturnObjectTypeTo: FALSE .
setFunctionTypeTo: "Conversion" .
;

FALSE getMessage: "classDescriptor" .
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information is 
stored here." .
setReturnObjectTypeTo: Schema ClassDescriptor .
setFunctionTypeTo: "Schema" .
;

FALSE getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: FALSE.
setFunctionTypeTo: "Schema" .
;

FALSE getMessage: "ifFalse:" .
setSelectorTo: "ifFalse: aBlock " .
setDescriptionTo: "Executes supplied Block." .
setFunctionTypeTo: "Evaluation" .
setParameter: 1 typeTo: Block .
;

FALSE getMessage: "ifTrue:" .
setSelectorTo: "ifTrue: aBlock" .
setDescriptionTo: "Never executes." .
setReturnObjectTypeTo: NA.
setFunctionTypeTo: "Evaluation" .
setParameter: 1 typeTo: Block .
;

FALSE getMessage: "ifTrue:ifFalse:" .
setSelectorTo: "ifTrue: aBlock ifFalse: aBlock" .
setDescriptionTo: "Executes supplied 'False' Block." .
setFunctionTypeTo: "Evaluation" .
setParameter: 1 typeTo: Block .
setParameter: 2 typeTo: Block .
;

FALSE getMessage: "ifFalse:else:".
setSelectorTo: "ifFalse: block1 else: block2 ".
setDescriptionTo: "Executes supplied FALSE block." .
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

FALSE getMessage: "ifTrue:ifFalse:else:".
setSelectorTo: "ifTrue: block1 ifFalse: block2 else: block3 ".
setDescriptionTo: "Executes supplied FALSE block." .
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
setParameter: 3 typeTo: Block.
;

FALSE getMessage: "isFALSE" .
setDescriptionTo: "Returns the string 'TRUE' " .
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

FALSE getMessage: "not".
setDescriptionTo: "Returns TRUE".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Conversion".
;

FALSE getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'FALSE'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

FALSE getMessage: "||" .
setSelectorTo: "|| aBoolean" .
setDescriptionTo: "Returns TRUE if parameter is TRUE.  Note that if
parameter is a block, it is evaluated first." .
setReturnObjectTypeTo: CoreWorkspace .
setFunctionTypeTo: "Inquiry" .
setParameter: 1 typeTo: Boolean .
;


#--------------------

TRUE getMessage: "&&".
setSelectorTo: "&& aBoolean" .
setDescriptionTo: "Returns TRUE if parameter is TRUE.  Note that if
parameter is a block, it is evaluated first." .
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: Boolean .
;

TRUE getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Conversion".
;

TRUE getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information is 
stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

TRUE getMessage: "defaultInstance" .
setDescriptionTo: "Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Schema" .
;

TRUE getMessage: "ifFalse:".
setSelectorTo: "ifFalse: aBlock" .
setDescriptionTo: "Never executes.".
setReturnObjectTypeTo: NA.
setFunctionTypeTo: "Evaluation" .
setParameter: 1 typeTo: Block.
;

TRUE getMessage: "ifTrue:".
setSelectorTo: "ifTrue: aBlock ".
setDescriptionTo: "Executes supplied Block".
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
;

TRUE getMessage: "ifTrue:ifFalse:".
setSelectorTo: "ifTrue: block1 ifFalse: block2 ".
setDescriptionTo: "Executes supplied TRUE block." .
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

TRUE getMessage: "ifTrue:else:".
setSelectorTo: "ifTrue: block1 else: block2 ".
setDescriptionTo: "Executes supplied TRUE block." .
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

TRUE getMessage: "ifTrue:ifFalse:else:".
setSelectorTo: "ifTrue: block1 ifFalse: block2 else: block3 ".
setDescriptionTo: "Executes supplied TRUE block." .
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
setParameter: 3 typeTo: Block.
;

TRUE getMessage: "isTRUE".
setDescriptionTo: "Returns the String 'TRUE' when sent to TRUE.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

TRUE getMessage: "not" .
setDescriptionTo: "Returns FALSE" .
setReturnObjectTypeTo: FALSE.
setFunctionTypeTo: "Conversion" .
;

TRUE getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'boolean'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

TRUE getMessage: "||".
setSelectorTo: "|| aBoolean" .
setDescriptionTo: "Returns TRUE independent of parameter." .
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: Boolean .
;



