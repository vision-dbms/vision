"
##################
####  Object  ####
##################
"
?g

##############################################################################
# Note:
#	The 'Object' methods, 'defineMethod:' and 'define:toBe' are defined
#	in 'NameBuildIns'.
##############################################################################

####################
#  Print and Other Standard Messages
####################

Prototypical Object
defineMethod: [ | print |  ^self whatAmI print; ^self ] .
defineMethod: [ | printNL | ^self print; newLine print; ^self ]  .
defineMethod: [ | print: aWidth | ^self print;  ^self ] .
defineMethod:
[ | printNL: aWidth |
  ^self print: aWidth;
  newLine print;
  ^self
] .
defineMethod: [ | printWithCommas: aWidth | ^self print: aWidth; ^self ] .
defineMethod: 
[ | printWithCommasNL: aWidth |  
  ^self printWithCommas: aWidth;
  newLine print;
  ^self
] .
defineMethod: [ | printUniversal: format | ^self print: format ] .
defineMethod: [ | printPRN | ^self print; ^self  ] .
defineMethod: [ | printPRN: aFormat | ^self print: aFormat; ^self ] .
defineMethod: [ | printOf: anObject |  anObject print] .

defineMethod:
[ | sprint: aFormat |
  [ ^self print ] divertOutput take: aFormat asInteger
] .

defineMethod:
[ | displayInfo | 
  " " print ; ^self whatAmI print ; " " print ; ^self code printNL ;
] .

defineMethod:
[ | profile | 
  "Class: " print: 30 ; ^self whatAmI printNL ;
  "Code: " print: 30 ; ^self code printNL ; 
  "Created: " print: 30 ; ^self creationDate printNL ;
  "Is Default: " print: 30 ; ^self isDefault printNL ;
  "Is Active: " print: 30 ; ^self isActive printNL ;
  ^self isDeleted && ^self deletionReason isntNA
     ifTrue: [ "  --> " print ; ^self deletionReason printNL ] ;
] .

;


####################
# Primitives and Converses
####################
Prototypical Object

#--------------------
#  class creation and management
#--------------------
    define: 'basicSpecialized'     toBe: 100 asPrimitive .     #Specialize
    define: 'specializeClass'      toBe: 101 asPrimitive .     #SpecializeClass
    define: 'basicSpecializeClass' toBe: 101 asPrimitive .     #SpecializeClass
    define: 'isToolKit' toBe: FALSE .
    define: 'isBridge' toBe: FALSE .
    define: 'isDataRecord' toBe: FALSE .
defineMethod: [ | specialized |
  !block <- "[ ]" evaluate; 
  !dontCluster <- 
     (^self = ^global CoreWorkspace Object) || 
     (^self isToolKit) || (^self isBridge) || (^self isDataRecord) ;
  dontCluster
    ifTrue: [ ^self basicExtend: block . newPrototype new ] 
   ifFalse: [ ^self new basicExtend: block ] .
     initializeGlobalSubclassProperties 
     initializeGlobalInstanceProperties
] .

defineMethod:
[ | specializeAs: name |
  ^self specializeAs: name at: ^global 
] .

defineMethod:
[ | specializeAs: name at: object |
!newClass <- ^self specialized; 
name isntNA 
ifTrue: 
  [ object define: name toBe: newClass; 
    !isMessage <- "is" concat: name; 
    !whatAmI <- name; 
    (object == ^global CoreWorkspace) || (object == ^global BuiltInWorkspace)
       || (object == ^global && ^global number = 3) 
       || (object == ^global && ^global code = "DDMS") 
       || (object == ^global && ^global code = "Schema") 
           ifFalse: [:whatAmI <- object whatAmI concat: " " .concat: name]; 
    newClass define: isMessage toBe: TRUE .
             define: 'whatAmI' toBe: whatAmI; 
    object objectSpace <= 3     #- change to object from ^global
        ifTrue: [^global CoreWorkspace Object define: isMessage toBe: FALSE]; 
  ]; 
newClass
] .

