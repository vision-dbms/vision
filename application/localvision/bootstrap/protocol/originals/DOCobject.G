Object getMessage: "!=".
setSelectorTo: "!= anObject" .
setDescriptionTo: 
"Determines whether recipient object is not identical to supplied object.  
At Object, the messages != and !== perform identical functions.  By convention,
the !== message is not redefined by subclasses. The != message is often 
redefined by subclasses to test on value equality instead of object identity.
For example, != has been redefined at the numeric classes so that the 
expression 3 != 3.0 produces a result of FALSE.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Object.
;

Object getMessage: "!==".
setSelectorTo: "!== anObject" .
setDescriptionTo: 
"Determines whether recipient object is not identical to supplied object.  By 
convention, this message is usually not redefined at subclasses (see != ) so 
that a version of object identity testing is always available.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Object.
;

Object getMessage: ",".
setSelectorTo: ", anObject" .
setDescriptionTo: 
"Creates a List consisting of recipient object, followed by supplied object.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: Object.
;

Object getMessage: "->".
setSelectorTo: "-> variable" .
setDescriptionTo: 
"Assigns recipient object to supplied variable.".
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: FixedProperty .
;

Object getMessage: "<".
setSelectorTo: "< anObject" .
setDescriptionTo: 
"Returns TRUE if the recipient value is less than the supplied object." .
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Object .
;

Object getMessage: "<=".
setSelectorTo: "<= anObject" .
setDescriptionTo: 
"Returns TRUE if the recipient value is less or equal to the supplied object." .
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Object .
;

Object getMessage: "=".
setSelectorTo: "= anObject" .
setDescriptionTo: 
"Determines whether recipient object is identical to supplied object.  At 
Object, the messages = and == perform identical functions.  By convention, the
== message is not redefined by subclasses. The = message is often redefined by
subclasses to test on value equality instead of object identity.  For example,
= has been redefined at the numeric classes so that the expression 3 = 3.0 
produces a result of TRUE.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Object.
;

Object getMessage: "==".
setSelectorTo: "== anObject" .
setDescriptionTo: 
"Determines whether recipient object is identical to supplied object.  By 
convention, this message is usually not redefined at subclasses (see = ) so 
that a version of object identity testing is always available.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Object.
;

Object getMessage: ">".
setSelectorTo: "> anObject" .
setDescriptionTo: 
"Returns TRUE if the recipient value is greater than the supplied object." .
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Object .
;

Object getMessage: ">=".
setSelectorTo: ">= anObject" .
setDescriptionTo: 
"Returns TRUE if the recipient value is greater or equal to the supplied 
object." .
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Object .
;

Object getMessage: "DBUpdateAppendDump:".
setSelectorTo: "DBUpdateAppendDump: filename" .
setLevelTo: "Internal" .
setParameter: 1 typeTo: NA .
setFunctionTypeTo: "Primitive" .
;

Object getMessage: "DBUpdateReplaceDump:".
setSelectorTo: "DBUpdateReplaceDump: filename" .
setLevelTo: "Internal" .
setParameter: 1 typeTo: NA .
setFunctionTypeTo: "Primitive" .
;

Object getMessage: "FALSE".
setDescriptionTo: 
"This global constant returns the Boolean value FALSE.".
setReturnObjectTypeTo: FALSE .
setFunctionTypeTo: "Constant".
;

Object getMessage: "NA".
setDescriptionTo: 
"This global constant returns the undefined value NA.".
setReturnObjectTypeTo: Undefined .
setFunctionTypeTo: "Constant".
;

Object getMessage: "TRUE".
setDescriptionTo: 
"This global constant returns the Boolean value TRUE.".
setReturnObjectTypeTo: TRUE .
setFunctionTypeTo: "Constant".
;

Object getMessage: "__define:" . 
setSelectorTo: "__define: aSelector" .
setDescriptionTo: "Primitive form of 'define:' ." . 
setFunctionTypeTo: "Messages" . 
setLevelTo: "Internal" .
setParameter: 1 typeTo: Selector . 
;

Object getMessage: "__define:toBe:" . 
setSelectorTo: "__define: aSelector toBe: anObject" .
setDescriptionTo: "Primitive form of 'define:toBe:' ." . 
setFunctionTypeTo: "Messages" . 
setLevelTo: "Internal" .
setParameter: 1 typeTo: Selector . 
setParameter: 2 typeTo: Object .
;

Object getMessage: "__defineFixedProperty:" . 
setSelectorTo: "__defineFixedProperty: aSelector" .
setDescriptionTo: "Primitive form of 'defineFixedProperty:' . " . 
setFunctionTypeTo: "Messages" . 
setLevelTo: "Internal" .
setParameter: 1 typeTo: Selector . 
;

Object getMessage: "__defineMethod:" . 
setSelectorTo: "__defineMethod: aBlock" .
setDescriptionTo: "Primitive form of 'defineMethod:' ." . 
setFunctionTypeTo: "Messages" . 
setLevelTo: "Internal" .
setParameter: 1 typeTo: Block . 
;

Object getMessage: "__defineProperty:" . 
setSelectorTo: "__defineProperty: aSelector" .
setDescriptionTo: "Primitive form of 'defineProperty:' ." .
setFunctionTypeTo: "Messages" . 
setLevelTo: "Internal" .
setParameter: 1 typeTo: Selector . 
;

Object getMessage: "accessExpression" . 
setLevelTo: "Advanced" . 
setDescriptionTo: "Returns a string that could be used to request recipient" . 
setFunctionTypeTo: "Format" . 
setReturnObjectTypeTo: String . 
;

Object getMessage: "activeList".
setDescriptionTo: "Returns list of non-deleted instances in class.".
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Schema".
;

Object getMessage: "adjacentInstancesFor:".
setSelectorTo: "adjacentInstancesFor: anInteger" .
setLevelTo: "Internal" . 
setReturnObjectTypeTo: List . 
setFunctionTypeTo: "Primitive" .
setParameter: 1 typeTo: Integer . 
;

Object getMessage: "alignStore".
setLevelTo: "Advanced" .
setDescriptionTo: "Aligns the store associated with the recipient object." .
setFunctionTypeTo: "DBAdmin" .
;

Object getMessage: "asBaseObject" . 
setDescriptionTo: "Returns value of recipient's baseObject" . 
setFunctionTypeTo: "Conversion" . 
;

Object getMessage: "asClassDescriptor" . 
setDescriptionTo: 
"Returns the class descriptor instance in Schema ClassDescriptor for the
recipient except when the recipient is a class descriptor in which case
the recipient is returned stripped of any extensions.
" .
setFunctionTypeTo: "Conversion" . 
;

Object getMessage: "asList".
setDescriptionTo: 
"Converts recipient Object into a List of 1 element.  Once converted, all List
messages can be used to manipulate the Object.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Conversion".
;

Object getMessage: "asLocalContextFor:".
setSelectorTo: "asLocalContextFor: aBlock" .
setLevelTo: "Advanced" .
setDescriptionTo: "Evaluates the supplied block in the context of ^local" .
setFunctionTypeTo: "Evaluation" .
;

Object getMessage: "asSelf".
setDescriptionTo: "Returns the original object, stripped of extensions.".
setReturnObjectTypeTo: Object.
setFunctionTypeTo: "Conversion".
;

Object getMessage: "asString".
setDescriptionTo: "Converts recipient object into a string.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Conversion".
;

Object getMessage: "baseObject".
setDescriptionTo: 
"Returns the 'child' object associated with the recipient. In most cases, 
'baseObject' will be the recipient.  If the recipient is a row in the super 
class hierarchy of an object, 'baseObject' returns the most detailed level.  
If the recipient has extensions, 'baseObject' returns the object without 
extensions." .
setFunctionTypeTo: "Descriptive" .
;

Object getMessage: "basicDelete".
setLevelTo: "Advanced" .
setDescriptionTo: "Primitive version of object deletion message." .
setFunctionTypeTo: "DBAdmin" .
;

Object getMessage: "basicDo:" . 
setSelectorTo: "basicDo: aBlock" .
setDescriptionTo: "Primitive version of 'do:' message." . 
setFunctionTypeTo: "Evaluation" . 
setParameter: 1 typeTo: Block . 
;

