#--------------------
#        Function
#           ComputedFunction
#              Block
#              Method
#              Primitive
#           EnumeratedFunction
#              Collection
#                 IndexedCollection
#                    IndexedList
#                    TimeSeries
#                 SequencedCollection
#                    List
#              Value
#--------------------

Function getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: Function.
setFunctionTypeTo: "Conversion".
;

Function getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

Function getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: Function.
setFunctionTypeTo: "Schema" .
;

Function getMessage: "isFunction".
setDescriptionTo: "Returns a TRUE value when sent to any Function.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

Function getMessage: "new" .
setDescriptionTo: "Disables the 'new' message." .
setReturnObjectTypeTo: Function.
setFunctionTypeTo: "Creation" .
;

Function getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'Function'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;


#--------------------

ComputedFunction getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: ComputedFunction.
setFunctionTypeTo: "Conversion".
;

ComputedFunction getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

ComputedFunction getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: ComputedFunction.
setFunctionTypeTo: "Schema" .
;

ComputedFunction getMessage: "isComputedFunction".
setDescriptionTo: "Returns a TRUE value when sent to any computed function.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

ComputedFunction getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'ComputedFunction'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

#--------------------
Block getMessage: "asClosure:".
setSelectorTo: "asClosure: anObject" .
setLevelTo: "Internal" .
#setReturnObjectTypeTo: .
#setFunctionTypeTo: "" .
setParameter: 1 typeTo: Object .
;

Block getMessage: "asMethod".
setLevelTo: "Internal" .
setDescriptionTo: "Converts Block to Method".
setReturnObjectTypeTo: Method.
setFunctionTypeTo: "Primitive".
;

Block getMessage: "asOf:".
setSelectorTo: "asOf: aDate" .
setDescriptionTo: 
"Returns the value of the Block as of the supplied date.  If no value is 
stored for the date, the closest date prior to the supplied date is used.".
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: Date.
;

Block getMessage: "asRowsColumns".
setDescriptionTo: 
"Executes the recipient block and returns a list of strings representing
the rows of the output where each element of the list is extended by the
variable 'columns' which represent the columns of output for the row.
Each element in 'columns' is a string that corresponds to each print 
statement in the recipient block." .
setReturnObjectTypeTo: List .
setReturnObjectElementTypeTo: String .
setFunctionTypeTo: "Format".
;

Block getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: Block.
setFunctionTypeTo: "Conversion".
;

Block getMessage: "asUndelimitedString".
setDescriptionTo: 
"Returns the block as a string with surrounding brackets [ ] removed.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Conversion".
;

Block getMessage: "aveForDateRange:".
setSelectorTo: "aveForDateRange: aDateRange" .
setDescriptionTo: 
"Computes the average value of the recipient block over the specified date 
range.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateRange.
;

Block getMessage: "cGrowForDateRange:".
setSelectorTo: "cGrowForDateRange: aDateRange" .
setDescriptionTo: 
"Computes the compound growth rate of the recipient block over the specified 
date range.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateRange.
;

Block getMessage: "changeLag:".
setSelectorTo: "changeLag: aDateOffset" .
setDescriptionTo: 
"Computes the absolute change between the value of the block as of the default 
date and an earlier relative date.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateOffset.
;

Block getMessage: "changeLead:".
setSelectorTo: "changeLead: aDateOffset" .
setDescriptionTo: 
"Computes the absolute change between the value of the block as of the default 
date and a future relative date.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateOffset.
;

Block getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

Block getMessage: "convergeTo:within:initialGuess:".
setSelectorTo: "convergeTo: aValue within: tolerance initialGuess: guess" .
setDescriptionTo: 
"Executes recipient block until it converges to within tolerance distance from 
supplied value using guess as the initial guess.  The recipient block contains 
the function you wish to evaluate.  Iteration terminates if value converges to 
within tolerance or a maximum of forty iterations have been reached.".
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Number .
setParameter: 2 typeTo: Number .
setParameter: 3 typeTo: Number .
;

Block getMessage: "current".
setDescriptionTo: "Returns current environment of block.".
setLevelTo: "Advanced" .
setFunctionTypeTo: "Access".
;

Block getMessage: "defaultInstance" .
setDescriptionTo: "Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: Block.
setFunctionTypeTo: "Schema" .
;

Block getMessage: "displayAll".
setDescriptionTo: "Displays block definition (see TimeSeries)".
setFunctionTypeTo: "Display".
;

Block getMessage: "divertDelimitedOutput".
setLevelTo: "Advanced" .
setDescriptionTo: 
"Prints block with marked areas for internal formatting purposes.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Primitive".
;

Block getMessage: "divertOutput".
setDescriptionTo: "Captures output as a string instead of to terminal.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
;

Block getMessage: "environment".
setDescriptionTo: "Returns block's environment.".
setFunctionTypeTo: "Access".
;

Block getMessage: "environment:".
setSelectorTo: "environment: anObject" .
setLevelTo: "Advanced" .
setDescriptionTo: 
"Return's block's environment, evaluated with one input.".
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: Object.
;

Block getMessage: "environmentWith:".
setSelectorTo: "environmentWith: anObject" .
setDescriptionTo: 
"Returns block's environment, evaluated with one inputs.".
setLevelTo: "Advanced" .
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: Object.
;

Block getMessage: "environmentWith:and:".
setSelectorTo: "environmentWith: object1  and: object2" .
setDescriptionTo: 
"Returns block's environment, evaluated with two inputs.".
setLevelTo: "Advanced" .
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: Object.
setParameter: 2 typeTo: Object.
;

Block getMessage: "environmentWith:and:and:".
setSelectorTo: "environmentWith: obj1 and: obj2 and: obj3" .
setDescriptionTo: 
"Returns block's environment, evaluated with three inputs.".
setLevelTo: "Advanced" .
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: Object.
setParameter: 2 typeTo: Object.
setParameter: 3 typeTo: Object.
;

Block getMessage: "environmentWith:and:and:and:".
setSelectorTo: "environmentWith: obj1 and: obj2 and: obj3 and: obj4" .
setDescriptionTo: 
"Returns block's environment, evaluated with four inputs.".
setLevelTo: "Advanced" .
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: Object.
setParameter: 2 typeTo: Object.
setParameter: 3 typeTo: Object.
setParameter: 4 typeTo: Object.
;

Block getMessage: "environmentWith:and:and:and:and:".
setSelectorTo: 
"environmentWith: obj1 and: obj2 and: obj3 and: obj4 and: obj5" .
setDescriptionTo: 
"Returns block's environment, evaluated with five inputs.".
setLevelTo: "Advanced" .
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: Object.
setParameter: 2 typeTo: Object.
setParameter: 3 typeTo: Object.
setParameter: 4 typeTo: Object.
setParameter: 5 typeTo: Object.
;

Block getMessage: "environmentWith:and:and:and:and:and:".
setSelectorTo: 
"environmentWith: obj1 and: obj2 and: obj3 and: obj4 and: obj5 and: obj6" .
setDescriptionTo: 
"Returns block's environment, evaluated with six inputs.".
setLevelTo: "Advanced" .
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: Object.
setParameter: 2 typeTo: Object.
setParameter: 3 typeTo: Object.
setParameter: 4 typeTo: Object.
setParameter: 5 typeTo: Object.
setParameter: 6 typeTo: Object.
;

Block getMessage: "extractForDateRange:".
setSelectorTo: "extractForDateRange: aDateRange" .
setDescriptionTo: 
"Extracts the value of the recipient block for each point in the date range 
and returns a time series containing those date-value pairs.".
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: DateRange.
;

Block getMessage: "formatForExcel".
setDescriptionTo: 
"Executes the recipient block and generates tab-delimited output suitable
for loading into Excel." .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
;

Block getMessage: "formatForPRN".
setDescriptionTo: 
"Executes the recipient block and generates PRN-style output." .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
;

Block getMessage: "gMeanForDateRange:".
setSelectorTo: "gMeanForDateRange: aDateRange" .
setDescriptionTo: 
"Computes the geometric mean value of the recipient block over the specified 
date range.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateRange.
;

Block getMessage: "isBlock".
setDescriptionTo: "Returns the value TRUE when sent to any block.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

Block getMessage: "lag:".
setSelectorTo: "lag: aDateOffset" .
setDescriptionTo: 
"Returns the value of the block as of a date relative to the default date.  
The derived date is defined as: ^date - anOffset.  If no value is stored for 
the derived date, the closest date prior to the supplied date is used.".
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: DateOffset.
;

Block getMessage: "latest".
setDescriptionTo: 
"Returns the value of the recipient block as of the current date.  Identical 
to [ ] asOf: ^today.".
setFunctionTypeTo: "Access".
;

Block getMessage: "lead:".
setSelectorTo: "lead: aDateOffset" .
setDescriptionTo: 
"Returns the value of the block as of a date relative to the default date.  
The derived date is defined as: ^date + aDateOffset.  If no value is stored for 
the derived date, the closest date prior to the supplied date is used.".
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: DateOffset.
;

Block getMessage: "lsGrowForDateRange:".
setSelectorTo: "lsGrowForDateRange: aDateRange" .
setDescriptionTo: 
"Computes the least squares growth value of the recipient block over the 
specified date range.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateRange.
;

Block getMessage: "maxForDateRange:".
setSelectorTo: "maxForDateRange: aDateRange" .
setDescriptionTo: 
"Computes the maximum value of the recipient block over the specified 
date range.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateRange.
;

Block getMessage: "medianForDateRange:".
setSelectorTo: "medianForDateRange: aDateRange" .
setDescriptionTo: 
"Computes the median value of the recipient block over the specified 
date range.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateRange.
;

Block getMessage: "minForDateRange:".
setSelectorTo: "minForDateRange: aDateRange" .
setDescriptionTo: 
"Computes the minimum value of the recipient block over the specified 
date range.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateRange.
;

Block getMessage: "my".
setDescriptionTo: "Returns environment in which block was defined.".
setLevelTo: "Advanced" .
setFunctionTypeTo: "Access".
;

Block getMessage: "pctChangeLag:".
setSelectorTo: "pctChangeLag: aDateOffset" .
setDescriptionTo: 
"Computes the percent change between the value of the block as of the default 
date and an earlier relative date.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateOffset.
;

Block getMessage: "pctChangeLead:".
setSelectorTo: "pctChangeLead: aDateOffset" .
setDescriptionTo: 
"Computes the percent change between the value of the block as of the default 
date and a later relative date.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateOffset.
;

Block getMessage: "print".
setDescriptionTo: 
"Redefines the standard print message to print out the contents of the 
recipient block.".
setReturnObjectTypeTo: Block.
setFunctionTypeTo: "Display".
;

Block getMessage: "productForDateRange:".
setSelectorTo: "productForDateRange: aDateRange" .
setDescriptionTo: 
"Computes the product of the values of the recipient block over the specified 
date range.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateRange.
;

Block getMessage: "selector".
setDescriptionTo: 
"Returns the selector for the block (i.e., the message name).".
setReturnObjectTypeTo: Selector.
setFunctionTypeTo: "Descriptive".
;

Block getMessage: "self".
setLevelTo: "Advanced" .
setDescriptionTo: "Returns recipient Block.".
setReturnObjectTypeTo: Block.
setFunctionTypeTo: "Access".
;

Block getMessage: "sprint".
setDescriptionTo: "Returns recipient Block as string.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
;

Block getMessage: "stdDevForDateRange:".
setSelectorTo: "stdDevForDateRange: aDateRange" .
setDescriptionTo: 
"Computes the standard deviation of the values of the recipient Block over 
the specified date range.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateRange.
;

Block getMessage: "totalForDateRange:".
setSelectorTo: "totalForDateRange: aDateRange" .
setDescriptionTo: 
"Computes the sum of the values of the recipient block over the specified 
date range.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateRange.
;

Block getMessage: "value".
setDescriptionTo: "Evaluates the recipient block.".
setFunctionTypeTo: "Evaluation".
;

