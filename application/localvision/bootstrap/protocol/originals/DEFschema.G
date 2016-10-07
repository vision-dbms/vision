##################################################
#  General Protocol for Schema Classes
##################################################

"
#################################
#   Schema DEFs Required for defining Core Classes
#################################
" print ;


####################
#  Schema
####################

Schema
   define: 'CD' toBe: Schema ClassDescriptor .
   define: 'MD' toBe: Schema MessageDescriptor .
   define: 'MID' toBe: Schema MessageImplementationDescriptor .
   define: 'MessageDictionary' toBe: SystemDictionary MessageDictionary .

   defineFixedProperty: 'lastPosting' .

#--------------------
#  add existing classes to schema
#--------------------

defineMethod:
[ | addClass: class |
  ^self addClass: class as: class whatAmI at: ^global
] .

defineMethod:
[ | addClass: class at: object | 
  ^self addClass: class as: class whatAmI at: object
] .

defineMethod:
[ | addClass: class as: name at: object | 
  !error ;
  !cd ;
  class hasSchema 
  ifTrue: [ :error <- "===> Schema Already Defined For Class." ]
  ifFalse:
    [
    class super hasSchema 
    ifTrue:
       [ :cd <- ClassDescriptor createInstance: class, name, object ;
         ^self initializeClass: class ;
       ] 
    ifFalse: 
       [ :error <- 
           [ "===> Schema Must Be Defined For Parent (" print ;
             class super whatAmI print ; ") First." print 
           ] divertOutput ;
       ] ;
     ] ;
  cd isNA
    ifTrue: [ error printNL ; ]
   ifFalse: [ cd ] 
] .

defineMethod:
[ | initializeClass: class |
#--  base objects
  class instanceList do: [ :baseObject <- ^self ] ;
#--  misc values
  class :defaultFlag <- TRUE ;
#-- check for class level messages
  'asSelf' locateInDictionaryOf: class . isNA
     ifTrue: [ class defineAsMessage: 'asSelf' ] ;
  'defaultInstance' locateInDictionaryOf: class . isNA
     ifTrue: [ class define: 'defaultInstance' toBe: class ] ;
  ^self
] .

#--------------------
#  auto generate md/mid for certain types of message definition
#--------------------

defineMethod:
[ | updateMethod: name at: cd with: source | 
  MessageDescriptor getOrCreate: name in: cd .
     setTypeTo: ^global Named MessageType Method .
     setImplementationTo: source 
] .

defineMethod:
[ | setDefaultValueOf: name at: cd to: default | 
  !type <- name locateInDictionaryOf: cd class . value isTimeSeriesProperty
      ifTrue: [ ^global Named MessageType TimeSeriesProperty ]    # <--
     ifFalse: [ ^global Named MessageType FixedProperty ] ;       # <--
  !mid <- MessageDescriptor getOrCreate: name in: cd .
     setTypeTo: type .    # <--
     setDefaultValueTo: default ;
  cd class instanceList
  do: [ 
      ^self send: ^my mid messageDescriptor updateBlock . 
          <- ^my default ;
      ] ;
  ^self
] .

defineMethod:
[ | deleteMessage: name in: cd |
  !mid <- MessageDescriptor getMessage: name in: cd ;
  mid isntNA ifTrue: [ mid flagForDeletion ] ;
] .

#--------------------
#  post new messages
#--------------------

defineMethod:
[ | processAllMessages |
  initializeForPosting ;

  displayStatus ;
  !updateList <- ^self getClassesNeedingUpdate ;
  "---  Classes To Update  ---" printNL ; 
  updateList do: [ name printNL ] ;
  newLine print ; 

  ^self
    createNewMessagesForClasses: updateList . 
    refreshExistingSelectorsForClasses: updateList .
    cleanupForClasses: updateList .
    audit ;
] .

