####################
#  AdminTools - General Protocol
####################

###  Network information primitives  ###
AdminTools
   define: 'totalNetworkAllocation'   toBe: 600 asPrimitive.
   define: 'spacesInNetwork'          toBe: 601 asPrimitive .  ## includes space 0

defineMethod: [ | objectSpaceList |
  spacesInNetwork sequence0 send: [ ^self asObjectSpace ]
] .
;

################
#  Define Integer conversions to the new classes
################
Integer 

defineMethod: [ | asObjectSpace |
  ^self >= ^global AdminTools spacesInNetwork
   ifTrue: [ NA ]
  ifFalse: [ ^self asPointerTo: ^global AdminTools ObjectSpace ]
] .

defineMethod: [ | asPOP |
  ^self asPointerTo: ^global AdminTools _POP
] .

defineMethod: [ | asClusterType |
  ^global AdminTools ClusterType getTypeIndex: ^self
] .
;

NA define: 'asObjectSpace' toBe: NA ;
NA define: 'asPOP' toBe: NA ;
NA define: 'asClusterType' toBe: NA ;

#---------------------------------------------

##################################################

####################
#  AdminTools ObjectSpace - protocol
####################

AdminTools ObjectSpace

###  Space information primitives  ###
  define: 'totalSpaceAllocation'     toBe: 610 asPrimitive.
  define: 'containerTableSize'       toBe: 611 asPrimitive.

  define: '_packPOP:'                toBe: 619 asPrimitive.

defineMethod: [ | packPOP: cntnrIndex |
  !result <- ^self _packPOP: cntnrIndex;
  result isntNA ifTrue: [ :result <- result asPOP ];
  result
] .

###  Return top-level space for object
defineMethod: [ | getSpace | 
  asNumber = 0 
   ifTrue: [ ^global ] .
   elseIf: [ asNumber <= 2 ] then: [ ^global BuiltInWorkspace ] .
     else: [ ^self packPOP: 1 . getNthPOP: 0 . asObject ] 
] .

defineMethod: [ | spaceName |
  !os <- asNumber ;
  os = 0 ifTrue: [ "Temporary" ] .
    elseIf: [ os <= 2 ] then: [ "BuiltInWorkspace" ] .
    elseIf: [ os = 3 ] then: [ "CoreWorkspace" ] .
      else: [ getSpace name ]
] .

###  Print and Support primitives  ###
   define: 'asPointerTo:' toBe: 31 asPrimitive .
   defineMethod: [ | asNumber | ^self asPointerTo: ^global Integer ] .
   define: 'print' toBe: 121 asPrimitive.
   defineMethod:[ | print: aFormat | aFormat printOf: ^self asSelf ] .

defineMethod: [ | asObjectSpace | ^self ] .

defineMethod: [ | containerList |
  !space <- ^self;
  containerTableSize sequence0
      send: [ ^my space packPOP: asSelf ] .   
  extendBy: [ !space <- ^my space ;
              !type <- containerType ;
            ] .
    select: [ type isntNA ] .
] .

defineMethod: [ | popList | 
  containerList collectListElementsFrom: [ popList ] .
     groupedBy: [ asSelf ] . send: [ asSelf ] 
] .
;

###  Converse Messages  ###
Integer

defineMethod: [ | _packPOPWithSpace: space |
  space _packPOP: asInteger
] .
;

#--------------------------------------------------

####################
#  AdminTools _POP
####################

AdminTools _POP
  define: 'containerRefCount'        toBe: 609 asPrimitive.
  define: '_containerAsObject'       toBe: 612 asPrimitive .
  define: 'containerSize'            toBe: 613 asPrimitive .
  define: '_containerType'           toBe: 614 asPrimitive .
  define: 'containerSegment'         toBe: 615 asPrimitive .
  define: 'containerSegmentOffset'   toBe: 616 asPrimitive .
  define: 'containerAddrType'        toBe: 617 asPrimitive .
  define: 'containerPOPCount'        toBe: 618 asPrimitive .
  define: '_extractSpace'            toBe: 620 asPrimitive .
  define: 'extractContainerIndex'    toBe: 621 asPrimitive .
  define: '_getNthPOP:'              toBe: 622 asPrimitive .
  define:'cleanDictionaryViaPOP'     toBe: 623 asPrimitive .
  define:'cleanDescriptorViaPOP'     toBe: 624 asPrimitive .

defineMethod: [ | containerAsObject |
  ^self _containerAsObject __fixReference
] .

defineMethod: [ | containerType |  _containerType asClusterType ] .
defineMethod: [ | extractSpace |  _extractSpace asObjectSpace ] .

defineMethod: [ | getNthPOP: index |
  !result <- ^self _getNthPOP: index ;
  result isntNA ifTrue: [ :result <- result asPOP ] ;
  result
] .

  define: 'asPointerTo:' toBe: 31 asPrimitive .
defineMethod: [ | asNumber |  ^self asPointerTo: ^global Integer ] .

defineMethod: [ | print |  asNumber print ; ^self ] .
defineMethod: [ | print: f |  asNumber print: f ; ^self ] .
defineMethod: [ | displayInfo |
  asNumber print ; 
  "[" print ;
  extractSpace print: 5 ; ":" print ; extractContainerIndex print ;
  "]" printNL ;
] .

defineMethod: [ | asObject |  containerAsObject ] .

defineMethod: [ | popList |
  !pop <- ^self ;
  ^self containerPOPCount else: 0 .
     sequence0 send: [ ^my pop getNthPOP: ^self ] 
] .
;

###  Converse Messages  ###
Integer 

defineMethod: [ | _nthPOPReferencedBy: pop |
  pop _getNthPOP: asInteger
] .
;

#--------------------------------------------------

################
#  Object - cluster profile protocol
################

Object
  define: 'clusterCardinality'                       toBe: 104 asPrimitive .
  define: 'clusterID'                                toBe: 490 asPrimitive .
  define: '_clusterType'                             toBe: 491 asPrimitive .
  define: 'clusterSize'                              toBe: 492 asPrimitive .
  define: 'clusterReferenceMapSize'                  toBe: 493 asPrimitive .
  define: 'clusterReferenceMapEntry:usingCollector:' toBe: 499 asPrimitive .