Object getMessage: "basicExtend:".
setSelectorTo: "basicExtend: aBlock" .
setDescriptionTo: 
"Uses supplied block to create an extension of recipient object.  Object and 
object basicExtend: [ ] are not identical.  At Object, the messages 
'basicExtend:' and 'extendBy:' perform identical functions.  By convention, the
'basicExtend:' message is not redefined by subclasses.".
setReturnObjectTypeTo: Object.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

Object getMessage: "basicExtend:with:" . 
setSelectorTo: "basicExtend: aBlock with: aValue" .
setDescriptionTo: 
"Evaluates the supplied block in the context of the recipient object,
where the supplied block specifies a set of new variables to which the
returned object will respond.  The supplied block is assumed to require
a single parameter which is evaluated using the supplied 'aValue' " .
setReturnObjectTypeTo: Object.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block .
setParameter: 2 typeTo: Object .
;

Object getMessage: "basicInstanceList".
setDescriptionTo: "Primitive version of instanceList message" .
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Schema" .
;

Object getMessage: "basicNew".
setDescriptionTo: 
"Creates a new instance of the recipient object's class.  Not valid for 'value'
classes like number and date where the set of instances is predefined and not 
alterable.  At Object, the messages 'basicNew' and 'new' perform identical 
functions.  By convention, the basicNew message is not redefined by subclasses." .
setFunctionTypeTo: "Creation" .
setLevelTo: "Advanced" .
;

Object getMessage: "basicNewPrototype".
setLevelTo: "Advanced" .
setDescriptionTo: 
"Creates a new store for the recipient object's class that shares all protocol
with the original class but stores instances separately. At Object, the 
messages 'basicNewPrototype' and 'newPrototype' perform identical functions.  
By convention, the basicNewPrototype message is not redefined by subclasses." .
setFunctionTypeTo: "Creation".
;

Object getMessage: "basicSend:".
setSelectorTo: "basicSend: aBlock" .
setDescriptionTo: 
"Evaluates all statements inside supplied block in the context of the recipient
object.  Identical to 'do:' message except 'do:' returns recipient object and 
'basicSend:' returns the result of evaluating the block.  At Object, the 
messages 'basicSend:' and 'send:' perform identical functions.  By convention, 
the 'basicSend:' message is not redefined by subclasses.".
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
;

Object getMessage: "basicSend:with:".
setSelectorTo: "basicSend: aBlock with: aValue " .
setDescriptionTo: "Evaluates block with supplied value" .
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block .
setParameter: 2 typeTo: Object .
;

Object getMessage: "basicSendCanonicalizedForSort:".
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive" .
setParameter: 1 typeTo: NA .
;

Object getMessage: "basicSpecializeClass" . 
setLevelTo: "Advanced" . 
setDescriptionTo: "Primitive version of 'specializeClass'" . 
setFunctionTypeTo: "Creation" . 
;

Object getMessage: "basicSpecialized".
setDescriptionTo: 
"Creates a specialization of recipient object's class.  The new store contains 
an initial default row, but no special initialization is done.  By convention,
the basicSpecialized message is not redefined by subclasses." .
setFunctionTypeTo: "Creation".
setLevelTo: "Advanced" .
;

Object getMessage: "canonicalizedForSort".
setLevelTo: "Internal" .
;

Object getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

Object getMessage: "cleanDictionary" . 
setLevelTo: "Advanced" . 
setDescriptionTo: "Flags old selector structures for garbage collection." . 
setFunctionTypeTo: "DBAdmin" . 
;

Object getMessage: "cleanStore" . 
setLevelTo: "Advanced" . 
setDescriptionTo: "Flags old property structures for garbage collection." . 
setFunctionTypeTo: "DBAdmin" . 
;

Object getMessage: "cleanStoreAndDictionary" . 
setLevelTo: "Advanced" . 
setDescriptionTo: "Runs 'cleanStore' and 'cleanDictionary'." . 
setFunctionTypeTo: "DBAdmin" . 
;

Object getMessage: "cleanupClassStructures" . 
setLevelTo: "Advanced" . 
setDescriptionTo: 
"Runs 'cleanStoreAndDictionary' at Object, this method is redefined as
needed by subclasses to perform general cleanup functions." .
setFunctionTypeTo: "DBAdmin" . 
;

Object getMessage: "cleanupDeletedObject" . 
setLevelTo: "Advanced" . 
setDescriptionTo: 
"This method is executed by the 'flagForDeletionWithReason:' message and
should not be accessed directly. " .
setFunctionTypeTo: "DBAdmin" . 
;

Object getMessage: "cleanupLocalAttributes" . 
setLevelTo: "Advanced" . 
setDescriptionTo: 
"This method is executed by the 'flagForDeletionWithReason:' message and
is redefined as needed by subclasses." .
setFunctionTypeTo: "DBAdmin" . 
;

Object getMessage: "cleanupLocalNames" . 
setLevelTo: "Advanced" . 
setDescriptionTo: 
"This method is executed by the 'flagForDeletionWithReason:' message and
is redefined as needed by subclasses." .
setFunctionTypeTo: "DBAdmin" . 
;

Object getMessage: "closureOf:" . 
setLevelTo: "Internal" . 
setParameter: 1 typeTo: Block.
setFunctionTypeTo: "Primitive" .
;

Object getMessage: "clusterNew".
setLevelTo: "Advanced" .
setDescriptionTo: 
"Creates a new Object, storing it in the same cluster as the recipient for 
access efficiency.".
setFunctionTypeTo: "Creation".
;

Object getMessage: "code".
setDescriptionTo: "Returns i.d. for object." .
setReturnObjectTypeTo: String .
setFunctionTypeTo: "Descriptive" .
;

Object getMessage: "collect:".
setSelectorTo: "collect: aBlock" .
setDescriptionTo: 
"Uses supplied block to create an extension of recipient Object.  The value of 
the block is assigned to the variable 'value'.  The returned extension 
therefore responds to the value message.".
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

Object getMessage: "containerIndex".
setLevelTo: "Advanced" .
setDescriptionTo: "Returns the container index number for recipient." .
setFunctionTypeTo: "DBAdmin" .
;

Object getMessage: "convertFrom:".
setSelectorTo: "convertFrom: string" .
setDescriptionTo: 
"This message converts the supplied string to an instance of the recipient's
class if applicable, returning NA otherwise.  The definition at Object
will lookup the string in the class' naming dictionary, if defined.
The version at String returns the recipient stripped of any extensions.
The versions at Number, Integer, Double, and Float convert the recipient
to a numeric value if possible.  The version at Date converts the recipient
to a Date using the 'asDate' message defined at String.  The version at
Boolean returns TRUE if the recipient contains an upper/lower case
combination of 'true' and FALSE if the recipient contains an upper/lower
case combination of 'false'.  The version at block returns a block 
containing the string as a message.  This message is called by the
'as:' message at String." .
setFunctionTypeTo: "Conversion".
setParameter: 1 typeTo: String .
;

Object getMessage: "createIncorporatorClass:".
setSelectorTo: "createIncorporatorClass: aString" .
setDescriptionTo: 
"Creates a new subclass in a form that is suitable for use by the 
incorporator." .
setFunctionTypeTo: "Creation".
setParameter: 1 typeTo: String .
;

Object getMessage: "createInstance".
setDescriptionTo: "Creates new instance of recipient's class." .
setFunctionTypeTo: "Creation".
;

Object getMessage: "createInstance:".
setSelectorTo: "createInstance: aString" .
setDescriptionTo: "Creates new named instance of recipient's class." .
setFunctionTypeTo: "Creation".
setParameter: 1 typeTo: String .
;

Object getMessage: "createSubclass".
setDescriptionTo: "Create subclass of recipient object." .
setFunctionTypeTo: "Creation".
;

Object getMessage: "createSubclass:".
setSelectorTo: "createSubclass: aString" .
setDescriptionTo: "Creates new named subclass of recipient object." .
setFunctionTypeTo: "Creation".
setParameter: 1 typeTo: String .
;

Object getMessage: "createSubclass:at:".
setSelectorTo: "createSubclass: aString at: anObject " .
setDescriptionTo: 
"Creates new named subclass of recipient object at anObject." .
setFunctionTypeTo: "Creation".
setParameter: 1 typeTo: String .
setParameter: 2 typeTo: Object .
;