defineMethod:
[ | displayStatus | 
  
  "Schema Message Cross Reference Last Updated: " print ;
  lastPosting printNL ;
  "---  Current Counts  ---" printNL ; 
  "CD: " print: 10 ; ClassDescriptor activeList count printNL ; 
  "MD: " print: 10 ; MessageDescriptor activeList count printNL ; 
  "MID: " print: 10 ; 
  MessageImplementationDescriptor activeList count printNL ; 
  newLine print ; 
] .

defineMethod:
[ | initializeForPosting | 
  MessageImplementationDescriptor activeList
     do: [ ^self initializeForPosting ]  ;
] .

defineMethod:
[ | getClassesNeedingUpdate | 
  ClassDescriptor instanceList 
     select: [ messagesHaveBeenUpdated ] 
] .

defineMethod:
[ | createNewMessagesForClasses: cdList |
  "...  Creating New Messages" printNL ; 
  cdList
  do: [ 
      !MD <- ^my MessageDescriptor ; 
      !cd <- ^self ;
      class selectorList
         send: [ ^my MD getOrCreate: ^self in: ^my cd ] ;
      ] ;
  ^self
] .

defineMethod:
[ | refreshExistingSelectorsForClasses: cdList |
  "...  Refreshing Messages" printNL ; 
  MessageImplementationDescriptor activeList 
     select: [ ^my cdList includesElement: messageClassDescriptor  ] .
     do: [ ^self refreshValues ]  ;
  ^self
] .

defineMethod:
[ | cleanupForClasses: cdList | 
  "...  Cleanup" printNL ; 
  cdList do: [ clearMessageUpdateFlag ] ;
  ClassDescriptor clusterStrings ; 
  MessageDescriptor clusterStrings ;
  MessageImplementationDescriptor clusterStrings ;
  ^self
] .

defineMethod:
[ | audit | 
  newLine print ; 
  :lastPosting <- ^global Utility UnixSeconds currentTime ;
  displayStatus ;
  MID activeList select: [ createdInLastPosting ] . count print ; 
  " Created." printNL ; 
  MID activeList select: [ refreshedInLastPosting ] . count print ; 
  " Refreshed." printNL ;
  ^self
] .
  
;





####################
#  Schema Descriptor
####################

Schema Descriptor
   defineFixedProperty: 'shortName' .
   defineFixedProperty: 'description' . 
defineMethod:
[ | setDescriptionTo: text | 
  :description <- text ; 
  ^self
] .
defineMethod:
[ | setShortNameTo: string | 
  :shortName <- string ; 
  ^self
] .
;

####################
#  Schema ClassDescriptor
####################

Schema ClassDescriptor
   define: 'objectDescriptor' toBe: Schema ClassDescriptor .
   defineMethod: [ | asClassDescriptor | ^self asSelf ] .
   defineMethod: [ | isObjectDescriptor | ^self asSelf = objectDescriptor ] .

   defineFixedProperty: 'class' .
   defineFixedProperty: 'parent' .
   defineFixedProperty: 'number' .
   defineFixedProperty: 'genericClassDescriptor' .
   defineFixedProperty: 'messageType' .

   defineFixedProperty: 'type' .
   defineMethod: [ | isBuiltInClass | type isBuiltIn ] .
   defineMethod: [ | isCoreClass | type isCore ] .
   defineMethod: [ | isUserClass | type isUser ] .
   defineMethod: [ | isIncorporatorClass | type isIncorporator ] .
   defineMethod: [ | isObjectSpaceClass | type isObjectSpace ] .

   defineFixedProperty: 'usageLevel' .

   defineFixedProperty: 'childList' .
   defineFixedProperty: 'inheritancePath' .
   defineFixedProperty: 'defaultPropertyList' .

   defineFixedProperty: 'defaultAccessPath' .
   defineFixedProperty: 'name' .

   defineFixedProperty: 'namingDictionary' .
   defineFixedProperty: 'autoCreateNamingDictionary' .
   defineFixedProperty: 'primaryStringCluster' .

   defineFixedProperty: 'messagesHaveBeenUpdated' .

