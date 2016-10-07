#---  Core classes: Dictionary and Entity only

Dictionary getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: Dictionary.
setFunctionTypeTo: "Conversion".
;

Dictionary getMessage: "at:".
setSelectorTo: "at: string" .
setDescriptionTo: "Returns the object stored at the supplied string.".
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: String ;

Dictionary getMessage: "at:put:".
setSelectorTo: "at: string put: object" .
setDescriptionTo: 
"Stores the supplied object at the supplied string in the recipient."  .
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: String  .
setParameter: 2 typeTo: Object ;

Dictionary getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level
information is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema";

Dictionary getMessage: "defaultInstance".
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: Dictionary.
setFunctionTypeTo: "Schema" .
;

Dictionary getMessage: "delete:".
setSelectorTo: "delete: string" .
setDescriptionTo: 
"Removes string entry from recipient dictionary.".
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: String ;

Dictionary getMessage: "initialize".
setDescriptionTo: 
"Initializes a new instance of the recipient dictionary.  Run as part of
the instance creation process." .
setFunctionTypeTo: "Creation".
setLevelTo: "Advanced" . 
setReturnObjectTypeTo: Dictionary ; 
;

Dictionary getMessage: "initializeGlobalSubclassProperties".
setDescriptionTo: 
"Initializes a new subclass of the recipient dictionary.  Run as part of
the subclass creation process." .
setFunctionTypeTo: "Creation".
setLevelTo: "Advanced" .
setReturnObjectTypeTo: Dictionary ; 
;

Dictionary getMessage: "initializeLocalAttributes".
setDescriptionTo: 
"Initializes the new instance of the recipient dictionary.  Run as part of
the instance creation process." .
setFunctionTypeTo: "Creation".
setLevelTo: "Advanced" . 
setReturnObjectTypeTo: Dictionary ; 
;

Dictionary getMessage: "isDictionary".
setDescriptionTo: "Returns a TRUE value when sent to any Dictionary".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry";

Dictionary getMessage: "locate:".
setSelectorTo: "locate: string" .
setDescriptionTo: 
"Finds object associated with the supplied string in dictionary, searching 
inheritance path if needed.".
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: String;

Dictionary getMessage: "new".
setDescriptionTo: 
"Special version of 'new' message designed to address the peculiar 
requirements of dictionary instance creation." .
setReturnObjectTypeTo: Dictionary.
setFunctionTypeTo: "Creation";

Dictionary getMessage: "newAs:".
setSelectorTo: "newAs: string" .
setDescriptionTo: 
"Special version of 'newAs:' message designed to address the peculiar 
requirements of dictionary instance creation." .
setReturnObjectTypeTo: Dictionary.
setFunctionTypeTo: "Creation";

Dictionary getMessage: "objects".
setDescriptionTo: "Returns list of objects referenced by dictionary entries.".
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Access" ;

Dictionary getMessage: "specialized".
setDescriptionTo: 
"Special version of 'specialized' message designed to address the peculiar 
requirements of dictionary subclass creation." .
setReturnObjectTypeTo: Dictionary.
setFunctionTypeTo: "Creation";

Dictionary getMessage: "uniformAt:".
setSelectorTo: "uniformAt: string" .
setDescriptionTo:
"Returns the object stored at the supplied string.  If no object is found,
returns the object stored at the string \"Default\".  If no object is
found, returns NA." .
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: String ;

Dictionary getMessage: "whatAmI".
setDescriptionTo: "Returns the string \"Dictionary\".".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema";

#--------------------------------------------------

Entity getMessage: "accessExpression".
setLevelTo: "Advanced" . 
setDescriptionTo: "Returns a string that could be used to request recipient" . 
setFunctionTypeTo: "Format" . 
setReturnObjectTypeTo: String . 
;

Entity getMessage: "addAlias:".
setSelectorTo: "addAlias: string" .
setDescriptionTo: 
"Adds supplied string as alternate name in recipient's naming dictionary.".
setReturnObjectTypeTo: Entity.
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: String;