defineMethod:
[ | initializeGlobalSubclassProperties |
  ^self :defaultFlag <- TRUE ;       #---
  ^self :code <- "Default" ;         #---
  ^self defineAsMessage: 'asSelf' ;
  ^self define: 'defaultInstance' toBe: ^self ;
  ^self
] .

    define: 'alignStore'           toBe: 116 asPrimitive .     #AlignStore
    define: 'cleanStore'           toBe: 109 asPrimitive .
    define: 'cleanDictionary'      toBe: 110 asPrimitive .
defineMethod:
[ | cleanStoreAndDictionary |
  ^self cleanStore || ^self cleanDictionary
] .

defineMethod:
[ | rcleanStoreAndDictionary |
  ^self rcleanStore || ^self rcleanDictionary
] .

defineMethod:
[ | rcleanStore |
  !status <- ^self != ^self super 
      ifTrue: [ ^self super rcleanStore ]
     ifFalse: [ FALSE ] ;
  status || ^self cleanStore
] .

defineMethod:
[ | rcleanDictionary |
  !status <- ^self != ^self super 
    ifTrue: [ ^self super rcleanDictionary ]
   ifFalse: [ FALSE ];
  status || ^self cleanDictionary
] .

defineMethod:
[ | cleanupClassStructures |
  ^self cleanStoreAndDictionary
    ifTrue: [ ^self whatAmI print ; " -- store and/or dictionary" printNL ] ;
] .

#--------------------
#  instance creation  and management
#--------------------
    define: 'basicNew'             toBe: 90 asPrimitive .     #NewInstance
    define: 'clusterNew'           toBe: 90 asPrimitive .
defineMethod:
[ | new |
  ^self clusterNew initializeGlobalInstanceProperties
] .

defineMethod:
[ | newAs: string | 
  ^self new do: [ ^self :code <- ^my string ] 
] .

defineMethod:
[ | initializeGlobalInstanceProperties |
  ^self :baseObject <- ^self ;       #---
  ^self :creationDate <- ^today ;    #---
  ^self
] .

    define: 'newPrototype'         toBe: 91 asPrimitive .     #NewPrototype
    define: 'basicNewPrototype'    toBe: 91 asPrimitive .     #NewPrototype
    define: 'basicInstanceList'    toBe: 105 asPrimitive .     #RowsInStore
    define: 'instanceList'         toBe: 105 asPrimitive .     #RowsInStore
    define: 'delete'               toBe: 95 asPrimitive .     #DeleteInstance
    define: 'basicDelete'          toBe: 95 asPrimitive .     #DeleteInstance
    defineMethod:
     [| rdelete |
         !selfsuper <- ^self super;
         (!status <- ^self delete) ifTrue: [selfsuper rdelete];
         status
     ] .

    define: 'objectSpace'      toBe: 85 asPrimitive .     #ObjectSpace
    define: 'containerIndex'   toBe: 86 asPrimitive .     #ContainerIndex
defineMethod: 
[ | displayPOP | 
  "[" print;
  ^self objectSpace print: 4;
  "," print;
     ^self containerIndex print: 6;
  "]" printNL;
  ^self
] .

   define:'_establishResidenceInSpaceOf:' toBe: 103 asPrimitive .
defineMethod: [ | establishResidenceInSpaceOf: object |
  object isNA 
   ifTrue: [
            ">>>  Cluster not forwarded: NA target  <<<" printNL;
            FALSE
           ]
  ifFalse: [
            !result <- ^self _establishResidenceInSpaceOf: object;
            result ifFalse: [ ">>>  Cluster not forwarded  <<<" printNL] ;
            result
          ]
] .

defineMethod: [ | provideResidenceFor: aCluster|
   aCluster establishResidenceInSpaceOf: ^self
] .

#--------------------
#  message definition and management
#--------------------
    define: 'selectorList'         toBe: 114 asPrimitive .     #DictionaryContents
defineMethod:
[ | define: aSelector | 
  aSelector locateOrAddInDictionaryOf: ^self. <- ^my TheDefaultProperty;
  ^self
] .
defineMethod:
[ | defineProperty: aSelector | 
  aSelector locateOrAddInDictionaryOf: ^self. <- ^my TheDefaultProperty;
  ^self
] .
defineMethod:
[ | defineFixedProperty: aSelector | 
  aSelector locateOrAddInDictionaryOf: ^self. <- ^my TheDefaultFixedProperty;
  ^self
] .
defineMethod:
[ | locateOrAddDictionaryEntryFor: aSelector |
  aSelector locateOrAddInDictionaryOf: ^self
] .
defineMethod:
[ | locateDictionaryEntryFor: aSelector | 
   aSelector locateInDictionaryOf: ^self
] .
defineMethod:
[ | deleteDictionaryEntryFor: aSelector | 
  aSelector deleteFromDictionaryOf: ^self
] .