Object getMessage: "creationDate".
setDescriptionTo: "Date object was created.".
setReturnObjectTypeTo: Date .
setFunctionTypeTo: "Descriptive".
;

Object getMessage: "defaultFlag" . 
setDescriptionTo: "Returns TRUE if recipient is default instance in class." . 
setFunctionTypeTo: "Descriptive" . 
setReturnObjectTypeTo: Boolean . 
;

Object getMessage: "define:".
setSelectorTo: "define: aSelector" .
setDescriptionTo: 
"Defines a Time Varying Property for the recipient object's class.  Only 
meaningful for classes that define meaningful instances.".
setFunctionTypeTo: "Messages".
setParameter: 1 typeTo: Selector .
;

Object getMessage: "define:toBe:".
setSelectorTo: "define: aSelector toBe: anObject" .
setDescriptionTo: 
"Defines a constant value to be returned by any instance in the recipient 
object's class.".
setFunctionTypeTo: "Messages".
setParameter: 1 typeTo: Selector .
setParameter: 2 typeTo: Object .
;

Object getMessage: "defineAsMessage:".
setSelectorTo: "defineAsMessage: aSelector" .
setDescriptionTo: 
"Defines the message denoted by supplied selector to return an extensionless 
version of the recipient object.".
setFunctionTypeTo: "Messages".
setParameter: 1 typeTo: Selector .
;

Object getMessage: "defineFixedProperty:".
setSelectorTo: "defineFixedProperty: aSelector" .
setDescriptionTo: 
"Defines a static (non-time varying) property for the recipient object's 
class.  Only meaningful for classes that define meaningful instances.".
setFunctionTypeTo: "Messages".
setParameter: 1 typeTo: Selector .
;

Object getMessage: "defineFixedProperty:withDefault:" . 
setSelectorTo: "defineFixedProperty: aSelector withDefault: anObject" .
setDescriptionTo: 
"Defines the fixed property and sets the default value to supplied object.
The default value is ignored if the recipient's class is not in the
Schema database." .
setFunctionTypeTo: "Messages" . 
setParameter: 1 typeTo: Selector . 
setParameter: 2 typeTo: Object . 
;

Object getMessage: "define:withDefault:" . 
setSelectorTo: "define: aSelector withDefault: anObject" .
setDescriptionTo: 
"Defines the time series property and sets the default value to supplied 
object.  The default value is ignored if the recipient's class is not in the
Schema database." .
setFunctionTypeTo: "Messages" . 
setParameter: 1 typeTo: Selector . 
setParameter: 2 typeTo: Object . 
;

Object getMessage: "defineInAList:".
setLevelTo: "Internal" .
setParameter: 1 typeTo: List .
setFunctionTypeTo: "Primitive" .
;

Object getMessage: "defineMethod:".
setSelectorTo: "defineMethod: aBlock" .
setDescriptionTo: 
"Defines the procedure provided by supplied method at the class of the 
recipient object.  The general form for the block provided to define a method 
is:
	[ | messagename |
	    statement1 ;
	    statement2 ;
	    . . .
	]
for methods that require no parameters and
	[ | keyword1: parameter1 keyword2: parameter2 ... |
	    statement1 ;
	    statement2 ;
     	    . . .
	] 
for methods having one or more parameters.".
setFunctionTypeTo: "Messages".
setParameter: 1 typeTo: Block .
;

Object getMessage: "defineProperty:".
setSelectorTo: "defineProperty: aSelector" .
setDescriptionTo: 
"Defines a Time Varying Property for the recipient object's class.  Only 
meaningful for classes that define meaningful instances.".
setFunctionTypeTo: "Messages".
setParameter: 1 typeTo: Selector .
;

Object getMessage: "delete".
setLevelTo: "Advanced" .
setDescriptionTo: "Deletes recipient object." .
setFunctionTypeTo: "DBAdmin" .
;

Object getMessage: "deleteColumnFromStore:".
setLevelTo: "Internal" .
setParameter: 1 typeTo: List .
setFunctionTypeTo: "Primitive" .
;

Object getMessage: "deleteDictionaryEntryFor:".
setSelectorTo: "deleteDictionaryEntryFor: aSelector" .
setDescriptionTo: 
"Deletes the message from the dictionary of the recipient object.".
setFunctionTypeTo: "Messages".
setParameter: 1 typeTo: Selector .
;

Object getMessage: "deleteFromAList:".
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive" .
setParameter: 1 typeTo: List .
;

Object getMessage: "deleteMessage:".
setSelectorTo: "deleteMessage: aString" .
setDescriptionTo: 
"Deletes the message from the dictionary of the recipient object and
modifies the Schema database." .
setFunctionTypeTo: "Messages".
setParameter: 1 typeTo: String .
;

Object getMessage: "deletionDate" . 
setDescriptionTo: 
"Returns the date recipient was flagged for deletion or NA if the
recipient is active." .
setFunctionTypeTo: "Descriptive" . 
setReturnObjectTypeTo: Date . 
;

Object getMessage: "deletionFlag".
setDescriptionTo: "Returns TRUE if recipient has been flagged for deletion." .
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Descriptive" ;

Object getMessage: "deletionReason" . 
setDescriptionTo:
"Returns the reason the recipient was flagged for deletion." .
setFunctionTypeTo: "Descriptive" . 
setReturnObjectTypeTo: String . 
;

Object getMessage: "displayInfo".
setDescriptionTo: 
"Displays a single line summary of the object terminated with a new line." .
setFunctionTypeTo: "Display" .
;

Object getMessage: "displayInheritance".
setDescriptionTo: 
"Displays the inheritance hierarchy starting at the recipient object." .
setFunctionTypeTo: "Schema" .
;

Object getMessage: "displayLabel".
setDescriptionTo: 
"Default label for display, primarily in UIMS applications." .
setReturnObjectTypeTo: String .
setFunctionTypeTo: "Display" .
;

Object getMessage: "displayLabelKey".
setDescriptionTo: 
"Default id for display, primarily in UIMS applications." .
setReturnObjectTypeTo: String .
setFunctionTypeTo: "Display" .
;

Object getMessage: "displayMessages".
setDescriptionTo: 
"Displays a list of all messages to which the recipient object responds.
Only protocol explicitly added or redefined by the recipient is displayed." .
setFunctionTypeTo: "Messages".
;

Object getMessage: "displayMessagesAll".
setDescriptionTo: 
"Displays a list of all messages to which the recipient object responds,
walking the inheritance hierarchy.  Output also shows type of message and
actual level in the hierarchy that the message was defined at.  Messages that
are redefined at various points in the hierarchy will appear once for each
explicit definition made." .
setFunctionTypeTo: "Messages".
;

Object getMessage: "displayMessagesX".
setDescriptionTo: 
"Displays a list of all messages to which the recipient object responds,
walking the inheritance hierarchy.  Output also shows type of message and
actual level in the hierarchy that the message was defined at.  Messages that
are redefined at various points in the hierarchy will appear once for each
explicit definition made.  Messages defined at Object are excluded." .
setFunctionTypeTo: "Messages".
;

Object getMessage: "displayPOP".
setLevelTo: "Advanced" .
setDescriptionTo: 
"Displays the container index and object space of the recipient object." .
setFunctionTypeTo: "DBAdmin" .
;

Object getMessage: "displayProperties".
setDescriptionTo: 
"Displays all the properties defined at the recipient object's class.
Only meaningful for classes that define meaningful instances." .
setFunctionTypeTo: "Messages".
;

Object getMessage: "displayTimeSeries".
setDescriptionTo: "Displays all the time series properties defined at the
recipient object's class." .
setFunctionTypeTo: "Messages".
;

Object getMessage: "do:".
setSelectorTo: "do: aBlock" .
setDescriptionTo: 
"Evaluates all statements inside supplied block in the context of the
recipient object.  Identical to 'send:' message except 'send:'
returns the result of evaluating the block and 'do:' returns the 
recipient object.".
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
;

Object getMessage: "earliestPossibleDate".
setDescriptionTo: "Returns earliest possible date, January 1, 0001.".
setReturnObjectTypeTo: Date .
setFunctionTypeTo: "Constant" .
;