#----------
#  subclass creation
#----------

defineMethod:
[ | initializeKeys: keys |    # newClass, name, object
  :class <- keys at: 1 ; 
  :code <- keys at: 2 ; 
  :defaultAccessPath <- keys at: 3 ;
  ^self
] .

defineMethod:
[ | initializeLocalAttributes |
  :name <- class whatAmI ; 
  :parent <- class super classDescriptor ;
  :number <- ^self instanceList count ;
  class define: 'classDescriptor' toBe: ^self ; 
  ^self
    setClassTypeTo: "User" .
    setLevelTo: "Basic" ;

  :childList <- objectDescriptor childList clusterNew ;
  :inheritancePath <- objectDescriptor inheritancePath clusterNew ; 
  :defaultPropertyList <- objectDescriptor defaultPropertyList clusterNew ;
  :messageType <- ^global Named MessageType Constant ;

  :autoCreateNamingDictionary <- parent autoCreateNamingDictionary ;
  :namingDictionary <- parent namingDictionary ; 
  :genericClassDescriptor <- parent genericClassDescriptor ; 

  ^self
     buildInheritance 
     initializeDefaultPropertyList
     initializeNamingDictionary
     initializeDefaultInstance 
     initializeStringCluster
] .

#----------
# classDescriptor initialization routines
#----------

defineMethod:
[ | setClassTypeTo: typeCode |
  :type <- ^global Named ClassType uniformAt: typeCode ;
  ^self
] .

defineMethod:
[ | setLevelTo: levelCode |
  :usageLevel <- ^global Named UsageLevel uniformAt: levelCode ;
  ^self
] .

defineMethod:
[ | buildInheritance |
  parent childList at: ^self put: ^self ; 
  !next <- ^self ;
  [ next isntNA ] 
      whileTrue: [ inheritancePath at: next put: next ;
                   next != next parent 
                      ifTrue: [ :next <- next parent ]
                     ifFalse: [ :next <- NA ] ;
                 ] ;
  ^self
] .

defineMethod:
[ | initializeDefaultPropertyList | 
  !cd <- ^self ; 
  parent defaultPropertyList
    do: [ ^my cd defaultPropertyList at: messageDescriptor put: ^self ] ;
  ^self
] .

defineMethod:
[ | initializeNamingDictionary |
#--- if autoCreate is TRUE, then create naming dictionary at
#--- the same reference as parent's dictionary (or Named)
  autoCreateNamingDictionary
    ifTrue: 
      [ !dictLocation <- parent namingDictionary classDescriptor class ;
        dictLocation isDictionary
           ifFalse: [ :dictLocation <- ^global Named ] ;
        ^self createNamingDictionaryAt: dictLocation ;
      ] ;
  ^self
] .

defineMethod:
[ | createNamingDictionaryAt: dictionary |
#--- create new dictionary if it does not exist
  :namingDictionary <- dictionary at: code ;
  namingDictionary isDictionary not
    ifTrue: [ :namingDictionary <- dictionary createInstance: code ;
              dictionary define: code toBe: namingDictionary ;
              #---  add existing names (if any) to dictionary
              class instanceList select: [ code isntNA ] . 
                do: [ ^my namingDictionary at: code put: ^self ] ;
            ] ;
  ^self
] .

defineMethod:
[ | initializeDefaultInstance |
  !instanceName <- "Default" ;
  namingDictionary = parent namingDictionary 
    ifTrue: [ :instanceName <- instanceName concat: code ] ;
  namingDictionary isntNA
     ifTrue: [ namingDictionary at: instanceName put: class ];
  ^self
] .

defineMethod: [ | initializeStringCluster |
  :primaryStringCluster <- ^global String newAssociativeCluster ;
  ^self
] .

#----------
#  optional values to set for classDescriptor
#----------