defineMethod:
[ | define: aSelector toBePrimitive: number |
    ^self define: aSelector toBe: number asInteger asPrimitive
].

defineMethod:
[ | define: aSelector toBePrimitive: number withControlValue: controlValue |
    ^self define: aSelector toBePrimitive: number + controlValue * 65536
].

#--------------------
#  do, send, extend
#--------------------
  define: 'do:'                toBe: 37 asPrimitive .
  define: 'basicDo:'           toBe: 37 asPrimitive .
  define: 'send:'              toBe: 38 asPrimitive.
  define: 'send:with:'         toBe: 38 asPrimitive.
  define: 'send:with:and:'     toBe: 38 asPrimitive.
  define: 'send:with:and:and:'   toBe: 38 asPrimitive.
  define: 'basicSend:'         toBe: 38 asPrimitive.
  define: 'basicSend:with:'    toBe: 38 asPrimitive.
  define: 'extendBy:'          toBe: 39 asPrimitive.
  define: 'extend:'            toBe: 39 asPrimitive.
  define: 'extend:with:'       toBe: 39 asPrimitive.
  define: 'basicExtend:'       toBe: 39 asPrimitive.
  define: 'basicExtend:with:'  toBe: 39 asPrimitive.

  define: 'canonicalizedForSort' toBe: NA .
  define: 'basicSendCanonicalizedForSort:' toBe: 38 asPrimitive.
  define: 'orderUsingDictionary:' toBe: 10 asPrimitive.     # Self

  defineMethod:
  [ | collect: aBlock | 
         [!value <- aBlock asMethod asClosure: ^self] environment
  ] .

 defineMethod: [ | value | ^self ] .

#--------------------
#  relational and boolean query
#--------------------
    define: 'isTrue'           toBe: FALSE .
    define: 'isFalse'          toBe: FALSE .
    define: 'isNA'             toBe: FALSE .
    define: 'isntNA'           toBe: TRUE .
    define: '=='               toBe: 20 asPrimitive .    #IsIdenticalTo
    define: '='                toBe: 20 asPrimitive .     #IsIdenticalTo
    define: '!=='              toBe: 21 asPrimitive .     #IsntIdenticalTo
    define: '!='               toBe: 21 asPrimitive .     #IsntIdenticalTo
    defineMethod: [  |  > object |  NA ] .
    defineMethod: [ | >= object | NA ] .
    defineMethod: [ | < object |  NA ] .
    defineMethod: [ | <= object | NA ] .

defineMethod:
[ | ifTrue: object1 else: object2 |
  object2 value
] .

defineMethod:
[ | ifFalse: object1 else: object2 |
  object2 value
] .

defineMethod:
[ | ifTrue: object1 ifFalse: object2 else: object3 |
  object3 value
] .

  defineMethod: [ | else: object | ^self ] .
  defineMethod: [ | elseIf: condition then: object | ^self ] .

defineMethod: [ | ifDefault: object | 
  ^self isntDefault ifTrue: [ ^self ] ifFalse: [ object value ] 
] .

defineMethod: [ | ifDefault: block1 else: block2 |
  ^self isDefault
     ifTrue: [ ^self send: block1 ] else: [ ^self send: block2 ]
] .

defineMethod: [ | isntDefault: block1 else: block2 |
  ^self isntDefault
     ifTrue: [ ^self send: block1 ] else: [ ^self send: block2 ]
] .

defineMethod: [ | if: condition then: sendYesBlock else: sendNoBlock |
 ^self send: condition .
   ifTrue: [ ^self send: sendYesBlock ] .
     else: [ ^self send: sendNoBlock ]
] .

defineMethod: [ | if: condition then: sendYesBlock |
 ^self send: condition .
   ifTrue: [ ^self send: sendYesBlock ] .
] .