Entity getMessage: "aliases".
setDescriptionTo: "Returns list of aliases named for entity.".
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Descriptive" .
;

Entity getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: Entity.
setFunctionTypeTo: "Conversion".
;

Entity getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level 
information is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema";

Entity getMessage: "createInitializationListFrom:".
setSelectorTo: "createInitializationListFrom: blockList" .
setDescriptionTo: 
"Creates a new class constant for the recipient's class named
'initializationList' and copies the blocks in blockList to this
list.  The initializationList is used during the instance creation
process to initialize default values in the new instance." .
setReturnObjectTypeTo: Entity .
setFunctionTypeTo: "Schema".
setLevelTo: "Advanced" . 
setParameter: 1 typeTo: List;

Entity getMessage: "defaultInstance".
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: Entity .
setFunctionTypeTo: "Schema" .
;

Entity getMessage: "displayInfo".
setDescriptionTo: 
"Displays the code and name for the recipient followed by a new line.
This message is often redefined by the subclasses to display one line
of simple formatted information about an entity." .
setFunctionTypeTo: "Display" .
;

Entity getMessage: "displayLabel".
setDescriptionTo: "Returns the recipient's name" .
;

Entity getMessage: "displayLabelKey".
setDescriptionTo: "Returns the recipient's code" .
;

Entity getMessage: "getOrCreate:".
setSelectorTo: "getOrCreate: string" .
setDescriptionTo:
"Returns object named by string in recipient class' naming dictionary.
If not found, a new instance is created and returned." .
setFunctionTypeTo: "Access" .
setReturnObjectTypeTo: Entity .
setParameter: 1 typeTo: String .
;

Entity getMessage: "helpMessage" . 
setDescriptionTo: "Returns a message describing the recipient." . 
setFunctionTypeTo: "Descriptive" . 
setReturnObjectTypeTo: String . 
;

Entity getMessage: "initializationList" . 
setDescriptionTo: 
"Returns the list of blocks used to initialize default values in new
instances of the recipient's class." .
setFunctionTypeTo: "Schema" . 
setLevelTo: "Advanced" . 
setReturnObjectTypeTo: List . 
;

Entity getMessage: "initialize".
setDescriptionTo: 
"Initializes a new instance.  Run as part of the instance creation process." .
setFunctionTypeTo: "Creation".
setLevelTo: "Advanced" . 
setReturnObjectTypeTo: Entity ; 
;

Entity getMessage: "initializeDataBaseLinks".
setDescriptionTo: 
"Initializes a new instance.  Run as part of the instance creation process." .
setFunctionTypeTo: "Creation".
setLevelTo: "Advanced" . 
setReturnObjectTypeTo: Entity ; 
;

Entity getMessage: "initializeGlobalInstanceProperties".
setDescriptionTo: 
"Initializes a new instance.  Run as part of the instance creation process." .
setFunctionTypeTo: "Creation".
setLevelTo: "Advanced" .
setReturnObjectTypeTo: Entity ; 
;

Entity getMessage: "initializeLocalAttributes".
setDescriptionTo: 
"Initializes a new instance.  Run as part of the instance creation process." .
setFunctionTypeTo: "Creation".
setLevelTo: "Advanced" . 
setReturnObjectTypeTo: Entity ; 
;

Entity getMessage: "isEntity".
setDescriptionTo: "Returns the value TRUE when sent to any Entity.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry";

Entity getMessage: "locateId:".
setSelectorTo: "locateId: string" .
setDescriptionTo:
"Returns entity associated with id supplied, searching naming dictionaries
associated with entity class.  By default only 'named' is searched; this
message can be redefined for subclasses that can use multiple dictionaries." .
setFunctionTypeTo: "Access" .
setReturnObjectTypeTo: Entity .
setParameter: 1 typeTo: String .
;