defineMethod: [ | asPOP | 
  ^self clusterID asPOP
] .

defineMethod: [ | clusterType |
  ^self _clusterType asClusterType
] .

defineMethod: [ | clusterReferenceMap |
  !self <- ^self;
  !selectors <- ^self clusterType isTable
  ifTrue: 
    [ ^self selectorList 
         select: [ ^self locateInDictionaryOf: ^my self . value isProperty ]
    ];
  ^self clusterReferenceMapSize sequence 
  send: [
        !cluster <- ^my self;
        !index <- ^self;
	cluster clusterReferenceMapEntry: ^self usingCollector:
           [ | :referenceTarget :referenceType :referenceSize :subsetSize 
               :internalIndex |
             !name <- ^my selectors isntNA 
                ifTrue: [ ^my selectors at: internalIndex ] 
               ifFalse: [ "" ] ;
             ^current
           ]
        ]
] .

defineMethod: [ | displayClusterProfile |
  "Level"     print: 8 ;
  "Index"     print: 8 ;
  "POP"	      print: 18 ;
  "Type"      print: 10 ;
  "Cardinal"  print: -10 ;
  "Size"      print: -13 ; "  " print ;
  "Vision Class" printNL ;
  ^self displayClusterProfileAtLevel: 1
] .

defineMethod: [ | displayClusterProfileAtLevel: level |
  ^self displayClusterSummary;

  ^self clusterReferenceMap do:
   [ name != "" ifTrue: [ "--> " print ; name printNL ] ;
     ^my level print: -8 ;
     index print: -8 ; 
     " " print: 18 ;
     referenceType asClusterType code print: 10 ;
     subsetSize printWithCommas: 10 ;
     referenceSize printWithCommas: 13.0 ;
     " " print ;

     referenceTarget clusterType isTable
      ifTrue: [ referenceTarget displayClusterSummary ] 
     ifFalse: 
        [ referenceTarget
              displayClusterProfileAtLevel: (^my level + 1) asInteger
        ]
   ] . count > 0 ifTrue: [ newLine print ] ;
] .

defineMethod: [ | displayClusterSummary |
  newLine print;
  "  Store: " print: 10 ; ^self clusterID print: -9 ;
  [ "[" print ; ^self objectSpace print: 4 ; ", " print ;
    ^self containerIndex print: -7 ;
  ] divertOutput stripBoundingBlanks concat: "]" . print: 15 ;
  ^self clusterType code print: 10 ;
  ^self clusterCardinality printWithCommas: 10 ;
  ^self clusterSize printWithCommas: 13.0 ;
  "  " print ;
  ^self whatAmI printNL ;
] .
;

##################################################

####################
#  AdminTools ClusterType - protocol
####################
AdminTools ClusterType
  defineFixedProperty: 'number' .
  defineFixedProperty: 'name' .
  defineFixedProperty: 'sortCode' .
  defineFixedProperty: 'internalFlag' .

defineMethod: [ | initializeLocalAttributes |
  :number <- (^self instanceList count - 1 ) asInteger ;
  :name <- code ;
  Named ClusterType at: code put: ^self ;
  ^self
] .

defineMethod: [ | setNameTo: string | 
  :name <- string ; 
  Named ClusterType at: string put: ^self ;
  ^self
] .

defineMethod: [ | setSortCodeTo: string | 
  :sortCode <- string ; 
  ^self
] .

defineMethod: [ | flagAsInternal | 
  :internalFlag <- TRUE ;
  ^self
] .

defineMethod: [ | print |  code print ; ^self ] .

defineMethod: [ | print: f |  code print: f ; ^self ] .

defineMethod: [ | displayInfo | 
  number print: -5 ; code print: 15 ; name printNL ;
] .

defineMethod: [ | asClusterType |  asSelf ] .

defineMethod: [ | getTypeIndex: index | 
  ^self instanceList at0: index asInteger .
     else: [ ^self defaultInstance ] 
] .

defineMethod: [ | typeCode: typeIndex |
  ^self getTypeIndex: typeIndex . code else: [ "" ] 
] .

defineMethod: [ | typeName: typeIndex |
  ^self getTypeIndex: typeIndex . name else: ["*** Unknown ***"]
] .

defineMethod: [ | isPToken |  number = 6 ] .
defineMethod: [ | isFixedProperty |  number = 20 ] .
defineMethod: [ | isIndex |  number = 25 ] .
defineMethod: [ | isProperty |  isIndex || isFixedProperty ] .
defineMethod: [ | isList |  number = 21 ] .
defineMethod: [ | isTable |  number = 23 ] .
defineMethod: [ | isntInternal |  internalFlag isNA ] .
defineMethod: [ | isDictionary |  number = 36 ].
defineMethod: [ | isDescriptor |  number = 26 ].
;

################
#  Create AdminTools ClusterType instances
#     (these will need to be modified by 6.0.0 release)
################
AdminTools ClusterType do:
 [ :code <- "Unused0";
   :number <- 0 ;
   ^self initializeLocalAttributes setNameTo: "*** Unused-0 ***";
   flagAsInternal ;
 ] ;
AdminTools ClusterType createInstance: "TCTable" .
    setNameTo: "Transient CTable" .
    setSortCodeTo: 101 ;
AdminTools ClusterType createInstance: "PCTable" .
    setNameTo: "Permanent CTable" .
    setSortCodeTo: 100 ;
AdminTools ClusterType createInstance: "*** Unused-3 ***" .
    setNameTo: "(was descriptor)" .
    flagAsInternal ;
AdminTools ClusterType createInstance:  "IString" .
    setNameTo: "Internal String".
    flagAsInternal ;
AdminTools ClusterType createInstance:  "IArray" .
    setNameTo: "Internal Array".
    flagAsInternal ;
AdminTools ClusterType createInstance:  "PToken" .
    setNameTo: "PToken".
    setSortCodeTo: 110 ;
AdminTools ClusterType createInstance:  "Link" .
    setNameTo: "Link".
    setSortCodeTo: 111 ;
AdminTools ClusterType createInstance:  "Char" .
    setNameTo: "Character".
    setSortCodeTo: 2 ;