#--------------------
#  ^local 
#--------------------

   define: 'asLocalContextFor:'	toBe: 36 asPrimitive.

defineMethod:
[ | extendedBy: block1 asLocalContextFor: block2 |
    ^self extendBy: block1. asLocalContextFor: block2
] .

#--------------------
#  misc converses and related covers
#--------------------
defineMethod:
[ | asList |  
  ^my Prototypical List new, ^self
] .

defineMethod:
[ | , anObject | 
  ^self asList, anObject
] .

defineMethod:
[ | -> cell | 
  cell <- ^self
] .

defineMethod:     
[ | defineInAList: aList | 
  aList newValueCell: ^self
] .
defineMethod:     
[ | locateInAList: aList | 
  aList valueCell: ^self
] .
defineMethod:     
[ | deleteFromAList: aList | 
  aList delete: ^self
] .
defineMethod: 
[ | deleteColumnFromStore: aList | 
   aList deleteColumn: ^self
] .

defineMethod:
[ | referencedBy: anInteger | 
   anInteger asReferenceTo: ^self
] .

defineMethod:
[ | adjacentInstancesFor: anInteger | 
  anInteger instancesAdjacentTo: ^self
] .

defineMethod:
[ | pointedToBy: aValue | 
  aValue asPointerTo: ^self
] .
defineMethod:
[ | closureOf: aBlock | 
  aBlock asClosure: ^self
] .

defineMethod:
[ | withCompilationErrorHandler: handler evaluate: string |
    string evaluateWithCompilationErrorHandler: handler in: ^self
] .

defineMethod:
[ |  subClassResponsibility: aSelector  | 
  ">>> '"print;
  aSelector print;
  "' must be implemented for '" print;
  ^self print;
  "' <<<" printNL;
  NA
] .

defineMethod:
[ | DBUpdateAppendDump: filename | 
  filename additionUpdateDumpOf: ^self
] .
defineMethod:
[ | DBUpdateReplaceDump: filename | 
  filename replacementUpdateDumpOf: ^self
] .
;

?g


##################################################
#    Object Definitions
#
#   - Message creation
#   - General properties and protocol
#   - Globals
#   - display
#   - general
#
##################################################


#----------
#  Define a generic method to create messages that strip extensions
#    at a class level
#  Usage:  Class defineAsMessage: 'asBaseClass'
#  Example:  Prototypical Integer defineAsMessage: 'asInteger'
#----------
Prototypical Object
  defineMethod:
  [ | defineAsMessage: aSelector |
    ^self define: aSelector toBe: 11 asPrimitive
  ] ;

####################
#  redefine basic message definition messages to be available using __define
####################
'__defineMethod:' locateOrAddInDictionaryOf: Prototypical Object . <-
[ | __defineMethod: aBlock |
  aBlock selector
    locateOrAddInDictionaryOf: ^self . <- aBlock asMethod ;
    ^self
] asMethod ;

Prototypical Object
__defineMethod:
[ | __define: aSelector toBe: anObject |
    aSelector locateOrAddInDictionaryOf: ^self. <- anObject;
    ^self
] .

__defineMethod:
[ | __define: aSelector |
  aSelector locateOrAddInDictionaryOf: ^self. <- ^my TheDefaultProperty;
  ^self
] .

__defineMethod:
[ | __defineProperty: aSelector |
  aSelector locateOrAddInDictionaryOf: ^self. <- ^my TheDefaultProperty;
  ^self
] .

__defineMethod:
[ | __defineFixedProperty: aSelector |
  aSelector locateOrAddInDictionaryOf: ^self. <- ^my TheDefaultFixedProperty;
  ^self
] .

;


####################
#  General Propeties and Protocol
####################

Prototypical Object
   defineFixedProperty: 'code' .
   defineFixedProperty: 'baseObject' .

defineMethod:
[ | masterList | 
  ^self asSelf instanceList select: [ isntDefault ] .
     send: [ ^self asBaseObject ] 
] .
defineMethod:
[ | activeList | 
  ^self asSelf instanceList send: [ ^self asBaseObject ] .
     select: [ ^self isntDefault && ^self isActive ] .
] .