Entity getMessage: "masterListCodes".
setDescriptionTo: 
"Displays all codes and names for all non-default instances in recipient's
class." .
setFunctionTypeTo: "Display";

Entity getMessage: "name".
setDescriptionTo: "Returns descriptive name for recipient." .
setReturnObjectTypeTo: String .
setFunctionTypeTo: "Descriptive";

Entity getMessage: "named".
setDescriptionTo: 
"Returns the naming dictionary for the recipient's class." .
setReturnObjectTypeTo: Dictionary .
setFunctionTypeTo: "Descriptive";

Entity getMessage: "newAs:".
setSelectorTo: "newAs: string" .
setDescriptionTo: 
"Special version of 'newAs:' message designed to address the additional
requirements of entity instance creation." .
setReturnObjectTypeTo: Entity .
setFunctionTypeTo: "Creation";

Entity getMessage: "numericCode".
setDescriptionTo: "Returns descriptive numeric code for recipient." .
setReturnObjectTypeTo: String .
setFunctionTypeTo: "Descriptive";

Entity getMessage: "print".
setDescriptionTo: 
"Redefines the standard print message so that the value of the recipient code
is printed.".
setReturnObjectTypeTo: Entity.
setFunctionTypeTo: "Display".
;

Entity getMessage: "print:".
setSelectorTo: "print: anInteger" .
setDescriptionTo: 
"Prints the recipient code using the supplied format.  " .
setReturnObjectTypeTo: Entity .
setFunctionTypeTo: "Display".
setParameter: 1 typeTo: Integer.
;

Entity getMessage: "setNameTo:".
setSelectorTo: "setNameTo: string" .
setDescriptionTo: 
"Updates the value of the recipient's 'name' property with string" .
setReturnObjectTypeTo: Entity .
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: String .
;

Entity getMessage: "setNumericCodeTo:".
setSelectorTo: "setNumericCodeTo: ordinal" .
setDescriptionTo: 
"Updates the value of the recipient's 'numericCode' property with string 
or number.  Used as an alternative identification scheme." .
setReturnObjectTypeTo: Entity .
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: Ordinal .
;

Entity getMessage: "setShortNameTo:".
setSelectorTo: "setShortNameTo: string" .
setDescriptionTo: 
"Updates the value of the recipient's 'shortName' property with string." .
setReturnObjectTypeTo: Entity .
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: String .
;

Entity getMessage: "setSortCodeTo:".
setSelectorTo: "setSortCodeTo: ordinal" .
setDescriptionTo: 
"Updates the value of the recipient's 'sortCode' property with string 
or number.  Used as an alternative identification scheme." .
setReturnObjectTypeTo: Entity .
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: Ordinal .
;

Entity getMessage: "shortName".
setDescriptionTo: "Returns descriptive shortName for recipient." .
setReturnObjectTypeTo: String .
setFunctionTypeTo: "Descriptive";

Entity getMessage: "sortCode".
setDescriptionTo: "Returns descriptive sortCode for recipient." .
setReturnObjectTypeTo: String .
setFunctionTypeTo: "Descriptive";

Object getMessage: "specializeAs:at:" . 
setSelectorTo: "specializeAs: string at: object" .
setDescriptionTo: 
"Redefines this message used to create a subclass named 'string' 
defined at the supplied object." .
setFunctionTypeTo: "Creation".
setParameter: 1 typeTo: String .
setParameter: 2 typeTo: Object .
;

Entity getMessage: "updateInitializationListWith:".
setSelectorTo: "updateInitializationListWith: block" .
setDescriptionTo: 
"Adds the supplied block to the recipient class' initializationList." .
setReturnObjectTypeTo: Entity .
setFunctionTypeTo: "Schema".
setLevelTo: "Advanced" . 
setParameter: 1 typeTo: Block;

Entity getMessage: "whatAmI".
setDescriptionTo: "Return the string \"Entity\".".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema";