defineMethod:
[ | setNamingDictionaryTo: dictionary |
  dictionary isDictionary
  ifTrue: 
    [ :namingDictionary <- dictionary;
      namingDictionary at: "Default" put: class ;
    ]; 
  ^self
] .

defineMethod:
[ | setAutoCreateNamingDictionaryOn |
  :autoCreateNamingDictionary <- TRUE ;
  ^self
] .

defineMethod:
[ | setAutoCreateNamingDictionaryOff |
  :autoCreateNamingDictionary <- FALSE ;
  ^self
] .

defineMethod:
[ | setGenericClassTo: descriptor | 
  descriptor isClassDescriptor
    ifTrue: [ :genericClassDescriptor <- descriptor ] 
   ifFalse: [ :genericClassDescriptor <- descriptor classDescriptor ] ;
  ^self
] .



#--------------------
#  default values tracking
#--------------------

defineMethod:
[ | trackDefaultValueUsing: mid |
  !storedMID <- defaultPropertyList at: mid messageDescriptor ;
  !okayToReplace <-  TRUE ;
  storedMID isntNA
  ifTrue: 
    [
    storedMID messageClassDescriptor number > 
          mid messageClassDescriptor number
     ifTrue: [ :okayToReplace <- FALSE ] ;
    ] ;
  okayToReplace
  ifTrue:
     [ defaultPropertyList at: mid messageDescriptor put: mid ;
       class send: mid messageDescriptor updateBlock . <- mid defaultValue ;
       childList select: [ type isBuiltIn not ] .
          do: [ ^self trackDefaultValueUsing: ^my mid ] ;
     ] ;
  ^self
] .

defineMethod:
[ | clearDefaultValueUsing: mid |
  !storedMID <- defaultPropertyList at: mid messageDescriptor; 
  storedMID = mid 
  ifTrue: 
    [ defaultPropertyList delete: mid messageDescriptor; 
      #-- clear it for any subclasses as well
      childList do: [ ^self clearDefaultValueUsing: ^my mid ] ;
    ] ;
^self
] .

#--------------------
#  message access
#--------------------

defineMethod:
[ | flagForMessageUpdate | 
  :messagesHaveBeenUpdated <- TRUE ; 
  ^self
] .

defineMethod:
[ | clearMessageUpdateFlag | 
  :messagesHaveBeenUpdated <- FALSE ;
  ^self
] .

defineMethod:
[ | getMessageImplementationFor: message |
  !mid <- ^global Schema MessageDescriptor getMessage: message in: ^self ;
  mid isNA && ^self != ^self parent
   ifTrue: [ ^self parent getMessageImplementationFor: message ] 
  ifFalse: [ mid ] 
] .

#--------------------------------------------------

#----------
#  printing
#----------

defineMethod:
[ | print | 
  ^self code print ; ^self
] .

defineMethod:
[ | print: format | 
  ^self code print: format ; ^self
] .

#----------
#  cleanup tools
#----------

defineMethod:
[ | clusterStrings | 
  ^self instanceList
  do: [ :code <- [ code print ] divertOutput ;
        :name <- [ name print ] divertOutput ;
      ] ;
  ^self rcleanStore ;
  ^self
] .

;

#--------------------
#  Reporting
#--------------------

Schema ClassDescriptor

#----------
#  query: boolean
#----------

defineMethod:
[ | inheritsFrom: classDescriptor |
  ^self inheritancePath at: classDescriptor asSelf . isntNA
] .

defineMethod:
[ | isSuperClassOf: classDescriptor |
  classDescriptor inheritsFrom: ^self 
] .

defineMethod:
[ | isSubClassOf: classDescriptor | 
  (^self inheritsFrom: classDescriptor ) && 
  (^self asSelf != classDescriptor asSelf)
] .

defineMethod:
[ | isInInheritancePathOf: classDescriptor |
  (^self inheritsFrom: classDescriptor) ||
  (classDescriptor inheritsFrom: ^self )
] .

#----------
#  query: list
#----------