defineMethod:
[ | asBaseObject |
  ^self baseObject isntNA
    ifTrue: [ ^self baseObject ] ifFalse: [ ^self ] 
] .
   defineFixedProperty: 'creationDate' .
   define: 'defaultInstance' toBe: Prototypical Object .
   defineFixedProperty: 'defaultFlag' .
   defineMethod: [ | isDefault | ^self defaultFlag = TRUE ] .
   defineMethod: [ | isntDefault | ^self isDefault not ] .
   defineFixedProperty: 'deletionFlag' .
   defineMethod: [ | isActive | ^self deletionFlag isNA ] .
   defineMethod: [ | isntActive | ^self isActive not ] .
   defineMethod: [ | isDeleted | ^self isntActive ] .
   defineMethod: [ | isntDeleted | ^self isActive ] .
   defineFixedProperty: 'deletionDate' .
   defineFixedProperty: 'deletionReason' .

defineMethod:
[ | flagForDeletionWithReason: reason |
  ^self
  do: [ :deletionFlag <- TRUE ;
        :deletionDate <- ^today ;
        :deletionReason <- ^my reason ;
        cleanupDeletedObject ;
      ] ;
] .

defineMethod:
[ | cleanupDeletedObject |
] .

defineMethod:
[ | flagForDeletion | 
  ^self flagForDeletionWithReason: NA
] .

defineMethod:
[ | isInternalDefault | 
  -1 asReferenceTo: ^self asSelf . = ^self asSelf
]  .

defineMethod: [ | respondsTo: message | 
!selector <- message asSelector  ;
selector isntNA
   ifTrue: [ selector locateInDictionaryOf: ^self .isntNA 
              ifTrue: [TRUE].
              elseIf: [^self super != ^self]
                then: [^self super respondsTo: selector ].
                else: [FALSE]
           ]
  ifFalse: [ FALSE ]
] .

#--------------------
#  Add methods to generic assign the value of a property or a
#    time series property 
#
#  setProperty: p to: object
#     p can be a string or a block containing an intensional value
#        (note that in list operations, the block will be more efficient);
#     if p is a time series, the property is updated as of ^date; since
#     ^self is returned, this message allows property assignments to be
#     cascaded:
#        Named Security IBM
#           setProperty: "x" to: 10 .
#           setProperty: [ :y ] to: Named Company IBM .
#           setProperty: "z" to NA ;
#
#  updateTS: ts with: object
#    ts is a string or block containing intensional form of ts property;
#    this version calls the updateWith: message at time series which will
#    only update the t/s as of ^date if the value is different from the
#    current value as of ^date
#--------------------

defineMethod:
[ | setProperty: p to: object |
#-- if p represents a property, set it to object asSelf in ^self
  !prop <- p isBlock
     ifTrue: [ p ] .
     elseIf: [ p isString ] 
       then: [ ":" concat: p . asBlock ] ;
  !update <- ^self send: prop ;
  update isValue
     ifTrue: [ update <- object asSelf ; ^self ] .
     elseIf: update isTimeSeries
       then: [ update put: object asSelf ; ^self ] .
       else: [ ">>> Property " print ; p print ; " Not Found For " print ;
               ^self whatAmI printNL ; ^self
             ]
] .

defineMethod:
[ | updateTS: ts with: object |
#-- update ts property only if value changes
  !prop <- ts isBlock
     ifTrue: [ ts ] .
     elseIf: [ ts isString ] 
       then: [ ":" concat: ts . asBlock ] ;
  !update <- ^self send: prop ;
  update isTimeSeries
    ifTrue: [ update updateWith: object ] 
   ifFalse: [ ">>> TS Property " print; ts print ; " Not Found For " print ;
               ^self whatAmI printNL 
             ] ;
  ^self
] .

defineMethod: [ | changeProperty: p to: object | 
   !prop <- p isBlock 
      ifTrue: [ p ]. else: [ p asString asUpdateBlock ] ;
   !update <- ^self send: prop; 
   update isValue || update isTimeSeries
     ifTrue: [ update changeTo: object ] 
    ifFalse: 
      [ ">>> Property " print; p print; " Not Found For " print; 
        ^self whatAmI printNL; 
      ] ;
  ^self
] .

