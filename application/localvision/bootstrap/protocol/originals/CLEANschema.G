##################################################
#  Schema modifications and cleanups for existing scripts
#     These need to be merged into appropriate *schema* scripts and DOC
#     files
##################################################

####################
#  New and modified schema protocol to support NAV and DataFeed projects
####################

#--------------------
#  Schema modifications
#--------------------

#----------
#  Object initializeDefaults
#     use updateBlock at mid, not md
#----------

Object defineMethod: [ | initializeDefaults | 
^self hasSchema 
ifTrue: 
  [ !object <- ^self; 
    ^self classDescriptor defaultPropertyList 
    do: [ !currentSlot <- ^my object send: updateBlock; 
          currentSlot value isNA 
              ifTrue: [ currentSlot <- defaultValue ] ; 
	]; 
  ]; 
^self 
] ;

#----------
#  Schema setDefaultValueOf: at: to: 
#    remove explicit update of class instances since this is 
#    handled by the setDefaultValueTo: message
#----------
Schema defineMethod: [ | setDefaultValueOf: name at: cd to: default | 
  !type <- name locateInDictionaryOf: cd class .value isTimeSeriesProperty
      ifTrue: [ ^global Named MessageType TimeSeriesProperty ]
     ifFalse: [ ^global Named MessageType FixedProperty ]; 
  !mid <- MessageDescriptor getOrCreate: name in: cd .
     setTypeTo: type .
     setDefaultValueTo: default; 
#  default isntNA
#  ifTrue:
#    [
#    cd class instanceList 
#    do: [ ^self send: ^my mid messageDescriptor updateBlock . <- ^my default; 
#        ]; 
#    ] ;
  ^self
] ;


#----------
#  Schema CD initializeLocalAttributes
#     add call to assignDefaultType and string clustering
#----------
Schema CD defineMethod: [ | initializeLocalAttributes |
  :name <- class whatAmI ; 
  :parent <- class super classDescriptor ;
  :number <- ^self instanceList count ;
  class define: 'classDescriptor' toBe: ^self ; 
  ^self assignDefaultType ;     # <---

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
] ;

#----------
#  Schema CD trackDefaultValueUsing: 
#    use updateBlock at mid, not md
#----------
Schema CD defineMethod: [ | trackDefaultValueUsing: mid |
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
       class send: mid updateBlock . <- mid defaultValue ;
       childList select: [ type isBuiltIn not ] .
          do: [ ^self trackDefaultValueUsing: ^my mid ] ;
     ] ;
  ^self
] ;

#----------
#  Schema MD initializeLocalAttributes
#     add upper case version for case-insensitive lookups
#----------

Schema MD defineMethod: [ | initializeLocalAttributes |
  MessageDictionary at: ("__" concat: code) put: ^self; 
  !upper <- "___" concat: code toUpper ;
  MessageDictionary at: upper . isNA || [ code capitalize != code ] 
     ifTrue: [ MessageDictionary at: upper put: ^self ] ;
  :implementationList <- ^self defaultInstance implementationList clusterNew; 
  ^self
] ;

#----------
#  Schema MD getMessageFor: 
#    check case-insensitive version if not found
#----------

Schema MD defineMethod: [ | getMessageFor: selector |
!string <- selector asString ;
(string take: 1 . != "_" )
  ifTrue: [ MessageDictionary at: ("__" concat: string) ] .
    else: [ MessageDictionary at: ("___" concat: string toUpper) ]
] ;

#Schema MD defineMethod:
#[ | getMessage: string in: cd |
#  !md <- ^self getMessageFor: string asString; 
#  md isntNA 
#    ifTrue: [md implementationList at: cd] .
#] ;

#----------
#  Schema MD getOrCreate: in:
#    add case-insensitive testing: if case-insens message is already 
#    defined for this class, we need a new md entry with its own mid;
#    also, the explicit code to use is now passed to the mid creation
#    so the mid reflects the correct 'spelling' of the message
#----------
Schema MD defineMethod: [ | getOrCreate: selector in: cd |
!name <- selector asString ;
!md <- ^self getMessageFor: name ;
!mid; 
md isntNA 
  ifTrue: [ :mid <- md implementationList at: cd ]
 ifFalse: [ :md <- ^self createInstance: selector ] ; 
#-- if the case-insenstive message is already defined for this class
#-- we need a new md entry with its own mid
mid isntNA && [ mid code != name ]
 ifTrue: 
   [ :md <- ^self createInstance: name ;
     :mid <- NA ;
   ] ;
mid isNA 
 ifTrue: 
   [:mid <- MessageImplementationDescriptor createInstance: md , cd, name ;
   ] ; 
mid
] ;

#----------
#  Schema MD setFunctionTypeTo: 
#     remove the auto initialize of similar messages
#----------
Schema MD defineMethod: [ | setFunctionTypeTo: type | 
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
#-->  implementationList select: [ functionType isNA ] .
#-->     do: [ ^self setFunctionTypeTo: ^my entry ] ;
  ^self
] ;

#----------
#  Schema MD cleanupLocalNames
#     remove dictionary lookups associated with this message
#----------

Schema MD defineMethod: [ | cleanupLocalNames |
  !id <- "__" concat: code asString ;
  MessageDictionary delete: id ;
  !id <- "___" concat: code asString ;
  MessageDictionary at: id . = asSelf 
     ifTrue: [ MessageDictionary delete: id ; ] ;
  ^self
] ;

#----------
#  add internalSelector and description to clustering
#----------
Schema MD defineMethod: [ | clusterStrings |
  ^self instanceList
  do: [ :code <- [ code print ] divertOutput ;
        !okay <- internalSelector isString ;
        okay 
        ifTrue: 
           [ :internalSelector <- [ internalSelector print; ] divertOutput ] ;
        !okay <- description isString ;
        okay ifTrue: [ :description <- [ description print ; ] divertOutput ] ;
      ] ;
  ^self rcleanStore ;
] ;

#----------
#  Schema MID 
#   message - use mid's code not md's so correct case is returned
#   initializeKeys:  - set code using 3rd parameter to distinguish from md's
#----------

Schema MID defineMethod: [ | message | code ] ;

Schema MID defineMethod: [ | initializeKeys: keys |
  :messageDescriptor <- keys at: 1 ; 
  :messageClassDescriptor <- keys at: 2 ; 
  :code <- keys at: 3 . else: [ messageDescriptor code ] ;
  ^self
] ;
  