AdminTools ClusterType createInstance:  "Double" .
    setNameTo: "Double".
    setSortCodeTo: 4 ;
AdminTools ClusterType createInstance:  "Float" .
    setNameTo: "Float".
    setSortCodeTo: 5 ;
AdminTools ClusterType createInstance:  "Integer" .
    setNameTo: "Integer".
    setSortCodeTo: 3 ;
AdminTools ClusterType createInstance:  "Pointer" .
    setNameTo: "Pointer (RefUV)".
    setSortCodeTo: 6 ;
AdminTools ClusterType createInstance:  "Selector" .
    setNameTo: "Selector".
    setSortCodeTo: 50 ;
AdminTools ClusterType createInstance:  "Undefined" .
    setNameTo: "Undefined".
    setSortCodeTo: 7 ;
AdminTools ClusterType createInstance:  "*** Unused-15 ***" .
    setNameTo: "Method Dictionary".
    flagAsInternal ;
AdminTools ClusterType createInstance:  "Block" .
    setNameTo: "Block".
    setSortCodeTo: 52 ;
AdminTools ClusterType createInstance:  "Method" .
    setNameTo: "Method".
    setSortCodeTo: 53 ;
AdminTools ClusterType createInstance:  "Context" .
    setNameTo: "Context".
    setSortCodeTo: 54 ;
AdminTools ClusterType createInstance:  "Closure" .
    setNameTo: "Closure".
    setSortCodeTo: 55 ;
AdminTools ClusterType createInstance:  "Property" .
    setNameTo: "Property (Vector)".
    setSortCodeTo: 10 ;
AdminTools ClusterType createInstance:  "List" .
    setNameTo: "Sequenced List".
    setSortCodeTo: 12;
AdminTools ClusterType createInstance:  "INA" .
    setNameTo: "Internal N/A" .
    flagAsInternal ;
AdminTools ClusterType createInstance:  "Table" .
    setNameTo: "Table (Value Store)".
    setSortCodeTo: 11 ;
AdminTools ClusterType createInstance:  "Unused24" .
    setNameTo: "*** Unused-24 ***" .
    flagAsInternal ;
AdminTools ClusterType createInstance:  "Index" .
    setNameTo: "Associative List".
    setSortCodeTo: 13 ;
AdminTools ClusterType createInstance:  "Descriptor" .
    setNameTo: "Internal Pointer".
    setSortCodeTo: 112 ;
27 sequenceTo: 35 .
do: [ !id <- "Unused" concat: ^self asString ;
      !name <- "*** Unused-" concat: ^self asString . concat: " ***" ;
      ^my AdminTools ClusterType createInstance:  id .
         setNameTo: name .
         flagAsInternal do: [ :number <- ^my asSelf ] ;
    ] ;
AdminTools ClusterType createInstance:  "Dictionary" .
    setNameTo: "Dictionary".
    setSortCodeTo: 51 ;
37 sequenceTo: 46 .
do: [ !id <- "Unused" concat: ^self asString ;
      !name <- "*** Unused-" concat: ^self asString . concat: " ***" ;
      ^my AdminTools ClusterType createInstance:  id .
         setNameTo: name .
         flagAsInternal do: [ :number <- ^my asSelf ] ;
    ] ;
AdminTools ClusterType createInstance:  "Uns64" .
    setNameTo: "Unsigned 64 bit".
    setSortCodeTo: 200 ;
AdminTools ClusterType createInstance:  "Uns96" .
    setNameTo: "Unsigned 96 bit".
    setSortCodeTo: 200 ;
AdminTools ClusterType createInstance:  "Uns128" .
    setNameTo: "Unsigned 128 bit".
    setSortCodeTo: 200 ;
AdminTools ClusterType createInstance:  "Direct" .
    setNameTo: "".
    flagAsInternal ;



######################################################################

####################
#  Tools to perform network clean up
####################

#--------------------
#  Notes:
#    - Don't store actual objects in the list, as this may result
#      in too many segments being referenced, causing the process to
#      run out of address space.
#--------------------

AdminTools ObjectSpace

defineMethod: [ | cleanSpace | 
  cleanTables ; 
  cleanIndexedLists ; 
  cleanLists ; 
  cleanDictionaries ;
  cleanDescriptors ;
] .

defineMethod: [ | cleanTables | 
  !space <- ^self ; 

  newLine print ; 
  "Cleaning Object Space " print ; space print: 5 ; "Tables" printNL: -16 ;
  newLine print ;
  "      Current Allocation:" print ;
  totalNetworkAllocation printWithCommasNL: 18 ;
  "       Current Threshold:" print ; 
  ^tmp allocationThreshold printWithCommasNL: 18 ;

###  Clean Tables
  !cleanCount <- 0 ;
  !tables <- containerTableSize sequence0
       send: [ ^my space packPOP: asSelf ] . 
     select: [ containerType isntNA && [ containerType isTable ]
             ] ;
  tables iterate:
    [ !table <- ^self;
      !result <- table asObject cleanStore ;
      updateNetworkIfSpaceIsLow ;
      result ifTrue: [ ^my :cleanCount <- (^my cleanCount + 1) asInteger ] ;
    ] ;

  "       Tables Considered:" print ; tables count printNL: 18 ;
  "          Tables Cleaned:" print ; cleanCount printNL: 18 ;
  newLine print ; 
] .

defineMethod: [ | cleanIndexedLists |
  !space <- ^self ;

  newLine print ;
  "Cleaning Object Space " print ; space print: 5 ; " IndexedLists" printNL: -16 ;
  newLine print ;
  " Current Allocation:" print ;
  totalNetworkAllocation printWithCommasNL: 23 ;
  "  Current Threshold:" print ; 
  ^tmp allocationThreshold printWithCommasNL: 23 ;

###  Clean Indexed Lists
  !cleanCount <- 0 ;
  !ilists <- containerTableSize sequence0
       send: [ ^my space packPOP: asSelf ] . 
     select: [ containerType isntNA && [ containerType isIndex ]
             ] ;
  ilists iterate:
    [ ^self containerAsObject cleanStore
        ifTrue: [ ^my :cleanCount <- (^my cleanCount + 1) asInteger ] ;
      updateNetworkIfSpaceIsLow ;
    ] ;

  "  ILists Considered:" print ; ilists count printNL: 23 ;
  "     ILists Cleaned:" print ; cleanCount printNL: 23 ;
  newLine print ; 
] .