Block getMessage: "value:".
setSelectorTo: "value: anObject" .
setDescriptionTo: 
"Evaluates the recipient block with one parameter.".
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Object.
;

Block getMessage: "valueWith:".
setSelectorTo: "valueWith: anObject" .
setDescriptionTo: 
"Evaluates the recipient block with one parameter.".
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Object.
;

Block getMessage: "valueWith:and:".
setSelectorTo: "valueWith: obj1 and: obj2 " .
setDescriptionTo: 
"Evaluates the recipient block with two parameters.".
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Object.
setParameter: 2 typeTo: Object.
;

Block getMessage: "valueWith:and:and:".
setSelectorTo: "valueWith: obj1 and: obj2 and: obj3 " .
setDescriptionTo: 
"Evaluates the recipient block with three parameters.".
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Object.
setParameter: 2 typeTo: Object.
setParameter: 3 typeTo: Object.
;

Block getMessage: "valueWith:and:and:and:".
setSelectorTo: "valueWith: obj1 and: obj2 and: obj3 and: obj4" .
setDescriptionTo: 
"Evaluates the recipient block with four parameters.".
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Object.
setParameter: 2 typeTo: Object.
setParameter: 3 typeTo: Object.
setParameter: 4 typeTo: Object.
;

Block getMessage: "valueWith:and:and:and:and:".
setSelectorTo: "valueWith: obj1 and: obj2 and: obj3 and: obj4 and: obj5" .
setDescriptionTo: 
"Evaluates the recipient block with five parameters.".
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Object.
setParameter: 2 typeTo: Object.
setParameter: 3 typeTo: Object.
setParameter: 4 typeTo: Object.
setParameter: 5 typeTo: Object.
;

Block getMessage: "valueWith:and:and:and:and:and:".
setSelectorTo: 
"valueWith: obj1 and: obj2 and: obj3 and: obj4 and: obj5 and: obj6" .
setDescriptionTo: 
"Evaluates the recipient block with six parameters.".
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Object.
setParameter: 2 typeTo: Object.
setParameter: 3 typeTo: Object.
setParameter: 4 typeTo: Object.
setParameter: 5 typeTo: Object.
setParameter: 6 typeTo: Object.
;

Block getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'Block'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

Block getMessage: "whileTrue:".
setSelectorTo: "whileTrue: aBlock" .
setDescriptionTo: 
"While recipient block evaluates to TRUE, executes supplied block".
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
;

#--------------------

Method getMessage: "asClosure:".
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: Object .
;

Method getMessage: "asMethod".
setLevelTo: "Internal" .
#setSelectorTo: "" .
#setDescriptionTo: "" .
#setReturnObjectTypeTo: .
#setFunctionTypeTo: "".
;

Method getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: Method.
setFunctionTypeTo: "Conversion".
;

Method getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

Method getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: Method.
setFunctionTypeTo: "Schema" .
;

Method getMessage: "isMethod".
setDescriptionTo: "Returns a TRUE value when sent to any Method.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

Method getMessage: "print".
setDescriptionTo: 
"Redefines the standard print message to print out the contents of the 
recipient method.".
setReturnObjectTypeTo: Method.
setFunctionTypeTo: "Display".
;

Method getMessage: "selector".
setDescriptionTo: 
"Returns the selector for the method (i.e., the message name).".
setReturnObjectTypeTo: Selector.
setFunctionTypeTo: "Descriptive".
;

Method getMessage: "sprint".
setDescriptionTo: "Returns recipient Method as a string.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
;

Method getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'Method'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

#--------------------

Primitive getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: Primitive.
setFunctionTypeTo: "Conversion".
;

Primitive getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

Primitive getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: Primitive.
setFunctionTypeTo: "Schema" .
;

Primitive getMessage: "isPrimitive".
setDescriptionTo: "Returns a TRUE value when sent to any Primitive.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

Primitive getMessage: "print".
setDescriptionTo: 
"Redefines the standard print message to indicate that method is primitive.".
setReturnObjectTypeTo: Method.
setFunctionTypeTo: "Display".
;

Primitive getMessage: "sprint".
setDescriptionTo: "Returns recipient Primitive as a string.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
;

Primitive getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'Primitive'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

#--------------------

EnumeratedFunction getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: EnumeratedFunction.
setFunctionTypeTo: "Conversion".
;

EnumeratedFunction getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

EnumeratedFunction getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class".
setReturnObjectTypeTo: EnumeratedFunction.
setFunctionTypeTo: "Schema" .
;

EnumeratedFunction getMessage: "isEnumeratedFunction".
setDescriptionTo: "Returns a TRUE value when sent to any EnumeratedFunction.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

EnumeratedFunction getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'EnumeratedFunction'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

#--------------------

Collection getMessage: "all:".
setSelectorTo: "all: booleanBlock" .
setDescriptionTo: 
"Returns TRUE if all elements in recipient respond to supplied block with
the value TRUE." . 
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: Block .
;

Collection getMessage: "any:".
setSelectorTo: "any: booleanBlock" .
setDescriptionTo: 
"Returns TRUE if any element in recipient respond to supplied block with
the value TRUE." . 
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: Block .
;

Collection getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: Collection.
setFunctionTypeTo: "Conversion".
;

Collection getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

Collection getMessage: "collect:".
setSelectorTo: "collect: aBlock" .
setDescriptionTo: 
"Uses supplied block to create an extension of collection. The value of the 
block is assigned to the variable value.  The returned extension responds to 
the value message.".
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

Collection getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: Collection.
setFunctionTypeTo: "Schema" .
;

Collection getMessage: "difference:".
setSelectorTo: "difference: aList" .
setDescriptionTo: 
"This message returns a list of two elements: the first element contains
the list of elements that are in list 1 and not in list 2; the second
element contains the list of elements that are in list 2 and not list 1.  
For example:

     !diffs <- (1,2,3,4,5) difference: (3,4,5,6) ;
     \"In 1 not 2\" print ; diffs at: 1 . do: [ print ] ; newLine print; 
     \"In 2 not 2\" print ; diffs at: 2 . do: [ print ] ;
" .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: List.
;

Collection getMessage: "doReverse:".
setSelectorTo: "doReverse: aBlock" .
setDescriptionTo: 
"Evaluates the statements provided in the supplied block for each element in the 
list in reverse order.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
;
Collection getMessage: "first:".
setSelectorTo: "first: n" .
setDescriptionTo: 
"Selects the first 'n' non-NA, non-Default elements from the recipient." .
setReturnObjectTypeTo: Collection.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Integer .
;

Collection getMessage: "from:to:".
setSelectorTo: "from: start to: end" .
setDescriptionTo: 
"Selects the elements from the supplied staring position to the supplied
ending position from the recipient." .
setReturnObjectTypeTo: Collection.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Integer .
setParameter: 2 typeTo: Integer .
;

Collection getMessage: "from:".
setSelectorTo: "from: start " .
setDescriptionTo: 
"Selects the elements from the supplied starting position to the
ending position in the recipient." .
setReturnObjectTypeTo: Collection.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Integer .
;

Collection getMessage: "to:".
setSelectorTo: "to: end" .
setDescriptionTo: 
"Selects the elements from the start of the recipient to the supplied
ending position." .
setReturnObjectTypeTo: Collection.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Integer .
;

Collection getMessage: "gMean:".
setSelectorTo: "gMean: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in supplied Block for each element in the list 
and computes the geometric mean." .
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Block .
;

Collection getMessage: "groupedBy:".
setSelectorTo: "groupedBy: aBlock" .
setDescriptionTo: 
"Groups the recipient collection based on the criteria specified by supplied 
block. After the groupedBy: message is sent, each element in the resultant 
list responds to the message groupList which returns the list of elements in 
that group.".
setReturnObjectTypeTo: Collection.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
;

Collection getMessage: "groupedByCriteria:".
setSelectorTo: "groupedByCriteria: aBlockList" .
setDescriptionTo: 
"Groups the recipient collection based on the criteria specified by supplied 
list of blocks. The resultant list contains one element for each 
combination of values present from processing the supplied blocks.  Each
element in the resultant list responds to the message 'keyList' which
is the list of values associated with this group, one per supplied
block.  Each element in the resultant list responds to the message 
'groupList' which returns the list of elements in the specific combination
of block values.  For example,
    Company masterList groupedByCriteria: [ sector ] , [ country ] 
returns a list of sector/country pairs.  To display the name of
each key and the group count, use:
    Company masterList groupedByCriteria: [ sector ] , [ country ] .
    do: [ keyList do: [ name print: 20 ] ;     #-- print keys
          groupList count printNL ;              #-- print count
        ] ;
" .
setReturnObjectTypeTo: Collection.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: List.
;

Collection getMessage: "collectGroupsForCriteria:atLevel:in:withKeys:" .
setLevelTo: "Internal" .
;

Collection getMessage: "correlate:with:".
setSelectorTo: "correlate: blockX with: blockY" .
setDescriptionTo: 
"Computes the correlation of all points in the recipient based on evaluation
of the formula provided by the two supplied blocks." .
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

Collection getMessage: "rankCorrelate:with:".
setSelectorTo: "rankCorrelate: blockX with: blockY" .
setDescriptionTo: 
"Computes the correlation between the relative ranks of the data items
defined by the two blocks.  " .
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

Collection getMessage: "groupedBy:union:".
setSelectorTo: "groupedBy: block union: list" .
setDescriptionTo: 
"Groups the recipient collection based on the criteria specified by supplied 
blocks. The resultant list contains one element for each value that
is included in supplied list OR that results from applying the
supplied block to the recipient collection.  For example,
    companyList groupedBy: [ country ] union: Country masterList
