?f
?d
"
*******************
*******************
*****  Setup  *****
*******************
*******************
" print,

"
***************************************
*****  Q-Register Symbolic Names  *****
***************************************
" print,

"*****  Specific Objects  *****",
TheScalarPTokenQ <- #qa,
TheScalarPToken  <- TheScalarPTokenQ referent,

ObjectQ <- #qb,
Object  <- ObjectQ referent,

FunctionQ <- #qc,
Function  <- FunctionQ referent,

ComputedFunctionQ <- #qd,
ComputedFunction  <- ComputedFunctionQ referent,

PrimitiveFnQ <- #qe,
PrimitiveFn  <- PrimitiveFnQ referent,

EnumeratedFunctionQ <- #qf,
EnumeratedFunction  <- EnumeratedFunctionQ referent,

CollectionQ <- #qg,
Collection  <- CollectionQ referent,

SequencedCollectionQ <- #qh,
SequencedCollection  <- SequencedCollectionQ referent,

IndexedCollectionQ <- #qi,
IndexedCollection  <- IndexedCollectionQ referent,

BooleanQ <- #qj,
Boolean  <- BooleanQ referent,

OrdinalQ <- #qk,
Ordinal  <- OrdinalQ referent,

NumberQ <- #ql,
Number  <- NumberQ referent,

UndefinedQ <- #qm,
Undefined  <- UndefinedQ referent,

"*****  Temporary Variables  *****",
methodDictionaryQ <- #qx,
methodDictionary  <- methodDictionaryQ referent,

vStoreQ <- #qy,
vStore  <- vStoreQ referent,

superVStoreQ <- #qz,
superVStore  <- superVStoreQ referent,

"
***********************************************
*****  Primitively Boot Strapped Objects  *****
***********************************************
" print,

TheScalarPTokenQ
    set: #ENVIR scalarPToken,
ObjectQ
    set: #ENVIR objectPrototype,
UndefinedQ
    set: #ENVIR undefinedBehavior,


"
******************************
******************************
*****  Function Classes  *****
******************************
******************************
" print,

"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 !!!!!  WARNING:							 !!!!!
 !!!!!		Primitive methods cannot be added to method		 !!!!!
 !!!!!		dictionaries until 'PrimitiveFn' is defined.		 !!!!!
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
",

"
************************
*****  'Function'  *****
************************
" print,

FunctionQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: Object rowPToken)
	    in:
		Object
	    withMethods:
		#RTYPE_C_Dictionary new),

"
***************************************
*****  Computed Function Classes  *****
***************************************
" print,

"
************************
*  'ComputedFunction'  *
************************
" print,

ComputedFunctionQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: Function rowPToken)
	    in:
		Function
	    withMethods:
		#RTYPE_C_Dictionary new),

"
*******************
*  'PrimitiveFn'  *
*******************
" print,

PrimitiveFnQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: ComputedFunction rowPToken)
	    in:
		ComputedFunction
	    withMethods:
		#RTYPE_C_Dictionary new),

#ENVIR setPrimitiveFnBehavior: PrimitiveFn,

"
**************************************************************************
*  'Function', 'ComputedFunction', and 'PrimitiveFn' Method Definitions  *
**************************************************************************
" print,

"
*****  'Function'  *****
" print,

methodDictionaryQ set: Function methodDictionary,

methodDictionary
,
Function align,

"
*****  'ComputedFunction'  *****
" print,

methodDictionaryQ set: ComputedFunction methodDictionary,

methodDictionary
,
ComputedFunction align,

"
*****  'PrimitiveFn'  *****
" print,

methodDictionaryQ set: PrimitiveFn methodDictionary,

methodDictionary
    at:  "print"		put: #PrintPrimitive
,
PrimitiveFn align,

"
***************
*  'Closure'  *
***************
" print,

"*****  Create the method dictionary...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

methodDictionary
    at:  "selector"		put: #ClosureSelector
;   at:  "asMethod"		put: #ClosureAsMethod
;   at:  "value"		put: #EvaluateForValue
;   at:  "environment"		put: #EvaluateForEnvironment
,

"*****  ...and create the value store.  *****",
vStoreQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: ComputedFunction rowPToken)
	    in:
		ComputedFunction
	    withMethods:
		methodDictionary),

#ENVIR setClosureBehavior: vStore,

"
**************
*  'Method'  *
**************
" print,