defineMethod: [ | cleanLists |
  !space <- ^self ;

  newLine print ;
  "Cleaning Object Space " print ; space print: 5 ; " Lists" printNL: -16 ;
  newLine print ;
  " Current Allocation:" print ;
  totalNetworkAllocation printWithCommasNL: 23 ;
  "  Current Threshold:" print ; 
  ^tmp allocationThreshold printWithCommasNL: 23 ;

###  Clean Lists
  !cleanCount <- 0 ;
  !lists <- containerTableSize sequence0
      send: [ ^my space packPOP: asSelf ] . 
    select: [ containerType isntNA && [ containerType isList ]
            ] . 
    select: [ # don't bother with string stores...
              ^self getNthPOP: 1 . containerType isFixedProperty
            ] ;
  lists iterate:
    [ ^self containerAsObject cleanStore
        ifTrue: [ ^my :cleanCount <- (^my cleanCount + 1) asInteger ] ;
      updateNetworkIfSpaceIsLow ;
    ] ;

  "   Lists Considered:" print ; lists count printNL: 23 ;
  "      Lists Cleaned:" print ; cleanCount printNL: 23 ;
  newLine print ; 
] .

defineMethod: [ | cleanDictionaries | 
  !space <- ^self ; 

  newLine print ; 
  "Cleaning Object Space " print ; space print: 5 ; 
  "Dictionaries" printNL: -16 ;
  newLine print ;
  "      Current Allocation:" print ;
  totalNetworkAllocation printWithCommasNL: 18 ;
  "       Current Threshold:" print ; 
  ^tmp allocationThreshold printWithCommasNL: 18 ;

###  Clean Dictionaries
  !dictionaries <- containerTableSize sequence0
       send: [ ^my space packPOP: asSelf ] . 
     select: [ containerType isntNA &&
		[ containerType isDictionary ]
             ] ;
  !results <- dictionaries send:
        [ ^self cleanDictionaryViaPOP ] ;
  updateNetworkIfSpaceIsLow ;

  " Dictionaries Considered:" print ; dictionaries count printNL: 18 ;
  "    Dictionaries Cleaned:" print ; results select:[^self = TRUE] .count printNL: 18 ;
  newLine print ; 
] .

defineMethod: [ | cleanDescriptors | 
  !space <- ^self ; 

  newLine print ; 
  "Cleaning Object Space " print ; space print: 5 ; 
  "Descriptors" printNL: -16 ;
  newLine print ;
  "      Current Allocation:" print ;
  totalNetworkAllocation printWithCommasNL: 18 ;
  "       Current Threshold:" print ; 
  ^tmp allocationThreshold printWithCommasNL: 18 ;

###  Clean Descriptors
  !descriptors <- containerTableSize sequence0
       send: [ ^my space packPOP: asSelf ] . 
     select: [ containerType isntNA &&
		[ containerType isDescriptor ]
             ] ;
  !results <- descriptors send:
        [ ^self cleanDescriptorViaPOP ] ;
  updateNetworkIfSpaceIsLow ;

  "  Descriptors Considered:" print ; descriptors count printNL: 18 ;
  "     Descriptors Cleaned:" print ; results select:[^self = TRUE] .count printNL: 18 ;
  newLine print ; 
] .

;

#---------

AdminTools

defineMethod: [ | cleanNetwork |
  #-- NOTE: spacesInNetwork count includes space 0
  (spacesInNetwork - 1) asInteger sequence
      send: [ ^self asObjectSpace ] .
   iterate: [ ^self cleanSpace ;
              "-" fill: 50 . printNL ;
            ] ;
] .

defineMethod: [ | cleanNetworkInclude: osList |
  !spaceList <- osList isList
      ifTrue: [ osList ] ifFalse: [ osList asList ];
  !ospaces <- spaceList send: [ ^self asObjectSpace ] .
      select: [ ^self isObjectSpace && [ ^self asNumber > 0 ] ] ;
  !delimit <- ospaces count > 1 ;
  ospaces iterate: [ ^self cleanSpace ;
                     ^my delimit ifTrue: [ "-" fill: 50 . printNL ] ;
                   ] ; 
] .

defineMethod: [ | cleanNetworkExclude: osList |
  !spaceList <- osList isList
      ifTrue: [ osList ] ifFalse: [ osList asList ];
  !excludeList <- (spaceList, 0) send: [ ^self asObjectSpace ] .
      select: [ ^self isObjectSpace ] ;
  objectSpaceList exclude: excludeList .
  iterate: [ ^self cleanSpace ;
             "-" fill: 50 . printNL ;
           ] ;
] .

defineMethod: [ | updateNetworkIfSpaceIsLow |
^tmp allocationThreshold <= totalNetworkAllocation 
ifTrue: [
        "Updating Network: Allocation Level    = " print; 
        totalNetworkAllocation printWithCommasNL: 18; 
        ^global Utility updateNetworkWithAnnotation: "updateNetworkIfSpaceIsLow"; 
        "Updated Network: Allocation Level now = " print; 
        totalNetworkAllocation printWithCommasNL: 18; 
        ]; 
^tmp allocationThreshold <= totalNetworkAllocation 
ifTrue: [
        "Adjusting Allocation Threshold to:      " print;
        ^tmp define:'allocationThreshold' 
             toBe:  (totalNetworkAllocation + 50000000) asInteger;
        ^tmp allocationThreshold printWithCommasNL: 18; 
        ];
] .

defineMethod: [ | setAllocationThresholdTo: bytes |
  bytes isNumber ifTrue:
  [ ^tmp define: 'allocationThreshold' toBe: bytes ;
    "Setting Allocation Threshold to: " print ; 
    bytes printWithCommasNL: 18 ;
  ]
] .
;


###  Set Default Tuning Parameter for Saves  ###
BuiltInWorkspace define: 'allocationThreshold' toBe: 2000000000; #-- 2 GB