defineMethod:
[ | getInheritancePath |
  ^self inheritancePath sortUp: [ number ] 
] .

defineMethod:
[ | getAllChildren | 
  !cd <- ^self ; 
  ^self instanceList
      select: [ ^self inheritsFrom: ^my cd ] .
      select: [ ^self != ^my cd ] 
] .  

#----------
#  display
#----------

defineMethod:
[ | showChildren |
   ^self childList 
      sortUp: [ name ] .
      do: [ ^self showChildrenAtLevel: 1 ] ;
] .

defineMethod:
[ | showChildrenAtLevel: level |
  " " print: (level * 3 ) asInteger ;
  ^self name printNL ;
  ^self childList 
      sortUp: [ ^self name ] .
      do: [ ^self showChildrenAtLevel: ^my level + 1 ] ;
] .

defineMethod:
[ | showParents |
  !path <- ^self getInheritancePath numberElements ; 
  path select: [ position < ^my path count ] .
  do: [  name printNL ;
        "  |" printNL ;
      ] ;
]  .

defineMethod:
[ | showInheritance |
   "***  Inheritance Map For Class: " print ; 
   ^self name print ;
   "  ***" printNL ; 
   newLine print ;
  !cd <- ^self ;
  cd showParents ;
  cd name print ; "   <===== YOU ARE HERE" printNL ; 
  ^self childList select: [ isActive ] .  #<---
     sortUp: [ name ] .
     do: [ ^self showChildrenAtLevel: 1 ] ;
   newLine print ;
] .
;

#--------------------
#  Schema CD getMessagesReturningClass
#     get messages that return this class or a subclass of this class
#--------------------
Schema CD defineMethod: [ | getMessagesReturningClass | 
  !baseClass <- asSelf ;
  MessageImplementationDescriptor activeList
    select: [ returnObjectType inheritsFrom: ^my baseClass ] 
] ;

#--------------------
#  Schema CD getMessagesReturningSuperClass
#    get messages that return a superclass of this one ;
#    excludes messages that are defined for this class or object
#--------------------
 
Schema CD defineMethod: [ | getMessagesReturningSuperClass |
  !baseClass <- asSelf ;
  MessageImplementationDescriptor activeList
    select: [ returnObjectType isObjectDescriptor not ] .
    select: [ returnObjectType != ^my baseClass ] .
    select: [ ^my baseClass inheritsFrom: returnObjectType ] .
] ;
 




####################
#  MessageDescriptor, MessageImplementationDescriptor, MessageDictionary
####################

Schema MessageDescriptor
   defineMethod: [ | message | code ] .
   defineFixedProperty: 'internalSelector' .
   defineFixedProperty: 'implementationList' .
   defineFixedProperty: 'functionType' .
   defineFixedProperty: 'accessBlock' .
   defineFixedProperty: 'updateBlock' .

   define: 'MessageDictionary' toBe: SystemDictionary MessageDictionary .
;

#--------------------

Schema MessageDescriptor 
do: [ :implementationList <- ^global IndexedList new ; 
    ] ;

#--------------------

Schema MessageDescriptor 

#----------
#  new instances
#----------

defineMethod:
[ | initializeKeys: key | 
  :code <- key asString ; 
  :internalSelector <- key ;
  ^self
] .

defineMethod:
[ | initializeLocalAttributes |
  MessageDictionary at: ("__" concat: code) put: ^self ;
  :implementationList <- ^self defaultInstance implementationList clusterNew ;
  ^self
] .

#----------
#  message access
#----------

defineMethod:
[ | getMessageFor: string |
  MessageDictionary at: ("__" concat: string asString ) 
] .

defineMethod:
[ | getMessage: string in: cd |
  !md <- ^self getMessageFor: string asString ;
  md isntNA ifTrue: [ md implementationList at: cd ] 
] .