Schema MID defineMethod: [ | initializeLocalAttributes |
  :fullSelector <- getSelector ;
  :type <- getInternalMessage classDescriptor messageType ;   #<---
  ^self setLevelTo: "Basic" ;
  :functionType <- messageDescriptor functionType ;
  :returnObjectType <-  defaultInstance returnObjectType ;
  :returnObjectContainer <- returnObjectType ;    #<---
  :parameterList <-  defaultInstance parameterList clusterNew ;
  messageDescriptor implementationList 
       at: messageClassDescriptor put: ^self ;
  :createdInLastPosting <- TRUE ; 
  ^self
] ;

Schema MID defineMethod: [ | describe |
  "Message: " print: 25 ; message printNL ;
  "Class: " print: 25 ; messageClassDescriptor name printNL ; 
  "Created: " print: 25 ; creationDate printNL ; 
  "Updated: " print: 25 ; updateDate printNL ;
  "Function Type: " print: 25 ; functionType printNL ;
  "Usage Level: " print: 25 ; usageLevel printNL ;
  description isntNA ifTrue: [ description printNL ] ;
  "Returns: " print: 25 ;  
  returnObjectContainer isntDefault      # <---
    ifTrue: [ returnObjectContainer name print ; " of " print ] ;
  returnObjectType name printNL ;
  hasDefaultValue isntNA
     ifTrue: [ "Default Value: " print: 25 ; defaultValue printNL ] ;
  "Implementation Type: " print: 25 ; 
  type print ; 
  type isMethod
      ifTrue: [ newLine print ; implementationSource print ] ;
  type isConstant
      ifTrue: [ " ( " print ; implementationSource print ; " )" print ] ;
  newLine print ;
  sampleUsage isntNA
    ifTrue: [ "Sample Usage: " printNL ; sampleUsage printNL ; newLine print ];
  seeAlsoList isntNA
  ifTrue: 
    [ "See Also: " printNL ;
      seeAlsoList do: [ displayInfo ] ;
    ] ;
  newLine print ; 
] ;

#----------
#  Schema MID getInternalMessage
#     if code is the same as the md, use the md version which may
#     return a selector, not a string; otherwise, use the mid version
#     which will always return a string
#----------
Schema MID defineMethod: [ | getInternalMessage | 
  code = messageDescriptor code
     ifTrue: [ messageDescriptor internalSelector ] 
    ifFalse: [ code ] . locateInDictionaryOf: class .value
] ;

Schema MID defineMethod: [ | getSelector | 
  type isMethod
  ifTrue:
    [
    !prefix <- (implementationSource prefixNotSpannedBy: "|" . + 1 ) asInteger;
    !selector <- implementationSource drop: prefix ;
    !prefix <- (selector prefixNotSpannedBy: newLine . + 1) asInteger ;
    prefix < 3
       ifTrue: [ :selector <- selector drop: prefix ] ;
    !prefix <- selector prefixNotSpannedBy: "|" ;
    selector take: prefix
    ] 
  ifFalse: [ code ] . stripBoundingBlanks       #<---
] ;

Schema MID defineMethod: [ | refreshValues | 
  !source <- getInternalMessage ;
  !currentSource <- internalMethod ;
  !sourceString <- [ source print ] divertOutput ;
  !currentType <- source classDescriptor messageType ;
  type isMethod && currentType isMethod    # <---
     ifTrue: [ :currentType <- type ] ;    # preserve MethodTS type
  currentType isConstant
    ifTrue: [ ^self setReturnObjectTypeTo: source classDescriptor ] ;
  currentType != type 
  ifTrue: 
    [ ^self setTypeTo: currentType ;
      ^self setImplementationTextTo: sourceString forMethod: sourceString ;
    ]
  ifFalse:
    [
#--> change next line to strip whitespace
#--> !newSource <- currentSource != sourceString ;
    !whiteSpace <- " 	" concat: newLine ;
    !newSource <- currentSource isNA ||
        [ currentSource isString && sourceString isString &&
             [ (currentSource stripChar: whiteSpace ) !=
               (sourceString stripChar: whiteSpace )
             ] 
        ] ;
    newSource
    ifTrue: 
       [ ^self setImplementationTextTo: sourceString forMethod: sourceString ;
       ] ;
    ] ;
  ^self
] ;

Schema MID defineMethod: [ | setImplementationTextTo: text forMethod: method |
  :updateDate <- ^today ;
  :refreshedInLastPosting <- TRUE ; 
  :implementationSource asOf: ^today put: (^self getClusteredString: text ) ;
  :internalMethod <- method ; 
  :fullSelector <- getSelector ;    # <---
  ^self
] ;

#----------
#  Schema MID setDefaultValueTo:
#     set and use updateBlock defined at mid, not md;
#     if supplied value is NA, then clears default
#----------
Schema MID defineMethod: [ | setDefaultValueTo: val |
  type isProperty && val isntNA
  ifTrue: 
    [ setupAccessBlocks; 
      :defaultValue <- val; 
      :hasDefaultValue <- TRUE; 
      ^self setReturnObjectTypeTo: val classDescriptor; 
      class instanceList 
      extendBy: [ !property <- ^self send: ^my updateBlock 
                ] .
      select: [ property value isNA ] .
      do: [ property <- ^my val ] ;
      messageClassDescriptor trackDefaultValueUsing: ^self; 
    ] .
  elseIf: [ type isProperty && val isNA ] then: [ clearDefaultValue ] ;
  ^self
] ;

#----------
#  Schema MID setupAccessBlocks
#     moved from MD
#----------
Schema MID defineMethod: [ | setupAccessBlocks |
  accessBlock isNA
   ifTrue: [ :accessBlock <- code asBlock ;
             :updateBlock <- code asUpdateBlock ;
           ] ;
  ^self
] ;

Schema MID defineMethod: [ | setParameter: number typeTo: parameterType |
  !cd <- parameterType asClassDescriptor ;      #<- eliminate ifT:ifF:
  parameterList at: number put: cd ;
  ^self
] ;

#----------
#   Schema MID clusterStrings
#      - add fullSelector, description to recluster list
#      - remove implementationSource - now managed by primary string cluster
#----------
Schema MID defineMethod: [ | clusterStrings |
^self instanceList 
do: [ :code <- [code print] divertOutput; 
#      :implementationSource <- [implementationSource print] divertOutput; 
      :internalMethod <- [internalMethod print] divertOutput; 
      :fullSelector <- [fullSelector print ] divertOutput ;
      !okay <- description isString ;
      okay ifTrue: [ :description <- [ description print ; ] divertOutput ] ;
    ]; 
^self rcleanStore; 
] ;

#--------------------
#  Schema - new messages
#--------------------

#----------
#  Schema
#----------
Schema
   define: 'siteName' toBe: "Core Vision Database" .
defineMethod: [ | siteDescription | defaultInstance description ] .
defineMethod: [ | setSiteNameTo: name | 
  ^self define: 'siteName' toBe: name ;
  ^self
] .
;