will return a list with one entry for each Country including 
countries not represented in 'companyList' (where groupList count
will be 0.".
setReturnObjectTypeTo: Collection.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block .
setParameter: 1 typeTo: List .
;

Collection getMessage: "groupedBy:in:".
setSelectorTo: "groupedBy: block in: list" .
setDescriptionTo: 
"Groups the recipient collection based on the criteria specified by supplied 
blocks. The resultant list contains one element for each value that
is included in supplied list.  Unlike 'groupedBy:union:', groups
that do not appear in supplied list are eliminated." .
setReturnObjectTypeTo: Collection.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block .
setParameter: 1 typeTo: List .
;

Collection getMessage: "groupedBy:intersect:".
setSelectorTo: "groupedBy: block intersect: list" .
setDescriptionTo: 
"Groups the recipient collection based on the criteria specified by supplied 
blocks. The resultant list contains one element for each value that
is included in both the supplied list AND the result of applying the
supplied block to the recipient collection." .
setReturnObjectTypeTo: Collection.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block .
setParameter: 1 typeTo: List .
;

Collection getMessage: "isCollection".
setDescriptionTo: "Returns a TRUE value when sent to any collection.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

Collection getMessage: "isDefault".
setDescriptionTo: "Returns TRUE if recipient collection is empty." .
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

Collection getMessage: "isEquivalentTo:" . 
setSelectorTo: "isEquivalentTo: object" .
setDescriptionTo:
"Returns TRUE if the recipient collection contains the same elements as
the supplied collection." .
setFunctionTypeTo: "Comparison" . 
setReturnObjectTypeTo: Boolean . 
setParameter: 1 typeTo: Object . 
;

Collection getMessage: "iterate:" . 
setSelectorTo: "iterate: block" .
setDescriptionTo:
"This message is used to consider each element in the recipient collection
or date range one at a time in sequential order.  Unlike the 'do:' message
which operates in parallel, this message works sequentially and may be
much slower.  It is useful in cases where you need to use the results of
a prior element in each iteration and where the size of the structures 
generated by parallel execution consumes more memorry resource than desired." .
setFunctionTypeTo: "Evaluation" . 
setReturnObjectTypeTo: Collection . 
setParameter: 1 typeTo: Block . 
;

Collection getMessage: "largest:".
setSelectorTo: "largest: block" .
setDescriptionTo: 
"Returns the largest element based on supplied 'block'." .
setReturnObjectTypeTo: Object .
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Integer .
;
Collection getMessage: "largest:cutoff:".
setSelectorTo: "largest: block cutoff: n" .
setDescriptionTo: 
"Selects the largest 'n' elements based on supplied 'block'
from the recipient.  If the cutoff is 1, the single object is returned;
otherwise, a list is returned. " .
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Integer .
;

Collection getMessage: "last:".
setSelectorTo: "last: n" .
setDescriptionTo: 
"Selects the last 'n' non-NA, non-Default elements from the recipient." .
setReturnObjectTypeTo: Collection.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Integer .
;

Collection getMessage: "linkElements".
setDescriptionTo:
"Extends each element in the collectionby the variables 'prior' and 'next'
which corresponds to the elements in the recipient lilst that are before
and after each element.  For example:

     5 sequence linkElements
     do: [ print ; prior print ; next printNL ] ;

" .
setReturnObjectTypeTo: Collection.
setFunctionTypeTo: "Extension".
;

Collection getMessage: "new" .
setDescriptionTo: 
"Creates a new instance of the recipient object's class." .
setReturnObjectTypeTo: Collection.
setFunctionTypeTo: "Creation" .
;

Collection getMessage: "rankDown:".
setSelectorTo: "rankDown: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in supplied block for each element in the 
collection and computes its rank (highest value = 1).".
setReturnObjectTypeTo: Collection.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

Collection getMessage: "rankUp:".
setSelectorTo: "rankUp: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in supplied block for each element in the 
collection and computes its rank (lowest value = 1).".
setReturnObjectTypeTo: Collection.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

Collection getMessage: "regress:".
setSelectorTo: "regress: list2" .
setDescriptionTo: 
"Performs a standard linear regression between the recipient collection
(the dependent variable) and the supplied parameter (the independent 
variable).  The returned object responds to the messages 'beta', 'alpha', 
'pearson', 'rsq', and 'stdErr'.  If either collection contains non-numeric
values or the two collections are not the same size, the returned values will
be NA.  For example:

     (2,3,9,1,8,7,5) regress: (6,5,11,7,5,4,4) .
     do: [ beta print ; alpha print ; 
           pearson print ; rsq print ; stdErr printNL ;
         ] ;

runs the regression and displays the results of the various computations." .
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Collection .
;

Collection getMessage: "runningTotal:".
setSelectorTo: "runningTotal: aBlock" .
setDescriptionTo: 
"Computes the running total for each element in the collection based on 
supplied block.".
setReturnObjectTypeTo: Collection.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

Collection getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'Collection'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;


Collection getMessage: "average:".
setSelectorTo: "average: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in supplied Block for each element in the list 
and computes the average value.  NA values are not included in the average.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Block.
;
Collection getMessage: "average".
setDescriptionTo: 
"Computes the average for the collection assuming the collection contains
numeric values." .
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
;
Collection getMessage: "average:withWeights:".
setSelectorTo: "average: aBlock withWeights: bBlock" .
setDescriptionTo: 
"Returns the weighted average using the formula in bBlock to weight
the formula supplied in aBlock." . 
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

Collection getMessage: "mavg:observations:".
setSelectorTo: "mavg: block observation: n" .
setDescriptionTo: 
"Extends the recipient collection by the variable 'mavg' 
or 'mavgXRef' which contains the single moving average or an indexed
list of moving averages for each point in the collection.  This
method is usually sent to TimeSeries objects but can be used for
any collection for which you want to do ordered running averages.

The supplied block is used to access the raw data value for which
the moving averages will be computed.  The number of observations to
use can be supplied as a single positive integer or as a list of 
positive integers.  If a single value is supplied, the 'mavg'
variable will contains the average of the last 'n' observations, 
where 'n' is the supplied observation count.  For example, if the
recipient is a daily time series of the last 200 days of prices and
'n' is 10, the result will be the 200-day time series extended by
the variable 'mavg' which will contain the 10-day moving average for
each point in the time series.  Note that the value will be NA for
the first nine dates since there will not be enough days of data to 
perform the calculation.  If 'n' is supplied as a list of values,
the extension will define the 'mavgXRef' variable which will contain
an indexed list of the different moving average computes.  For example,
      dailyPrices mavg: [ ^self ] observations: 2,5,10,20 .
          select: [ (mavgXRef at: 2 ) > (mavgXRef at: 5) &&
                    (mavgXRef at: 10 ) > (mavgXRef at: 20) 
                  ] .
computes the four moving averages and selects the dates where the
2-day price average is greater than the 5-day average and the 10-day
price average is greater than the 20-day average." .
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Integer.
;
Collection getMessage: "max:".
setSelectorTo: "max: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in the supplied block for each element in the 
list and returns the maximum value.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Block.
;
Collection getMessage: "max".
setDescriptionTo: 
"Computes the maximum for the collection assuming the collection contains
numeric values." .
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
;

Collection getMessage: "min:".
setSelectorTo: "min: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in the supplied block for each element in the 
list and returns the minimum value.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Block.
;
Collection getMessage: "min".
setDescriptionTo: 
"Computes the minimum for the collection assuming the collection contains
numeric values." .
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
;
Collection getMessage: "mode".
setDescriptionTo: 
"Computes the mode for the collection assuming the collection contains
numeric values." .
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
;



Collection getMessage: "product:" .
setSelectorTo: "product: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in the supplied block for each element in the 
list and computes the product of the values." .
setReturnObjectTypeTo: Number .
setFunctionTypeTo: "Computation" .
setParameter: 1 typeTo: Block .
;
Collection getMessage: "product".
setDescriptionTo: 
"Computes the product for the collection assuming the collection contains
numeric values." .
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
;
Collection getMessage: "total:" .
setSelectorTo: "total: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in the supplied block for each element in the 
list and computes the sum of the values." .
setReturnObjectTypeTo: Number .
setFunctionTypeTo: "Computation" .
setParameter: 1 typeTo: Block .
;
Collection getMessage: "total".
setDescriptionTo: 
"Computes the sum for the collection assuming the collection contains
numeric values." .
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
;

Collection getMessage: "compound:" .
setSelectorTo: "compound: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in the supplied block for each element in the 
list and computes the compounded value." .
setReturnObjectTypeTo: Number .
setFunctionTypeTo: "Computation" .
setParameter: 1 typeTo: Block .
;
Collection getMessage: "compound".
setDescriptionTo: 
"Computes the compounded value for the collection assuming the collection
contains numeric values." .
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
;

Collection getMessage: "harmonicMean:".
setSelectorTo: "harmonicMean: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in supplied Block for each element in the list 
and computes the harmonicMean value.  ".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Block.
;
Collection getMessage: "harmonicMean:withWeights:".
setSelectorTo: "harmonicMean: aBlock withWeights: bBlock" .
setDescriptionTo: 
"Returns the weighted harmonic mean using the formula in bBlock to weight
the value supplied in aBlock." . 
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;
Collection getMessage: "harmonicMean".
setDescriptionTo: 
"Computes the harmonicMean for the collection assuming the collection contains
numeric values." .
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
;

Collection getMessage: "normalize:".
setSelectorTo: "normalize: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in the supplied block for each element in the 
collection and computes its normalized value based on the mean and standard
deviation of the collection.   Returns the recipient extended by the property
'norm'.".
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

Collection getMessage: "normalize".
setDescriptionTo: 
"Returns the recipient extended by the property 'norm' assuming the recipient
is a collectin of numbers.".
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

Collection getMessage: "normalize:by:".
setSelectorTo: "normalize: aBlock by: bBlock" .
setDescriptionTo: 
"Groups the recipient by the criteria in bBlock and generates a normalized
value for each element relative to its group. The result object is the original
collection extended by the variable 'norm'.  For example:
     Company masterList normalize: [ sales ] by: [ country ] 
" .
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

Collection getMessage: "normalize:by:by:".
setSelectorTo: "normalize: aBlock by: bBlock by: cBlock" .
setDescriptionTo: 
"Groups the recipient by the criteria in bBlock and cBlock and generates a
normalized value for each element relative to its group. The result object 
is the original collection extended by the variable 'norm'.  For example:
     Company masterList
        normalize: [ sales ] by: [ country ] by: [ industry ] 
" .
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;


Collection getMessage: "rankDown:usingCollector:" .
setSelectorTo: "rankDown: block1 usingCollector: block2" .
setLevelTo: "Internal" .
setDescriptionTo: 
"Ranks down the recipient collection based on the criteria specified by block1.  
The second block provides the name of the extension. The 'rankDown:' message 
is implemented using 'rank' as the collector.".
setReturnObjectTypeTo: IndexedList.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

Collection getMessage: "rankUp:usingCollector:".
setSelectorTo: "rankUp: block1 usingCollector: block2" .
setLevelTo: "Internal" .
setDescriptionTo: 
"Ranks up the recipient collection based on the criteria specified by block1.  
The second block provides the name of the extension. The 'rankUp:' message 
is implemented using 'rank' as the collector.".
setReturnObjectTypeTo: IndexedList.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

Collection getMessage: "runningTotal:usingCollector:".
setSelectorTo: "runningTotal: block1 usingCollector: block2" .
setLevelTo: "Internal" .
setDescriptionTo: 
"Computes the running total for each element in the recipient collection based 
on criteria specified by block1. The second block provides the name of the
extension.  The 'runningTotal:' message is implemented using 'runningTotal'
as the collector.".
setReturnObjectTypeTo: IndexedList.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

Collection getMessage: "smallest:".
setSelectorTo: "smallest: block" .
setDescriptionTo: 
"Returns the smallest element based on supplied 'block'." .
setReturnObjectTypeTo: Object .
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Integer .
;
Collection getMessage: "smallest:cutoff:".
setSelectorTo: "smallest: block cutoff: n" .
setDescriptionTo: 
"Selects the smallest 'n' elements based on supplied 'block'
from the recipient.  If the cutoff is 1, the single object is returned;
otherwise, a list is returned. " .
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Integer .
;

#--------------------

IndexedCollection getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: IndexedCollection.
setFunctionTypeTo: "Conversion".
;

IndexedCollection getMessage: "at:".
setSelectorTo: "at: index" .
setDescriptionTo: 
"Returns the value of the recipient IndexedCollection stored at the 
supplied index.".
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: Object .
;

IndexedCollection getMessage: "at:put:".
setSelectorTo: "at: index put: aValue" .
setDescriptionTo: 
"Stores the supplied value at the supplied index in the recipient 
IndexedCollection.".
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: Object .
setParameter: 2 typeTo: Object .
;

IndexedCollection getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

IndexedCollection getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: IndexedCollection.
setFunctionTypeTo: "Schema" .
;

IndexedCollection getMessage: "isIndexedCollection".
setDescriptionTo: "Returns a TRUE value when sent to any IndexedCollection.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

IndexedCollection getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'IndexedCollection'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

#--------------------

IndexedList getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: IndexedList.
setFunctionTypeTo: "Conversion".
;

IndexedList getMessage: "basicRankDown:usingCollector:".
setLevelTo: "Internal" .
setReturnObjectTypeTo: IndexedList.
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
setParameter: 2 typeTo: NA .
;

IndexedList getMessage: "basicRankUp:usingCollector:".
setLevelTo: "Internal" .
setReturnObjectTypeTo: IndexedList.
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
setParameter: 2 typeTo: NA .
;

IndexedList getMessage: "basicRunningTotal:usingCollector:".
setLevelTo: "Internal" .
setReturnObjectTypeTo: IndexedList.
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
setParameter: 2 typeTo: NA .
;

IndexedList getMessage: "basicSortDown:".
setLevelTo: "Internal" .
setSelectorTo: "basicSortDown: aBlock" .
setDescriptionTo: 
"Evaluates the statements provided in the supplied block and sorts the list in 
descending order.  Sort is highest to lowest for numbers.  Sort is ZZZ 
through A for strings.".
setReturnObjectTypeTo: IndexedList.
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: Block.
;

IndexedList getMessage: "basicSortUp:".
setLevelTo: "Internal" .
setSelectorTo: "basicSortUp: aBlock" .
setDescriptionTo: 
"Evaluates the statements provided in the supplied block and sorts the list in 
ascending order.  Sort is lowest to highest for numbers.  Sort is A through Z 
for strings.".
setReturnObjectTypeTo: IndexedList.
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: Block.
;

IndexedList getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information is 
stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

IndexedList getMessage: "count".
setDescriptionTo: 
"Counts the number of elements in the IndexedList.  Returns a non-negative 
integer.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Inquiry".
;

IndexedList getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: IndexedList.
setFunctionTypeTo: "Schema" .
;

IndexedList getMessage: "delete:".
setSelectorTo: "delete: anObject" .
setDescriptionTo: 
"Removes element stored at supplied parameter from recipient IndexedList.".
setReturnObjectTypeTo: IndexedList.
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: Object.
;

IndexedList getMessage: "deleteColumn:".
setLevelTo: "Internal" .
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: NA.
;

IndexedList getMessage: "do:".
setSelectorTo: "do: aBlock" .
setDescriptionTo: 
"Evaluates the statements provided in the supplied block for each element 
in the IndexedList.  The block is evaluated in the context of each element in 
the list.".
setReturnObjectTypeTo: IndexedList.
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
;

IndexedList getMessage: "extendBy:".
setSelectorTo: "extendBy: aBlock" .
setDescriptionTo: 
"Evaluates the supplied block for each element in the list, where the supplied 
block specifies a set of new properties to which each element in the list can 
respond.".
setReturnObjectTypeTo: IndexedList.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

IndexedList getMessage: "extendIndex".
setDescriptionTo: 
"Returns the recipient containing its original
elements extended by the variable 'index' which represents the value
of the object's index in the recipient." .
setReturnObjectTypeTo: IndexedList.
setFunctionTypeTo: "Extension".
;

IndexedList getMessage: "groupedBy:usingCollector:".
setSelectorTo: "groupedBy: block1 usingCollector: block2" .
setLevelTo: "Internal" .
setDescriptionTo: 
"Groups the recipient collection based on the criteria specified by block1.  
The second block provides the name of the extension. The 'groupedBy:' message 
is implemented using groupList as the collector.".
setReturnObjectTypeTo: IndexedList.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

IndexedList getMessage: "isIndexedList" .
setDescriptionTo: "Returns the value TRUE when sent to any IndexedList.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

IndexedList getMessage: "iterate:by:".
setSelectorTo: "iterate: aBlock by: anInteger" .
setLevelTo: "Advanced" .
setDescriptionTo: 
"Similar to the 'do:' message.  The supplied integer is used to process
recipient indexedList as a series of sublists.".
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Integer .
;

IndexedList getMessage: "ncount:".
setLevelTo: "Internal" .
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: NA.
;

IndexedList getMessage: "newValueCell:".
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

IndexedList getMessage: "print".
setDescriptionTo: "Redefines the standard print message.".
setReturnObjectTypeTo: IndexedList.
setFunctionTypeTo: "Display".
;

IndexedList getMessage: "select:".
setSelectorTo: "select: aBlock" .
setDescriptionTo: 
"Evaluates the statements provided in the supplied block for each element in the 
list and selects those elements that evaluate to TRUE.".
setReturnObjectTypeTo: IndexedList.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
;

IndexedList getMessage: "send:".
setSelectorTo: "send: aBlock" .
setDescriptionTo: 
"Evaluates the statements provided in the supplied block for each element in the 
IndexedList.  The IndexedList returned has the same number of elements as the 
recipient, with each element being the result of evaluating the block.".
setReturnObjectTypeTo: IndexedList.
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
;

IndexedList getMessage: "sortDown:".
setSelectorTo: "sortDown: aBlock" .
setDescriptionTo: 
"Evaluates the statements provided in the supplied block and sorts the list in 
descending order.  Sort is highest to lowest for numbers.  Sort is ZZZ 
through A for strings.  Returns a List object.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
;

IndexedList getMessage: "sortDownBy:then:".
setSelectorTo: "sortDownBy: block1 then: block2 " .
setDescriptionTo: "Two-level descending sort.  A list object is returned." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

IndexedList getMessage: "sortDownBy:then:then:".
setSelectorTo: "sortDownBy: block1 then: block2 then: block3" .
setDescriptionTo: "Three-level descending sort.  A list object is returned." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
setParameter: 3 typeTo: Block.
;

IndexedList getMessage: "sortDownBy:then:then:then:".
setSelectorTo: "sortDownBy: block1 then: block2 then: block3 then: block4" .
setDescriptionTo: "Four-level descending sort.  A list object is returned." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
setParameter: 3 typeTo: Block.
setParameter: 4 typeTo: Block.
;

IndexedList getMessage: "sortUp:".
setSelectorTo: "sortUp: aBlock" .
setDescriptionTo: 
"Evaluates the statements provided in the supplied block and sorts the list in 
ascending order.  Sort is lowest to highest for numbers.  Sort is A through Z 
for strings.  A List object is returned.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
;

IndexedList getMessage: "sortUpBy:then:".
setSelectorTo: "sortUpBy: block1 then: block2 " .
setDescriptionTo: "Two-level ascending sort.  A list object is returned." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

IndexedList getMessage: "sortUpBy:then:then:".
setSelectorTo: "sortUpBy: block1 then: block2 then: block3 " .
setDescriptionTo: "Three-level ascending sort.  A list object is returned." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
setParameter: 3 typeTo: Block.
;

IndexedList getMessage: "sortUpBy:then:then:then:".
setSelectorTo: "sortUpBy: block1 then: block2 then: block3 then: block4" .
setDescriptionTo: "Four-level ascending sort.  A list object is returned." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
setParameter: 3 typeTo: Block.
setParameter: 4 typeTo: Block.
;

IndexedList getMessage: "toList".
setDescriptionTo: "Converts an IndexedList into a standard List.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Conversion".
;

IndexedList getMessage: "total:".
setSelectorTo: "total: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in the supplied block for each element 
in the list and computes the total value.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Block.
;

IndexedList getMessage: "valueCell:".
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: "NA" .
;

IndexedList getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'IndexedList'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

#--------------------

TimeSeries getMessage: "<-".
setSelectorTo: "<- anObject" .
setDescriptionTo: 
"Assigns the value provided as of the default date into the recipient.  This 
operations does NOT create new points in the time series; it updates the point 
on or before the default date.  If there is no point on or before the default, 
the value is stored at the date 1/1/1.  <- only works with true time series, 
not methods (i.e., if formula is a method defined to compute a ratio of two 
numbers, the <- message can not be used to change the value of formula).".
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: Object.
;

TimeSeries getMessage: "+".
setSelectorTo: "+ anObject" .
setDescriptionTo: 
"This message is used to perform vector addition between two lists or a
list and a scalar value.  If the supplied value is a scalar, the value is
added to each element in the recipient list.  If the supplied value is a
list, the operation is applied positionally." .
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: TimeSeries .
;

TimeSeries getMessage: "-".
setSelectorTo: "- anObject" .
setDescriptionTo: 
"This message is used to perform vector subtraction between two lists or a
list and a scalar value.  If the supplied value is a scalar, the value is
subtracted from each element in the recipient list.  If the supplied value
is a list, the operation is applied positionally." .
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: TimeSeries .
;
TimeSeries getMessage: "*".
setSelectorTo: "* anObject" .
setDescriptionTo: 
"This message is used to perform vector multiplication between two lists 
or a list and a scalar value.  If the supplied value is a scalar, the value
is multiplied by each element in the recipient list.  If the supplied value
is a list, the operation is applied positionally." .
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: TimeSeries .
;

TimeSeries getMessage: "/".
setSelectorTo: "/ anObject" .
setDescriptionTo: 
"This message is used to perform vector division between two lists 
or a list and a scalar value.  If the supplied value is a scalar, the
value divides each element in the recipient list.  If the supplied value
is a list, the operation is applied positionally." .
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: TimeSeries .
;

TimeSeries getMessage: "anElement".
setDescriptionTo: "Returns a sample element from the recipient timeSeries.".
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Access".
;

TimeSeries getMessage: "asOf:".
setSelectorTo: "asOf: aDate" .
setDescriptionTo: 
"Returns the value of the time series as of the supplied date.  If no value is 
stored for the date, the closest date prior to the supplied date is used.".
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: Date.
;

TimeSeries getMessage: "on:".
setSelectorTo: "on: aDate" .
setDescriptionTo: 
"Returns the value of the time series on the supplied date.  If no value is 
stored for the date, NA is returned." .
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: Date.
;

TimeSeries getMessage: "asOf:assign:".
setSelectorTo: "asOf: aDate assign: aValue" .
setDescriptionTo: 
"Assigns supplied value into the time series as of the supplied date.  This 
operation does NOT create new points in the time series; it updates the first 
date on or before the supplied date that already exists in the time series.  
Message only works with true time series, not methods (i.e., if formula is a 
method defined to compute a ratio of two numbers, the message can not be used 
to change the value of formula).".
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: Date.
setParameter: 2 typeTo: Object .
;

TimeSeries getMessage: "asOf:put:".
setSelectorTo: "asOf: aDate put: anObject" .
setDescriptionTo: 
"Assigns supplied value into the time series as of the date supplied.  The 
message creates a new time point in the time series if the date did not exist.
Message only works with true time series, not methods (i.e., if formula is a 
method defined to compute a ratio of two numbers, the message can not be used 
to change the value of formula).".
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: Date.
setParameter: 2 typeTo: Object.
;

TimeSeries getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Conversion".
;

TimeSeries getMessage: "aveForDateRange:".
setSelectorTo: "aveForDateRange: aDateRange" .
setDescriptionTo: 
"computes the average value of a time series over the specified date range.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateRange.
;

TimeSeries getMessage: "basicRankDown:usingCollector:".
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: "NA" .
setParameter: 2 typeTo: "NA" .
;

TimeSeries getMessage: "basicRankUp:usingCollector:".
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: "NA" .
setParameter: 2 typeTo: "NA" .
;

TimeSeries getMessage: "basicRunningTotal:usingCollector:".
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: "NA" .
setParameter: 2 typeTo: "NA" .
;

TimeSeries getMessage: "basicSortDown:".
setSelectorTo: "basicSortDown: aBlock" .
setDescriptionTo: 
"Evaluates the statements provided in the supplied block and sorts the TimeSeries 
in descending order.  Sort is highest to lowest for numbers.  Sort is ZZZ 
through A for strings.".
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Transformation".
setLevelTo: "Internal" .
setParameter: 1 typeTo: Block.
;

TimeSeries getMessage: "basicSortUp:".
setSelectorTo: "basicSortUp: aBlock" .
setDescriptionTo: 
"Evaluates the statements provided in the supplied block and sorts the TimeSeries 
in ascending order.  Sort is lowest to highest for numbers.  Sort is A 
through Z for strings.".
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Transformation".
setLevelTo: "Internal" .
setParameter: 1 typeTo: Block.
;

TimeSeries getMessage: "cGrow".
setDescriptionTo: "Computes the compound growth rate of the time series.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
;

TimeSeries getMessage: "cGrowForDateRange:".
setSelectorTo: "cGrowForDateRange: aDateRange" .
setDescriptionTo: 
"Computes the compound growth rate of the time series over the specified 
date range.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateRange.
;

TimeSeries getMessage: "changeLag:".
setSelectorTo: "changeLag: aDateOffset" .
setDescriptionTo: 
"Computes the absolute change between the value of the time series as of the 
default date and an earlier relative date.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateOffset.
;

TimeSeries getMessage: "changeLead:".
setSelectorTo: "changeLead: aDateOffset" .
setDescriptionTo: 
"Computes the absolute change between the value of the time series as of the 
default date and a future relative date.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateOffset.
;

TimeSeries getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

TimeSeries getMessage: "count".
setDescriptionTo: "Counts the number of points in the TimeSeries.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Inquiry".
;

TimeSeries getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Schema" .
;

TimeSeries getMessage: "delete:".
setSelectorTo: "delete: aDate" .
setDescriptionTo: 
"Deletes the point at the specified date from the time series.  Only deletes 
the point if the exact date exists in the time series.".
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: Date.
;

TimeSeries getMessage: "deleteAllPoints".
setDescriptionTo: "Deletes all points in the recipient time series.".
setFunctionTypeTo: "Update".
;

TimeSeries getMessage: "deleteColumn".
setLevelTo: "Internal" .
setFunctionTypeTo: "Update".
;

TimeSeries getMessage: "deleteColumnForDate:".
setSelectorTo: "deleteColumnForDate: aDate" .
setLevelTo: "Advanced" .
setDescriptionTo: 
"Removes supplied date from the TimeSeries for all instances in recipient's
class" .
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: Date.
;

TimeSeries getMessage: "deletePoint".
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
;

TimeSeries getMessage: "displayAll".
setDescriptionTo: 
"Displays all the date-value pairs that exist in the time series.".
setFunctionTypeTo: "Display".
;

TimeSeries getMessage: "displayAllChanges".
setDescriptionTo: 
"Displays all date-value pairs that in the recipient TimeSeries that
contain a new value." .
setFunctionTypeTo: "Display".
;

TimeSeries getMessage: "do:".
setSelectorTo: "do: aBlock" .
setDescriptionTo: 
"Evaluates the criteria provided in supplied block for each element in the 
time series.".
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
;

TimeSeries getMessage: "effectiveDate".
setDescriptionTo: 
"Returns the date on or before the default date.  If there are no points in 
the time series on or before the default date, NA is returned.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Access".
;

TimeSeries getMessage: "effectiveDateAsOf:".
setSelectorTo: "effectiveDateAsOf: aDate" .
setDescriptionTo: 
"Returns the date in the time series on or before the supplied date.  If 
there are no points in the time series on or before the default date, NA 
is returned.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: Date.
;

TimeSeries getMessage: "extendBy:".
setSelectorTo: "extendBy: aBlock" .
setDescriptionTo: 
"Evaluates supplied block for each element in the time series, where supplied 
block specifies a set of new variables to which each element in the 
time series can respond.".
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

TimeSeries getMessage: "extractForDateRange:".
setSelectorTo: "extractForDateRange: aDateRange" .
setDescriptionTo: 
"Creates a new time series that contains a point for each value in the supplied 
date range whose value is extracted from the recipient time series.".
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: DateRange.
;

TimeSeries getMessage: "extractExactValuesForDateRange:".
setSelectorTo: "extractExactValuesForDateRange: aDateRange" .
setDescriptionTo: 
"Creates a new time series that contains a point for each value in the supplied 
date range whose value is extracted from the recipient time series.  If there
is no value on the exact date in the date range, the default value for the
time series is stored as of the date.  The value as of 1/1/1 is used as
the default value." .
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: DateRange.
;

TimeSeries getMessage: "firstDate".
setDescriptionTo: 
"Returns date of the first point in the time series.  If the time series is 
empty, NA is returned.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Access".
;

TimeSeries getMessage: "firstObservation".
setDescriptionTo: 
"Returns the value stored at the first point in the time series.  If the time 
series is empty, NA is returned.".
setFunctionTypeTo: "Access".
;

TimeSeries getMessage: "from:to:".
setSelectorTo: "from: date1 to: date2" .
setDescriptionTo: 
"Returns the subset of points in the recipient that fall with the supplied
dates." . 
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Date .
setParameter: 2 typeTo: Date .
;

TimeSeries getMessage: "from:".
setSelectorTo: "from: date1" .
setDescriptionTo: 
"Returns the subset of points in the recipient on or after the supplied
date." . 
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Date .
;

TimeSeries getMessage: "to:".
setSelectorTo: "to: date1" .
setDescriptionTo: 
"Returns the subset of points in the recipient on or before the supplied
date." . 
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Date .
;

TimeSeries getMessage: "gMean".
setDescriptionTo: "Computes the geometric mean of the time series".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
;

TimeSeries getMessage: "gMeanForDateRange:".
setSelectorTo: "gMeanForDateRange: aDateRange" .
setDescriptionTo: 
"Computes the geometric mean of the time series over the specified date range.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateRange.
;

TimeSeries getMessage: "groupedBy:usingCollector:".
setSelectorTo: "groupedBy: aBlock usingCollector: aBlock" .
setLevelTo: "Internal" .
setDescriptionTo: 
"Groups the recipient TimeSeries based on the criteria specified by block1.  
The second block provides the name of the extension.  The groupedBy: message is 
implemented using 'groupList' as the collector.".
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

TimeSeries getMessage: "intervalEnd".
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
;

TimeSeries getMessage: "intervalStart".
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
;

TimeSeries getMessage: "isTimeSeries".
setDescriptionTo: "Returns a TRUE value when sent to any time series.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

TimeSeries getMessage: "iterate:by:".
setSelectorTo: "iterate: aBlock by: anInteger" .
setDescriptionTo: 
"Similar to the 'do:' message.  The supplied integer is used to process
recipient TimeSeries as a series of sublists.".
setFunctionTypeTo: "Evaluation".
setLevelTo: "Advanced" .
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Integer .
;

TimeSeries getMessage: "lag:".
setSelectorTo: "lag: aDateOffset" .
setDescriptionTo: 
"Returns the value of the time series as of a date relative to the default 
date.  The derived date is defined at: ^date - aDateOffset.  If no value is 
stored for the derived date, the closest date prior to the supplied date is
used.".
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: DateOffset.
;

TimeSeries getMessage: "lastDate".
setDescriptionTo: "Returns date of the last point in the time series.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Access".
;

TimeSeries getMessage: "lastObservation".
setDescriptionTo: 
"Returns the value stored at the last point in the time series.  If the time 
series is empty, NA is returned.".
setFunctionTypeTo: "Access".
;

TimeSeries getMessage: "latest".
setDescriptionTo: 
"Returns the value of the time series as of the current date.  Identical to 
ts asOf: ^today.".
setFunctionTypeTo: "Access".
;

TimeSeries getMessage: "lead:".
setSelectorTo: "lead: aDateOffset" .
setDescriptionTo: 
"Returns the value of the time series as of a date relative to the default 
date.  The derived date is defined as: ^date + aDateOffset.  If no value is 
stored for the derived date, the closest date prior to the supplied date is
used.".
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: DateOffset.
;

TimeSeries getMessage: "lsGrow".
setDescriptionTo: 
"Computes the least squares growth value of the recipinent time series.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateRange.
;

TimeSeries getMessage: "lsGrowForDateRange:".
setSelectorTo: "lsGrowForDateRange: aDateRange" .
setDescriptionTo: 
"Computes the least squares growth value of the recipient time series over the 
specified date range.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateRange.
;

TimeSeries getMessage: "maxForDateRange:".
setSelectorTo: "maxForDateRange: aDateRange" .
setDescriptionTo: 
"Computes the maximum value of the recipient time series over the specified 
date range.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateRange.
;

TimeSeries getMessage: "median".
setDescriptionTo: "Computes the median value of the recipient time series.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
;

TimeSeries getMessage: "median:".
setSelectorTo: "median: aBlock" .
setDescriptionTo: 
"Computes the median of all points in the time series based on evaluation of 
criteria provided by supplied block.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Block.
;

TimeSeries getMessage: "medianForDateRange:".
setSelectorTo: "medianForDateRange: aDateRange" .
setDescriptionTo: 
"Computes the median value of the recipient time series over the specified 
date range.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateRange.
;

TimeSeries getMessage: "minForDateRange:".
setSelectorTo: "minForDateRange: aDateRange" .
setDescriptionTo: 
"Computes the minimum value of the recipient time series over the specified 
date range.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateRange.
;

TimeSeries getMessage: "mode:".
setSelectorTo: "mode: aBlock" .
setDescriptionTo: 
"Computes the mode of all points in the time series based on evaluation of 
criteria provided by supplied block.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Block.
;

TimeSeries getMessage: "modeForDateRange:".
setSelectorTo: "modeForDateRange: aDateRange" .
setDescriptionTo: 
"Computes the mode value of the recipient time series over the specified 
date range.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateRange.
;

TimeSeries getMessage: "ncount:".
setLevelTo: "Internal" .
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: "NA" .
;

TimeSeries getMessage: "newValueCell".
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
;

TimeSeries getMessage: "newValueCell:".
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: "NA" .
;

TimeSeries getMessage: "nextDate".
setDescriptionTo: 
"Returns the date after the default date.  If there are no points in the time 
series after the default date, NA is returned.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Access".
;

TimeSeries getMessage: "nextDateAsOf:".
setSelectorTo: "nextDateAsOf: aDate" .
setDescriptionTo: 
"Returns the date in the time series after the supplied date. If there are no 
points in the time series after the supplied date, NA is returned.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: Date.
;

TimeSeries getMessage: "pctChangeLag:".
setSelectorTo: "pctChangeLag: aDateOffset" .
setDescriptionTo: 
"Computes the percent change between the value of the time series as of the 
default date and an earlier relative date.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateOffset.
;

TimeSeries getMessage: "pctChangeLead:".
setSelectorTo: "pctChangeLead: aDateOffset" .
setDescriptionTo: 
"Computes the percent change between the value of the time series as of the 
default date and a late relative date.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateOffset.
;

TimeSeries getMessage: "print".
setDescriptionTo: "Redefines the standard print message.".
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Display".
;

TimeSeries getMessage: "productForDateRange:".
setSelectorTo: "productForDateRange: aDateRange" .
setDescriptionTo: 
"Computes the product of the values in the recipient time series over the 
specified date range.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateRange.
;

TimeSeries getMessage: "put:".
setSelectorTo: "put: anObject" .
setDescriptionTo: 
"Assigns supplied value into the time series as of the default date.  This 
message creates a new time point in the time series if the date did not exist.  
Message only works with true time series, not methods (i.e., if formula is a 
method defined to compute a ratio of two numbers, the message can not be used 
to change the value of formula).".
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: Object.
;

TimeSeries getMessage: "select:".
setSelectorTo: "select: aBlock" .
setDescriptionTo: 
"Evaluates the statements provided in supplied block for each point in the time 
series and selects those elements that evaluate to TRUE.".
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
;

TimeSeries getMessage: "send:".
setSelectorTo: "send: aBlock" .
setDescriptionTo: 
"Returns a time series that contains the result of evaluating the statements 
provided in supplied block for each element in the recipient.".
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
;

TimeSeries getMessage: "sortDown:".
setSelectorTo: "sortDown: aBlock" .
setDescriptionTo: 
"Evaluates the statements provided in supplied block and sorts the time series 
in descending order.  Sort does not return a time series.  Sort is highest 
to lowest for numbers.  Sort is ZZZ through A for strings. A List object is
returned.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
;

TimeSeries getMessage: "sortDownBy:then:".
setSelectorTo: "sortDownBy: block1 then: block2 " .
setDescriptionTo: "Two-level descending sort.  A list object is returned." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

TimeSeries getMessage: "sortDownBy:then:then:".
setSelectorTo: "sortDownBy: block1 then: block2 then: block3" .
setDescriptionTo: "Three-level descending sort.  A list object is returned." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
setParameter: 3 typeTo: Block.
;

TimeSeries getMessage: "sortDownBy:then:then:then:".
setSelectorTo: "sortDownBy: block1 then: block2 then: block3 then: block4" .
setDescriptionTo: "Four-level descending sort.  A list object is returned." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
setParameter: 3 typeTo: Block.
setParameter: 4 typeTo: Block.
;

TimeSeries getMessage: "sortUp:".
setSelectorTo: "sortUp: aBlock" .
setDescriptionTo: 
"Evaluates the statements provided in supplied block and sorts the time series 
in ascending order.  Sort does not return a time series.  Sort is lowest to 
highest for numbers.  Sort is A through Z for strings.  A List object is
returned.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
;

TimeSeries getMessage: "sortUpBy:then:".
setSelectorTo: "sortUpBy: block1 then: block2 " .
setDescriptionTo: "Two-level ascending sort.  A list object is returned." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

TimeSeries getMessage: "sortUpBy:then:then:".
setSelectorTo: "sortUpBy: block1 then: block2 then: block3" .
setDescriptionTo: "Three-level ascending sort.  A list object is returned." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
setParameter: 3 typeTo: Block.
;

TimeSeries getMessage: "sortUpBy:then:then:then:".
setSelectorTo: "sortUpBy: block1 then: block2 then: block3 then: block4" .
setDescriptionTo: "Four-level ascending sort.  A list object is returned." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
setParameter: 3 typeTo: Block.
setParameter: 4 typeTo: Block.
;

TimeSeries getMessage: "stdDev".
setDescriptionTo: "Computes the standard deviation of the values in the
recipient time series.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
;

TimeSeries getMessage: "stdDev:".
setSelectorTo: "stdDev: aBlock" .
setDescriptionTo: 
"Computes the standard deviation of all points in the time series based on 
evaluation of criteria provided by supplied block.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Block.
;

TimeSeries getMessage: "stdDevForDateRange:".
setSelectorTo: "stdDevForDateRange: aDateRange" .
setDescriptionTo: 
"Computes the standard deviation of the values in the recipient time series 
over the specified date range.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateRange.
;

TimeSeries getMessage: "toList".
setDescriptionTo: "Converts a TimeSeries into a standard List.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Conversion".
;

TimeSeries getMessage: "totalForDateRange:".
setSelectorTo: "totalForDateRange: aDateRange" .
setDescriptionTo: 
"Computes the total of the values in the recipient time series over the 
specified date range.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateRange.
;

TimeSeries getMessage: "updateWith:".
setSelectorTo: "updateWith: anObject" .
setDescriptionTo: 
"Adds a new point to the time series as of the current evaluation date if
the supplied value is different from the current value.  If the time series
has an NA or Default value as of the earliest possible date and this update 
supplies the first 'real' point in the time series, the value will be stored 
as of earliest possible date as well." .
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: Object.
;

TimeSeries getMessage: "value".
setDescriptionTo: "Evaluates the time series as of the default date.".
setReturnObjectTypeTo: Object.
setFunctionTypeTo: "Evaluation".
;

TimeSeries getMessage: "valueCell".
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
;

TimeSeries getMessage: "valueCell:".
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

TimeSeries getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'TimeSeries'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

#--------------------

SequencedCollection getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: SequencedCollection.
setFunctionTypeTo: "Conversion".
;

SequencedCollection getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

SequencedCollection getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: SequencedCollection.
setFunctionTypeTo: "Schema" .
;

SequencedCollection getMessage: "isSequencedCollection".
setDescriptionTo: "Returns a TRUE value when sent to any SequencedCollection.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

SequencedCollection getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'SequencedCollection'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;


#--------------------

List getMessage: ",".
setSelectorTo: ", anObject" .
setDescriptionTo: 
"Appends supplied element to recipient List.   The , message modifies the 
recipient List.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: Object.
;

List getMessage: "+".
setSelectorTo: "+ anObject" .
setDescriptionTo: 
"This message is used to perform vector addition between two lists or a
list and a scalar value.  If the supplied value is a scalar, the value is
added to each element in the recipient list.  If the supplied value is a
list, the operation is applied positionally." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: List .
;

List getMessage: "-".
setSelectorTo: "- anObject" .
setDescriptionTo: 
"This message is used to perform vector subtraction between two lists or a
list and a scalar value.  If the supplied value is a scalar, the value is
subtracted from each element in the recipient list.  If the supplied value
is a list, the operation is applied positionally." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: List .
;
List getMessage: "*".
setSelectorTo: "* anObject" .
setDescriptionTo: 
"This message is used to perform vector multiplication between two lists 
or a list and a scalar value.  If the supplied value is a scalar, the value
is multiplied by each element in the recipient list.  If the supplied value
is a list, the operation is applied positionally." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: List .
;

List getMessage: "/".
setSelectorTo: "/ anObject" .
setDescriptionTo: 
"This message is used to perform vector division between two lists 
or a list and a scalar value.  If the supplied value is a scalar, the
value divides each element in the recipient list.  If the supplied value
is a list, the operation is applied positionally." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: List .
;


List getMessage: "anElement".
setDescriptionTo: 
"Extracts a single element from the recipient List.  The last element in the 
List is returned.  Useful when dealing with single element lists to strip of 
the last level of protocol.".
setFunctionTypeTo: "Access".
;

List getMessage: "append:".
setSelectorTo: "append: aList" .
setDescriptionTo: 
"Appends the supplied List to a copy of the recipient List.  The original 
list is not altered.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: List.
;

List getMessage: "appendListElementsFrom:".
setSelectorTo: "appendListElementsFrom: aList" .
setDescriptionTo: 
"Appends supplied list to the recipient list, changing the original.  The 
original list IS altered.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: List.
;

List getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Conversion".
;

List getMessage: "at0:".
setSelectorTo: "atO: aNumber" .
setDescriptionTo: 
"Returns the element at the supplied number, zero relative (begins counting 
at 0).  Returns NA if number is greater than number of elements in the list.".
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: Number.
;

