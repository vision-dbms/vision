?f
#############################################################################
#  This file:
#
#	1) binds 'Prototypical' to an object naming prototypes for the
#	   abstract and instantiable classes created during boot strap.
#
#	2) binds:
#		'TRUE', 'FALSE', 'NA',
#		'TheDefaultProperty',
#		'TheDefaultListProperty',
#		'TheDefaultStringProperty',
#		'TheDefaultFixedProperty',
#		'TheDefaultFixedListProperty',
#		'TheDefaultFixedStringProperty'.
#
#	3) defines the following basic protocol definition protocol:
#		'defineMethod:' and
#		'define:toBe:'  at 'Prototypical Object'
#		'asPrimitive'	at 'Prototypical Integer'
#
#############################################################################

################
####  Create an object to reference prototypes of the built in classes (using
####  'class' variables except when that would confuse the interpreter).
################

'Prototypical' locateOrAddInDictionaryOf: ^current. <-
	[!Primitive; !Method; !Closure; !Value; !TimeSeries] environment;

################
#### Create 'variables' to hold the default properties...
################

!TheDefaultProperty;
!TheDefaultListProperty;
!TheDefaultStringProperty;
!TheDefaultFixedProperty;
!TheDefaultFixedListProperty;
!TheDefaultFixedStringProperty;
?g

################
#### Access prototypes not representable by VISION programs...
################
?d

"*****  Allocate Q-Registers  *****
" print,
UserEnvironmentQ <- #qa,
UserEnvironment  <- UserEnvironmentQ ref,

UserVStoreQ <- #qb,
UserVStore  <- UserVStoreQ ref,

UserDictionaryQ <- #qc,
UserDictionary  <- UserDictionaryQ ref,

PrototypicalVStoreQ <- #qd,
PrototypicalVStore  <- PrototypicalVStoreQ ref,

PrototypicalDictionaryQ <- #qe,
PrototypicalDictionary  <- PrototypicalDictionaryQ ref,

TheScalarPTokenQ <- #qf,
TheScalarPToken  <- TheScalarPTokenQ ref,

VStoreQ <- #qg,
VStore  <- VStoreQ ref,

"*****  Initialize Q-Registers  *****
" print,
UserEnvironmentQ set: #ENVIR theUserEnvironment,
UserVStoreQ set: #ENVIR theUserEnvironment store,
UserDictionaryQ set: UserVStore methodDictionary,

PrototypicalVStoreQ set: (UserDictionary at: "Prototypical") store,
PrototypicalDictionaryQ set: PrototypicalVStore methodDictionary,

TheScalarPTokenQ set: #ENVIR scalarPToken,