Schema defineMethod: [ | createMessagesForClass: class |
  !cd <- class classDescriptor ;
  class selectorList
     do: [ ^my MessageDescriptor getOrCreate: ^self in: ^my cd ] ;
] ;

#----------
#  Schema CD
#----------

Schema ClassDescriptor
   defineMethod: [ | isApplicationClass | type isApplication ] .
   defineMethod: [ | isIncorporatorClass | class isIncorporatorPrototype ] .
;

Schema ClassDescriptor defineMethod: [ | assignDefaultType |
  ^self setClassTypeTo: 
     (class basicSend: 
          [ isCharacter || isPrinterTools || isCatalog || isChain ||
            isUserEnvironment || isLink || whatAmI = "Specification"
          ] .
        ifTrue: [ "Unused" ] .
       elseIf: [ class isToolKit ] then: [ "ToolKit" ] .   #-- ov is bi
       elseIf: [ class isDictionary ] then: ["Dictionary" ] .
       elseIf: [ class isObjectSpace ] then: [ "ObjectSpace" ] .
       elseIf: [ class
                 send: [ isEntity || isDataRecord || isBridge 
                           || isLinkRecord || isDataSet 
                           || isDBEntity || isPrivateRecord
                       ] .
               ] then: [ "Application" ] .
       elseIf: [ class isIncorporatorPrototype ] then:[ "DataFeed" ] .
       elseIf:
         [ isObjectDescriptor ||
             [ class basicSend:
                [ isBoolean || isDateRange || isFixedProperty ||
                  isFunction || isOrdinal || isSelector || isUndefined ||
                  isUtility || isUnixSeconds || isSessionAttribute
                ]
             ]
         ] then: [ "BuiltIn" ] .
         else: [ "Other" ] 
     )  ;
] ;

#---  web/doc support
Schema CD
  defineFixedProperty: 'sampleUsage' .
  defineFixedProperty: '_htmlDocName' .
  defineFixedProperty: '_imageName' .
  defineMethod: [ | includeInDataModel | type isApplication ] .

defineMethod: [ | setSampleUsageTo: text |
  :sampleUsage <- text ;
  ^self
] .

defineMethod: [ | setHtmlDocNameTo: string |
  :_htmlDocName <- string ;
  ^self
] .
 
defineMethod: [ | htmlDocName |    #- terminate at Object
  _htmlDocName
     elseIf: [ isObjectDescriptor not ] then: [ parent htmlDocName ]
] .

defineMethod: [ | imageName | 
  _imageName isntNA
    ifTrue: [ FormatTools buildImage: _imageName withAlt: _imageName ]
   ifFalse: [ name ]
] .

;

#----------
#  Schema MID
#----------

Schema MessageImplementationDescriptor
   defineFixedProperty: 'returnObjectContainer' .
   defineFixedProperty: 'accessBlock' .
   defineFixedProperty: 'updateBlock' .

   defineFixedProperty: 'okayToUpload' .
   defineFixedProperty: 'keyType' .
   defineFixedProperty: 'sampleUsage' .
   defineFixedProperty: 'seeAlsoList' .

defineMethod: [ | isTimeVarying | type isTimeVarying ] .
defineMethod: [ | includeInDataModel | functionType code = "Data" ] .
;

Schema MessageImplementationDescriptor
  do: [
        :returnObjectContainer <- ^global Object classDescriptor ;
      ] ;

Schema MID defineMethod: [ | setReturnObjectContainerTo: cd |
  !container <- cd asClassDescriptor class ;
  :returnObjectContainer <- container isCollection
     ifTrue: [ container ] ifFalse: [ ^global Object ] . classDescriptor ;
  ^self
] ;

Schema MID defineMethod: [ | defaultDataType |
 defaultValue isntNA ifTrue: [ defaultValue ] .
     elseIf: [ returnObjectType isObjectDescriptor not ]
       then: [ returnObjectType class ] .
     elseIf: [ class isDataFeed ] then: [ ^global String ] 
] ;

Schema MID defineMethod: [ | enableOkayToUpload | 
  type isProperty && [ defaultValue isDataRecord not ] 
     ifTrue: [ :okayToUpload <- TRUE ] ;
  ^self
] ;

Schema MID defineMethod: [ | disableOkayToUpload | 
  :okayToUpload <- NA ;
  ^self
] ;

Schema MID defineMethod: [ | flagAsKeyField | 
  :keyType <- ^global Named KeyType Full ;
  ^self
] ;

Schema MID defineMethod: [ | flagAsPartialKeyField | 
  :keyType <- ^global Named KeyType Partial ;
  ^self
] ;

Schema MID defineMethod: [ | setSampleUsageTo: string | 
  :sampleUsage <- string ; 
  ^self
] ;

Schema MID defineMethod: [ | setSeeAlsoTo: midList |
  :seeAlsoList <- midList ;
  ^self
] ;

Schema MID defineMethod: [ | formatMessageDescription |
  [ 
  description isntNA 
  ifTrue:
    [ !preserve <- 
         (description contains: (newLine concat: newLine)  ) ||
         [description contains: "example:" ] ;
      preserve
       ifTrue: [ description translate: newLine1 to: FormatTools startLine .
               ] 
      ifFalse: [ description ] . print ;
    ] 
  ifFalse: [ " No Description Available" print ]
  ] divertOutput

] ;

Schema MID defineMethod: [ | formatMessageImplementation |
  [ type isMethod
    ifTrue: 
      [ messageClassDescriptor name print ; 
        " defineMethod: " printNL ;
        implementationSource 
            translate: "{" to: "[" .
            translate: "}" to: (newLine concat: "]" ) .
        do: [ !prefix <- ^self take: 2 ; 
              !body <- ^self drop: 2 ;
              ^self take: 4 . = ("[| " concat: newLine)
              ifTrue: 
                [ :prefix <- ^self take: 3 ;
                  :body <- ^self drop: 4 ;
                ] ;
              prefix print ; 
              (prefix take: -1 . != " " ) && (body take: 1 . != " " )
                ifTrue: [ " " print ] ;
              body printNL ;
            ] ;
      ] .
    elseIf: [ type isFixedProperty ]
    then: 
      [ messageClassDescriptor name print ;
        " defineFixedProperty: '" print ; 
        code print ; "' ;" printNL ;
      ] .
    elseIf: [ type isTimeSeriesProperty ]
    then: 
      [ messageClassDescriptor name print ;
        " define: '" print ; 
        code print ; "' ;" printNL ;
      ] .
    elseIf: [ type isConstant ] 
    then: 
      [ messageClassDescriptor name print ;
        " define: '" print ; code print ; "' " print ;
        " toBe: (constant goes here) ;" printNL ; 
        " (constant is of class " print ; 
        returnObjectType class whatAmI print ;
        " with value of " print ; implementationSource print ; ")" printNL ;
      ] .
    elseIf: [ type isPrimitive ] 
    then:
      [ "Primitive --  " print ; 
        implementationSource stripChar: "<>" . printNL ;
      ] .
    else: [ "" printNL ] ;
  ] divertOutput
      translate: newLine1 to: FormatTools startLine .        # nl-mod
] ;