List getMessage: "at:".
setSelectorTo: "at: aNumber" .
setDescriptionTo: "Returns the element at the supplied position number.".
setReturnObjectTypeTo: Object.
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: Number.
;

List getMessage: "at:put:".
setSelectorTo: "at: integer put: aValue" .
setDescriptionTo: 
"Stores the supplied value at the supplied integer in the recipient 
List, if the position exists in the list.  The supplied integer must be
a positive number less than or equal to the recipient's count." .
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: Integer .
setParameter: 2 typeTo: Object .
;

List getMessage: "base0CellAtPosition:".
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

List getMessage: "base1CellAtPosition:".
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

List getMessage: "basicRankDown:usingCollector:".
setLevelTo: "Internal" .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
setParameter: 2 typeTo: NA.
;

List getMessage: "basicRankUp:usingCollector:".
setLevelTo: "Internal" .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: NA.
;

List getMessage: "basicRunningTotal:usingCollector:".
setLevelTo: "Internal" .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: NA.
;

List getMessage: "basicSortDown:".
setSelectorTo: "basicSortDown: aBlock" .
setDescriptionTo: 
"Evaluates the statements provided in the supplied block and sorts the list in 
descending order.  Sort is highest to lowest for numbers.  Sort is ZZZ 
through A for strings.".
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Transformation".
setLevelTo: "Internal" .
setParameter: 1 typeTo: Block.
;