######################################################################

####################
#  Default References Must be fixed for built-in classes
####################

## The default Reference Fixer ....
Object
defineMethod: [ | __fixReference | 
  ^self
];

###  Define Reference fixers  ###
Utility UnixSeconds
defineMethod: [ | __fixReference |
	^self == super ifTrue: [0 asPointerTo: ^self]
			ifFalse:[^self]
];

Utility SessionAttribute
defineMethod: [ | __fixReference |
	^self == super ifTrue: [0 asPointerTo: ^self]
			ifFalse:[^self]
];

DateOffset
defineMethod: [ | __fixReference |
	^self == super ifTrue: [0 asPointerTo: ^self]
			ifFalse:[^self]
];

Date
defineMethod: [ | __fixReference |
	^self == super ifTrue: [0 asPointerTo: ^self]
			ifFalse:[^self]
];

Integer
defineMethod: [ | __fixReference |
	^self == super ifTrue: [0 asPointerTo: ^self]
			ifFalse:[^self]
];

Float
defineMethod: [ | __fixReference |
	^self == super ifTrue: [0.000000 asFloat asPointerTo: ^self]
			ifFalse:[^self]
];

Double
defineMethod: [ | __fixReference |
	^self == super ifTrue: [0.000000 asPointerTo: ^self]
			ifFalse:[^self]
];

OpenVision Channel
defineMethod: [ | __fixReference |
	^self == super ifTrue: [0.000000 asPointerTo: ^self]
			ifFalse:[^self]
];

OpenVision Channel State
defineMethod: [ | __fixReference |
	^self == super ifTrue: [0 asPointerTo: ^self]
			ifFalse:[^self]
];

OpenVision Channel Status
defineMethod: [ | __fixReference |
	^self == super ifTrue: [0 asPointerTo: ^self]
			ifFalse:[^self]
];

OpenVision Channel Error
defineMethod: [ | __fixReference |
	^self == super ifTrue: [0 asPointerTo: ^self]
			ifFalse:[^self]
];

OpenVision Channel ChannelType
defineMethod: [ | __fixReference |
	^self == super ifTrue: [0 asPointerTo: ^self]
			ifFalse:[^self]
];

OpenVision Channel Option
defineMethod: [ | __fixReference |
	^self == super ifTrue: [0 asPointerTo: ^self]
			ifFalse:[^self]
];

OpenVision Channel PromptFormat
defineMethod: [ | __fixReference |
	^self == super ifTrue: [0 asPointerTo: ^self]
			ifFalse:[^self]
];

OpenVision Channel BinaryFormat
defineMethod: [ | __fixReference |
	^self == super ifTrue: [0 asPointerTo: ^self]
			ifFalse:[^self]
];

OpenVision Channel TrimFormat
defineMethod: [ | __fixReference |
	^self == super ifTrue: [0 asPointerTo: ^self]
			ifFalse:[^self]
];

Primitive
defineMethod: [ | __fixReference |
	^self == super ifTrue: [0 asPointerTo: ^self]
			ifFalse:[^self]
];

####################
# AdminTools profiling tools
####################

AdminTools _POP 

defineMethod:
[| profileTimeSeries: prefix inFull: verbose|

!pop <- ^self;
!listPop <- pop getNthPOP:0;
!keyPop <- pop getNthPOP:2;
!mapPop <- pop getNthPOP:3;

!productMapped <- mapPop containerType == mapPop Named ClusterType Link;

prefix print; 
"TimeSeries ID: "print; pop printNL;
prefix print; 
"Number of TimeSeries in Store: "print; listPop count printNL;
prefix print; 
"Number of TimePoints in Store: "print; (keyPop count + 1) asInteger printNL;
prefix print; 
productMapped ifTrue:[
"Size of Cartesian Space:       "print; ((keyPop count + 1) *listPop count) asInteger printNL;
prefix print; 
"Number of MapTransitions:      "print; mapPop count printNL;
prefix print; 
];
"TimeSeries LStore profile:" printNL;
listPop profileLStore: (prefix concat:"  ") inFull: verbose;
^self
].

defineMethod:
[| profileIndexedList: prefix inFull: verbose|

!pop <- ^self;
!listPop <- pop getNthPOP:0;
!keyPop <- pop getNthPOP:2;
!mapPop <- pop getNthPOP:3;

prefix print; 
"IList ID: "print; pop printNL;
prefix print; 
"Number of IndexedLists in Store: "print; listPop count printNL;
prefix print; 
"Number of MapTransitions:      "print; mapPop count printNL;
prefix print; 
"IndexedList Key profile:" printNL;
keyPop profileVector: (prefix concat:"  ") inFull: verbose;
prefix print; 
"IndexedList LStore profile:" printNL;
listPop profileLStore: (prefix concat:"  ") inFull: verbose;
^self
].

defineMethod:
[| profileLStore: prefix inFull: verbose|

!pop <- ^self;
!contentPop <- pop getNthPOP:1;

prefix print; 
"LStore ID: "print; pop printNL;
prefix print; 
"Number of Lists in Store: "print; pop count printNL;
prefix print; 
"LStore Content profile:" printNL;
contentPop containerType isProperty ifTrue: [
     contentPop profileVector: (prefix concat:"  ") inFull: verbose;
] ifFalse: [
     contentPop displayContainerInfo;
];
^self
].