#--------------------
#  Schema : SystemTable modifications
#--------------------

MessageType 
  defineFixedProperty: '_imageName' .

  defineMethod: [ | isConstant | code = "Constant" ] .
  defineMethod: [ | isFixedProperty | code = "FixedProperty" ] .
  defineMethod: [ | isTimeSeriesProperty | code = "TimeSeriesProperty" ] .
  defineMethod: [ | isMethod | code contains: "Method" ] .
  defineMethod: [ | isPrimitive | code = "Primitive" ] .
  defineMethod: [ | isFixed | isFixedProperty ] .
  defineMethod: [ | isProperty | isFixedProperty || isTimeSeriesProperty ] .

defineMethod: 
  [ | isTimeVarying | code = "MethodTV" || isTimeSeriesProperty
  ] .

defineMethod: [ | imageName |
  _imageName isString
     ifTrue: [ FormatTools buildImage: _imageName withAlt: shortName ] .
     elseIf: [ _imageName isBlock ] then: [ ^self send: _imageName ] .
       else: [ shortName ] 
] .

;

ClassType
  defineFixedProperty: 'description' .

  defineMethod: [ | isApplication | code = "Application" ] .
  defineMethod: [ | isDataFeed | code = "DataFeed" ] .
  defineMethod: [ | isToolKit | code = "ToolKit" ] .
  defineMethod: [ | isDictionary | code = "Dictionary" ] .
  defineMethod: [ | isObjectSpace | code = "ObjectSpace" ] .
  defineMethod: [ | isOther | code = "Other" ] .
  defineMethod: [ | isUnused | code = "Unused" ] .
  defineMethod: [ | isBuiltIn | code = "BuiltIn" ] .
  defineMethod: [ | isObjectSpace | code = "ObjectSpace" ] .
;

UsageLevel
;

SystemTable createSubclass: "KeyType" ;

KeyType
   defineMethod: [ | isFull | code = "Full" ] .
   defineMethod: [ | isPartial | code = "Partial" ] .
;

#============================================================




####################
#  run schema cleanups as needed
####################

#--------------------
#  install string clusters
#--------------------

"...  installing string clusters for each CD" printNL ;
Schema CD instanceList 
   iterate: [ ^self initializeStringCluster ; ] ;

"...  saving new structures so instances needing strings in indexed list
      are not installed in same update" printNL ;

Utility basicUpdateNetwork ;

#--------------------
#  recluster MID implemtations in primary string store
#--------------------
"...  reclustering MID implementationSource" printNL ;
Schema MID instanceList
do: [ !mid <- ^self ;
      :implementationSource select: [ isString ] .
      do: [ ^my mid :implementationSource
                put: (^my mid getClusteredString: asSelf ) ;
          ] ;
    ] ;
Schema MID instanceList collectListElementsFrom: [ :implementationSource ] .
    select: [isString ] .
    groupedBy: [ ^self clusterID ] .
do: [ "Cluster: " print ; print ;
      groupList at: 1 . instanceList count print ;
      groupList count print ;
      groupList groupedBy: [ asSelf ] . count print ;
      groupList at: 1 . displayPOP ;
    ] ;

#--------------------
#  missing instances
#--------------------
FunctionType getOrCreate: "Data"  ;

KeyType createInstance: "Partial" ;
KeyType createInstance: "Full" ;

MessageType createInstance: "MethodTV" ;

UsageLevel createInstance: "DBA" ;

#--------------------
#  reassign class types and update descriptions
#--------------------
ClassType newAs: "BuiltIn" .
   setNumericCodeTo: 1 ;
ClassType newAs: "Application" .
   setNumericCodeTo: 2 ;
ClassType newAs: "DataFeed" .
   setNumericCodeTo: 3 ;
ClassType newAs: "ToolKit" .
   setNumericCodeTo: 4 ;
ClassType newAs: "Dictionary" .
   setNumericCodeTo: 5 ;
ClassType newAs: "ObjectSpace" .
   setNumericCodeTo: 6 ;
ClassType newAs: "Other" .
   setNumericCodeTo: 7 ;
ClassType newAs: "Unused" .
   setNumericCodeTo: 8 ;
Schema CD instanceList do: [ assignDefaultType ] ;

Named ClassType BuiltIn :description <- 
"The BuiltIn Classes are defined as part of all Vision databases.
Although most of these classes are defined as part of the database
bootstrap process, you can modify most of the supplied messages and
define new messages for any of these classes." ;
Named ClassType Application :description <- 
"The Application Classes are used to define the entities and data classes
that store, manipulate, and cross reference the information in your
database.  These classes are all defined in Vision.  Protocol can
be modified and added to existing classes any number of new Application
classes can be defined." ;
Named ClassType DataFeed :description <- 
"The DataFeed Classes are used to define the structures that hold
data loaded from external sources.  These instances are normally
used to create and refresh entities and to populate the appropriate
data records.  They are not normally used as part of applications.
The DataSet classes are also subclasses of IncorporatorPrototype.
Instances of these classes are normally linked to directly or
indirectly to other Application classes." ;
Named ClassType ToolKit :description <- 
"The ToolKit Classes are used to organize packages of classes and
messages offered as a unit to perform a set of related functions.  " ;
Named ClassType Dictionary :description <- 
"The Dictionary Classes are used to manage a set of names that
return related objects.  Dictionary classes do not have instances.  
All the messages defined for a particular Dictionary usually return
objects of the same class.  " ;
Named ClassType ObjectSpace :description <-
"The different ObjectSpaces in your database function as classes.
You can modify or define messages at the ^global level of an
object space.  All objects space inheritance from the CoreWorkspace
which inherits from the BuiltInWorkspace." ;

#--------------------
#  MessageType attributes
#--------------------
Named MessageType Constant
   setSortCodeTo: "C" ;
Named MessageType FixedProperty
   setShortNameTo: "Fixed" .
   setSortCodeTo: "FP" ;
Named MessageType TimeSeriesProperty
   setShortNameTo: "TimeSeries" .
   setSortCodeTo: "TS" ;
Named MessageType Method
   setSortCodeTo: "M" ;