"*****  Access 'TRUE', 'FALSE', and 'NA'  *****
" print,
UserDictionary
    at: "TRUE"
   put: (#RTYPE_C_Descriptor
	    new: (#RTYPE_C_UndefUV
			new: TheScalarPToken
		referencing: #ENVIR trueBehavior rowPToken)
	     in: #ENVIR trueBehavior)
;   at: "FALSE"
   put: (#RTYPE_C_Descriptor
	    new: (#RTYPE_C_UndefUV
			new: TheScalarPToken
		referencing: #ENVIR falseBehavior rowPToken)
	     in: #ENVIR falseBehavior)
;   at: "NA"
   put: (#RTYPE_C_Descriptor
	    new: (#RTYPE_C_UndefUV
			new: TheScalarPToken
		referencing: #ENVIR undefinedBehavior rowPToken)
	     in: #ENVIR undefinedBehavior)
,

"*****  Access the Default Property Types  *****
" print,
UserVStore setColumnPtr: "TheDefaultProperty",
UserVStore column at: 0 put: #ENVIR theDefaultTimeVaryingProperty,

UserVStore setColumnPtr: "TheDefaultListProperty",
UserVStore column at: 0 put: #ENVIR theDefaultTimeVaryingListProperty,

UserVStore setColumnPtr: "TheDefaultStringProperty",
UserVStore column at: 0 put: #ENVIR theDefaultTimeVaryingStringProperty,

UserVStore setColumnPtr: "TheDefaultFixedProperty",
UserVStore column at: 0 put: #ENVIR theDefaultProperty,

UserVStore setColumnPtr: "TheDefaultFixedListProperty",
UserVStore column at: 0 put: #ENVIR theDefaultListProperty,

UserVStore setColumnPtr: "TheDefaultFixedStringProperty",
UserVStore column at: 0 put: #ENVIR theDefaultStringProperty,


"*****  Access Prototypes  *****
" print,

"-----  Primitive  -----
" print,
VStoreQ set: #ENVIR primitiveFnBehavior,

PrototypicalVStore setColumnPtr: "Primitive",
PrototypicalVStore column
    at: 0
   put: (#RTYPE_C_Descriptor
	    new: 0
	     in: VStore),

"-----  List  -----
" print,
VStoreQ set: #ENVIR listPrototype,

PrototypicalDictionary
    at: "List"
   put: (#RTYPE_C_Descriptor
	    new: (#RTYPE_C_Link
			new: TheScalarPToken referencing: VStore rowPToken)
	     in: VStore),

"-----  TimeSeries  -----
" print,
VStoreQ set: #ENVIR timeVaryingValueBehavior,

PrototypicalVStore setColumnPtr: "TimeSeries",
PrototypicalVStore column
    at: 0
   put: (#RTYPE_C_Descriptor
	    new: (#RTYPE_C_Link
			new: TheScalarPToken referencing: VStore rowPToken)
	     in: VStore),

"-----  IndexedList  -----
" print,
VStoreQ set: #ENVIR aListPrototype,

PrototypicalDictionary
    at: "IndexedList"
   put: (#RTYPE_C_Descriptor
	    new: (#RTYPE_C_Link
			new: TheScalarPToken referencing: VStore rowPToken)
	     in: VStore),

"-----  Character  -----
" print,
VStoreQ set: #ENVIR characterBehavior,

PrototypicalDictionary
    at: "Character"
   put: (#RTYPE_C_Descriptor
	    new: (#RTYPE_C_Link
			new: TheScalarPToken referencing: VStore rowPToken)
	     in: VStore),

"*****  Align the 'Prototypical' V-Store  *****
" print,
PrototypicalVStore align,

QUIT

################
#### Complete the prototype naming process...
################

Prototypical :Primitive <-
	0 asPointerTo: Prototypical Primitive;

'ComputedFunction' locateOrAddInDictionaryOf: Prototypical. <-
	Prototypical Primitive super;

'Function' locateOrAddInDictionaryOf: Prototypical. <-
	Prototypical ComputedFunction super;

'Object' locateOrAddInDictionaryOf: Prototypical. <-
	Prototypical Function super;

Prototypical :Method <-
	[] asMethod;

#>>>> Prototypical Method super == Prototypical ComputedFunction

Prototypical :Closure <-
	[];

#>>>>  Prototypical Closure super == Prototypical ComputedFunction

Prototypical :Value <-
	[!x] value;

'EnumeratedFunction' locateOrAddInDictionaryOf: Prototypical. <-
	Prototypical Value super;

#>>>>  Prototypical EnumeratedFunction super == Prototypical Function

'List' locateOrAddInDictionaryOf: Prototypical. <-
	Prototypical List new;

'SequencedCollection' locateOrAddInDictionaryOf: Prototypical. <-
	Prototypical List super;

'Collection' locateOrAddInDictionaryOf: Prototypical. <-
	Prototypical SequencedCollection super;

#>>>>  Prototypical Collection super == Prototypical EnumeratedFunction

Prototypical :TimeSeries <-
	Prototypical TimeSeries new;

'IndexedCollection' locateOrAddInDictionaryOf: Prototypical. <-
	Prototypical TimeSeries super;

#>>>>  Prototypical IndexedCollection super == Prototypical Collection

'IndexedList' locateOrAddInDictionaryOf: Prototypical. <-
	Prototypical IndexedList new;

#>>>>  Prototypical IndexedList super == Prototypical IndexedCollection

'Undefined' locateOrAddInDictionaryOf: Prototypical. <-
	NA;

#>>>>  Prototypical Undefined super == Prototypical Object

'Boolean' locateOrAddInDictionaryOf: Prototypical. <-
	TRUE super;

#>>>>  Prototypical Boolean super == Prototypical Object

'Selector' locateOrAddInDictionaryOf: Prototypical. <-
	'+';

#>>>>  Prototypical Selector super == Prototypical Object

'Integer' locateOrAddInDictionaryOf: Prototypical. <-
	0;

'Number' locateOrAddInDictionaryOf: Prototypical. <-
	Prototypical Integer super;

'Ordinal' locateOrAddInDictionaryOf: Prototypical. <-
	Prototypical Number super;

#>>>>  Prototypical Ordinal super == Prototypical Object

'Float' locateOrAddInDictionaryOf: Prototypical. <-
	0 asFloat;

#>>>>  Prototypical Float super == Prototypical Number

'Double' locateOrAddInDictionaryOf: Prototypical. <-
	0.0;

#>>>>  Prototypical Double super == Prototypical Number

'Date' locateOrAddInDictionaryOf: Prototypical. <-
	10101 asDate;

#>>>>  Prototypical Date super == Prototypical Ordinal

#'Character' locateOrAddInDictionaryOf: Prototypical. <-
#	32 asPointerTo: Prototypical Character;

#>>>>  Prototypical Character super == Prototypical Ordinal

'String' locateOrAddInDictionaryOf: Prototypical. <-
	"";

#>>>>  Prototypical String super == Prototypical Ordinal

################
####  Define basic protocol definition protocol...
################

'defineMethod:' locateOrAddInDictionaryOf: Prototypical Object. <-
	[|defineMethod: aBlock|
	    aBlock selector
		locateOrAddInDictionaryOf: ^self. <- aBlock asMethod;
	    ^self
	] asMethod;

Prototypical Object
defineMethod:
[|define: aSelector toBe: anObject|
    aSelector locateOrAddInDictionaryOf: ^self. <- anObject;
    ^self
];

Prototypical Integer
defineMethod:
[|asPrimitive| ^current asPointerTo: ^my Prototypical Primitive]
;
?g