defineMethod:
[| profileVector: prefix inFull: verbose|

!pop <- ^self;
!popCnt <- pop containerPOPCount;
!popList <- (popCnt - 1) sequence
	    send: [ ^my pop getNthPOP: ^self ] .
	    select:[
		containerType != Named ClusterType INA
							#RTYPE_C_Undefined
	    ];
!usegCnt;
!transitionCount;
!unalignedUsegCnt;
pop getUSegCount: :usegCnt 
    andTransCount: :transitionCount 
    andUSegAligns: :unalignedUsegCnt;
prefix print; "Vector ID: " print; pop printNL;
prefix print; "Number of elements: "print:45;
	^self asObject clusterCardinality printNL;
prefix print; "Number of usegments: "print:45;
	usegCnt printNL;
prefix print; "Number needing alignment: "print:45;
	unalignedUsegCnt printNL;
prefix print; "Number of transitions: "print:45;
	transitionCount printNL;

### Profile usegments ...
verbose ifTrue: [
!useg <- 0;
!elementCount <- 0;
!outOfAlignmentCnt <- 0;
!lstoreCount <- 0;
!listCount <- 0;
!listContentElementCount <- 0;
!listContentUSegCount <- 0;
!listContentUSegUnalignCnt <- 0;
!listContentTransCount <- 0;
!vstoreCount <- 0;
!indexCount <- 0;
!blockCount <- 0;
!closureCount <- 0;
!methodCount <- 0;

[useg < usegCnt] whileTrue: [
   !storePOP <- popList at: (useg * 3 + 2) asInteger;
   !uvectorPOP <- popList at: (useg * 3 + 3) asInteger;

   :elementCount <- elementCount + uvectorPOP count;
   :outOfAlignmentCnt <- outOfAlignmentCnt + uvectorPOP needsAlignment;
   storePOP containerType == storePOP Named ClusterType Table
	ifTrue: [ :vstoreCount <- vstoreCount + 1 ];
   storePOP containerType == storePOP Named ClusterType Index
	ifTrue: [ :indexCount <- indexCount + 1 ];
   storePOP containerType == storePOP Named ClusterType Block
	ifTrue: [ :blockCount <- blockCount + 1 ];
   storePOP containerType == storePOP Named ClusterType Closure
	ifTrue: [ :closureCount <- closureCount + 1 ];
   storePOP containerType == storePOP Named ClusterType Method
	ifTrue: [ :methodCount <- methodCount + 1 ];
   storePOP containerType == storePOP Named ClusterType List
	ifTrue: [ 
		:lstoreCount <- lstoreCount + 1;
		:listCount <- listCount + storePOP count;
		!contentPop <- storePOP getNthPOP:1;
		contentPop containerType == 
				contentPop Named ClusterType Property
		ifTrue: [
			!contentUSegCount;
			!contentTransCount;
			!contentUSegAlignments;
			contentPop getUSegCount: :contentUSegCount
				   andTransCount: :contentTransCount
				   andUSegAligns: :contentUSegAlignments;
			:listContentUSegCount <- listContentUSegCount +
				contentUSegCount;
			:listContentUSegUnalignCnt <- listContentUSegUnalignCnt +
				contentUSegAlignments;
			:listContentTransCount <- listContentTransCount +
				contentTransCount;
			:listContentElementCount <- listContentElementCount +
				contentPop asObject clusterCardinality;
		];
		contentPop containerType == contentPop Named ClusterType Char
		ifTrue: [
			:listContentElementCount <- listContentElementCount +
				contentPop count;
		];
	];
   :useg <- (useg + 1) asInteger;
];
prefix print; "Number of defined elements: "print:45;
	elementCount asInteger printNL;
prefix print; "Number of referenced VStores: "print:45;
	vstoreCount asInteger printNL;
prefix print; "Number of referenced Indexed LStores: "print:45;
	indexCount asInteger printNL;
prefix print; "Number of referenced LStores: "print:45;
	lstoreCount asInteger printNL;
(0 < lstoreCount asInteger) ifTrue: [
  prefix print; "  Number of lists in referenced LStores: "print:45;
	listCount asInteger printNL;
  prefix print; "  Number of elements in referenced LStores: "print:45;
	listContentElementCount asInteger printNL;
  prefix print; "  Number of usegments in referenced LStores: "print:45;
	listContentUSegCount asInteger printNL;
  prefix print; "  Number of usegments not aligned: "print:45;
	listContentUSegUnalignCnt asInteger printNL;
  prefix print; "  Number of transitions in referenced LStores: "print:45;
	listContentTransCount asInteger printNL;
];
prefix print; "Number of referenced Blocks: "print:45;
	blockCount asInteger printNL;
prefix print; "Number of referenced Closures: "print:45;
	closureCount asInteger printNL;
prefix print; "Number of referenced Methods: "print:45;
	methodCount asInteger printNL;
];
^self
].

defineMethod:

[| profile |
!rtype <- ^self containerType;
rtype == Named ClusterType List
ifTrue: [ ^self profileLStore: "" inFull: TRUE ];
rtype == Named ClusterType Property
ifTrue: [ ^self profileVector: "" inFull: TRUE ];
rtype == Named ClusterType Index && [^self asObject isTimeSeries]
ifTrue: [ ^self profileTimeSeries: "" inFull: TRUE ];
rtype == Named ClusterType Index && [^self asObject isIndexedList]
ifTrue: [ ^self profileIndexedList: "" inFull: TRUE ];
^self
].

defineMethod:

[| briefProfile |
!rtype <- ^self containerType;
rtype == Named ClusterType List
ifTrue: [ ^self profileLStore: "" inFull: FALSE ];
rtype == Named ClusterType Property
ifTrue: [ ^self profileVector: "" inFull: FALSE ];
rtype == Named ClusterType Index && [^self asObject isTimeSeries]
ifTrue: [ ^self profileTimeSeries: "" inFull: FALSE ];
rtype == Named ClusterType Index && [^self asObject isIndexedList]
ifTrue: [ ^self profileIndexedList: "" inFull: FALSE ];
^self
].