List getMessage: "basicAverage:" .
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: Block .
;

List getMessage: "basicMax:" .
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: Block .
;
List getMessage: "basicMin:" .
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: Block .
;

List getMessage: "basicProduct:" .
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: Block .
;

List getMessage: "basicTotal:" .
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: Block .
;

List getMessage: "basicSortUp:".
setSelectorTo: "basicSortUp: aBlock" .
setDescriptionTo: 
"Evaluates the statements provided in the supplied block and sorts the list in 
ascending order.  Sort is lowest to highest for numbers.  Sort is A through Z 
for strings.".
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Transformation".
setLevelTo: "Internal" .
setParameter: 1 typeTo: Block.
;

List getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

List getMessage: "collect:".
setSelectorTo: "collect: aBlock" .
setDescriptionTo: 
"Uses supplied block to create an extension of recipient List.  The value of 
the block is assigned to the variable value.  The returned extension responds 
to the value message.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

List getMessage: "collect:with:".
setLevelTo: "Advanced" .
setSelectorTo: "collect: aBlock with: input " .
setDescriptionTo: 
"Uses supplied block to create an extension of recipient List extended by 
the variable 'value'.  The block is evaluated using the supplied input
parameter." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Object.
;

List getMessage: "collectListElementsFrom:".
setSelectorTo: "collectListElementsFrom: aBlock" .
setDescriptionTo: 
"Evaluates the statements provided in supplied block for each element in 
the list and produces a new list containing these elements.  Used to combine 
a list of lists.  The supplied block must evaluate to a list.  For example,
the expression:
                5 sequence collectListElementsFrom: [ ^self sequence ] 