#--------------------
#  Define an object and a  date range method that evaluates a block 
#    for a supplied object for each date in the range and returns the 
#    results as a time series, one point for each date in the range.
#  If supplied date range is a single date, the value is returned as
#    of the date.
#--------------------

defineMethod:
[ | extract: block forDateRange: dr |
  dr extract: block for: ^self
] .

;


####################
#  'Global Constants'
####################
Prototypical Object
  define: 'NA' toBe: NA .
  define: 'TRUE' toBe: TRUE .
  define: 'FALSE' toBe: FALSE .
  define: 'newLine' toBe: "
" .
 define: 'newPage' toBe: "" .
 define: 'earliestPossibleDate' toBe: Prototypical Date .
;

#--------------------------------------------------

####################
#  display Methods
####################

Prototypical Object 

#----------
#  displays all selectors defined explicitly at recipient object
#    (i.e., does not walk the inheritance hierarchy
#----------
defineMethod: 
[ | displayMessages |
^self selectorList do: [ printNL ] .
count = 0
   ifTrue: [ "...  No Messages Defined At This Level." printNL ] ;

] .

#----------

#----------
#  Extended displayMessages
#     walks the object inheritance hierarchy and displays all messages
#        an object can respond to (unlike displayMessages which only
#        prints protocol added or redefined by class)
#     sorts the output alphabetically and display type of message, and
#         the class it was defined at
#----------

#----------
#  Extended displayMessages
#     walks the object inheritance hierarchy and displays all messages
#        an object can respond to (unlike displayMessages which only
#        prints protocol added or redefined by class)
#     sorts the output alphabetically and display type of message, and
#         the class it was defined at
#   Note: messages at Object are excluded
#----------

defineMethod:
[ | displayMessagesX |

"Extended Message List For: " print;
^self whatAmI printNL ;  
"   (excludes messages defined at the class Object)" printNL ;

!messagesList <- ^global Prototypical List new ;
!class <- ^self ;
!notDone <- TRUE ;

###  Collect information for all messages in each class in the hierarchy
[ notDone ] 
whileTrue: [ class selectorList 
                extendBy:
                    [ !level <- ^my class whatAmI ;
                      !type <- ^self locateInDictionaryOf: ^my class . value ;
                      type isProperty || type isPrimitive || type isMethod 
                          ifTrue: [ :type <- type whatAmI ]
                         ifFalse: [ :type <- "Constant" ] ;
                      !name <- asString ;
                    ] .
                do: [ ^my messagesList, ^self ] ;

### test if hierarchy stops  == used because = may be redefined (eg String)
           class super == class super super
                                ifTrue: [ :notDone <- FALSE ] 
                               ifFalse: [ :class <- class super ]
           ] ;

###  Display Report
messagesList sortUp: [ name ] .
do: [ name print: 30 ; type print: 20 ; level printNL ] ;
] .

#----------
#  Extended displayMessages
#     walks the object inheritance hierarchy and displays all messages
#        an object can respond to (unlike displayMessages which only
#        prints protocol added or redefined by class)
#     sorts the output alphabetically and display type of message, and
#         the class it was defined at
#   Note: messages at Object are included
#----------

defineMethod:
[ | displayMessagesAll |

"Extended Message List For: " print;
^self whatAmI printNL ;  
"   (includes messages defined at the class Object)" printNL ;

!messagesList <- ^global Prototypical List new ;
!class <- ^self ;
!notDone <- TRUE ;

###  Collect information for all messages in each class in the hierarchy
[ notDone ] 
whileTrue: [ class selectorList 
                extendBy:
                    [ !level <- ^my class whatAmI ;
                      !type <- ^self locateInDictionaryOf: ^my class . value ;
                      type isProperty || type isPrimitive || type isMethod 
                          ifTrue: [ :type <- type whatAmI ]
                         ifFalse: [ :type <- "Constant" ] ;
                      !name <- asString ;
                    ] .
                do: [ ^my messagesList, ^self ] ;

### test if hierarchy stops  == used because = may be redefined (eg String)
                class == class super
                                ifTrue: [ :notDone <- FALSE ] 
                               ifFalse: [ :class <- class super ]
           ] ;

###  Display Report
messagesList sortUp: [ name ] .
do: [ name print: 30 ; type print: 20 ; level printNL ] ;
] .