defineMethod:
[ | getOrCreate: name in: cd |
  !md <- ^self getMessageFor: name ;
  !mid ;
  md isntNA 
    ifTrue: [ :mid <- md implementationList at: cd ] 
   ifFalse: [ :md <- ^self createInstance: name ] ;
 mid isNA
   ifTrue: [ :mid <- MessageImplementationDescriptor createInstance: md, cd ] ;
 mid
] .

#--------------------

#----------
#  values setting
#----------

defineMethod:
[ | setDescriptionTo: text |
  ^super setDescriptionTo: text ;
  implementationList select: [ description isNA ] .
     do: [ ^self setDescriptionTo: ^my text ] ;
  ^self
] .

defineMethod:
[ | setFunctionTypeTo: type | 
  !entry <- type ;
  type isString
   ifTrue:
     [ :entry <- ^global Named FunctionType at: type ;
       entry isNA
       ifTrue: [ :entry <- 
                   ^global FunctionType createInstance: type ;
               ] ;
     ] ;
  :functionType <- entry ;
  implementationList select: [ functionType isNA ] .
     do: [ ^self setFunctionTypeTo: ^my entry ] ;
  ^self
] .

defineMethod:
[ | setupAccessBlocks |
  accessBlock isNA
   ifTrue: [ :accessBlock <- ("[ " concat: code . concat: " ]") evaluate ;
             :updateBlock <- (" [ :" concat: code . concat: " ]") evaluate ;
           ] ;
  ^self
] .

#----------
#  cleanup
#----------

defineMethod:
[ | clusterStrings |
  ^self instanceList
  do: [ :code <- [ code print ] divertOutput ;
      ] ;
  ^self rcleanStore ;
] .

;


####################
#  MID
####################

Schema MessageImplementationDescriptor
   defineFixedProperty: 'messageDescriptor' .
   defineFixedProperty: 'messageClassDescriptor' .
   defineMethod: [ | message | messageDescriptor code ] .
   defineMethod: [ | class | messageClassDescriptor class ] .
   defineFixedProperty: 'updateDate' .
   defineFixedProperty: 'createdInLastPosting' .
   defineFixedProperty: 'refreshedInLastPosting' .

   defineFixedProperty: 'type' .
   defineFixedProperty: 'functionType' .
   defineFixedProperty: 'usageLevel' .
   define: 'implementationSource' .
   defineFixedProperty: 'internalMethod' .
   defineFixedProperty: 'returnObjectType' .
   defineFixedProperty: 'returnObjectElementType' .
   defineFixedProperty: 'parameterList' .
   defineFixedProperty: 'fullSelector' .

   defineFixedProperty: 'defaultValue' .
   defineFixedProperty: 'hasDefaultValue' .

;

#--------------------
Schema MessageImplementationDescriptor
  do: [ :messageDescriptor <- ^global Schema MessageDescriptor ;
        :messageClassDescriptor <- ^global Schema ClassDescriptor ;
        :type <- ^global MessageType ;
        :usageLevel <- ^global UsageLevel ;
        :parameterList <- ^global IndexedList new ;
        :returnObjectType <- ^global Schema ClassDescriptor ; 
      ] ;

#--------------------


Schema MessageImplementationDescriptor

#----------
# class management
#----------

defineMethod:
[ | clusterStrings |
  ^self instanceList
  do: [ :code <- [ code print ] divertOutput ;
        :implementationSource <- [ implementationSource print ] divertOutput ;
        :internalMethod <- [ internalMethod print ] divertOutput ;
      ] ;
  ^self rcleanStore ;
] .

defineMethod:
[ | initializeKeys: keys |
  :messageDescriptor <- keys at: 1 ; 
  :messageClassDescriptor <- keys at: 2 ; 
  :code <- messageDescriptor code ;
  ^self
] .
  
defineMethod:
[ | initializeLocalAttributes |
  :fullSelector <- getSelector ;
  :type <- defaultInstance type ;
  ^self setLevelTo: "Basic" ;
  :functionType <- messageDescriptor functionType ;
  :returnObjectType <-  defaultInstance returnObjectType ;
  :parameterList <-  defaultInstance parameterList clusterNew ;
  messageDescriptor implementationList 
       at: messageClassDescriptor put: ^self ;
  :createdInLastPosting <- TRUE ; 
  ^self
] .