returns the list elements 1, 1, 2, 1, 2, 3, 1, 2, 3, 4, 1, 2, 3, 4, 5 ".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Creation".
setParameter: 1 typeTo: Block.
;

List getMessage: "copyListElements".
setDescriptionTo: 
"Produces a copy of the recipient list.  Should be used to avoid modifying 
the recipient List.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Creation".
;

List getMessage: "count".
setDescriptionTo: 
"Counts the number of elements in the List.  Returns a non-negative integer.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Inquiry".
;

List getMessage: "createLinks".
setLevelTo: "Reserved" .
setFunctionTypeTo: "Extension".
;

List getMessage: "decileDown:".
setSelectorTo: "decileDown: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in the supplied block for each element in the 
list and computes its decile (highest values = 1; lowest values = 10).".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

List getMessage: "decileUp:".
setSelectorTo: "decileUp: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in the supplied block for each element in the 
list and computes its decile (lowest values = 1; highest = 10).".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

List getMessage: "decileDown:using:".
setSelectorTo: "decileDown: aBlock using: variableName" .
setDescriptionTo: 
"Evaluates the formula provided in the supplied block for each element in the 
list and computes its decile (highest values = 1; lowest values = 10).
The decile value is returned as an extension to the recipient named
by the supplied variableName".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: String .
;

List getMessage: "decileUp:using:".
setSelectorTo: "decileUp: aBlock using: variableName" .
setDescriptionTo: 
"Evaluates the formula provided in the supplied block for each element in the 
list and computes its decile (highest values = 1; lowest values = 10).
The decile value is returned as an extension to the recipient named
by the supplied variableName".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: String .
;

List getMessage: "percentileDown:".
setSelectorTo: "percentileDown: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in the supplied block for each element in the 
list and computes its percentile (highest values = 1; lowest values = 100).".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

List getMessage: "percentileDown:using:".
setSelectorTo: "percentileDown: aBlock using: variableName" .
setDescriptionTo: 
"Evaluates the formula provided in the supplied block for each element in the 
list and computes its percentile (highest values = 1; lowest values = 100).
The percentile value is returned as an extension to the recipient named
by the supplied variableName".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: String .
;

List getMessage: "percentileUp:".
setSelectorTo: "percentileUp: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in the supplied block for each element in the 
list and computes its percentile (lowest values = 1; highest = 100).".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

List getMessage: "percentileUp:using:".
setSelectorTo: "percentileUp: aBlock using: variableName" .
setDescriptionTo: 
"Evaluates the formula provided in the supplied block for each element in the 
list and computes its percentile (lowest values = 1; highest values = 100).
The percentile value is returned as an extension to the recipient named
by the supplied variableName".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: String .
;

List getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Schema".
;

List getMessage: "display:".
setSelectorTo: "display: aBlockList" .
setDescriptionTo: 
"For each element in the recipient list display the list of data items 
specified by aBlockList.  The result of this message is a report with the 
recipient list elements forming the rows and the data items forming the 
columns.  All columns are formatted as 12 characters wide.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Display".
setParameter: 1 typeTo: Block.
;

List getMessage: "displayAcross:".
setSelectorTo: "displayAcross: aBlock" .
setDescriptionTo: 
"Evaluates the statements provided in the supplied block for each element in the 
list and displays the results on one line.  All columns are formatted as 12 
characters wide.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Display".
setParameter: 1 typeTo: Block.
;

List getMessage: "do:".
setSelectorTo: "do: aBlock" .
setDescriptionTo: 
"Evaluates the statements provided in the supplied block for each element in the 
list.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
;