Object getMessage: "else:". 
setSelectorTo: "else: object" .
setDescriptionTo: 
 "When sent to NA, evalutes supplied object, otherwise returns ^self.
The expression:
    variable1 else: variable2 . else: variable3
can be used select the first non-na value encountered.  If the
parameters are supplied as blocks, they are not evaluated unless
all the prior parameters have produced NA values.  
".
setFunctionTypeTo: "Evaluation" .
setParameter: 1 typeTo: Object .
;

Object getMessage: "elseIf:then:". 
setSelectorTo: "elseIf: boolean then: object" .
setDescriptionTo: 
 "When sent to NA, evaluates supplied object if boolean is TRUE, otherwise
 returns ^self.  For example:
   v isNumber
     ifTrue: [ list at: v asInteger ] .
     elseIf: v isString
       then: [ dictionary at: v ] .
     elseIf: list isList
       then: [ list at: 1 ] .
     else: [ \"Unknown object type\" ]
" .
setFunctionTypeTo: "Evaluation" .
setParameter: 1 typeTo: Boolean .
setParameter: 2 typeTo: Object .
;

Object getMessage: "if:then:else:". 
setSelectorTo: "if: conditionBlock then: yesBlock else: noBlock" .
setDescriptionTo: 
 "Supplied condition block is sent to recipient.  If it evaluates to
TRUE, the 'yesBlock' is sent to the recipient; otherwise, the 'noBlock'
is sent." .
setFunctionTypeTo: "Evaluation" .
setParameter: 1 typeTo: Block .
setParameter: 2 typeTo: Block .
setParameter: 3 typeTo: Block .
;
Object getMessage: "if:then:". 
setSelectorTo: "if: conditionBlock then: yesBlock" .
setDescriptionTo: 
 "Supplied condition block is sent to recipient.  If it evaluates to
TRUE, the 'yesBlock' is sent to the recipient; otherwise, NA is returned." .
setFunctionTypeTo: "Evaluation" .
setParameter: 1 typeTo: Block .
setParameter: 2 typeTo: Block .
;

Object getMessage: "establishResidenceInSpaceOf:".
setLevelTo: "Advanced" .
setDescriptionTo: 
"Defines the object space in which to locate the recipient transient object
as the object space of the supplied object." .
setFunctionTypeTo: "Creation".
;
Object getMessage: "provideResidenceFor:".
setLevelTo: "Advanced" .
setDescriptionTo: 
"Defines the object space in which to locate the supplied transient cluster
as the object space of the recipient object." .
setFunctionTypeTo: "Creation".
;

Object getMessage: "execute:in:". 
setSelectorTo: "execute: message in: class" .
setDescriptionTo: "Used By DDMS ToolKit" .
setLevelTo: "Internal" .
setFunctionTypeTo: "Evaluation" .
setParameter: 1 typeTo: String .
setParameter: 2 typeTo: Object .
;

Object getMessage: "extend:".
setSelectorTo: "extend: aBlock" .
setDescriptionTo: 
"Evaluates the supplied block in the context of the recipient object,
where the supplied block specifies a set of new variables to which the
returned object will respond." .
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

Object getMessage: "extend:with:".
setSelectorTo: "extend: aBlock with: aValue" .
setDescriptionTo: 
"Evaluates the supplied block in the context of the recipient object,
where the supplied block specifies a set of new variables to which the
returned object will respond.  The supplied 'aValue' is passed to the
block as a parameter" .
setFunctionTypeTo: "Extension".
setLevelTo: "Advanced" .
setParameter: 1 typeTo: Block .
setParameter: 2 typeTo: Object .
;

Object getMessage: "extendBy:".
setSelectorTo: "extendBy: aBlock" .
setDescriptionTo: 
"Uses supplied block to create an extension of recipient object.  Object and 
object extendBy: [ ] are not identical.  At Object, the messages extend:, 
basicExtend: and extendBy: perform identical functions." . 
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

Object getMessage: "extendedBy:asLocalContextFor:".
setSelectorTo: "extendedBy: block1 asLocalContextFor: block2" .
setLevelTo: "Advanced" .
setDescriptionTo: 
"Extends ^local by block1 and evaluates block2 in its context." .
setFunctionTypeTo: "Evaluation" .
;

DateRange getMessage: "extract:forDateRange:".
setSelectorTo: "extract: block forDateRange: drange".
setDescriptionTo: 
"This cover method extracts data for each date in the supplied range for the
recipient object using the 'extract:for:' message at DateRange." .
setReturnObjectTypeTo: TimeSeries .
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Object .
;

Object getMessage: "flagForDeletion" . 
setDescriptionTo: "Flags recipient for deletion." . 
setFunctionTypeTo: "DBAdmin" . 
;

Object getMessage: "flagForDeletionWithReason:".
setSelectorTo: "flagForDeletionWithReason: aString" .
setDescriptionTo: 
"Flags recipient object for deletion.  The supplied string is used to set
the deletionReason property." .
setFunctionTypeTo: "DBAdmin".
setParameter: 1 typeTo: String .
;

Object getMessage: "getClusteredString:" .
setDescriptionTo: "returns the supplied string from the recipient's
primary string cluster.  If the string is not in the cluster, it
is added to the cluster and returned." .
setFunctionTypeTo: "DBAdmin".
setParameter: 1 typeTo: String .
;

Object getMessage: "getInitialScreeningUniverse" . 
setDescriptionTo: 
"Returns list of objects related to recipient that represent an initial
universe for screening purposes.  This is usually the 'activeList', but
can be redefined as needed by subclasses." .
setFunctionTypeTo: "Schema" . 
setReturnObjectTypeTo: List . 
;

Object getMessage: "getMessage:".
setSelectorTo: "getMessage: aString" .
setDescriptionTo: 
"This method returns the message implementation descriptor associated with
the recipient's class and the message string provided.  This method
will search the recipient's super class hierarchy until it encounters
the first implementation of this message.  If no implementation is
found, NA is returned." .
setReturnObjectTypeTo: Schema MessageImplementationDescriptor .
setFunctionTypeTo: "Messages".
setParameter: 1 typeTo: String .
;

Object getMessage: "getMessageNamesContaining:".
setSelectorTo: "getMessageNamesContaining: aString" .
setDescriptionTo: 
"Returns list of message implementation descriptors containing the 
supplied string as part of their message name.  Search is restricted
to messages defined at classes in the recipient's inheritance path." .
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Messages".
setParameter: 1 typeTo: String .
;

Object getMessage: "getMessages".
setDescriptionTo: 
"Returns list of message implementation descriptors associated with the
messages defined for recipient's class." .
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Messages".
;

Object getMessage: "getMessagesDefining:".
setSelectorTo: "getMessageNamesDefining: aString" .
setDescriptionTo: 
"Returns list of message implementation descriptors defining the 
supplied message string.  Search is restricted to messages defined
at classes in the recipient's inheritance path." .
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Messages".
setParameter: 1 typeTo: String .
;

Object getMessage: "getMessagesInInheritancePath".
setDescriptionTo: 
"Returns list of message implementation descriptors for all messages
defined at classes in the recipient's inheritance path." .
setFunctionTypeTo: "Messages".
setReturnObjectTypeTo: List .
;

Object getMessage: "getMessagesUsing:" . 
setSelectorTo: "getMessagesUsing: aString" .
setDescriptionTo: 
"This method returns a list of message implementation descriptors whose 
method definitions contain the supplied string. All messages defined in 
the recipient's inheritance path are considered (i.e., super classes and 
subclasses). The returned list is sorted by message, then class descriptor
within message.  The parameter can utilize the standard Unix regular 
expression syntax for partial matches." .
setFunctionTypeTo: "Messages" . 
setReturnObjectTypeTo: List . 
setParameter: 1 typeTo: String .
;

Object getMessage: "getMessagesX".
setDescriptionTo: 
"Returns list of message implementation descriptors for all messages
defined at classes in the recipient's super class hierarchy, excluding
messages defined at Object." .
setFunctionTypeTo: "Messages".
setReturnObjectTypeTo: List .
;

Object getMessage: "getPotentialChoicesForString:".
setSelectorTo: "getPotentialChoiceForString: aString" .
setDescriptionTo: 
"Returns list of objects that could be implied by the supplied string." .
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: String .
;