Named MessageType MethodTV
   setNameTo: "Method (time varying)" . 
   setSortCodeTo: "MT" ;

#--------------------
#  descriptions 
#--------------------

Schema
   setDescriptionTo: "The Core Vision Database includes Built-In classes
such as Number, String, and List, Application classes such as Entity,
DataRecord, and DataSet, ToolKit classes such as Interface and Schema,
and other standard classes and protocol needed to support general
database and application development." ;

#-----

Named classDescriptor setDescriptionTo:
"Abstract class organizes the naming dictionaries for the entity subclasses.
Subclasses of this class correspond to the individual naming dictionaries.
A new subclass is automatically created when a new entity subclass is
created.  For instance, if Company is an subclass of Entity, the class
Named Company would be used to store names that reference individual
company instances." ;

NoValue classDescriptor setDescriptionTo:
"This subclass of the Undefined class is the value returned when
a block returns 'nothing'." .  
setSampleUsageTo: "!x <- 3 < 5 ifTrue: [ 2 ; ]" ;

Utility SessionAttribute classDescriptor setDescriptionTo:
"This class manages various attributes that control various aspects
of your Vision session." .
;
XRef classDescriptor setDescriptionTo:
"This abstract class is a subclass of Dictionary is used to organize
alternative naming dictionaries for classes.  " ;

#-----

BuiltInWorkspace classDescriptor setDescriptionTo:
"This object space contains all the primitive protocol for the Vision
database and corresponds to the top level of object space 2." ;

CoreWorkspace classDescriptor setDescriptionTo:
"This object space is used to store non-primitive classes and messages.
The information in this space is directly accessible to all Vision users.
This space corresponds to the top level of object space 3 and inherits
from the BuiltInWorkspace (object space 2)." ;

Environment DBA DDMS classDescriptor setDescriptionTo:
"This object space is designed to be used to store miscellaneous
data dictionary information.  It is not currently used but is 
reserved for future use.  This object space corresponds to object
space 4." ;

Environment DBA Schema classDescriptor setDescriptionTo:
"This object space stores the actual schema information associated
with the class and message descriptors.  The message 'Schema' in
the CoreWorkspace returns the top level of this space so there is
normally no reason to navigate to it explicitly.  This object space
corresponds to object space 5." ;

Environment classDescriptor setDescriptionTo:
"This class manages references to the top-level of various object spaces
and supplies protocol for creating and managing object spaces." ;

Environment MaintenanceTools classDescriptor setDescriptionTo:
"This class supplies tools for creating new object spaces." ;

#-----

BusinessDays classDescriptor setDescriptionTo:
"This class is used to generate date offsets measured in business days
(Monday through Friday)." .
setSampleUsageTo: "-3 businessDays" ;

Days classDescriptor setDescriptionTo:
"This class is used to generate date offsets measured in days." .
setSampleUsageTo: "-3 days" ;

MonthBeginnings classDescriptor setDescriptionTo:
"This class is used to generate date offsets that start on the first
day of each month." .
setSampleUsageTo: "-3 monthBeginnings" ;

Months classDescriptor setDescriptionTo:
"This class is used to generate date offsets that start on the same
day of the month as the recipient date if possible.  If the computed
date does not exist for a month (i.e., 31st of April), the last date
of the month is used." .
setSampleUsageTo: "-3 months" ;

MonthEnds classDescriptor setDescriptionTo:
"This class is used to generate date offsets that start on the last
day of each month." .
setSampleUsageTo: "-3 monthEnds" ;

QuarterBeginnings classDescriptor setDescriptionTo:
"This class is used to generate date offsets that start on the first
day of each quarter." .
setSampleUsageTo: "-3 quarterBeginnings" ;

Quarters classDescriptor setDescriptionTo:
"This class is used to generate date offsets that start on the same
day of the quarter as the recipient date if possible.  If the computed
date does not exist for a quarter (i.e., 31st of April), the last date
of the month is used." .
setSampleUsageTo: "-3 quarters" ;

QuarterEnds classDescriptor setDescriptionTo:
"This class is used to generate date offsets that start on the last
day of each quarter." .
setSampleUsageTo: "-3 quarterEnds" ;

YearBeginnings classDescriptor setDescriptionTo:
"This class is used to generate date offsets that start on the first
day of each year." .
setSampleUsageTo: "-3 yearBeginnings" ;

Years classDescriptor setDescriptionTo:
"This class is used to generate date offsets that start on the same
day of the year as the recipient date if possible.  If the computed
date does not exist for a year (i.e., 29nd of February), the last date
of the month is used." .
setSampleUsageTo: "-3 years" ;

YearEnds classDescriptor setDescriptionTo:
"This class is used to generate date offsets that start on the last
day of each year." .
setSampleUsageTo: "-3 yearEnds" ;

Date DayOfWeekDescriptor classDescriptor setDescriptionTo:
"Instances of this class represent the different days of the week." .
setSampleUsageTo: "^date getDayOfWeekDescriptor code" ;

Date MonthDescriptor classDescriptor setDescriptionTo:
"Instances of this class represent the different months of the year." .
setSampleUsageTo: "^date getMonthDescriptorDescriptor shortName" ;

#-----

FixedProperty classDescriptor setDescriptionTo:
"This class is used to represent fixed properties internally in Vision." ;

TimeSeriesProperty classDescriptor setDescriptionTo:
"This class is used to represent time series properties internally in Vision.";

Method classDescriptor setDescriptionTo:
"This class is used to represent methods internally in Vision." ;

Primitive classDescriptor setDescriptionTo:
"This class is used to represent primitives internally in Vision." ;

#-----

FormatToolProtocol classDescriptor setDescriptionTo:
"The FormatTools ToolKit has been created to provide a modular
approach for generating output that can be formatted for different
environments without changing the methods that produce the output.
All vanilla versions of messages are defined here.  Subclasses of 
this class are used to define new packagings of these messages.
This class is not normally accessed directly - use the cover 
method 'FormatTools' or one of its subclasses for most usage." .
setSampleUsageTo: 
"FormatTools evaluate: [ FormatTools bold: \"xyz\" . printNL ; ] " ;

FormatToolProtocol Html classDescriptor setDescriptionTo:
"This class redefines formatting messages to use Html tags where
appropriate. This class is not normally accessed directly - use the cover 
method 'FormatTools Html' for most usage." .
setSampleUsageTo: 
"FormatTools Html evaluate: [ FormatTools bold: \"xyz\" . printNL ; ] " ;

Interface classDescriptor setDescriptionTo:
"The Interface ToolKit has been created to provide a set of classes
and messages that aid in interfacing with environments external
to Vision.  This ToolKit enables you to standardize your interactions
with Vision and provides protocol for arbitrary expression
evaluation, structured data extraction, and parameterized
queries.  " ;