List getMessage: "exclude:".
setSelectorTo: "exclude: aList" .
setDescriptionTo: 
"Finds the elements in the recipient list that are NOT in the supplied list.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: List.
;

List getMessage: "exclude:using:".
setLevelTo: "Advanced" .
setSelectorTo: "exclude: aList using: aBlock " .
setDescriptionTo: 
"Excludes elements in aList from recipient using rules supplied by block
to strip any extensions from elements before comparing for equality." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: List.
setParameter: 2 typeTo:Block .
;

List getMessage: "excludesElement:".
setSelectorTo: "excludesElement: anObject" .
setDescriptionTo: 
"Returns TRUE if recipient List does not include supplied object.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: Object.
;

List getMessage: "execute:in:".
setLevelTo: "Reserved" .
#setSelectorTo: "execute: ... in: ..." .
#setDescriptionTo: "Needs Further Definition.".
# setReturnObjectTypeTo: .
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: NA.
setParameter: 2 typeTo: NA.
;

List getMessage: "extend:".
setSelectorTo: "extend: aBlock" .
setDescriptionTo: "See 'extendBy:' message" .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block .
;

List getMessage: "extendBy:".
setSelectorTo: "extendBy: aBlock" .
setDescriptionTo: 
"Evaluates the supplied block for each element in the list, where the supplied 
block specifies a set of new variables to which each element in the list can 
respond.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

List getMessage: "groupPrintUsing:".
setSelectorTo: "groupPrintUsing: aBlock" .
setDescriptionTo: 
"Groups recipient List by supplied block and displays a report showing the
count in each group." .
setFunctionTypeTo: "Display".
setParameter: 1 typeTo: Block.
;

List getMessage: "groupedBy:".
setSelectorTo: "groupedBy: aBlock" .
setDescriptionTo: 
"Groups the recipient list based on the criteria specified by the supplied 
block.  After the groupedBy: message is sent, each element in the resultant 
list responds to the message groupList which returns the list of elements in 
that group.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
;

List getMessage: "groupedByString:".
setSelectorTo: "groupedByString: aBlock" .
setDescriptionTo: 
"Used as an alternative to 'groupedBy:' when the supplied block returns
string values, this version guarantees that strings with identical 
content will be in the same group.  The standard 'groupedBy:' will
place unique string in separate groups even if they have identical
content." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
;

List getMessage: "groupedBy:and:".
setSelectorTo: "groupedBy: block1 and: block2" .
setDescriptionTo: 
"Groups the recipient list based on the criteria specified by the two
supplied criteria.  Each element in the resultant list responds to the
message 'groupList' which returns the list of elements in that group.
The value of each group is defined as the string formed by:
     first criteria value.second critria value" .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

List getMessage: "groupedBy:usingCollector:".
setSelectorTo: "groupedBy: aBlock usingCollector: aBlock" .
setLevelTo: "Internal" .
setDescriptionTo: 
"Groups the recipient list based on the criteria specified by block1.  The 
second block provides the name of the extension.  The 'groupedBy:' message 
is implemented using groupList as the collector.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

List getMessage: "groupedBy:usingCutoffs:".
setSelectorTo: "groupedBy: aBlock usingCutoffs: aList" .
setDescriptionTo: 
"Groups recipient List into partitions specified in aList, based on criteria 
specified in aBlock.  The supplied list should be a list of Numbers.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: List.
;

List getMessage: "histogram:title:".
setSelectorTo: "histogram: aBlock title: aString" .
setDescriptionTo: 
"Produces a distribution chart based on each element's value of the
supplied block.".
setFunctionTypeTo: "Display".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: String.
;

List getMessage: "includesElement:".
setSelectorTo: "includesElement: anObject" .
setDescriptionTo: 
"Returns TRUE if the recipient list does include the supplied object." .
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: Object.
;

List getMessage: "intersect:".
setSelectorTo: "intersect: aList" .
setDescriptionTo: 
"Finds the elements that are in BOTH the recipient list and supplied list".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: List.
;

List getMessage: "intersect:using:".
setLevelTo: "Advanced" .
setSelectorTo: "intersect: aList using: aBlock " .
setDescriptionTo: 
"Finds intersection of elements in aList and recipient  using rules supplied
by block to strip any extensions from elements before comparing for 
equality." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: List.
setParameter: 2 typeTo: Block.
;

List getMessage: "isList".
setDescriptionTo: "Returns a TRUE value when sent to any List.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

List getMessage: "iterate:by:".
setSelectorTo: "iterate: aBlock by: anInteger" .
setDescriptionTo: 
"Similar to the 'do:' message.  The supplied integer is used to process
recipient List as a series of sublists.".
setFunctionTypeTo: "Evaluation".
setLevelTo: "Advanced" .
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Integer .
;

List getMessage: "median:".
setSelectorTo: "median: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in the supplied block for each element in the 
list and computes the median (middle) value.  NA values are not included.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Block.
;

List getMessage: "mgroupedBy:".
setSelectorTo: "mgroupedBy: aBlock" .
setDescriptionTo: 
"Groups the recipient list using result of block, where supplied block should
generate a list as its result.  Elements in the original list will be
included in one or more groupLists.  For example, if instances of the class
EntityCategory respond to the message 'entities' with the list of entities
included in the instance, then the expression:

     EntityCategory masterList mgroupedBy: [ entities ] 

returns a list of the entities included in any EntityCategory where each
element of this new list responds to the 'groupList' message with the
list of EntityCategories that include the element." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
;


List getMessage: "mode:".
setSelectorTo: "mode: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in aBlock for each element in the list and 
computes the mode (most frequently occurring value).  NA values are not 
included.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Block.
;

List getMessage: "ncount:".
setLevelTo: "Internal" .
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: NA.
;

List getMessage: "nonDefaults".
setDescriptionTo:
"Selects any element in the recipient collection that is not a default." .
setReturnObjectTypeTo: Collection.
setFunctionTypeTo: "Transformation".
;

List getMessage: "numberElements".
setDescriptionTo:
 "Extends each element in the list by the variable 'position' which 
corresponds to its position number in the list." .
setReturnObjectTypeTo: Collection.
setFunctionTypeTo: "Extension".
;

List getMessage: "numberElementsInReverse".
setDescriptionTo:
"Extends each element in the list by the variable 'position' which 
corresponds to its position number in the list when the list is
sorted in reverse order." .
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Extension".
;

List getMessage: "print".
setDescriptionTo: "Redefines the standard print message.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Display".
;

List getMessage: "quintileDown:".
setSelectorTo: "quintileDown: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in aBlock for each element in the list and 
computes its quintile (highest values = 1; lowest values = 5)".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

List getMessage: "quintileUp:".
setSelectorTo: "quintileUp: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in aBlock for each element in the list and 
computes its quintile (lowest values = 1; highest = 5).".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

List getMessage: "quintileDown:using:".
setSelectorTo: "quintileDown: aBlock using: variableName" .
setDescriptionTo: 
"Evaluates the formula provided in the supplied block for each element in the 
list and computes its quintile (highest values = 1; lowest values = 5).
The quintile value is returned as an extension to the recipient named
by the supplied variableName".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: String .
;

List getMessage: "quintileUp:using:".
setSelectorTo: "quintileUp: aBlock using: variableName" .
setDescriptionTo: 
"Evaluates the formula provided in the supplied block for each element in the 
list and computes its quintile (lowest values = 1; highest values = 5).
The quintile value is returned as an extension to the recipient named
by the supplied variableName".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: String .
;

List getMessage: "rankDown:".
setSelectorTo: "rankDown: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in the supplied block for each element in the 
list and computes its rank (highest value = 1).  Returns the recipient list 
extended by the property 'rank'.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

List getMessage: "rankDown:usingCollector:" .
setSelectorTo: "rankDown: block1 usingCollector: block2" .
setLevelTo: "Internal" .
setDescriptionTo: 
"Ranks down the recipient List based on the criteria specified by block1.  
The second block provides the name of the extension. " .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

List getMessage: "rankUp:".
setSelectorTo: "rankUp: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in the supplied block for each element in the 
list and computes its rank (lowest value = 1). Returns a copy of the recipient 
list extended by the property 'rank'.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

List getMessage: "rankUp:usingCollector:".
setSelectorTo: "rankUp: block1 usingCollector: block2" .
setLevelTo: "Internal" .
setDescriptionTo: 
"Ranks up the recipient List based on the criteria specified by block1.  
The second block provides the name of the extension. " .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

List getMessage: "runningTotal:".
setSelectorTo: "runningTotal: aBlock" .
setDescriptionTo: 
"Computes the running total for each element in the list based on the 
supplied block.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

List getMessage: "runningAverage:".
setSelectorTo: "runningAverage: aBlock" .
setDescriptionTo: 
"Extends each element in the list by the variable 'runningAverage' which 
is the computed running average for the element based on the supplied block." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

List getMessage: "runningTotal:usingCollector:".
setSelectorTo: "runningTotal: block1 usingCollector: block2" .
setLevelTo: "Internal" .
setDescriptionTo: 
"Computes the running total for the recipient timeSeries based on the criteria 
specified by block1.  The second block provides the name of the extension. " .
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

List getMessage: "select:".
setSelectorTo: "select: aBlock" .
setDescriptionTo: 
"Evaluates the statements provided in the supplied block for each element in the 
list and selects those elements that evaluate to TRUE.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
;

List getMessage: "send:".
setSelectorTo: "send: aBlock" .
setDescriptionTo: 
"Returns a list that contains the result of evaluating the statements provided 
in the supplied block for each element in the recipient.  The List returned has 
the same number of elements as the recipient, with each element being the 
result of evaluating the block.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
;

List getMessage: "send:with:".
setSelectorTo: "send: aBlock with: aValue " .
setDescriptionTo: 
"Sends the block to each element of the recipient.  The block is evaluated
with aValue as a parameter" .
setLevelTo: "Advanced" .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Object .
;

List getMessage: "sortDown:".
setSelectorTo: "sortDown: aBlock" .
setDescriptionTo: 
"Evaluates the statements provided in the supplied block and sorts the list in 
descending order.  Sort is highest to lowest for numbers.  Sort is ZZZ 
through A for strings.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
;

List getMessage: "sortDownBy:then:".
setSelectorTo: "sortDownBy: block1 then: block2 " .
setDescriptionTo: "Two-level descending sort." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

List getMessage: "sortDownBy:then:then:".
setSelectorTo: "sortDownBy: block1 then: block2 then: block3" .
setDescriptionTo: "Three-level descending sort." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
setParameter: 3 typeTo: Block.
;

List getMessage: "sortDownBy:then:then:then:".
setSelectorTo: "sortDownBy: block1 then: block2 then: block3 then: block4" .
setDescriptionTo: "Four-level descending sort." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
setParameter: 3 typeTo: Block.
setParameter: 4 typeTo: Block.
;

List getMessage: "sortUp:".
setSelectorTo: "sortUp: aBlock" .
setDescriptionTo: 
"Evaluates the statements provided in the supplied block and sorts the list in 
ascending order.  Sort is lowest to highest for numbers.  
Sort is A through Z for strings.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
;

List getMessage: "sortUpBy:then:".
setSelectorTo: "sortUpBy: block1 then: block2 " .
setDescriptionTo: "Two-level ascending sort." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

List getMessage: "sortUpBy:then:then:".
setSelectorTo: "sortUpBy: block1 then: block2 then: block3" .
setDescriptionTo: "Three-level ascending sort." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
setParameter: 3 typeTo: Block.
;

List getMessage: "sortUpBy:then:then:then:".
setSelectorTo: "sortUpBy: block1 then: block2 then: block3 then: block4" .
setDescriptionTo: "Four-level ascending sort." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
setParameter: 3 typeTo: Block.
setParameter: 4 typeTo: Block.
;

List getMessage: "standardize".
setLevelTo: "Advanced"  .
setDescriptionTo: "Strips off one layer of extension from recipient.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
;