defineMethod:
[ | cleanupDeletedObject |
  messageDescriptor implementationList delete: messageClassDescriptor ;
  asSelf clearDefaultValue ;
] .

#----------
#  display
#----------

defineMethod:
[ | displayInfo |
  message print: 30 ; " " print ; 
  type code print: 15 ; " " print ; 
  messageClassDescriptor name printNL ;
] .

defineMethod:
[ | describe |
  "Message: " print: 25 ; message printNL ;
  "Class: " print: 25 ; messageClassDescriptor name printNL ; 
  "Created: " print: 25 ; creationDate printNL ; 
  "Updated: " print: 25 ; updateDate printNL ;
  "Function Type: " print: 25 ; functionType printNL ;
  "Usage Level: " print: 25 ; usageLevel printNL ;
  description isntNA ifTrue: [ description printNL ] ;
  "Returns: " print: 25 ;  returnObjectType name print ;
  returnObjectElementType isntNA
    ifTrue: [ " of " print ; returnObjectElementType name printNL ]
   ifFalse: [ newLine print ] ;
  hasDefaultValue isntNA
     ifTrue: [ "Default Value: " print: 25 ; defaultValue printNL ] ;
  "Implementation Type: " print: 25 ; 
  type print ; 
  type isMethod
      ifTrue: [ newLine print ; implementationSource print ] ;
  type isConstant
      ifTrue: [ " ( " print ; implementationSource print ; " )" print ] ;
  newLine print ; 
] .

#----------
#  message management
#----------

defineMethod:
[ | initializeForPosting | 
  :createdInLastPosting <- FALSE ; 
  :refreshedInLastPosting <- FALSE ; 
  ^self
] .

defineMethod: [ | getInternalSelector |
code = messageDescriptor code 
   ifTrue: [messageDescriptor internalSelector]
  ifFalse: [code] .
] .

defineMethod:
[ | getInternalMessage | 
  messageDescriptor internalSelector locateInDictionaryOf: class . value 
] .

defineMethod:
[ | getSelector | 
  type isMethod
  ifTrue:
     [
     !prefix <- (implementationSource prefixNotSpannedBy: "|" . + 1 ) asInteger ;
     !selector <- implementationSource drop: prefix ;
     !prefix <- (selector prefixNotSpannedBy: newLine . + 1) asInteger ;
     prefix < 3
         ifTrue: [ :selector <- selector drop: prefix ] ;
     !prefix <- selector prefixNotSpannedBy: "|" ;
     selector take: prefix
     ] 
   ifFalse: [ code ] 
] .

defineMethod:
[ | refreshValues | 
  !source <- getInternalMessage ;
  !currentSource <- internalMethod ;
  !sourceString <- [ source print ] divertOutput ;
  !currentType <- source classDescriptor messageType ;
  currentType isConstant
    ifTrue: [ ^self setReturnObjectTypeTo: source classDescriptor ] ;
  currentType != type 
  ifTrue: 
    [ ^self setTypeTo: currentType ;
      ^self setImplementationTextTo: sourceString forMethod: sourceString ;
    ]
  ifFalse:
    [
    !newSource <- currentSource != sourceString ;
    newSource
    ifTrue: 
       [ ^self setImplementationTextTo: sourceString forMethod: sourceString ;
       ] ;
    ] ;
  ^self
] .

defineMethod:
[ | setTypeTo: newType | 
  :type <- newType ; 
  :refreshedInLastPosting <- TRUE ; 
  ^self
] .

defineMethod:
[ | setImplementationTo: text | 
  ^self setImplementationTextTo: text 
        forMethod: [ ^self getInternalMessage print ] divertOutput
] .