Interface Constant classDescriptor setDescriptionTo:
"The Interface Constant class is used to define messages that
return constant values used by the various Interface subclasses.  " ;

Interface ErrorLog classDescriptor setDescriptionTo:
"The Interface ErrorLog class provides protocol to standardize error
reporting for the Interface classes." ;

Interface External classDescriptor setDescriptionTo:
"The Interface External class is an abstract class
that is subclassed to define protocol specific to an external
environment.  " ;

Interface HtmlAccess classDescriptor setDescriptionTo:
"This subclass of Interface External is used to define specific
protocol for interacting with Html-based server requests." ;

Interface Global classDescriptor setDescriptionTo:
"The Interface Global class is used to define properties that are
set and accessed by the Interface Workspace classes.  " ;

Interface Named classDescriptor setDescriptionTo:
"This class organizing naming dictionaries for the Interface classes." ;

Interface Workspace classDescriptor setDescriptionTo:
"The class Interface Workspace is an abstract class used to
define common protocol for different types of structured query." ;

Interface ApplicationWS classDescriptor setDescriptionTo:
"The class Interface ApplicationWS supports parameterized
application execution." .
setSampleUsageTo: 
   "Interface ApplicationWS runWith: \"Profile\", \"Company\", \"IBM\" " ;

Interface ExtractWS classDescriptor setDescriptionTo:
"The class Interface ExtractWS supports structured data extraction in
a number of different formats." .
setSampleUsageTo: 
   "Interface ExtractWS
         setOrientationTo: \"EI\" .
         setEntityTypeTo: Company .
         setEntityListTo: \"IBM, GM, XON\" .
         setItemListTo: \"ticker\", \"price\" .
     run ;
" ;

#-----

OpenVision classDescriptor setDescriptionTo:
"The OpenVision ToolKit is a collection of Vision methods
and primitives that allows you to write Vision programs that 
function as clients, servers, and peers of other programs.  
Using the OpenVision tools, a Vision program can read and 
write files, start and control other programs, and create and manage 
sockets." ;

OpenVision Channel classDescriptor setDescriptionTo:
"The OpenVision ToolKit provides a single abstraction -- an Open Vision
Channel -- that unifies and encapsulates the mechanisms used for
communication and data sharing.  A channel represents Vision's end
of a potentially bi-directional 'connection' to data.  " ;

#-----

Schema classDescriptor setDescriptionTo:
"The Schema ToolKit organizes protocol for managing information about
the classes and messages in the Vision database." ;

Schema CD classDescriptor setDescriptionTo:
"The Schema ClassDescriptor class manages information about each class
in the Vision database.  There is one instance in this class for every
subclass created." .
setSampleUsageTo: "object classDescriptor" ;

Schema MID classDescriptor setDescriptionTo:
"The Schema MessageImplementationDescriptor class manages information about 
each message in the Vision database.  There is one instance in this class
for every message defined for any class in your database.  A separate
instance is created for each definition of the message, even if the
same name is used with multiple classes." .
setSampleUsageTo: "object getMessage: \"messageName\" . " ;

Schema MD classDescriptor setDescriptionTo:
"The Schema MessageDescriptor class manages information about 
each unique message name in the Vision database.  There is one instance 
for every unique message name in the database.  The instances of this
class are not normally referenced directly." ;

Schema Descriptor classDescriptor setDescriptionTo:
"This abstract class organizes the Schema descriptor subclasses." ;

#-----

PrivateRecord classDescriptor setDescriptionTo: 
"Subclasses of this class are designed to be saved in private object
spaces and serve as 'extensions' to shared entity classes." ;

Currency classDescriptor setDescriptionTo:
"This class represents the individual currencies in which monetary
values can be expressed." ;

Classification classDescriptor setDescriptionTo:
"This abstract class is a subclass of Entity and a superclass of
classes used primarily for grouping and aggregation purposes." ;
RangeClassification classDescriptor setDescriptionTo:
"This abstract class is a subclass of Classification and a superclass
of classes used to manage classes whose instances are used to assign
values falling into a particular numeric range." ;

Support classDescriptor setDescriptionTo:
"This abstract class is used to organize miscellaneous classes that
support the application." ;
Table classDescriptor setDescriptionTo:
"This abstract class is used to organize miscellaneous classes that
require simple aliases and names for their instances." ;

SystemTable classDescriptor setDescriptionTo:
"Subclasses of this abstract class manage tables defined to support
built-in and application classes." ;

ClassType classDescriptor setDescriptionTo:
"The ClassType instances are used to categorize the different classes
in the Vision database.  The 'type' property of a class descriptor instance
is an instance of this class.  The available class types are:
BuiltIn, Application, DataFeed, ToolKit, Dictionary, ObjectSpace, Other,
and Unused." .
setSampleUsageTo: "Number classDescriptor type" ;

FunctionType classDescriptor setDescriptionTo:
"The FunctionType instances are used to categorize messages by user-defined
types.  The 'functionType' property of a message implementation descriptor
instance is an instance of this class." .
setSampleUsageTo: "Number getMessage: \"asDate\" . functionType" ;

UsageLevel classDescriptor setDescriptionTo:
"Instances of this class are used to categorize messages by expected user
level such as Basic or DBA.  The 'usageLevel' property of a message
implementation descriptor instance is an instance of this class." ;

KeyType classDescriptor setDescriptionTo:
"The KeyType instances are used to define different types of 'key'
that can be associated with a message.  Vision does not use keys as
part of its database definition; however, this is often useful when
describing a data model of the database.  The 'keyType' property of
a message implementation descriptor instance can be set to an instance
of this class.  The current key types are Full and Partial." ;

MessageType classDescriptor setDescriptionTo:
"The MessageType instances are used to categorize the different types
of messages.  The 'type' property of a message implementation descriptor
instance is an instance of this class.  The available class types are:
FixedProperty, TimeSeriesProperty, Constant, Method, MethodTV, and 
Primitive." .
setSampleUsageTo: "Number getMessage: \"asDate\" . type" ;

SystemDictionary classDescriptor setDescriptionTo:
"Instances of this class manage dictionaries used by methods defined
for built-in classes." ;

DataSource classDescriptor setDescriptionTo:
"This class is used to store information describing different 
sources of data." ;

UpdateType classDescriptor setDescriptionTo:
"Instances of this class describe different styles of database 
reconciliation of data from external sources." ;

DataRecord classDescriptor setDescriptionTo:
"This abstract class is used to define subclasses that manage
subsets of data about a specific entity so that data can be
organized into manageable units." ;

LinkRecord classDescriptor setDescriptionTo:
"This abstract class is used to define subclasses that symmetrically
related two or more entities to other data." ;