"*****  Create the method dictionary...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

"*****  ...and the value store.  *****",
vStoreQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: ComputedFunction rowPToken)
	    in:
		ComputedFunction
	    withMethods:
		methodDictionary),

#ENVIR setMethodBehavior: vStore,

"
*****************************************
*****  Enumerated Function Classes  *****
*****************************************
" print,

"
**************************
*  'EnumeratedFunction'  *
**************************
" print,

"*****  Create the method dictionary...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

"*****  ...and the value store.  *****",
EnumeratedFunctionQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: Function rowPToken)
	    in:
		Function
	    withMethods:
		#RTYPE_C_Dictionary new),

"
*************
*  'Value'  *
*************
" print,

"*****  Create the method dictionary...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

methodDictionary
    at:  "<-"			put: #Assign
,

"*****  ...and the value store.  *****",
vStoreQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: EnumeratedFunction rowPToken)
	    in:
		EnumeratedFunction
	    withMethods:
		methodDictionary),

#ENVIR setValueBehavior: vStore,

"
******************
*  'Collection'  *
******************
" print,

"*****  Create the method dictionary...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

"*****  ...and the value store.  *****",
CollectionQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: EnumeratedFunction rowPToken)
	    in:
		EnumeratedFunction
	    withMethods:
		methodDictionary),

"
***************************
*  'SequencedCollection'  *
***************************
" print,

"*****  Create the method dictionary...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

"*****  ...and the value store.  *****",
SequencedCollectionQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: Collection rowPToken)
	    in:
		Collection
	    withMethods:
		methodDictionary),

"
************
*  'List'  *
************
" print,

"*****  Define methods...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

methodDictionary
    at:  "new"			put: #NewList
,

"*****  ...and create the value store.  *****",
vStoreQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: SequencedCollection rowPToken)
	    in:
		SequencedCollection
	    withMethods:
		methodDictionary),

"-----  Declare the 'List' Prototype:  -----",
#ENVIR setListPrototype: vStore,

"
*************************
*  'IndexedCollection'  *
*************************
" print,

"*****  Create the method dictionary...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

"*****  ...and the value store.  *****",
IndexedCollectionQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: Collection rowPToken)
	    in:
		Collection
	    withMethods:
		methodDictionary),

"
******************
*  'TimeSeries'  *
******************
" print,

"*****  Create the method dictionary...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

methodDictionary
    at:  "new"			put: #NewTimeSeries
,

"*****  ...and the value store.  *****",
vStoreQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: IndexedCollection rowPToken)
	    in:
		IndexedCollection
	    withMethods:
		methodDictionary),

#ENVIR setTimeVaryingValueBehavior: vStore,

"
*******************
*  'IndexedList'  *
*******************
" print,

"*****  Create the method dictionary...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

methodDictionary
    at:  "new"			put: #NewAList
,

"*****  ...and the value store.  *****",
vStoreQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: IndexedCollection rowPToken)
	    in:
		IndexedCollection
	    withMethods:
		methodDictionary),

#ENVIR setAListPrototype: vStore,

"
************************************
************************************
*****  Undefined Data Classes  *****
************************************
************************************
" print,

"
***************
*  'NoValue'  *
***************
" print,

"*****  Create the method dictionary...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

methodDictionary
    at:  "print"		put: #ReturnSelf
,

"*****  ...and the value store.  *****",
vStoreQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_UndefUV
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: Undefined rowPToken)
	    in:
		Undefined
	    withMethods:
		methodDictionary),

#ENVIR setNoValueBehavior: vStore,

"
****************************
****************************
*****  Symbol Classes  *****
****************************
****************************
" print,

"
**************
* 'Selector' *
**************
" print,

"*****  Create the method dictionary...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

methodDictionary
    at:  "print"			put: #PrintSelector
;   at:  "locateOrAddInDictionaryOf:"	put: #LocateOrAddEntryInDictionaryOf
,

"*****  ...and the value store.  *****",
vStoreQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#ENVIR theSelectorPToken)
		    referencing: Object rowPToken)
	    in:
		Object
	    withMethods:
		methodDictionary),

#ENVIR setSelectorBehavior: vStore,

"
*****************************
*****************************
*****  Boolean Classes  *****
*****************************
*****************************
" print,

"
***************
*  'Boolean'  *
***************
" print,

"*****  Create the method dictionary...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