Object getMessage: "hasExtensions".
setDescriptionTo: 
"Returns TRUE if recipient object has extensions, FALSE otherwise." .
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "hasSchema" . 
setDescriptionTo: 
"This method returns TRUE if the recipient's class is part of the Schema 
database, FALSE otherwise." .
setFunctionTypeTo: "Inquiry" . 
setReturnObjectTypeTo: Boolean . 
;

Object getMessage: "helpMessage" . 
setDescriptionTo: "Returns a message describing the recipient." . 
setFunctionTypeTo: "Descriptive" . 
setReturnObjectTypeTo: String . 
;

Object getMessage: "ifDefault:". 
setSelectorTo: "ifDefault: object" .
setDescriptionTo: 
"If the recipient is the default object, this message returns the parameter,
otherwise the recipient is returned.  If the parameter is a block, it is
evaluated." .
setFunctionTypeTo: "Evaluation" .
setParameter: 1 typeTo: Object .
;

Object getMessage: "ifDefault:else:". 
setSelectorTo: "ifDefault: block1 else: block2" .
setDescriptionTo: 
"If the recipient is the default object, block1 is sent to it; otherwise,
block2 is sent to it.  This can be used to eliminate some conditional
testing.  For example:
 
    date ifDefault: [ \"initial\" ] else: [ formatUsingShortName ] 

replaces:

    date isDefault
       ifTrue: [ \"initial\" ] ifFalse: [ date formatUsingShortName ] 
" .
setFunctionTypeTo: "Evaluation" .
setParameter: 1 typeTo: Block .
setParameter: 2 typeTo: Block .
;

Object getMessage: "isntDefault:else:". 
setSelectorTo: "isntDefault: block1 else: block2" .
setDescriptionTo: 
"If the recipient is not the default object, block1 is sent to it; otherwise,
block2 is sent to it.  This can be used to eliminate some conditional
testing.  " .
setFunctionTypeTo: "Evaluation" .
setParameter: 1 typeTo: Block .
setParameter: 2 typeTo: Block .
;

Object getMessage: "ifFalse:else:".
setSelectorTo: "ifFalse: block1 else: block2 ".
setDescriptionTo: "Executes supplied 'else' block by default." .
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

Object getMessage: "ifTrue:else:".
setSelectorTo: "ifTrue: block1 else: block2 ".
setDescriptionTo: "Executes supplied 'else' block by default." .
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

TRUE getMessage: "ifTrue:ifFalse:else:".
setSelectorTo: "ifTrue: block1 ifFalse: block2 else: block3 ".
setDescriptionTo: "Executes supplied 'else' block by default." .
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
setParameter: 3 typeTo: Block.
;

Object getMessage: "inheritsFrom:".
setSelectorTo: "inheritsFrom: anObject" .
setDescriptionTo: 
"Returns TRUE if recipient inherits from supplied object." .
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Schema".
setParameter: 1 typeTo: Object ;

Object getMessage: "initializeDefaults" . 
setLevelTo: "Advanced" . 
setDescriptionTo: 
"This method is run as part of the 'createInstance' method and is used
to initialize any properties that have default values." .
setFunctionTypeTo: "Creation" . 
;

Object getMessage: "initializeGlobalInstanceProperties" . 
setLevelTo: "Advanced" . 
setDescriptionTo: 
"This method is run as part of the 'new' method and is used to initialize
properties in the new instance." .
setFunctionTypeTo: "Creation" . 
;

Object getMessage: "initializeGlobalSubclassProperties" . 
setLevelTo: "Advanced" . 
setDescriptionTo: 
"This method is run as part of the 'specialized' method and is used to 
initialize properties in the default row of the new class." .
setFunctionTypeTo: "Creation" . 
;

Object getMessage: "initializeKeys:".
setSelectorTo: "initializeKeys: aString" .
setLevelTo: "Advanced" . 
setDescriptionTo: 
"This method is run as part of the 'createInstance:' process.  A single
string representing the code of the new object is supplied.  Many classes
redefine this message to require a list of keys as a parameter." .
setFunctionTypeTo: "Creation".
setParameter: 1 typeTo: String .
;

Object getMessage: "initializeLocalAttributes".
setLevelTo: "Advanced" . 
setDescriptionTo: 
"This method is run as part of the 'createInstance:' process." .
setFunctionTypeTo: "Creation" .
;

Object getMessage: "initializeNames".
setLevelTo: "Advanced" . 
setDescriptionTo: 
"This method is run as part of the 'createInstance:' process." .
setFunctionTypeTo: "Creation" .
;

Object getMessage: "initializeNamesFor:".
setLevelTo: "Advanced" . 
setSelectorTo: "initializeNamesFor: stringList" .
setDescriptionTo: 
"This method is run as part of the 'createInstance:' process." .
setFunctionTypeTo: "Creation" .
setParameter: 1 typeTo: List .
;

Object getMessage: "inputExpression" . 
setLevelTo: "Advanced" . 
setDescriptionTo: 
"Returns a quoted string that could be used to identify the recipient" . 
setFunctionTypeTo: "Format" . 
setReturnObjectTypeTo: String . 
;

Object getMessage: "instanceList".
setDescriptionTo: 
"Returns the set of instances in the recipient object's class.  Only 
meaningful for classes that define meaningful instances.".
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Schema".
;

Object getMessage: "isActive" . 
setDescriptionTo: 
"Returns TRUE if the recipient has not been flagged for deletion." . 
setFunctionTypeTo: "Inquiry" . 
setReturnObjectTypeTo: Boolean . 
;

Object getMessage: "isApplicationCatalog" . 
setDescriptionTo: "Returns the value TRUE when sent to any ApplicationCatalog.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry" .
;

Object getMessage: "isBlock".
setDescriptionTo: "Returns the value TRUE when sent to any block.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isBoolean".
setDescriptionTo: "Returns the value TRUE when sent to any boolean.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isBusinessDays".
setDescriptionTo: "Returns the value TRUE when sent to any BusinessDays.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isCatalog" . 
setDescriptionTo: "Returns the value TRUE when sent to any Catalog.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry" .
;

Object getMessage: "isChain".
setDescriptionTo: "Returns the value TRUE when sent to any Chain.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isCharacter".
setDescriptionTo: "Returns the value TRUE when sent to any Character.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isClassDescriptor".
setDescriptionTo: "Returns the value TRUE when sent to any ClassDescriptor.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isClassType".
setDescriptionTo: "Returns the value TRUE when sent to any ClassType.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isCollection".
setDescriptionTo: "Returns the value TRUE when sent to any Collection.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isComputedFunction".
setDescriptionTo: "Returns the value TRUE when sent to any ComputedFunction.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isDate".
setDescriptionTo: "Returns the value TRUE when sent to any Date.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isDateOffset".
setDescriptionTo: "Returns the value TRUE when sent to any DateOffset.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isDateRange".
setDescriptionTo: "Returns the value TRUE when sent to any DateRange.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isDayOfWeekDescriptor".
setDescriptionTo: "Returns the value TRUE when sent to any DayOfWeekDescriptor.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isDays".
setDescriptionTo: "Returns the value TRUE when sent to any Days.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry";

Object getMessage: "isDefault" . 
setDescriptionTo: 
"Returns the value TRUE if recipient is flagged as a default instance." .
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry" .
;

Object getMessage: "isDeleted" . 
setDescriptionTo: 
"Returns the value TRUE if recipient is flagged for deletion." .
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry" .
;

Object getMessage: "isDescriptor" . 
setDescriptionTo: "Returns the value TRUE when sent to any Descriptor.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry" .
;

Object getMessage: "isDictionary".
setDescriptionTo: "Returns the value TRUE when sent to any Dictionary.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry";

Object getMessage: "isDouble".
setDescriptionTo: "Returns the value TRUE when sent to any Double.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isEntity".
setDescriptionTo: "Returns the value TRUE when sent to any Entity.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isEntityCatalog" . 
setDescriptionTo: "Returns the value TRUE when sent to any EntityCatalog.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry" .
;

Object getMessage: "isEnumeratedFunction".
setDescriptionTo: "Returns the value TRUE when sent to any EnumeratedFunction.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isEnvironment".
setDescriptionTo: "Returns the value TRUE when recipient is an Environment.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isEquivalentTo:" . 
setSelectorTo: "isEquivalentTo: object" .
setDescriptionTo:
"Returns TRUE if the recipient object is equivalent to the supplied object.
By default, this message is the same as the '=' message." .
setFunctionTypeTo: "Comparison" . 
setReturnObjectTypeTo: Boolean . 
setParameter: 1 typeTo: Object . 
;

Object getMessage: "isntEquivalentTo:" . 
setSelectorTo: "isntEquivalentTo: object" .
setDescriptionTo:
"Returns TRUE if the recipient object is not equivalent to the supplied 
object." .
setFunctionTypeTo: "Comparison" . 
setReturnObjectTypeTo: Boolean . 
setParameter: 1 typeTo: Object . 
;

Object getMessage: "isFALSE" . 
setDescriptionTo: "Returns the value TRUE when sent a FALSE recipient.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry" .
;

Object getMessage: "isFixedProperty" . 
setDescriptionTo: "Returns the value TRUE when sent to any FixedProperty.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry" .
;

Object getMessage: "isFloat".
setDescriptionTo: "Returns the value TRUE when sent to any Float.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isFunction".
setDescriptionTo: "Returns the value TRUE when sent to any Function.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isFunctionType".
setDescriptionTo: "Returns the value TRUE when sent to any FunctionType.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isIncorporatorPrototype".
setDescriptionTo: "Returns the value TRUE when sent to any IncorporatorPrototype.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isIndexedCollection".
setDescriptionTo: "Returns the value TRUE when sent to any IndexedCollection.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isIndexedList".
setDescriptionTo: "Returns the value TRUE when sent to any IndexedList.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isInteger".
setDescriptionTo: "Returns the value TRUE when sent to any Integer.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isInternalDefault" . 
setDescriptionTo: 
"Returns the value TRUE if recipient is a pointer to an instance that has
been deleted." .
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry" .
setLevelTo: "Advanced" .
;

Object getMessage: "isLink".
setDescriptionTo: "Returns the value TRUE when recipient is a Link object.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isList".
setDescriptionTo: "Returns the value TRUE when sent to any List.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isMaintenanceTools" . 
setDescriptionTo: "Returns the value TRUE when sent to any MaintenanceTools.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry" .
;

Object getMessage: "isMessageDescriptor".
setDescriptionTo: "Returns the value TRUE when sent to any MessageDescriptor.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isMessageImplementationDescriptor".
setDescriptionTo: 
"Returns the value TRUE when sent to any MessageImplementationDescriptor.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isMessageType".
setDescriptionTo: 
"Returns the value TRUE when sent to any MessageType.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isMethod".
setDescriptionTo: "Returns the value TRUE when sent to any Method.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isMonthBeginnings".
setDescriptionTo: "Returns the value TRUE when sent to any MonthBeginnings.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isMonthDescriptor".
setDescriptionTo: "Returns the value TRUE when sent to any MonthDescriptor.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isMonthEnds".
setDescriptionTo: "Returns the value TRUE when sent to any MonthEnds.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isMonths".
setDescriptionTo: "Returns the value TRUE when sent to any Months.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isNA".
setDescriptionTo: "Flags whether recipient object is an NA value.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isNoValue".
setDescriptionTo: "Returns TRUE if recipient is a NoValue NA value.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isTrue".
setDescriptionTo: "Returns False unless object is the Boolean value TRUE." .
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isFalse".
setDescriptionTo: "Returns False unless object is the Boolean value False." .
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isNamed" . 
setDescriptionTo: 
"Returns the value TRUE when recipient is a Named dictionary.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry" .
;

Object getMessage: "isNumber".
setDescriptionTo: "Returns the value TRUE when sent to any Number.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isObject".
setDescriptionTo: "Returns the value TRUE when sent to any Object.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isObjectSpace".
setDescriptionTo: "Returns the value TRUE when sent to any object space.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isOrdinal".
setDescriptionTo: "Returns the value TRUE when sent to any Ordinal.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isPrimitive".
setDescriptionTo: "Returns the value TRUE when sent to any Primitive.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isPrinterTools".
setDescriptionTo: "Returns the value TRUE when sent to any PrinterTools.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isProperty".
setDescriptionTo: "Returns the value TRUE when sent to any Property.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isQuarterBeginnings".
setDescriptionTo: "Returns the value TRUE when sent to any QuarterBeginnings.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isQuarterEnds".
setDescriptionTo: "Returns the value TRUE when sent to any QuarterEnds.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isQuarters".
setDescriptionTo: "Returns the value TRUE when sent to any Quarters.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isSchema" . 
setDescriptionTo: 
"Returns the value TRUE when recipient a Schema object." .
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry" .
;

Object getMessage: "isSelector".
setDescriptionTo: "Returns the value TRUE when sent to any Selector.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isSequencedCollection".
setDescriptionTo: "Returns the value TRUE when sent to any SequencedCollection.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isSpecification".
setDescriptionTo: "Returns the value TRUE when sent to any Specification.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isString".
setDescriptionTo: "Returns the value TRUE when sent to any String.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isSuperClassOf:" . 
setSelectorTo: "isSuperClassOf: class" .
setDescriptionTo:
"Returns TRUE if recipient is a super class of supplied class." .
setFunctionTypeTo: "Inquiry" . 
setReturnObjectTypeTo: Boolean . 
setParameter: 1 typeTo: Object . 
;

Object getMessage: "isSystemDictionary" . 
setDescriptionTo: 
"Returns the value TRUE when recipient a SystemDictionary." .
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry" .
;

Object getMessage: "isSystemTable".
setDescriptionTo: "Returns the value TRUE when sent to any SystemTable".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isTRUE" . 
setDescriptionTo: 
"Returns the value TRUE when recipient is TRUE." .
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry" .
;

Object getMessage: "isTimeSeries".
setDescriptionTo: "Returns the value TRUE when sent to any TimeSeries.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isTimeSeriesProperty".
setDescriptionTo: "Returns the value TRUE when sent to any TimeSeriesProperty.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isToolKit".
setDescriptionTo: "Returns the value TRUE when sent to any ToolKit.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isUndefined".
setDescriptionTo: "Returns the value TRUE when sent to any Undefined.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isUnixSeconds" . 
setDescriptionTo: 
"Returns the value TRUE when recipient is UnixSeconds." .
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry" .
;

Object getMessage: "isUserEnvironment" . 
setDescriptionTo: 
"Returns the value TRUE when recipient is UserEnvironment." .
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry" .
;

Object getMessage: "isUtility" . 
setDescriptionTo: 
"Returns the value TRUE when recipient is Utility." .
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry" .
;

Object getMessage: "isValue".
setDescriptionTo: "Returns the value TRUE when sent to any Value.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isYearBeginnings".
setDescriptionTo: "Returns the value TRUE when sent to any YearBeginnings.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isYearEnds".
setDescriptionTo: "Returns the value TRUE when sent to any YearEnds.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isYears".
setDescriptionTo: "Returns the value TRUE when sent to any Years.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isntActive" . 
setDescriptionTo: 
"Returns TRUE when recipient has been flagged for deletion." .
setFunctionTypeTo: "Inquiry" . 
setReturnObjectTypeTo: Boolean . 
;

Object getMessage: "isntDefault".
setDescriptionTo: 
"Returns TRUE if the recipient is not a default instance of its class." .
setReturnObjectTypeTo: Boolean . 
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "isntDeleted" . 
setDescriptionTo: 
"Returns TRUE when recipient has not been flagged for deletion." .
setFunctionTypeTo: "Inquiry" . 
setReturnObjectTypeTo: Boolean . 
;

Object getMessage: "isntNA".
setDescriptionTo: "Flags whether recipient object is not an NA value.".
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Object getMessage: "locateDictionaryEntryFor:".
setSelectorTo: "locateDictionaryEntryFor: aSelector" .
setDescriptionTo: 
"Looks up the supplied selector in the recipient object's dictionary and 
returns the intensional form of the value if present.  Send 'value' messag
to intensional form to evaluate it.".
setLevelTo: "Advanced" .
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: Selector .
;

Object getMessage: "locateInAList:".
setLevelTo: "Internal" .
setParameter: 1 typeTo: List.
setFunctionTypeTo: "Primitive" .
;

Object getMessage: "locateOrAddDictionaryEntryFor:".
setSelectorTo: "locateOrAddDictionaryEntryFor: aSelector" .
setDescriptionTo: 
"Looks up the supplied selector in the recipient object's dictionary and 
returns the intensional form of the value if present.  Send value message 
to intensional form to evaluate it.".
setFunctionTypeTo: "Access".
setLevelTo: "Advanced" .
setParameter: 1 typeTo: Selector .
;

Object getMessage: "masterList".
setDescriptionTo: 
"Returns the non-default, base object instances associated with a class. " .
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Schema".
;

Object getMessage: "new".
setDescriptionTo: 
"Creates a new instance of the recipient object's class.  Not valid for 'value' 
classes like number and date where the set of instances is predefined and 
not-alterable." .
setFunctionTypeTo: "Creation";
;

Object getMessage: "newAs:" . 
setSelectorTo: "newAs: aString" .
setDescriptionTo: 
"Create a new names instance in recipient object's class. " .
setFunctionTypeTo: "Creation" . 
setParameter: 1 typeTo: String . 
;

Object getMessage: "newLine".
setDescriptionTo: 
"This global constant returns a string containing the new line character.".
setReturnObjectTypeTo: String .
setFunctionTypeTo: "Constant".
;

Object getMessage: "newPage".
setDescriptionTo: 
"This global constant returns a string containing the form feed character.".
setReturnObjectTypeTo: String .
setFunctionTypeTo: "Constant".
;

Object getMessage: "newPrototype".
setLevelTo: "Advanced" .
setDescriptionTo: 
"Creates a new store for the recipient object's class that shares all
protocol with the original class but stores instances separately.
At Object, the messages basicNewPrototype and newPrototype perform identical
functions.  By convention, the basicNewPrototype message is not redefined by
subclasses.".
setFunctionTypeTo: "Creation".
;

Object getMessage: "objectSpace".
setLevelTo: "Advanced" .
setDescriptionTo: "Returns object space in which the recipient is stored." .
setReturnObjectTypeTo: Integer .
setFunctionTypeTo: "DBAdmin" .
;

Object getMessage: "orderUsingDictionary:".
setLevelTo: "Internal" .
setParameter: 1 typeTo: NA .
setFunctionTypeTo: "Primitive" .
;

Object getMessage: "pointedToBy:".
setSelectorTo: "pointedToBy: aValue" .
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive" .
setParameter: 1 typeTo: Object .
;

Object getMessage: "profile".
setDescriptionTo: 
"Displays a multi-line description of the recipient object." .
setFunctionTypeTo: "Display" .
;

Object getMessage: "print".
setDescriptionTo: 
"Prints the recipient object using a default format.  This message is
typically redefined at the class level for class-specific printing style.".
setFunctionTypeTo: "Display".
;

Object getMessage: "print:".
setSelectorTo: "print: aFormat" .
setDescriptionTo: 
"Prints the recipient object using the supplied format.  By default,
classes ignore the format and send the unary print message.  Classes
that can sensibly use the format will redefine the print: message." .
setFunctionTypeTo: "Display".
setParameter: 1 typeTo: Number .
;

Object getMessage: "printNL".
setDescriptionTo: "Prints the recipient object followed by a new line.".
setFunctionTypeTo: "Display".
;

Object getMessage: "printNL:".
setSelectorTo: "printNL: aFormat" .
setDescriptionTo: 
"Prints the recipient using the supplied format followed by a new line.".
setFunctionTypeTo: "Display".
setParameter: 1 typeTo: Number .
;

Object getMessage: "printOf:".
setLevelTo: "Advanced" .
setDescriptionTo: 
"Uses recipient object as a mask for printing.  This method is called by
the print: messages to control the formatting and is redefined for any
class that offers a format mask such as Integer and Double." .
setFunctionTypeTo: "Display".
;

Object getMessage: "printPRN".
setDescriptionTo: 
"Prints the recipient object using 'prn' format.  This message is identical
to print for most classes.  Classes like string redefine this message to
to conform with 'prn' format requirements.".
setFunctionTypeTo: "Display" .
;

Object getMessage: "printPRN:".
setSelectorTo: "printPRN: aFormat" .
setDescriptionTo: 
"Prints the recipient object using 'prn' format.  This message is identical
to print for most classes.  Classes like string redefine this message to
conform with 'prn' format requirements.".
setFunctionTypeTo: "Display".
setParameter: 1 typeTo: Number .
;

Object getMessage: "printUniversal:" . 
setSelectorTo: " printUniversal: aFormat" .
setDescriptionTo: "General purpose cover for 'print:' message." .
setFunctionTypeTo: "Display" . 
setParameter: 1 typeTo: Number .
;

Object getMessage: "printWithCommas:".
setSelectorTo: "printWithCommas: aFormat" .
setDescriptionTo: 
"Prints the recipient object using supplied format, inserting commas where
appropriate.  This message is identical to print: for most classes.  Classes
like number redefine this message.".
setFunctionTypeTo: "Display".
setParameter: 1 typeTo: Number .
;

Object getMessage: "printWithCommasNL:".
setSelectorTo: "printWithCommasNL: aFormat" .
setDescriptionTo: 
"Prints the recipient object using supplied format, inserting commas where
appropriate, followed by a new line.  This message is identical to printNL: 
for most classes.  Classes like number redefine this message.".
setFunctionTypeTo: "Display".
setParameter: 1 typeTo: Number .
;

Object getMessage: "rcleanDictionary" . 
setLevelTo: "Advanced" . 
setDescriptionTo: 
"Recursively runs 'cleanDictionary' for super class hierarchy." .
setFunctionTypeTo: "DBAdmin" . 
;

Object getMessage: "rcleanStore" . 
setLevelTo: "Advanced" . 
setDescriptionTo: 
"Recursively runs 'cleanStore' for super class hierarchy." .
setFunctionTypeTo: "DBAdmin" . 
;

Object getMessage: "rcleanStoreAndDictionary" . 
setLevelTo: "Advanced" . 
setDescriptionTo: 
"Recursively runs 'cleanStoreAndDictionary' for super class hierarchy." .
setFunctionTypeTo: "DBAdmin" . 
;

Object getMessage: "rdelete".
setLevelTo: "Advanced" .
setDescriptionTo: 
"Recursively deletes recipient object and its related instances in its
super class hierarchy." .
setFunctionTypeTo: "DBAdmin" .
;

Object getMessage: "referencedBy:".
setSelectorTo: "referencedBy: anInteger" .
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive" .
setParameter: 1 typeTo: Integer .
;

Object getMessage: "respondsTo:".
setSelectorTo: "respondsTo: message" .
setDescriptionTo: 
"Returns boolean value indicating whether recipient responds to supplied
message.  For example:
     3.0 respondsTo: \"absoluteValue\" . printNL ;
     3.0 respondsTo: \"mod:\" . printNL ;
     3.0 respondsTo: \"+\" . printNL ;
     3.0 respondsTo: \"within:percentOf:\" . printNL ;
" .
setFunctionTypeTo: "Inquiry" .
setReturnObjectTypeTo: Boolean .
setParameter: 1 typeTo: String .
;

Object getMessage: "selectorList".
setDescriptionTo: 
"Returns a list of all selectors (messages) to which the recipient object
responds.".
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Messages".
;

Object getMessage: "send:".
setSelectorTo: "send: aBlock" .
setDescriptionTo: 
"Evaluates all statements inside supplied block in the context of the
recipient object.  Identical to 'do:' message except 'do:' returns
recipient object and 'send:' returns the result of evaluating the block.  At
Object, the messages 'basicSend:' and 'send:' perform identical functions." .
setFunctionTypeTo: "Evaluation" .
setParameter: 1 typeTo: Block .
;

Object getMessage: "send:with:".
setSelectorTo: "send: aBlock with: aValue" .
setDescriptionTo: 
"Evaluates all statements inside supplied block in the context of the
recipient object, passing supplied value as a parameter.  " .
setFunctionTypeTo: "Evaluation" .
setLevelTo: "Advanced" .
setParameter: 1 typeTo: Block .
setParameter: 2 typeTo: Object .
;

Object getMessage: "send:with:and:" . 
setSelectorTo: "send: aBlock with: object1 and: object2" .
setLevelTo: "Advanced" . 
setDescriptionTo: "Evaluates block with two supplied parameters." .
setFunctionTypeTo: "Evaluation" . 
setParameter: 1 typeTo: Block .
setParameter: 2 typeTo: Object .
setParameter: 3 typeTo: Object .
;

Object getMessage: "send:with:and:and:" . 
setSelectorTo: "send: aBlock with: object1 and: object2 and: object3" .
setLevelTo: "Advanced" . 
setDescriptionTo: "Evaluates block with three supplied parameters." .
setFunctionTypeTo: "Evaluation" . 
setParameter: 1 typeTo: Block .
setParameter: 2 typeTo: Object .
setParameter: 3 typeTo: Object .
setParameter: 4 typeTo: Object .
;

Object getMessage: "setProperty:to:".
setSelectorTo: "setProperty: p to: object" .
setDescriptionTo: 
"This message is used to update a property in the recipient with the
supplied object.  The supplied property 'p' can be a string or a
block containing the intensional form of the property to update.
The block form is more efficient especially if this message is invoked
from within a Collection operation.  If 'p' refers to a fixed property,
its value is set to the supplied object.  If 'p' refers to a time series
property, the property is updated as of the current evaluation date 
(i.e., ^date).  This message returns the recipient object so you can
stream multiple set messages to the same object.  For example:

     object
        setProperty: [ :property1 ] to: object1 .
        setProperty: [ :property2 ] to: object2 .
     ;" .
setFunctionTypeTo: "Update" .
setParameter: 1 typeTo: Block .
setParameter: 2 typeTo: Object .
;

Object getMessage: "changeProperty:to:".
setSelectorTo: "changeProperty: p to: object" .
setDescriptionTo: 
"This message is used to update a property in the recipient with the
supplied object if the value has changed.  The supplied property 'p'
can be a string or a
block containing the intensional form of the property to update.
The block form is more efficient especially if this message is invoked
from within a Collection operation.  
This message returns the recipient object so you can
stream multiple set messages to the same object.  " .
setFunctionTypeTo: "Update" .
setParameter: 1 typeTo: Block .
setParameter: 2 typeTo: Object .
;

Object getMessage: "subclassList".
setDescriptionTo: 
"Returns list containing the default instance of each subclass of recipient.
For example:
     Entity subclassList
     do: [ whatAmI print: 30 ; masterList count printNL ] ;
" .
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Schema".
;

Object getMessage: "updateTS:with:".
setSelectorTo: "updateTS: ts with: object" .
setDescriptionTo: 
"This message is used to update a time series property in the recipient 
with the supplied object as of the current evaluation date, if the
value is new.  The supplied property 'p' can be a string or a
block containing the intensional form of the property to update.
The block form is more efficient especially if this message is invoked
from within a Collection operation.  ".
setFunctionTypeTo: "Update" .
setParameter: 1 typeTo: Block .
setParameter: 2 typeTo: Object .
;

Object getMessage: "showInheritance".
setDescriptionTo: 
"This method displays the inheritance hierarchy for the recipient." .
setFunctionTypeTo: "Schema" .
;

Object getMessage: "showMessageNamesContaining:".
setSelectorTo: "showMessageNamesContaining: aString" .
setDescriptionTo: 
"This method displays a report using the results of executing the
'getMessageNamesContaining:' message." .
setFunctionTypeTo: "Messages".
setParameter: 1 typeTo: String .
;

Object getMessage: "displayMessageNamesContaining:".
setSelectorTo: "displayMessageNamesContaining: aString" .
setDescriptionTo: 
"This message displays all messages in the recipient's class that
contain the supplied string in their name.  Note that this message
excludes super classes of the recipient but includes messages that
have been defined during the current session." .
setFunctionTypeTo: "Messages".
setParameter: 1 typeTo: String .
;

Object getMessage: "showMessageSummaryFor:".
setSelectorTo: "showMessageSummaryFor: aList" .
setDescriptionTo: 
"Produces a summary report for the list of message implementation
descriptors provided" .
setFunctionTypeTo: "Messages".
setParameter: 1 typeTo: List .
;

Object getMessage: "showMessages".
setDescriptionTo: 
"This method displays a report using the results of the 'getMessages' 
message." .
setFunctionTypeTo: "Messages".
;

Object getMessage: "showMessagesDefining:".
setSelectorTo: "showMessagesDefining: aString" .
setDescriptionTo:
"This method displays a report using the results of executing the 
'getMessagesDefining:' message." .
setFunctionTypeTo: "Messages".
setParameter: 1 typeTo: String .
;

Object getMessage: "showMessagesUsing:".
setSelectorTo: "showMessagesUsing: aString" .
setDescriptionTo: 
"This method displays a report using the results of the 'getMessageUsing:' 
message." .
setFunctionTypeTo: "Messages".
setParameter: 1 typeTo: String .
;

Object getMessage: "showMessagesX".
setDescriptionTo:
"This method displays a report using the results of the 'getMessagesX' 
message. Messages defined for the class Object are excluded from this 
report." .
setFunctionTypeTo: "Messages" .
;

Object getMessage: "showMethodsUsing:" . 
setSelectorTo: "showMethodsUsing: string" .
setDescriptionTo: 
"This method displays a report showing actual method implementations using 
the results of the 'getMessagesUsing:' message." .
setFunctionTypeTo: "Messages" . 
setParameter: 1 typeTo: String .
;

Object getMessage: "showProperties" . 
setDescriptionTo: 
"This method displays a report using the results of the 'getMessageX' 
message, restricted to message implementations whose 'type' is a property." .
setFunctionTypeTo: "Messages" . 
;

Object getMessage: "showVariables".
setDescriptionTo: 
"If the recipient contains extensions, this message displays any local
variables defined at each level of extension." .
setFunctionTypeTo: "Messages" .
;

Object getMessage: "specializeAs:".
setSelectorTo: "specializeAs: aString" .
setDescriptionTo:
"Creates a subclass of recipient named by supplied string.  This class is
NOT installed in the Schema database by default" .
setFunctionTypeTo: "Creation".
setParameter: 1 typeTo: String.
;

Object getMessage: "specializeAs:at:" . 
setSelectorTo: "specializeAs: aString at: anObject" .
setDescriptionTo: 
"Creates a subclass of recipient named by supplied string.  The name is
defined at the supplied object.  Note that this class is NOT installed
in the Schema database by default" .
setFunctionTypeTo: "Creation".
setParameter: 1 typeTo: String .
setParameter: 2 typeTo: Object .
;

Object getMessage: "specializeClass".
setLevelTo: "Advanced" .
setDescriptionTo: 
"Creates a specialization of recipient object's class.  The new store contains
no initial row." .
setFunctionTypeTo: "Creation".
;

Object getMessage: "specialized".
setDescriptionTo: 
"Creates a specialization of recipient object's class.  This class is
NOT installed in the Schema database by default." .
setFunctionTypeTo: "Creation" .
;

Object getMessage: "sprint:".
setSelectorTo: "sprint: aFormat" .
setDescriptionTo: "Formats as string.".
setReturnObjectTypeTo: String .
setFunctionTypeTo: "Format".
setParameter: 1 typeTo: Number ;

Object getMessage: "subClassResponsibility:".
setDescriptionTo: 
"Prints reminder that this message must be implemented at the subclass
level" .
setSelectorTo: "subClassResponsibility: aSelector" .
setLevelTo: "Advanced" .
setFunctionTypeTo: "Messages" .
setParameter: 1 typeTo: Selector .
;

Object getMessage: "validateInputObject:".
setSelectorTo: "validateInputObject: anObject" .
setDescriptionTo: 
"Validates that supplied object is in the recipient's class." .
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: Object .
;

Object getMessage: "validateInputString:".
setSelectorTo: "validateInputString: aString" .
setDescriptionTo: 
"Returns the object in the recipient's classs associated with the 
supplied string, using the recipient's class naming dictionary." .
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: String .
;

Object getMessage: "value".
setDescriptionTo: "Returns ^self by default." .
setFunctionTypeTo: "Evaluation".
;

Object getMessage: "whatAmI".
setDescriptionTo: "Returns the name of the recipient's class." .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

Object getMessage: "withCompilationErrorHandler:evaluate:" .
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
setParameter: 2 typeTo: NA.
;