Bridge classDescriptor setDescriptionTo:
"The subclasses of the Bridge class are used to manage
the protocol for connecting an entity to one more DataRecord
instances.  It is useful when there are complex relationships that
need to be managed and you do not want to overburden the entity
class with all the required information." ;

DBEntity classDescriptor setDescriptionTo:
"This abstract class is used to define psuedo-entities that define
a subset of entities being tracked for a specific data source.
Subclasses of this class are normally named LocalEntity and are
created in an object space associated with a particular data source." .
setSampleUsageTo: "Environment DB SourceXYZ LocalEntity" ;

LocalDBDictionary classDescriptor setDescriptionTo:
"This subclass of Dictionary is used to organize names associated with
an external data source for which local entities are managed.  Each
LocalEntity subclass is associated with its own subclass of this
LocalDBDictionary class.  Subclasses of this class are normally
created in an object space associated with a particular data source." .
setSampleUsageTo: "Environment DB SourceXYZ LocalDictionary" ;

IncorporatorPrototype classDescriptor setDescriptionTo:
"This abstract class isused to model external data." ;

DataFeed classDescriptor setDescriptionTo:
"This abstract class is used to model
external data sets that are not integrated into the main access
paths of the database once the information within the class
has been reconciled." ;
DataSet classDescriptor setDescriptionTo:
"This abstract class is used to model
external data sets that use a standard set of processing rules
to integrate the instances of the subclasses with other objects
in the database." ;

#--------------------
#  web support
#--------------------

Block classDescriptor :_htmlDocName <- "clBlock.htm" ;
Boolean classDescriptor :_htmlDocName <- "clBoolean.htm" ;
Bridge classDescriptor :_htmlDocName <- "appClasses.htm" ;
BuiltInWorkspace classDescriptor :_htmlDocName <- "admNet.htm" ;
ClassType classDescriptor :_htmlDocName <- "tkSchema.htm" ;
Classification classDescriptor :_htmlDocName <- "clClassify.htm" ;
Collection classDescriptor :_htmlDocName <- "clCollect.htm" ;
Currency classDescriptor :_htmlDocName <- "clCurrency.htm" ;
DBEntity classDescriptor :_htmlDocName <- "appClasses.htm" ;
DataFeed classDescriptor :_htmlDocName <- "clDataFeed.htm" ;
DataRecord classDescriptor :_htmlDocName <- "appClasses.htm" ;
Date classDescriptor :_htmlDocName <- "clDate.htm" ;
DateOffset classDescriptor :_htmlDocName <- "clDate.htm" ;
DateRange classDescriptor :_htmlDocName <- "clDate.htm" ;
Dictionary classDescriptor :_htmlDocName <- "clDict.htm" ;
Entity classDescriptor :_htmlDocName <- "clEntity.htm" ;
Environment MaintenanceTools classDescriptor :_htmlDocName <- "admNet.htm" ;
FormatToolProtocol classDescriptor :_htmlDocName <- "tkInterface.htm" ;
FunctionType classDescriptor :_htmlDocName <- "tkSchema.htm" ;
IncorporatorPrototype classDescriptor :_htmlDocName <- "appClasses.htm" ;
IndexedList classDescriptor :_htmlDocName <- "clIList.htm" ;
Interface classDescriptor :_htmlDocName <- "tkInterface.htm" ;
KeyType classDescriptor :_htmlDocName <- "tkSchema.htm" ;
LinkRecord classDescriptor :_htmlDocName <- "appClasses.htm" ;
List classDescriptor :_htmlDocName <- "clList.htm" ;
MessageType classDescriptor :_htmlDocName <- "tkSchema.htm" ;
Number classDescriptor :_htmlDocName <- "clNumber.htm" ;
Object classDescriptor :_htmlDocName <- "clObject.htm" ;
OpenVision classDescriptor :_htmlDocName <- "tkOpenVision.htm" ;
PrivateRecord classDescriptor :_htmlDocName <- "admUpdat.htm" ;
Schema classDescriptor :_htmlDocName <- "tkSchema.htm" ;
Selector classDescriptor :_htmlDocName <- "clXRef.htm" ;
String classDescriptor :_htmlDocName <- "clString.htm" ;
TimeSeries classDescriptor :_htmlDocName <- "clTS.htm" ;
ToolKit classDescriptor :_htmlDocName <- "clXRef.htm" ;
Undefined classDescriptor :_htmlDocName <- "clNA.htm" ;
Utility SessionAttribute classDescriptor :_htmlDocName <- "admTools.htm" ;

Object classDescriptor :_imageName <- "Object.gif" ;
List classDescriptor :_imageName <- "List.gif" ;
IndexedList classDescriptor :_imageName <- "IList.gif" ;
TimeSeries classDescriptor :_imageName <- "TS1.gif" ;

Named MessageType FixedProperty :_imageName <- "FP.gif" ;
Named MessageType TimeSeriesProperty :_imageName <- "TS2.gif" ;
Named MessageType Method :_imageName <- "M1.gif" ;
Named MessageType MethodTV :_imageName <- "M2.gif" ;
Named MessageType Constant :_imageName <-
  [ FormatTools font: "TimesRoman" inSize: "+1" 
           andColor: "black" for: (FormatTools bold: "C") 
  ] ;


#--------------------
#  reassign descriptions an remove property at Descriptor
#--------------------
Schema CD instanceList do: [ ^super :description <- description ] ;
Schema MID instanceList do: [ ^super :description <- description ] ;
Schema Descriptor deleteMessage: "description" ;

#--------------------
#  Cleanup full selectors
#--------------------

Schema MID activeList select: [ fullSelector take: -1 . = ":" ] .
   do: [ :fullSelector <- getSelector ] ;

#--------------------
#  cleanup return object types/elements/containers
#--------------------

#-- initialize containers
Schema MID instanceList
do: [ 
      :returnObjectContainer <- ^global Object classDescriptor ;
    ];

#--  reorganize those that had element types
#--     element type becomes object type and object type becomes container
Schema MID activeList
   select: [ returnObjectElementType isntNA ] .
do: [ ^self
        setReturnObjectContainerTo: returnObjectType .
        setReturnObjectTypeTo: returnObjectElementType .
    ] ;

#--  reorganize those that just return a collection ; 
#--  this should be the container type; manually reset the object type
Schema MID activeList select: [ returnObjectType class isCollection ] .
do: [ ^self setReturnObjectContainerTo: returnObjectType .
            setReturnObjectTypeTo: ^global Object ;
    ] ;