"*****  ...and the value store.  *****",
BooleanQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: Object rowPToken)
	    in:
		Object
	    withMethods:
		methodDictionary),

"
************
*  'True'  *
************
" print,

"*****  Create the method dictionary...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

"*****  ...and create the value store.  *****",
vStoreQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: Boolean rowPToken)
	    in:
		Boolean
	    withMethods:
		methodDictionary),

#ENVIR setTrueBehavior: vStore,

"
*************
*  'False'  *
*************
" print,

"*****  Create the method dictionary...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

"*****  ...and the value store.  *****",
vStoreQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: Boolean rowPToken)
	    in:
		Boolean
	    withMethods:
		methodDictionary),

#ENVIR setFalseBehavior: vStore,

"
******************************
******************************
*****  Ordinal Classes  ******
******************************
******************************
" print,

"
***************
*  'Ordinal'  *
***************
" print,

"*****  Define methods...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

"*****  ...and create the value store.  *****",
OrdinalQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: Object rowPToken)
	    in:
		Object
	    withMethods:
		methodDictionary),

"
**************
*  'Number'  *
**************
" print,

"*****  Create the method dictionary...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

"*****  ...and the value store.  *****",
NumberQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: Ordinal rowPToken)
	    in:
		Ordinal
	    withMethods:
		methodDictionary),

"
**************
*  'Double'  *
**************
" print,

"*****  Create the method dictionary...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

"*****  ...and the value store.  *****",
vStoreQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: Number rowPToken)
	    in:
		Number
	    withMethods:
		methodDictionary),

#ENVIR setDoubleBehavior: vStore,

"
*************
*  'Float'  *
*************
" print,

"*****  Create the method dictionary...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

"*****  ...and the value store.  *****",
vStoreQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: Number rowPToken)
	    in:
		Number
	    withMethods:
		methodDictionary),

#ENVIR setFloatBehavior: vStore,

"
***************
*  'Integer'  *
***************
" print,

"*****  Create the method dictionary...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

methodDictionary
    at:  "asDate"		put: #IntegerAsDate
;   at:  "asFloat"		put: #IntegerAsFloat
;   at:  "asPointerTo:"		put: #ValueAsPointerTo
,

"*****  ...and the value store.  *****",
vStoreQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: Number rowPToken)
	    in:
		Number
	    withMethods:
		methodDictionary),

#ENVIR setIntegerBehavior: vStore,

"
************
*  'Date'  *
************
" print,

"*****  Create the method dictionary...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

"*****  ...and value store.  *****",
vStoreQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: Ordinal rowPToken)
	    in:
		Ordinal
	    withMethods:
		methodDictionary),

#ENVIR setDateBehavior: vStore,

"
*****************
*  'Character'  *
*****************
" print,

"*****  Create the method dictionary...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

"*****  ...and value store.  *****",
vStoreQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: Ordinal rowPToken)
	    in:
		Ordinal
	    withMethods:
		methodDictionary),

#ENVIR setCharacterBehavior: vStore,

"
**************
*  'String'  *
**************
" print,

"*****  Define methods...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

methodDictionary
    at: "print"		put: #PrintString
,

"*****  ...and create the value store.  *****",
vStoreQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: Ordinal rowPToken)
	    in:
		Ordinal
	    withMethods:
		methodDictionary),

"-----  Declare the 'String' Prototype:  -----",
#ENVIR setStringPrototype: vStore,

"
********************************************
********************************************
*****  Property Classes and Instances  *****
********************************************
********************************************
" print,

"
****************
*  'Property'  *
****************
" print,

"*****  Define methods...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

"*****  ...and create the value store.  *****",
vStoreQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: Object rowPToken)
	    in:
		Object
	    withMethods:
		methodDictionary),

"-----  Declare the 'Property' Prototype:  -----",
#ENVIR setPropertyPrototype: vStoreQ,

"
*****  The Default Properties  *****
" print,