defineMethod:
[| count |

!containerOverhead <- 16; #bytes
!rtypeOverhead <- 0;
!rtypeGranularity <- 0;

!rtype <- ^self containerType;
!size <- ^self containerSize;
!result <- 0;

rtype == Named ClusterType PToken
ifTrue:[ :rtypeOverhead <- 12; :rtypeGranularity <- 8; ];#Cartesian ignored
rtype == Named ClusterType Link
ifTrue:[ :rtypeOverhead <- 20; :rtypeGranularity <- 8; ];
rtype == Named ClusterType Char
ifTrue:[ :rtypeOverhead <- 21; :rtypeGranularity <- 1; ];
rtype == Named ClusterType Double
ifTrue:[ :rtypeOverhead <- 28; :rtypeGranularity <- 8; ];
rtype == Named ClusterType Float
ifTrue:[ :rtypeOverhead <- 24; :rtypeGranularity <- 4; ];
rtype == Named ClusterType Integer
ifTrue:[ :rtypeOverhead <- 24; :rtypeGranularity <- 4; ];
rtype == Named ClusterType Pointer
ifTrue:[ :rtypeOverhead <- 24; :rtypeGranularity <- 4; ];
rtype == Named ClusterType Selector
ifTrue:[ :rtypeOverhead <- 24; :rtypeGranularity <- 4; ];
rtype == Named ClusterType Undefined
ifTrue:[ :rtypeOverhead <- 20; :rtypeGranularity <- 0; ];
rtype == Named ClusterType List
ifTrue:[ :rtypeOverhead <- 20; :rtypeGranularity <- 4; ];

rtypeGranularity != 0
ifTrue:
  [ :result <- (size - containerOverhead - rtypeOverhead)/rtypeGranularity];

result asInteger
].

defineMethod:

[| needsAlignment |

!rtype <- ^self containerType;
!size <- ^self containerSize;
!result <- 0;

rtype == Named ClusterType Char		||
rtype == Named ClusterType Double	||
rtype == Named ClusterType Float	||
rtype == Named ClusterType Integer	||
rtype == Named ClusterType Pointer	||
rtype == Named ClusterType Selector	||
rtype == Named ClusterType Undefined
ifTrue:[^self getNthPOP:0 . count > 0 ifTrue:[:result <- 1]];

rtype == Named ClusterType Pointer
ifTrue:[^self getNthPOP:1 . count > 0 ifTrue:[:result <- 1]];
result asInteger
].

defineMethod: [|getUSegCount: usegCnt
		andTransCount: transCnt
		andUSegAligns: alignCnt|

usegCnt <- NA;
transCnt <- NA;
alignCnt <- NA;
containerType == Named ClusterType Property ifTrue:[
!pop <- ^self asSelf;
!popCnt <- pop containerPOPCount;
!popList <- (popCnt - 1) sequence
	    send: [ ^my pop getNthPOP: ^self ] .
	    select:[
		containerType != Named ClusterType INA    #RTYPE_C_Undefined
	    ];
usegCnt <- ( popList count / 3) # 3 POPs per useg
	     asInteger;
transCnt <-
		((pop containerSize -
		  16    -		     # general container overhead
		  84    -		     # minimum vector size
		  (popCnt - 13) * 4 -  	     # extra pops at 4 bytes each
		  usegCnt value * 4) /	     # 4 bytes per usegment
		  12)    		     # size of each transition.
	     asInteger;
!useg <- 0;
alignCnt <- 0;
[useg < usegCnt value] whileTrue:
[
   !uvectorPOP <- popList at: (useg * 3 + 3) asInteger;
   alignCnt <- (alignCnt value + uvectorPOP needsAlignment) asInteger;
   :useg <- (useg + 1) asInteger;
];
];
].


defineMethod:

[|displayLocalVisionReferents|
!pop <- ^self; 
!listReferencingThisPOP <- pop referentsInSpace: pop extractSpace; 
listReferencingThisPOP 
  do: [
    !pop <- ^my pop; 
    ^self containerType isTable ifTrue: [
	!referencingObject <- ^self asObject; 
        referencingObject isntNA ifTrue: [
            referencingObject 
            do: [!pop <- ^my pop; 
		^self whatAmI print; " " print; 
		^self asPOP print; 
                !prop <- ^self clusterReferenceMap select: [
			    ^self referenceTarget clusterID = ^my pop
			];
		prop count > 0 ifTrue: [
		    prop do: [
			" '" print; name print; "'" print;
		    ];
		];
		newLine print;
                pop asObject isntNA ifTrue: [
			pop briefProfile isntNA 
					ifTrue:  [newLine print; ]
					ifFalse: [pop displayContainerInfo;]
		] ifFalse: [
			pop displayContainerInfo;
		];
            ]
	] ifFalse: [
	    "***  Unidentified Table Object  ***" printNL; 
            pop printNL; newLine print; 
        ]; 
    ] ifFalse: [
       ^self displayLocalVisionReferents
    ]; 
  ];
  ^self
].

defineMethod: [ | referentsInSpace: space |
  !pop <- ^self;
  space asObjectSpace containerList select: [
      !cntnr <- ^self;
      !pop <- ^my pop;
      cntnr containerPOPCount sequence0 select: [
        (^my cntnr getNthPOP: ^self) = ^my pop
      ] . count > 0 
  ]
]  .

defineMethod: [| displayContainerInfo |

    extractSpace print:-3;
    extractContainerIndex print:8;
    containerAddrType print: 3;
    containerType print: -12;
    containerSize print: 12;
    containerSegment print: 8;
    containerSegmentOffset print: 12;
    containerPOPCount print: 8;
    newLine print;
    ^self
].

defineMethod:[| displayContainerInfoWithReferences |
    displayContainerInfo;
    displayReferences
].

defineMethod: [| displayReferents |
  !pop <- ^self;
  objectSpaceList select: [
    (^self == 0 asObjectSpace) not
  ] . do: [
    ^my pop displayReferentsInSpace: ^self
  ];
  ^self
].
defineMethod: [| displayReferentsInSpace: space| 
   ^self referentsInSpace: space . do:[ 
      "  " print; displayContainerInfo
   ];
   ^self
].

defineMethod:[| displayReferences |
  !pop <- ^self;
  containerPOPCount sequence0 extendBy:[!xxx] . do: [
    ^my pop getNthPOP:^self . do: ["	:" print;
				   displayContainerInfo]
  ];
  ^self
].

defineMethod: [| displayReferentsInSpace: space toLevel: level|
  !pop <- ^self asSelf;
  space asObjectSpace containerList do: [
      !level <- ^my level;
      !cntnr <- ^self;
      !pop <- ^my pop;
      cntnr containerPOPCount sequence0 select: [
	(^my cntnr getNthPOP: ^self) = ^my pop
      ] . count > 0 
	  ifTrue: [
		   level print: 10 - level; 
		   cntnr displayContainerInfo;
		   level > 1 
		   ifTrue:[ cntnr displayReferentsInSpace: cntnr extractSpace
                                  toLevel: (level - 1) asInteger
			  ]
		  ]
  ];
  ^self
].