#--  set return object type for those returned as Collection containers
Entity getMessage: "aliases" .   setReturnObjectTypeTo: String ;
Entity getMessage: "initializationList" .   setReturnObjectTypeTo: Block ;
TimeSeries getMessage: "+" .   setReturnObjectTypeTo: Number ;
TimeSeries getMessage: "-" .   setReturnObjectTypeTo: Number ;
TimeSeries getMessage: "*" .   setReturnObjectTypeTo: Number ;
TimeSeries getMessage: "/" .   setReturnObjectTypeTo: Number ;
String getMessage: "asCSVRecords" .   setReturnObjectTypeTo: String ;
String getMessage: "asCells" .   setReturnObjectTypeTo: String ;
String getMessage: "asCellsOn:" .   setReturnObjectTypeTo: String ;
String getMessage: "asLines" .   setReturnObjectTypeTo: String ;
String getMessage: "asRecords" .   setReturnObjectTypeTo: String ;
String getMessage: "asUpperLowerWords" .   setReturnObjectTypeTo: String ;
String getMessage: "asWords" .   setReturnObjectTypeTo: String ;
String getMessage: "breakOn:" .   setReturnObjectTypeTo: String ;
String getMessage: "cbreakOn:" .   setReturnObjectTypeTo: String ;
String getMessage: "tokenizeOn:" .   setReturnObjectTypeTo: String ;
String getMessage: "ctokenizeOn:" .   setReturnObjectTypeTo: String ;
String getMessage: "breakOnString:" .   setReturnObjectTypeTo: String ;
String getMessage: "tokenizeOnString:" .   setReturnObjectTypeTo: String ;
String getMessage: "breakOnPattern:" .   setReturnObjectTypeTo: String ;
String getMessage: "tokenizeOnPattern:" .   setReturnObjectTypeTo: String ;
Object getMessage: "getMessageNamesContaining:" .   
   setReturnObjectTypeTo: Schema MID ;
Object getMessage: "getMessages" .   
   setReturnObjectTypeTo: Schema MID ;
Object getMessage: "getMessagesDefining:" .   
   setReturnObjectTypeTo: Schema MID ;
Object getMessage: "getMessagesInInheritancePath" .   
   setReturnObjectTypeTo: Schema MID ;
Object getMessage: "getMessagesUsing:" .   
   setReturnObjectTypeTo: Schema MID ;
Object getMessage: "getMessagesX" .   
   setReturnObjectTypeTo: Schema MID ;
DateRange getMessage: "asDateList" .   setReturnObjectTypeTo: Date ;
List getMessage: "+" .   setReturnObjectTypeTo: Number ;
List getMessage: "-" .   setReturnObjectTypeTo: Number ;
List getMessage: "*" .   setReturnObjectTypeTo: Number ;
List getMessage: "/" .   setReturnObjectTypeTo: Number ;
Integer getMessage: "sequence" .   setReturnObjectTypeTo: Integer ;
Integer getMessage: "sequence0" .   setReturnObjectTypeTo: Integer ;
Integer getMessage: "sequenceTo:" .   setReturnObjectTypeTo: Integer ;
Number getMessage: "sequence" .   setReturnObjectTypeTo: Integer ;
Number getMessage: "sequence0" .   setReturnObjectTypeTo: Integer ;
Collection getMessage: "anElement" .   
  setReturnObjectTypeTo: Object .
  setReturnObjectContainerTo: Object ;

#--------------------
#  Delete unused messages
#--------------------
Schema MID deleteMessage: "returnObjectElementType" ;
Schema MID deleteMessage: "setReturnObjectElementTypeTo:" ;

#--------------------
#  move update/access block data from MD toMID
#--------------------
Schema MID instanceList 
   select: [ messageDescriptor updateBlock isntNA ] .
do: [ :updateBlock <- messageDescriptor updateBlock ;
      :accessBlock <- messageDescriptor accessBlock ;
    ] ;

#--------------------
#  case insenstive lookup support
#--------------------
Schema MD activeList groupedByString: [ code toUpper ] .
do: [!rec <- groupList 
         sortUp: [ code capitalize = code ifTrue: [ 1 ] else: [ 0 ] ] . at: 1;
     !id <- "__" concat: asSelf ;
     rec MessageDictionary delete: id ;
     !id <- "___" concat: asSelf ;
     rec MessageDictionary at: id put: rec asSelf ;
    ] ;

Schema MD activeList
  extendBy: [ !md <- MessageDictionary at: ("___" concat: code toUpper) ] .
select: [ md != asSelf ] .
do: [ md code print: 25 ; md implementationList count print; 
      " | " print ; code print: 25 ; implementationList count printNL  ;
      !toMerge <- md implementationList append: implementationList .
         groupedBy: [ messageClassDescriptor ] ;
#-- only merge the lists for classes that are distinct
     toMerge select: [ groupList count = 1 ] .
        send: [ groupList at: 1 ] .
        select: [ messageDescriptor != ^my md ] .
     do: [ :code <- ^my code ;
           :messageDescriptor <- ^my md ;
           ^my md implementationList at: messageClassDescriptor put: asSelf ;
           ^my implementationList delete: messageClassDescriptor ;
           "  --> merged: " print ; displayInfo ;
         ] ;
      !removeIt <- TRUE ;
      toMerge select: [ groupList count > 1 ] .
      do: [ groupList 
            do: [ " --> not merged: " print ; displayInfo ] ; 
            ^my :removeIt <- FALSE ;
          ] ;
     removeIt ifTrue: [ flagForDeletion ] ;
      newLine print ;
    ] ;

#--------------------
#  Other Message Deletions
#--------------------

Schema Descriptor
   deleteMessage: "shortName" .
   deleteMessage: "setDescriptionTo:" .
   deleteMessage: "setShortNameTo:" .
;

Schema ClassDescriptor
   deleteMessage: "isCoreClass" .
   deleteMessage: "isUserClass" .
   deleteMessage: "usageLevel" .
   deleteMessage: "setLevelTo:" .
;

Schema MessageDescriptor
   deleteMessage: "accessBlock" .
   deleteMessage: "updateBlock" .
   deleteMessage: "setupAccessBlocks" .
;

Schema MessageImplementationDescriptor 
   deleteMessage: "returnObjectElementType" .
   deleteMessage: "setReturnObjectElementTypeTo:" .
;

ClassType deleteMessage: "isCore" ;
ClassType deleteMessage: "isUser" ;
ClassType deleteMessage: "isIncorporator" ;

#UsageLevel deleteMessage: "isBasic" ;
#UsageLevel deleteMessage: "isInternal" ;
#UsageLevel deleteMessage: "isAdvanced" ;
#UsageLevel deleteMessage: "isReserved" ;


#--------------------
#  post messages
#--------------------
Schema processAllMessages;