#----------
#  display only time series properties 
#__________

defineMethod:
[ | displayTimeSeries |
"Time Series Defined At: " print;
^self whatAmI printNL ;
!class <- ^self ;
class selectorList 
  select: [ ^self locateInDictionaryOf: ^my class . value isTimeSeriesProperty
          ] .
do: [ printNL ]  .
    count = 0 ifTrue: [ "... No Time Series Found." printNL ] ;
] .

#----------

#----------
#  display only propeties - * for time series properties
#----------

defineMethod:
[ | displayProperties |
"Properties Defined At: " print;
^self whatAmI printNL ;
!class <- ^self ;
!tsFound <- FALSE ;
^self selectorList 
do: [
    !type <- ^self locateInDictionaryOf: ^my class . value ;
    type isProperty 
    ifTrue: [ type isTimeSeriesProperty
              ifTrue: [ " * " print ; 
                        ^my :tsFound <- TRUE ;
                      ]
             ifFalse: [ "   " print ; ] ;
             printNL ;
            ] ;
   ] ;
tsFound 
ifTrue: 
   [  newLine print ;
      " *  Indicates Time Series Property" printNL ;
   ] ;
  
] .

#----------

defineMethod: 
[ | displayMessageNamesContaining: aString |
  "  ---> Displaying Messages at Class " print; ^self whatAmI print ;
  "  That Contain " print; "\"" print ; aString print ; "\"" printNL ;
  "       (Note - Excludes Super Classes)" printNL ;
  newLine print ;
  !list <- ^self selectorList select: [ asString contains: ^my aString ] ;
  list count > 0 
     ifTrue: [ list do: [ printNL ] ] 
    ifFalse: [ "...  No Messages Found. " printNL ] ;
   
] .

#----------

#----------
#  display each level of recipient object's inheritance chain
#----------

defineMethod:
[ | displayInheritance |

!count <- 0 ;
!obj <- ^self ;
[ obj !== obj super ]
whileTrue: [ :count <- count + 3 ;
             "-" fill: count asInteger . concat: "> " . print;
              obj whatAmI printNL; 
             :obj <- obj super;
            ] ;
"-" fill: (count + 3 ) asInteger . concat: "> Object" .  printNL ;
] ;


####################
#  Other general messages
####################

Prototypical Object

defineMethod:
[ | isEquivalentTo: object | 
  ^self asSelf = object asSelf 
] .
defineMethod: 
[ | isntEquivalentTo: object | 
  ^self isEquivalentTo: object . != TRUE
] .

defineMethod:
[ | hasExtensions | 
  ^self !== ^self asSelf
] .

defineMethod:
[ | displayLabel |
  [ ^self print ] divertOutput
] .

defineMethod: [ | displayLabelKey | ^self displayLabel ] .

defineMethod:
[ | helpMessage |
  [ ^self printNL; 
    "(Class - " print; ^self whatAmI print; " ) " printNL; 
    "...  No Additional Help Available For: " printNL; 
    ^self printNL; 
  ] divertOutput
] .

defineMethod:
[ | validateInputString: string |
  !nd <- ^self classDescriptor namingDictionary; 
  nd isntNA 
    ifTrue: [nd at: string stripBoundingBlanks]
   ifFalse:
      [ ^global respondsTo: string asString .
           ifTrue: [ string asString evaluate ] 
      ] 
] .

defineMethod:
[ | validateInputObject: object |
  object inheritsFrom: ^self
] .

defineMethod:
[ | getPotentialChoicesForString: string |
  ^self masterList select: [ code contains: ^my string ] 
] .

defineMethod:
[ | getInitialScreeningUniverse |
  ^self masterList
] .

defineMethod:
[ | accessExpression |
  [ ^self print ; " " print ; ] divertOutput
] .

defineMethod:
[ | inputExpression |
  [ " \"" print ; ^self print ; "\" " print ] divertOutput
] .

defineMethod:
[ | asString |
  [ ^self print ] divertOutput
] .

defineMethod: [ | convertFrom: string | 
  ^self validateInputString: string asString . 
      else: [ ^self defaultInstance ]
] .

defineMethod:
[ | execute: message in: class |
  ^self asList execute: message in: class
] .

;