defineMethod:[ | findPTokenTail |

  ^self containerType isPToken ifTrue:[
    !ptList <- ^self extractSpace containerList
	select:[type isPToken]. 
	select:[containerSize > 28]. 
	select:[containerPOPCount = 1].
        extend:[!referencedPOP <- ^self getNthPOP:0];
    !currentPOP <- ^self;
    !nextPOP <- ^self;
    !chainLength <- 0;
    [nextPOP isntNA] whileTrue: [
	:currentPOP <- nextPOP;
	:nextPOP <- currentPOP findPOPinList: ptList;
#	currentPOP displayContainerInfo;
	:chainLength <- chainLength + 1;
    ];
    "Chain length: " print; chainLength printWithCommasNL:7.0;
    currentPOP
  ] else: [NA]
].

defineMethod:[ | findPOPinList: pops |

  !pop <- ^self asSelf;
  !result <- pops select:[
	referencedPOP = ^my pop
  ];

  result count > 0 ifTrue:[ result at:1] else:[NA]
].

defineMethod: [|print|
  "[" print;
  extractSpace print:3;
  ":" print;
  extractContainerIndex print:7;
  "]" print;
].


defineMethod: [|= object|
  ^self asNumber = object
].

;

####################
#
#            Object Space profiling 
#
####################

AdminTools ObjectSpace 

defineMethod: 
[ | profile |
  !containersInSpace <- ^self containerList;
  !bigContainersInSpace <-containersInSpace select:[containerSize > 10000];

  !containerCount <- containersInSpace count;
  !bigContainerCount <- bigContainersInSpace count;
  !totalSize <- containersInSpace total:[containerSize];
  !bigSize <- bigContainersInSpace total:[containerSize];
  bigSize isNA ifTrue:[:bigSize <- 0];

  !containersByType <- containersInSpace groupedBy: [type];

  "Profile of ObjectSpace " print;
  ^self print:3; ":" printNL;
  newLine print;

  "  Total Size (in bytes): " print:35; totalSize printWithCommas:20.0;
	newLine print;
  "        (containers > 10,000): " print:35; bigSize printWithCommas:20.0;
	newLine print;
  "        (containers < 10,000): " print:35;
	(totalSize - bigSize) printWithCommas:20.0;
	newLine print;
  "  Number of Containers:  " print:35; containerCount printWithCommas:20.0;
	newLine print;
  "         (containers > 10,000):  " print:35;
	bigContainerCount printWithCommas:20.0;
	newLine print;
  "         (containers < 10,000):  " print:35;
	(containerCount - bigContainerCount) printWithCommas:20.0;
	newLine print;
  "  Unused Container Table Slots:  " print:35;
	(containerTableSize - containerCount) printWithCommas:20.0;
	newLine print;
  newLine print;

  "  Type         Count         Total     %         Max         Min     Average      Median" printNL;
  newLine print;

  containersByType 
  do: [
        ^self print:12;
        groupList count printWithCommas: 8;
        !tot <- groupList total:[containerSize]. printWithCommas:14.0;
        tot / ^my totalSize * 100 print: 6.1 ;
	groupList max:[containerSize]. printWithCommas:12.0;
	groupList min:[containerSize]. printWithCommas:12.0;
	groupList average:[containerSize]. printWithCommas:12.0;
	groupList median:[containerSize]. printWithCommas:12.0;
	newLine print;
	^self isPToken ifTrue:[
		"  Shift" print:12;
		groupList select:[containerSize > 28 && 
				  containerPOPCount = 1].
			  count printWithCommas: 8;
		newLine print;
		"  Cartesian" print:12;
		groupList select:[containerSize > 24 && 
				  containerPOPCount > 1].
			  count printWithCommas: 8;
		newLine print;
	];
	^self isList ifTrue:[
		"  SStore" print:12;
		!sslist <-
		groupList select:[^self getNthPOP:1 .
					containerType isFixedProperty not];
		sslist count printWithCommas: 8;
        	!tot <- sslist total:[containerSize]. printWithCommas:14.0;
                tot / ^my totalSize * 100 print: 6.1 ;
		sslist max:[containerSize]. printWithCommas:12.0;
		sslist min:[containerSize]. printWithCommas:12.0;
		sslist average:[containerSize]. printWithCommas:12.0;
		sslist median:[containerSize]. printWithCommas:12.0;
		newLine print;
		"  LStore" print:12;
		!lslist <-
		groupList select:[^self getNthPOP:1 .
					containerType isFixedProperty];
		lslist count printWithCommas: 8;
        	!tot <- lslist total:[containerSize]. printWithCommas:14.0;
                tot / ^my totalSize * 100 print: 6.1 ;
		lslist max:[containerSize]. printWithCommas:12.0;
		lslist min:[containerSize]. printWithCommas:12.0;
		lslist average:[containerSize]. printWithCommas:12.0;
		lslist median:[containerSize]. printWithCommas:12.0;
		newLine print;
	];
	^self isIndex  ifTrue:[
		"  TimeS" print:12;
		groupList select:[^self getNthPOP:1 .
					containerType isTable].
			  count printWithCommas: 8;
		newLine print;
		"  IList" print:12;
		groupList select:[^self getNthPOP:1 .
					containerType isTable not].
			  count printWithCommas: 8;
		newLine print;
	];
  ];

].
;

AdminTools defineMethod: [ | objectSpaceProfile |

!ospaces <- (spacesInNetwork -1) sequence 
  send: [ asObjectSpace extendBy: [ !containers <- ^self containerList ] 
        ] ;

"Profile of Network" printNL ;

"  Total Size (in bytes): " print: 35; 
ospaces total: [ containers total: [ containerSize ] ] .
   printWithCommasNL: 20.0 ;
"  Number of Containers:  " print: 35; 
ospaces total: [ containers count ] . printWithCommasNL: 20.0 ;

ospaces 
iterate: 
  [ newLine print; "=" fill: 70 .printNL; newLine print; 
    getSpace whatAmI print; "  -  " print; 
    profile; 
  ]; 
] ;