defineMethod:
[ | setImplementationTextTo: text forMethod: method |
  :updateDate <- ^today ;
  :refreshedInLastPosting <- TRUE ; 
  :implementationSource asOf: ^today put: text ;
  :internalMethod <- method ; 
  ^self
] .

defineMethod:
[ | setReturnObjectTypeTo: cd |
  cd isClassDescriptor 
    ifTrue: [ :returnObjectType <- cd ] 
   ifFalse: [ :returnObjectType <- cd classDescriptor ] ;
  ^self
] .

defineMethod:
[ | setReturnObjectElementTypeTo: cd |
  cd isClassDescriptor 
    ifTrue: [ :returnObjectElementType <- cd ] 
   ifFalse: [ :returnObjectElementType <- cd classDescriptor ] ;
  ^self
] .

defineMethod:
[ | setDefaultValueTo: val |
type isProperty     #<-- was type isFixedProperty
ifTrue: 
  [ messageDescriptor setupAccessBlocks; 
    :defaultValue <- val; 
    :hasDefaultValue <- TRUE; 
    ^self setReturnObjectTypeTo: val classDescriptor; 
    messageClassDescriptor trackDefaultValueUsing: ^self; 
  ] ; 
^self
] .

defineMethod:
[ | clearDefaultValue |
  :defaultValue <- NA ; 
  :hasDefaultValue <- NA ; 
  messageClassDescriptor clearDefaultValueUsing: ^self ;
  ^self
] .

defineMethod:
[ | setDescriptionTo: text |
  ^super setDescriptionTo: text ;
  messageDescriptor description isNA
     ifTrue: [ messageDescriptor setDescriptionTo: text ] ;
  ^self
] .

defineMethod:
[ | setFunctionTypeTo: type | 
  !entry <- type ;
  type isString
   ifTrue:
     [ :entry <- ^global Named FunctionType at: type ;
       entry isNA
       ifTrue: [ :entry <- 
                   ^global FunctionType createInstance: type ;
               ] ;
     ] ;
  :functionType <- entry ;
  messageDescriptor functionType isNA
     ifTrue: [ messageDescriptor setFunctionTypeTo: entry ] ;
  ^self
] .

defineMethod:
[ | setLevelTo: string |
  :usageLevel <- ^global Named UsageLevel uniformAt: string ;
  ^self
] .

defineMethod:
[ | setParameter: number typeTo: parameterType |
  !cd <- parameterType ;
  cd isClassDescriptor 
     ifFalse: [ :cd <- cd classDescriptor ] ;
  parameterList at: number put: cd ;
  ^self
] .

defineMethod:
[ | setSelectorTo: string |
  :fullSelector <- string ; 
  ^self
] .  

;



####################
#  SystemTable 
####################

MessageType 
  defineMethod: [ | isConstant | code = "Constant" ] .
  defineMethod: [ | isFixedProperty | code = "FixedProperty" ] .
  defineMethod: [ | isTimeSeriesProperty | code = "TimeSeriesProperty" ] .
  defineMethod: [ | isMethod | code = "Method" ] .
  defineMethod: [ | isPrimitive | code = "Primitive" ] .
  defineMethod: [ | isFixed | isFixedProperty ] .
  defineMethod: [ | isProperty | isFixedProperty || isTimeSeriesProperty ] .
;

ClassType
  defineMethod: [ | isBuiltIn | code = "BuiltIn" ] .
  defineMethod: [ | isCore | code = "Core" ] .
  defineMethod: [ | isUser | code = "User" ] .
  defineMethod: [ | isIncorporator | code = "Incorporator" ] .
  defineMethod: [ | isObjectSpace | code = "ObjectSpace" ] .
;

UsageLevel
  defineMethod: [ | isBasic | code = "Basic" ] .
  defineMethod: [ | isInternal | code = "Internal" ] .
  defineMethod: [ | isAdvanced | code = "Advanced" ] .
  defineMethod: [ | isReserved | code = "Reserved" ] .
;



