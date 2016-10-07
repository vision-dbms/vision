########################################################################
###  This script is used to temporarily define any new primitives
###  until they are officially added to the networks by the
###  testdeck administrator/release administrator.
###
###  Jul 21,2004	jck   definitions for string stripping primitives
########################################################################

   # Define new way to define primitives ...

Object

defineMethod: [| define: aSelector toBePrimitive: number| 
    ^self define: aSelector toBe: number asInteger asPrimitive
].
defineMethod: [| define: aSelector toBePrimitive: number withControlValue: controlValue| 
    ^self define: aSelector toBePrimitive: number + controlValue * 65536
].
define:	'clusterCardinality' toBePrimitive: 104 ;



   # Define the new primitives ...

String

define:'stripLeading:'            toBePrimitive: 386 .
define:'stripTrailing:'           toBePrimitive: 386 withControlValue: 1 .
define:'stripBounding:'           toBePrimitive: 386 withControlValue: 2 .
define:'stripLeadingAlwaysCopy:'  toBePrimitive: 386 withControlValue: 3 .
define:'stripTrailingAlwaysCopy:' toBePrimitive: 386 withControlValue: 4 .
define:'stripBoundingAlwaysCopy:' toBePrimitive: 386 withControlValue: 5 .


   # and their binary converses ....

defineMethod:[ | stripFromFrontOf:    aString | aString asString stripLeading: super ] .
defineMethod:[ | stripFromEndOf:      aString | aString asString stripTrailing: super ] .
defineMethod:[ | stripFromBothEndsOf: aString | aString asString stripBounding: super ] .
defineMethod:[ | stripFromFrontOfAlwaysCopy: aString |
    aString asString stripLeadingAlwaysCopy: super
] .
defineMethod:[ | stripFromEndOfAlwaysCopy: aString |
    aString asString stripTrailingAlwaysCopy: super
] .
defineMethod:[ | stripFromBothEndsOfAlwaysCopy: aString |
    aString asString stripBoundingAlwaysCopy: super
];

Utility SessionAttribute do:	[
    ^self define:	"cachingContainerHandles" toBe:	(274 asPointerTo:	^self) ;
    ^self define:	"cachingDictionaries"     toBe:	(272 asPointerTo:	^self) ;
    cachingDictionaries <- FALSE ;
    cachingContainerHandles <- FALSE ;
    ^self define:	"visionPre7Ordering"      toBe:	(284 asPointerTo:	^self) ; 
];