List getMessage: "stdDev:".
setSelectorTo: "stdDev: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in aBlock for each element in the list and 
computes the standard deviation of the values.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Block.
;

List getMessage: "tileDown:usingCollector:tiles:" .
setSelectorTo: "tileDown: block1 usingCollector: block2 tiles: aNumber" .
setLevelTo: "Internal" .
setDescriptionTo: 
"Divides recipient list into number of tiles specified based on criteria 
provided in block1; extends recipient by its tile value with the extension 
being named by the collector provided in block2.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
setParameter: 3 typeTo: Number.
;

List getMessage: "tileDown:using:tiles:".
setSelectorTo: "tileDown: block using: variableName tiles: aNumber" .
setDescriptionTo: 
"Divides recipient list into number of tiles specified based on criteria 
provided in block; extends recipient by its tile value with the extension 
being named by the string provided in variableName.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: String .
setParameter: 3 typeTo: Number.
;

List getMessage: "tileUp:usingCollector:tiles:".
setSelectorTo: "tileUp: block1 usingCollector: block2 tiles: aNumber" .
setLevelTo: "Internal" .
setDescriptionTo: 
"Divides recipient list into number of tiles specified based on criteria 
provided in block1; extends recipient by its tile value with the extension 
being named by the collector provided in block2.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
setParameter: 3 typeTo: Number.
;

List getMessage: "tileUp:using:tiles:".
setSelectorTo: "tileUp: block using: variableName tiles: aNumber" .
setDescriptionTo: 
"Divides recipient list into number of tiles specified based on criteria 
provided in block; extends recipient by its tile value with the extension 
being named by the string provided in variableName.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: String .
setParameter: 3 typeTo: Number.
;

List getMessage: "tileDown:tiles:" .
setSelectorTo: "tileDown: block1 tiles: aNumber" .
setDescriptionTo: 
"Divides recipient list into number of tiles specified based on criteria 
provided in block1; extends recipient by its tile value with the extension 
'tile'." .
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Number.
;

List getMessage: "tileUp:tiles:" .
setSelectorTo: "tileUp: block1 tiles: aNumber" .
setDescriptionTo: 
"Divides recipient list into number of tiles specified based on criteria 
provided in block1; extends recipient by its tile value with the extension 
'tile'." .
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Number.
;


List getMessage: "toList" .
setDescriptionTo: "Returns the recipient List." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Conversion".
;

List getMessage: "uniformAt0:".
setSelectorTo: "uniformAt0: aNumber" .
setDescriptionTo: 
"Returns the element at the supplied number, begins counting at zero 
(0 relative). If supplied number is greater than the number of elements in the 
list, the first element is returned.".
setReturnObjectTypeTo: Object.
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: Number.
;

List getMessage: "uniformAt:".
setSelectorTo: "uniformAt: aNumber" .
setDescriptionTo: 
"Returns the element at the supplied number. If supplied number is greater than 
the number of elements in the list, the first element is returned.".
setReturnObjectTypeTo: Object.
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: Number.
;

List getMessage: "union:".
setSelectorTo: "union: aList" .
setDescriptionTo: 
"Finds the elements that are in EITHER or BOTH the recipient list and aList.  
Only unique elements are returned.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: List.
;

List getMessage: "union:using:".
setSelectorTo: "union: aList using: aBlock" .
setLevelTo: "Advanced" .
setDescriptionTo:
"Finds union of elements in aList and recipient  using rules supplied
by block to strip any extensions from elements before comparing for 
equality." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
setParameter: 1 typeTo: List.
setParameter: 2 typeTo: Block.
;

List getMessage: "unique".
setDescriptionTo: "Finds the unique elements in the list.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Transformation".
;

List getMessage: "weightedDecile:".
setSelectorTo: "weightedDecile: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in aBlock for each element in the list and 
computes its weighted decile (lowest values = 1; highest values = 10).  The 
result is each decile contains approximately the same total value for the block 
provided.  The recipient list is returned sorted from lowest to highest based 
on supplied Block.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

List getMessage: "weightedQuintile:".
setSelectorTo: "weightedQuintile: aBlock" .
setDescriptionTo: 
"Evaluates the formula provided in the supplied block for each element in the 
list and computes its weighted quintile (lowest values = 1; highest values = 5).  
The result is each quintile contains approximately the same total value for the 
block provided.  The recipient list is returned sorted from lowest to highest 
based on the supplied block".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
;

List getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'List'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

#--------------------
#--------------------

Value getMessage: "<-".
setSelectorTo: "<- aValue" .
setDescriptionTo: "Assigns supplied value to recipient value cell.".
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: Value.
;

Value getMessage: "asOf:".
setSelectorTo: "asOf: aDate" .
setDescriptionTo: 
"Returns the value of the Value as of the supplied date.  If no value is stored 
for the date, the closest date prior to the supplied date is used.".
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: Date.
;

Value getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setFunctionTypeTo: "Conversion".
;

Value getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

Value getMessage: "decrement".
setSelectorTo: "decrement" .
setDescriptionTo: 
"This message is used to decrement the value of a fixed property by 1.
The message is sent to the intensional form of the property.  For example:
     !x <- 10 ;
     :x decrement ;
     x printNL ;" .
setFunctionTypeTo: "Update".
;

Value getMessage: "decrementBy:".
setSelectorTo: "decrementBy: integer" .
setDescriptionTo: 
"This message is used to decrement the value of a fixed property by
the amount supplied as the parameter.  The message is sent to the
intensional form of the property.  For example:
     !x <- 10 ;
     :x decrementBy: 3 ;
     x printNL ;" .
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: Integer.
;

Value getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: Value.
setFunctionTypeTo: "Schema" .
;

Value getMessage: "increment".
setSelectorTo: "increment" .
setDescriptionTo: 
"This message is used to increment the value of a fixed property by 1.
The message is sent to the intensional form of the property.  For example:
     !x <- 10 ;
     :x increment ;
     x printNL ;" .
setFunctionTypeTo: "Update".
;

Value getMessage: "incrementBy:".
setSelectorTo: "incrementBy: integer" .
setDescriptionTo: 
"This message is used to increment the value of a fixed property by
the amount supplied as the parameter.  The message is sent to the
intensional form of the property.  For example:
     !x <- 10 ;
     :x incrementBy: 3 ;
     x printNL ;" .
setFunctionTypeTo: "Update".
setParameter: 1 typeTo: Integer.
;

Value getMessage: "isValue".
setDescriptionTo: "Returns a TRUE value when sent to any value.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

Value getMessage: "print".
setDescriptionTo: 
"Redefines the standard print message to print out the intensional form of the 
value cell.".
setFunctionTypeTo: "Display".
;

Value getMessage: "value".
setDescriptionTo: "Evaluates the recipient.".
setFunctionTypeTo: "Evaluation".
;

Value getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'Value'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

#--------------------------------------------------

Collection getMessage: "rankDown:by:".
setSelectorTo: "rankDown: aBlock by: bBlock" .
setDescriptionTo: 
"Groups the recipient by the criteria in bBlock and generates a rank value
for each element relative to its group. The result object is the original
collection extended by the variable 'rank'.  " .
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

Collection getMessage: "rankDown:by:by:".
setSelectorTo: "rankDown: aBlock by: bBlock by: cBlock" .
setDescriptionTo: 
"Groups the recipient by the criteria in bBlock and cBlock and generates a
rank value for each element relative to its group. The result object 
is the original collection extended by the variable 'rank'.  " .
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

Collection getMessage: "rankUp:by:".
setSelectorTo: "rankUp: aBlock by: bBlock" .
setDescriptionTo: 
"Groups the recipient by the criteria in bBlock and generates a rank value
for each element relative to its group. The result object is the original
collection extended by the variable 'rank'.  " .
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

Collection getMessage: "rankUp:by:by:".
setSelectorTo: "rankUp: aBlock by: bBlock by: cBlock" .
setDescriptionTo: 
"Groups the recipient by the criteria in bBlock and cBlock and generates a
rank value for each element relative to its group. The result object 
is the original collection extended by the variable 'rank'.  " .
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

Collection getMessage: "decileDown:by:".
setSelectorTo: "decileDown: aBlock by: bBlock" .
setDescriptionTo: 
"Groups the recipient by the criteria in bBlock and generates a decile value
for each element relative to its group. The result object is the original
collection extended by the variable 'decile'.  " .
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

Collection getMessage: "decileDown:by:by:".
setSelectorTo: "decileDown: aBlock by: bBlock by: cBlock" .
setDescriptionTo: 
"Groups the recipient by the criteria in bBlock and cBlock and generates a
decile value for each element relative to its group. The result object 
is the original collection extended by the variable 'decile'.  " .
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

Collection getMessage: "decileUp:by:".
setSelectorTo: "decileUp: aBlock by: bBlock" .
setDescriptionTo: 
"Groups the recipient by the criteria in bBlock and generates a decile value
for each element relative to its group. The result object is the original
collection extended by the variable 'decile'.  " .
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

Collection getMessage: "decileUp:by:by:".
setSelectorTo: "decileUp: aBlock by: bBlock by: cBlock" .
setDescriptionTo: 
"Groups the recipient by the criteria in bBlock and cBlock and generates a
decile value for each element relative to its group. The result object 
is the original collection extended by the variable 'decile'.  " .
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

Collection getMessage: "percentileDown:by:".
setSelectorTo: "percentileDown: aBlock by: bBlock" .
setDescriptionTo: 
"Groups the recipient by the criteria in bBlock and generates a percentile value
for each element relative to its group. The result object is the original
collection extended by the variable 'percentile'.  " .
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

Collection getMessage: "percentileDown:by:by:".
setSelectorTo: "percentileDown: aBlock by: bBlock by: cBlock" .
setDescriptionTo: 
"Groups the recipient by the criteria in bBlock and cBlock and generates a
percentile value for each element relative to its group. The result object 
is the original collection extended by the variable 'percentile'.  " .
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

Collection getMessage: "percentileUp:by:".
setSelectorTo: "percentileUp: aBlock by: bBlock" .
setDescriptionTo: 
"Groups the recipient by the criteria in bBlock and generates a percentile value
for each element relative to its group. The result object is the original
collection extended by the variable 'percentile'.  " .
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

Collection getMessage: "percentileUp:by:by:".
setSelectorTo: "percentileUp: aBlock by: bBlock by: cBlock" .
setDescriptionTo: 
"Groups the recipient by the criteria in bBlock and cBlock and generates a
percentile value for each element relative to its group. The result object 
is the original collection extended by the variable 'percentile'.  " .
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

Collection getMessage: "quintileDown:by:".
setSelectorTo: "quintileDown: aBlock by: bBlock" .
setDescriptionTo: 
"Groups the recipient by the criteria in bBlock and generates a quintile value
for each element relative to its group. The result object is the original
collection extended by the variable 'quintile'.  " .
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

Collection getMessage: "quintileDown:by:by:".
setSelectorTo: "quintileDown: aBlock by: bBlock by: cBlock" .
setDescriptionTo: 
"Groups the recipient by the criteria in bBlock and cBlock and generates a
quintile value for each element relative to its group. The result object 
is the original collection extended by the variable 'quintile'.  " .
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

Collection getMessage: "quintileUp:by:".
setSelectorTo: "quintileUp: aBlock by: bBlock" .
setDescriptionTo: 
"Groups the recipient by the criteria in bBlock and generates a quintile value
for each element relative to its group. The result object is the original
collection extended by the variable 'quintile'.  " .
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

Collection getMessage: "quintileUp:by:by:".
setSelectorTo: "quintileUp: aBlock by: bBlock by: cBlock" .
setDescriptionTo: 
"Groups the recipient by the criteria in bBlock and cBlock and generates a
quintile value for each element relative to its group. The result object 
is the original collection extended by the variable 'quintile'.  " .
setReturnObjectTypeTo: Collection .
setFunctionTypeTo: "Extension".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Block.
;