#ENVIR
    setTheDefaultProperty:
	(#RTYPE_C_Descriptor
	    new: (#ENVIR propertyPrototype addRows: TheScalarPToken)
	    in:  #ENVIR propertyPrototype)
;   setTheDefaultListProperty:
	(#RTYPE_C_Descriptor
	    new: (#ENVIR propertyPrototype addRows: TheScalarPToken)
	    in:  #ENVIR propertyPrototype)
;   setTheDefaultStringProperty:
	(#RTYPE_C_Descriptor
	    new: (#ENVIR propertyPrototype addRows: TheScalarPToken)
	    in:  #ENVIR propertyPrototype),

"
***************************
*  'TimeVaryingProperty'  *
***************************
" print,

"*****  Create the method dictionary...  *****",
methodDictionaryQ set: #RTYPE_C_Dictionary new,

"*****  ...and the value store.  *****",
vStoreQ
    set:
        (#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: #ENVIR propertyPrototype rowPToken)
	    in:
		#ENVIR propertyPrototype
	    withMethods:
		methodDictionary),

"-----  Declare the 'TimeVaryingProperty' Prototype:  -----",
#ENVIR setTimeVaryingPropertyPrototype: vStoreQ,

"
*****  The Default Time Varying Properties  *****
" print,

#ENVIR
    setTheDefaultTimeVaryingProperty:
	(#RTYPE_C_Descriptor
	    new: (#ENVIR timeVaryingPropertyPrototype addRows: TheScalarPToken)
	    in:  #ENVIR timeVaryingPropertyPrototype)
;   setTheDefaultTimeVaryingListProperty:
	(#RTYPE_C_Descriptor
	    new: (#ENVIR timeVaryingPropertyPrototype addRows: TheScalarPToken)
	    in:  #ENVIR timeVaryingPropertyPrototype)
;   setTheDefaultTimeVaryingStringProperty:
	(#RTYPE_C_Descriptor
	    new: (#ENVIR timeVaryingPropertyPrototype addRows: TheScalarPToken)
	    in:  #ENVIR timeVaryingPropertyPrototype),

"
*******************************
*******************************
*****  Dictionary Access  *****
*******************************
*******************************
" print,

"
*****  'Closure'  *****
" print,

#ENVIR
    setClosureDictionary: #ENVIR closureBehavior methodDictionary,

"
*****  'Method'  *****
" print,

#ENVIR
    setMethodDictionary: #ENVIR methodBehavior methodDictionary,

"
*****  'Value'  *****
" print,

#ENVIR
    setValueDictionary: #ENVIR valueBehavior methodDictionary,

"
*****  'TimeSeries'  *****
" print,

#ENVIR
    setTimeVaryingValueDictionary:
	#ENVIR timeVaryingValueBehavior methodDictionary,

"
*****  'IndexedList'  *****
" print,

#ENVIR setAListDictionary: #ENVIR aListPrototype methodDictionary,

"
*****  'List'  *****
" print,

#ENVIR setListDictionary: #ENVIR listPrototype methodDictionary,

"
*****  'Selector'  *****
" print,

#ENVIR setSelectorDictionary: #ENVIR selectorBehavior methodDictionary,

"
*****  'String'  *****
" print,

#ENVIR setStringDictionary: #ENVIR stringPrototype methodDictionary,

"
*******************************
*******************************
*  Root Environment Creation  *
*******************************
*******************************
" print,

methodDictionaryQ set: #RTYPE_C_Dictionary new,

vStoreQ
    set:
	(#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    new: (#RTYPE_C_PToken new: 0)
		    referencing: Object rowPToken)
	    in:
		Object
	    withMethods:
		methodDictionary),

#ENVIR setTheRootEnvironment:
    (#RTYPE_C_Descriptor
	new: (vStore addRows: TheScalarPToken)
	in:  vStore),

"
****************************
****************************
*****  Space Creation  *****
****************************
****************************
" print,

"
*************************
*****  System Root  *****
*************************
" print,

#M updateNetwork,

"
***********************
*****  User Root  *****
***********************
" print,

superVStoreQ set: vStore,

methodDictionaryQ set: #RTYPE_C_Dictionary new,

vStoreQ
    set:
	(#RTYPE_C_ValueStore
	    new:
		(#RTYPE_C_Link
		    newPos: (#RTYPE_C_PToken new: 1)
		    ref:    superVStore rowPToken
		    init:   0)
	    in:
		superVStore
	    withMethods:
		methodDictionary),

#M createSpace:
    (#RTYPE_C_Descriptor
	new:
	    (#RTYPE_C_Link
		newPos: TheScalarPToken
		ref:    vStore rowPToken
		init:   0)
	in:  vStore),
#M updateNetwork,

"
******************
******************
*****  Done  *****
******************
******************
" print,

QUIT
