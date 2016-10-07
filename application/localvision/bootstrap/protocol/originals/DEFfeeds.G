##################################################
#  DataFeed processing - Core protocol
##################################################

Object defineMethod: [ | printTimeWith: label |
#label print ; ^global Utility UnixSeconds currentTime printNL ;
] ;

Object defineFixedProperty: 'lastModifiedByFeed' ;



#======================================================================

####################
#  DataFeed Protocol
####################

#----------
#  redefine defineFP: withDefault: at DataFeed to use the Object version
#----------

DataFeed defineMethod: [ | defineFixedProperty: name withDefault: default |
  ^self defineFixedProperty: name ;
  ^self hasSchema 
  ifTrue:
    [ ^global Schema 
        setDefaultValueOf: name at: ^self classDescriptor to: default ;
    ] 
  ifFalse:     #-- explicit initialize if no place to store default
    [ !updateBlock <- ":" concat: name . asBlock ;
      ^self instanceList 
         do: [ ^self send: ^my updateBlock . <- ^my default ] ;
    ] ;
  ^self
] ;

#--------------------
#  General Protocol
#--------------------

DataFeed
   defineFixedProperty: 'entityId' .
   defineFixedProperty: 'date' .
   defineFixedProperty: 'currencyId' .
   defineFixedProperty: 'valueList' .

   defineFixedProperty: 'underlyingRecord' .
   defineFixedProperty: 'underlyingCurrency' .

   defineFixedProperty: 'isProcessed' .
   defineMethod: [ | isntProcessed | isProcessed isNA ] .
   defineMethod: [ | hasStatusInfo | FALSE ] .
;

#----------
#  subclass display
#----------


DataFeed defineMethod: [ | getUploadProperties |
!feed <- ^self asSelf; 
!base <- feed getUpdateClass ;

!list <- feed usesHeaderForFields
ifTrue:
  [ !list <- feed getMessagesX select: [ okayToUpload ] ;
    feed usesLocalFields
    ifFalse:
      [ :list <- list append:(base getMessagesX select: [ okayToUpload] ) ] ;
    list
  ]
ifFalse:
  [ !class <- feed usesLocalFields
        ifTrue: [ feed ] ifFalse: [ base ] ;
    feed fieldOrderList 
       send: [ ^my class getMessage: asSelf ] . 
       select: [ isntDefault ] .
  ] ;

!keys <- "entityId" , "date" , "currencyId" ;
feed isSchemaFeeds
  ifTrue: 
    [ list select: [ ^my keys includesElement: code . not ]
    ]
  ifFalse: 
    [ keys send: [ ^my feed getMessage: asSelf ] .
         append: list
    ] .
   select: [code != "code"] .
   numberElements
   groupedByString: [ code ] .
     send: [ groupList sortUp: [ position ] . at: 1 ] . sortUp: [ position ] .
     extendBy: [ !triggerKey <- 
                    code = "entityId" ifTrue: [ 1 ] .
                    elseIf: [ code = "date" ] then: [ 2 ] ;
               ]
] ;

DataFeed defineMethod: [ | showAll | 
  ^self describeFeed ; "-" fill: 50 . printNL ;
  ^self classDescriptor childList sortUp: [ name ] .
  do: [ class showAll ] ;
] ;

DataFeed defineMethod: [ | describeFeed | 
  !feed <- ^self asSelf ;
  "Feed: " print ; feed whatAmI printNL ;
  feed super isDataFeed
    ifTrue: [ "Parent: " print ; feed super whatAmI printNL ] ;
  feed classDescriptor description
       else: "   --- no description available --- " . printNL ;
  newLine print ;
  feed displayFeedConstants ;
  newLine print ;
  "Available Fields" printNL ;
  feed getUploadProperties do: [ code printNL ] ;
  newLine print ;
  "Current Field Order" printNL ;
  feed fieldOrderList numberElements
  do: [ position print: -10 ;
        printNL ;
       ] . count = 0 
     ifTrue: [ "   ---  no current field order set  ---   " printNL ] ;
] ;

DataFeed defineMethod: [ | displayFeedConstants | 
  !feed <- ^self asSelf ;
  "required fields: " print: 20 ; feed requiredFields printNL ;
  "fields from header: " print: 20 ; feed usesHeaderForFields printNL ;
  "uses local fields: " print: 20 ; feed usesLocalFields printNL ;
] ;

DataFeed defineMethod: [ | profile | 
  !feed <- ^self asSelf ;
  "Feed: " print ; feed whatAmI printNL ;
  feed classDescriptor description else: "" . printNL ;
  newLine print ;
  feed displayFeedConstants ;
  newLine print ;
  "Field #" print: 10 ; "Field" printNL ;
  feed fieldOrderList numberElements
  do: [ position print: -10 ;
        print ; "   |   " print ; 
        !block <- ^my feed updateBlockList else: ^global List . at: position ;
        block isNA 
        ifTrue:
          [ asSelf isntDefault
                ifTrue: [ "  (property not defined)" printNL ] 
                else: [ newLine print ] ;
          ]
        ifFalse:
          [ ^my feed usesLocalFields 
             ifTrue: [ ^my feed send: block . value printNL ] 
            ifFalse: 
              [ ^my feed valueList else: ^global List . 
                    at: position . printNL 
              ] ;
          ] ;
      ] ;
] ;

#--------------------
#  Update controls
#--------------------

DataFeed define: 'rejectReason' toBe: "" ;

DataFeed

defineMethod: [ | setRequiredFieldsTo: n | 
  ^self define: 'requiredFields' toBe: n asNumber ;
  ^self
] .

defineMethod: [ | enableFieldsFromHeader |
   ^self define: 'usesHeaderForFields' toBe: TRUE ;
   ^self
] .

defineMethod: [ | disableFieldsFromHeader |
   ^self define: 'usesHeaderForFields' toBe: FALSE ;
   ^self
] .

defineMethod: [ | enableLocalFields |
   ^self define: 'usesLocalFields' toBe: TRUE ;
   ^self
] .

defineMethod: [ | disableLocalFields |
   ^self define: 'usesLocalFields' toBe: FALSE ;
   ^self
] .

defineMethod: [ | enablePurge | 
   ^self define: 'purgeEnabled' toBe: TRUE ;
   ^self
] .

defineMethod: [ | disablePurge |
  ^self define: 'purgeEnabled' toBe: FALSE ;
  ^self
] .

defineMethod: [ | enableDisplayNewOnes |
   ^self define: 'displayNewOnesFlag' toBe: TRUE ;
   ^self
] .

defineMethod: [ | disableDisplayNewOnes |
   ^self define: 'displayNewOnesFlag' toBe: FALSE ;
   ^self
] .

defineMethod: [ | setAsofDateTo: date |
  ^self define: 'asofDate' toBe: date ;
  ^self
] .

defineMethod: [ | setFieldOrderListTo: fields | 
   !feed <- ^self asSelf ;
   fields isntNA
   ifTrue:
     [ !list <- fields isList 
          ifTrue: [ fields ] ifFalse: [ fields asList ] ;
       feed define: 'fieldOrderList' toBe: list ;
     ] ;
   ^self
] .

defineMethod: [ | setUpdateBlocks |
   !feed <- ^self asSelf ;
   !keyList <- ^global List new ;
   !localPropertyList <- ^global List new ;
   !updateBlocks <- feed fieldOrderList numberElements send:
      [ !id <- asString toUpper stripChar: " " ;
        !mid <- ^my feed getMessage: id ;
        (id contains: "^ENTITY" ) || (id = "ID")
            || [ id = "DATE" ] 
            || [ id contains: "^CURRENCY" ] 
        ifTrue:      #-- field is a key field
          [ ^my keyList , position ;
             id contains: "^[IE]" . ifTrue: [ "entityId" ] .
             elseIf: [ id contains: "^D" ] then: [ "date" ] .
             else: [ "currencyId" ] . 
          ] .
        elseIf: [ mid isntDefault ] 
        then:    #--  field defines a local field
          [ ^my localPropertyList , position ;
            mid code
          ] .
        else:   #-- field should define a pass-thru field if allowed
          [ !message <- ^my feed getActualMessageFor: ^self;
            message isntDefault    #- modify message name if needed
               ifTrue: [ ^my feed fieldOrderList at: position put: message ] ;
            message 
          ] . isntDefault: [ asUpdateBlock ] else: [ NA ] 
      ] ;
   feed define: 'updateBlockList' toBe: updateBlocks ;
   feed define: 'keyList' toBe: keyList ;
   feed define: 'localPropertyList' toBe: localPropertyList ;
   feed define: 'updateBlocksToDo' 
        toBe: ( feed updateBlockList numberElements select: [ isntNA ] ) ;
   feed define: 'localOnlyBlocks' 
        toBe: ( feed keyList append: feed localPropertyList .
                  send: [ !itemNum <- asSelf ;
                          ^my feed updateBlockList at: itemNum .
                            extendBy: [ !block <- asSelf ; 
                                        !itemNum <- ^my itemNum ;
                                      ] 
                        ] 
              ) ;
   !naTest <- feed getNaTest ;
   feed define: 'naTest' toBe: naTest ;     #- returns a list now
   ^self
] .

defineMethod: [ | getActualMessageFor: field |
   !feed <- ^self ;
   !classToUpdate <- feed usesLocalFields
     ifTrue: [ feed ] ifFalse: [ feed getUpdateClass ] ;
   !message <- field stripChar: " " ;
   !mid <- classToUpdate getMessage: message ;
   mid isDefault || [ mid type isProperty not ]
     ifTrue: [ :mid <- classToUpdate getMessage: ("_" concat: message) ] ;
#-> remove and replace with next line
#->   mid isntDefault ifTrue: [ mid message ] else: [ "" ] 
   mid isntDefault && [ mid type isProperty ] 
     ifTrue: [mid message ] else: [ "" ] 
] .

defineMethod: [ | getUpdateClass | 
   NA
] .
;

DataFeed defineMethod: [ | getValueList |
!feed <- ^self ;
feed fieldOrderList numberElements 
send: 
  [ !block <- ^my feed updateBlockList else: ^global List . at: position ;
    block isntNA
    ifTrue:
      [ ^my feed usesLocalFields 
           ifTrue: [^my feed send: block . value sprint: 18.8 ] 
          ifFalse: [^my feed valueList else: ^global List . at: position ] 
      ]
  ] 
] ;

#----------
#  global run-time attributes
#----------

DataFeed defineMethod: [ | getHeaderLineNumber | 
   ^self getGlobalOption: "headerLineNumber" . asNumber asInteger
      if: [ ^self > 0 ] then: [ ^self ] 
] ;

DataFeed defineMethod: [ | getAsofDateLineNumber | 
   ^self getGlobalOption: "asofDateLineNumber" . asNumber asInteger
      else: 0 .  if: [ ^self > 0 ] then: [ ^self ] 
] ;

DataFeed defineMethod: [ | getRequiredFields |
  ^self requiredFields 
    else: [ ^self getGlobalOption: "requiredFields" . asNumber ] .
     else: 0 
] ;

DataFeed defineMethod: [ | maxRecordsToProcess |
   !max <- ^self getGlobalOption: "maxRecords" . asNumber asInteger ;
   max > 0 ifTrue: [ max ] else: [ NA ] 
] ;

DataFeed defineMethod: [ | getDelimiter | 
  ^self getGlobalOption: "delimiter" . else: "	|"
] ;

DataFeed defineMethod: [ | getSkipLinesAtTop | 
^self getGlobalOption: "skipTop" . asNumber 
    if: [ ^self > 0 ] then: [ ^self ] .
     else: [ !headerLine <- getHeaderLineNumber else: 0 ;
             !asofLine <- getAsofDateLineNumber else: 0 ;
             headerLine max: asofLine
           ] 
] ;

DataFeed defineMethod: [ | getSkipLinesAtBottom | 
  ^self getGlobalOption: "skipBottom" . asNumber else: 0
] ;

DataFeed defineMethod: [ | getNaTest |
   !naList <- ^global List new ;
   !naTest <- ^self getGlobalOption: "naTest"  ;
   naTest isDefault
     ifTrue: [ naList , [ isDefault ] ] .
       else: 
       [ naTest breakOn: "," . send: [ copyString stripBoundingBlanks ] .
           send: [ ("asString = " concat: asSelf asQuotedString) asBlock ] .
         do: [ ^my naList , asSelf ]
       ] ;
   !naTest <- ^self getGlobalOption: "naPatternTest"  ;
   naTest isntDefault
     ifTrue:
       [ naTest breakOn: "," . send: [ copyString stripBoundingBlanks ] .
           send: [ ("asString contains: " concat: asSelf asQuotedString) 
                        asBlock 
                 ] .
         do: [ ^my naList , asSelf ]
       ] ;
  naList
] ;

DataFeed defineMethod: [ | getSkipChars | 
  ^self getGlobalOption: "skipChars" 
] ;

DataFeed defineMethod: [ | getDateFormat |
   ^self getGlobalOption: "dateFormat" 
] ;

#--------------------
#  Loader Variations
#--------------------

DataFeed defineMethod: [ | loadFromFile: file |
   !feed <- ^self ;
   !okay <- FALSE ;
   feed isGlobalsFeed
       ifTrue: [  "---> Loading Config File: " ]
      ifFalse: [  "---> Loading Data File: " ] . print ;
   file print ; " for " print ; ^self whatAmI printNL ;

   !contents <- file asFileContents ;
   contents isNA 
      ifTrue: 
        [ "...  file not present. " printNL ;
          ^global GlobalsFeed setDefaultGlobals ;
        ] .
      elseIf: [ contents count = 0 ] 
        then: 
        [ "...  file is empty.  " printNL ;
          ^global GlobalsFeed setDefaultGlobals ;
        ] .
       else: 
        [ 
          !rejectFile <- ^self getGlobalOption: "rejectFileName" . 
             else: [ file else: "" . concat: ".rejects" ] ;
          ^self updateFromString: contents ;
          feed isGlobalsFeed
            ifFalse: [ feed saveRejectsToFile: rejectFile withLabel: "" ] ;
         :okay <- TRUE ;
        ] ;
   okay
] ;

DataFeed defineMethod: [ | loadConfig: file|
!okay <- TRUE ;
file isntDefault
  ifTrue: [ :okay <- ^global GlobalsFeed loadFromFile: file ; ] ;
okay 
];

DataFeed defineMethod: [ | loadFromFile: file withConfig: cfgFile |
  ^self loadConfig: cfgFile .
     ifTrue: [ ^self loadFromFile: file ] 
    ifFalse: [ "     File " print ; file print ; " not loaded." printNL; 
               FALSE
             ]
] ;

DataFeed defineMethod: [ | updateFromString: string |
   !feed <- ^self asSelf currentStore ; 
feed printTimeWith: "... start of updateFromString: " ;

#--  eliminate records that are blank or start with a # as first non-space
#--  eliminate any other characters defined for skip
#--  break into delimited fields
#--  eliminate any lines with less than required number of fields
   !required <- feed purgeEnabled
       ifTrue: [ 1 ] ifFalse: [ feed getRequiredFields ] ;
   !delim <- feed getDelimiter ;
   !skipChars <- feed getSkipChars ;
   !recs <- skipChars isntNA
       ifTrue: [ string else: "" . stripChar: skipChars ] 
      ifFalse: [ string else: "" ] . asLines
      select: [ stripBoundingBlanks contains: "^#" . not ] .
      select: [ isBlank not ] ;
#--> this next step is redundant - remove it
#-->   skipChars isntNA
#-->       ifTrue: [ :recs <- recs send: [ ^self stripChar: ^my skipChars ] ] ;
   :recs <- recs
      extendBy: [ !fields <- ^self breakOn: ^my delim .
#--> next step will cause the initial reclustering if total fields
#--> (recs * fields) > 50,000 ; defer action to select statement below
#-->                      send: [ asSelf concat: " " . stripBoundingBlanks ] .
                ] .
#-->  eliminate required field criteria - defer to actual feed if desired
#-->      select: [ fields count >= ^my required ] .

#--> modify this select to assume that blanks have NOT been stripped yet
#--> the goal is to check that at least one field has a non-blank value
      select: [ fields any: [ isBlank not ] ] .
      numberElements ;
feed printTimeWith: ("... after recs creation " concat: recs count) ;

#--  process asof date and header if present
   feed processAsofDateFor: recs ;       # <<<---
   feed processHeadersFor: recs ;
feed printTimeWith: ("... after headers " concat: recs count) ;

#--  skip extra top and bottom lines if specified (including header)
   !bot <- recs count - feed getSkipLinesAtBottom ;
   bot > 0 ifTrue: [ :recs <- recs select: [ position <= ^my bot ] ; ] ;
   !top <- feed getSkipLinesAtTop ;
   top > 0 ifTrue: [ :recs <- recs select: [ position > ^my top ] ] ;

#--  if max record limit set, choose the correct number
   !max <- feed maxRecordsToProcess ;
   max isntNA ifTrue: [ :recs <- recs first: max ] ;

#--  purge current instances in feed class 
   feed cleanupCurrentInstances ;
feed printTimeWith: "... after purge " ;

#--  create new instances from supplied string in feed class
   feed setUpdateBlocks ;
feed printTimeWith: "... after setUpdateBlocks" ;
   recs do: [ ^my feed createInstanceFrom: ^self ] ;
feed printTimeWith: ("... after createInstanceFrom:  " concat: recs count) ;

#--  set currency if supplied as a global
   feed updateCurrencyValue ;      # <<<---   

#-->>>  run as of Global supplied date if present 
   feed asofDate else: ^date . evaluate: [
   feed purgeEnabled
     ifTrue: [ feed purgeRecords ] 
    ifFalse: [ feed reconcile ] ;
   feed resetRunTimeAttributes ;
   ] ;
feed printTimeWith: "... end of updateFromString: " ;
] ;

DataFeed defineMethod: [ | processAsofDateFor: recs |
#--  if there is asofDate use it;
#--  otherwise, use the line indicated by getAsofDateLineNumber
#--  otherwise, NA will be stored
   !feed <- ^self asSelf ;
   !asof <- feed getGlobalOption: "asofDate" . asDate ;
   !asofLine <- feed getAsofDateLineNumber else: 0 ;
   asof isDate
     ifTrue: [ feed setAsofDateTo: asof ]
    ifFalse: [ !asof <- recs at: asofLine . asDate ;
               feed setAsofDateTo: asof ;     
             ] ;
  ^self
] ;

DataFeed defineMethod: [ | processHeadersFor: recs |
#--  if there is fieldOrderList use it;
#--  otherwise, use the line indicated by getHeaderLineNumber
   !feed <- ^self asSelf ;
   feed usesHeaderForFields
   ifTrue:
     [ !headers <- feed getGlobalOption: "fieldOrderList" ;
       !headerLine <- feed getHeaderLineNumber ;
       headers isString
       ifTrue: 
         [ !fields <- headers breakOn: "	,|" .
               send: [ copyString stripBoundingBlanks ] ;
           feed setFieldOrderListTo: fields ;
         ] .
       elseIf: [ headerLine isNumber ] 
       then:
         [ !header <- recs at: headerLine ;
           header isntNA
           ifTrue:
             [ #--> add the sbb here since removed above
               !fields <- header fields
                   send: [ asSelf concat: " " . stripBoundingBlanks ] ;
               feed setFieldOrderListTo: fields ;
             ] ;
         ] ;
     ] ;
  ^self
] ;

DataFeed defineMethod: [ | updateCurrencyValue | 
   !feed <- ^self ;
   !curr <- ^self getGlobalOption: "currency" ;
   curr isntDefault
   ifTrue:
     [ feed masterList select: [ currencyId isDefault ] .
          do: [ :currencyId <- ^my curr ] ;
     ] ;
  ^self
] ;

DataFeed defineMethod: [ | cleanupCurrentInstances |
  ^self currentStore masterList
  do: [ ^self rdelete ] ;
  ^self currentStore rcleanStoreAndDictionary ;
] ;

DataFeed defineMethod: [ | createInstanceFrom: string |
    ^self createInstance: string fields
] ;

#-----
#  operating on todo list of blocks is the best way to control clustering;
#  make sure that blanks are stripped (use strip leadning/bounding to get
#  a new string in single cluster) since they are no longer stripped early on
#-----

DataFeed defineMethod: [ | initializeKeys: inputs | 
  !newOne <- ^self asSelf ;
  !todo <- newOne updateBlocksToDo ;
  newOne usesLocalFields
     ifFalse: 
       [ newOne :valueList <- ^global IndexedList new ;
#--         todo do: [ ^my newOne valueList at: position put: NA ] ;
       ] ;
  todo count > 0
  ifTrue:    #-- walk through the update blocks if present
    [
    todo
    do: [ !input <- ^my inputs at: position . else: "" .
              stripLeadingBlanks stripTrailingBlanks ;
          !noValue <- input isDefault || 
            [ ^my newOne naTest any: [ ^my input send: asSelf ] ];
          !toStore <- noValue ifTrue: [ NA ] ifFalse: [ input ] ;
          ^my newOne usesLocalFields
         ifTrue: [ ^my newOne send: ^self . <- toStore ]
         ifFalse: [ ^my newOne valueList at: position put: toStore ] 
        ] ;
     ]
   ifFalse:     #--- todo count = 0 --> AliasFeed case
     [ newOne :valueList <- inputs
       send: [ !input <- ^self ;
               !noValue <- ^self isDefault ||
                  [ ^my newOne naTest any: [ ^my input send: asSelf ] ];
               noValue 
                   ifTrue: [ NA ] 
                  ifFalse: [ ^self stripLeadingBlanks stripTrailingBlanks ] 
             ] ;
     ] ;

  newOne usesLocalFields
  ifFalse:
    [ !todo <- newOne localOnlyBlocks ;
      todo
      do: [ !block <- asSelf ;
            !value <- ^my newOne valueList at: itemNum ;
            value isntDefault
               ifTrue: [ ^my newOne send: block . <- value copyString ]
              else: [ NA ] ;
          ] ;
    ] ;
  newOne 
] ;

DataFeed defineMethod: [ | initializeLocalAttributes |
  ^super initializeLocalAttributes ; 
  ^self initializeDate ;
  ^self
] ;

DataFeed defineMethod: [ | initializeDate |
  !realDate <- date isString
  ifTrue: 
    [ date toUpper = "DEFAULT"
       ifTrue: [ earliestPossibleDate ] 
      ifFalse:
        [ !format <- getDateFormat else: "" . stripBoundingBlanks ;
          !paddedDate <- "0" fill: format count . 
             concat: date . take: -1 * format count ;
          format = "MMYYYY"  || format = "MMYY"
          ifTrue: 
            [ !mm <- paddedDate take: 2 . asNumber asInteger ;
              !year <- paddedDate drop: 2 . asNumber asInteger ;
              year isNumber && mm isNumber
              ifTrue: [ year asDateFromYearForMonth: mm andDay: 1 .
                             + 0 monthEnds 
                      ]
            ] .
          elseIf: [ format = "MMDDYY" || format = "MMDDYYYY" ] 
          then: 
            [ !mm <- paddedDate take: 2 . asNumber asInteger ;
              !dd <- paddedDate drop: 2 . take: 2 . asNumber asInteger ;
              !year <- paddedDate drop: 4 . asNumber asInteger ;
              year isNumber && mm isNumber && dd isNumber
                ifTrue: [ year asDateFromYearForMonth: mm andDay: dd ] 
            ] .
          elseIf: [ format = "YYYYMM" || format = "YYYYMMXX" ]
          then: 
            [ !yyyy <- paddedDate take: 4 . asNumber asInteger ;
              !mm <- paddedDate drop: 4 . take: 2 . asNumber asInteger ;
              yyyy isNumber && mm isNumber 
                ifTrue: [ yyyy asDateFromYearForMonth: mm andDay: 1 .
                             + 0 monthEnds
                        ] 
            ] .
           elseIf: [ date contains: ":" . not ] then: [ date asDate ] .
        ] 
    ] ;
  :date <- realDate isDate
     ifTrue: [ realDate ] 
    ifFalse: [ ^self purgeEnabled
                   ifTrue: [ date ]      #-- could be a string comment
                  ifFalse: [ ^self asofDate ] 
             ] ;
  ^self
] ;

DataFeed defineMethod: [ | reconcile |
   !feed <- ^self asSelf ;
feed printTimeWith: "... start of reconcile" ;
   newLine print ; newLine print ;
   "---  Begin Reconcile for " concat: feed whatAmI . concat: "  ---" .
       centerNL: 80 . printNL ;

   feed initializeProcessing ;
feed printTimeWith: "... after initializeProcessing" ;
   feed runUpdate ;
feed printTimeWith: "... after runUpdate " ;
   feed displayExceptions ;
   feed runWrapup ;
   feed runLocalWrapup ;
   feed runUpdateStats ;

   newLine print ; newLine print ;
   "---  End Reconcile for " concat: feed whatAmI . concat: "  ---" .
       centerNL: 80 . printNL ;
   newLine print ; newLine print ;
] ;

DataFeed defineMethod: [ | initializeProcessing | 
] ;

DataFeed defineMethod: [ | runUpdate |
] ;

DataFeed defineMethod: [ | runWrapup | ] ;
DataFeed defineMethod: [ | runLocalWrapup | ^self ] ;


DataFeed defineMethod: [ | hasBeenModified | 
   ^self isProcessed && [ ^self underlyingRecord isntDefault ] 
] ;

DataFeed defineMethod: [ | runUpdateStats | 
  !time <- ^global LowResolutionTime localTimeNow ;
  !list <- ^self currentStore masterList ;
  ^self define: 'lastUpdateTime' toBe: time ;
  ^self define: 'lastUpdateCount' toBe: list count ;
  list select: [ hasBeenModified ] . 
     do: [ underlyingRecord :lastModifiedByFeed <- ^my time ] ;
  ^self
] ;

DataFeed defineMethod: [ | displayExceptions | 
!feed <- ^self asSelf ;
feed displayExceptionSummary ;
feed displayBadOnes ;
  ^self displayNewOnesFlag isTrue
      ifTrue: [ feed displayNewOnes ] ;
feed displayOtherExceptions ;
feed displayLocalExceptions ;
] ;

DataFeed defineMethod: [ | displayExceptionSummary |
!feed <- ^self asSelf ;
!list <- feed masterList ;
!badFields <- feed fieldOrderList numberElements
  select: [ isntDefault && 
                [ ^my feed updateBlockList at: position . isNA ] 
          ] ;
!notDone <- list select: [ isntProcessed ] ;
!otherStatus <- list select: [ hasStatusInfo ] ;      #<--- was isProcessed &&
"  Upload Feed:   " print ; feed whatAmI printNL ;
"  Total Records: " print ; list count printNL ;
"  Bad Field Ids: " print ; badFields count printNL ;
"  Not Processed: " print ; notDone count printNL ;
"  Other Status:  " print ; otherStatus count printNL ;

badFields count > 0 
ifTrue:
  [ newLine print ; "The following fields are not defined:" printNL ;
    badFields 
    do: [ position print: 5 ; " " print: 5 ;
          printNL ;
        ] ;
  ] ;

] ;


DataFeed defineMethod: [ | displayBadOnes | ] ;
DataFeed defineMethod: [ | displayNewOnes | ] ;
DataFeed defineMethod: [ | displayOtherExceptions | ] ;
DataFeed defineMethod: [ | displayLocalExceptions | ] ;


DataFeed defineMethod: [ | resetRunTimeAttributes |
  ^global GlobalsFeed setDefaultGlobals ;
  ^self disablePurge ;
  ^self
] ;

DataFeed defineMethod: [ | saveRejectsToFile: fileName withLabel: label |
  !feed <- ^self currentStore ;
  !rejects <- feed masterList select: [ isntProcessed ] ;
  rejects count > 0
  ifTrue:
    [ :fileName <- fileName else: "" ;
      !isBulk <- ^self getGlobalOption: "bulkLoadInProgress" . else: FALSE ;
      isBulk not ||
         [ !currentFile <- fileName asFileContents ;
           currentFile isNA || [currentFile count = 0 ] 
         ] 
      ifTrue: 
        [ fileName substituteOutputOf:
             [ feed fieldOrderList do: [ print; " | " print; ] ;
               newLine print ;
             ] ;
        ] ;

      fileName appendOutputOf: 
        [ "#-" fill: 80 .printNL ;
          "# Rejects from feed " print; feed whatAmI printNL ;
          "#                at " print;
          ^global Utility UnixSeconds currentTime printNL ;
          "# " concat: (label else: " ") . printNL ;
          rejects
          do: [ getValueList do: [ print ; " | " print ] ;
                !reason <- ^self rejectReason ;
                reason isntDefault
                  ifTrue: [ "  # " print; reason print ] ;
                newLine print ;
              ] ;
        ] ;
    rejects count print ; 
    " rejects saved to file " print; fileName else: "" . printNL;
    ] ;
  rejects count > 0
] ;

DataFeed defineMethod: [ | purgeRecords | 
!feed <- ^self asSelf ;
feed displayExceptionSummary ;
newLine print ; 
">>>  Purge Not Defined for Class." printNL ;
newLine print ;
] ;


#--------------------
#  bulk updates
#--------------------
DataFeed defineMethod: [ | isBulkUpdate |
  'isBulkUpdate' locateInDictionaryOf: ^tmp . value isTrue
] ;

DataFeed defineMethod: 
[ | bulkLoadFromFile: fname withConfig: cfgName
       andBatchSize: bsize fromBatch: bnum to: enum | 
  !feed <- ^self ;

  newLine print ; 
  "--->  Bulk Loading " print ; ^self whatAmI printNL ;
  "      from file " print ; fname printNL ;
  "      using config " print ; cfgName printNL ;
  "      " print ; ^global Utility UnixSeconds currentTime printNL ;

  !file <- "file:" concat: fname .
      asOpenVisionChannel
      setTrimFormatToUntrimmed ;
  !fsize  <- file byteCount;
  fsize = 0 
    ifTrue: [ ">>> Empty file or file not found." printNL ] .
    elseIf: [ cfgName isntDefault && [cfgName asFileContents isNA ] ]
      then: [ ">>> Config file " concat: cfgName . concat: " not found." .
               printNL ;
            ] .
  else:
    [
    :bsize <- bsize else: fsize ;
    !approxBatches <- (fsize / bsize + 1) asInteger ; 
    !firstBatch <- bnum else: 0 ;
    !lastBatch <- enum else: (approxBatches - 1) . asInteger ;

#--- load the cfg file, if present
    ^self loadConfig: cfgName ;

#--- remove unused lines at top, including header
    !skipLines <- ^self getSkipLinesAtTop ;
    !startOfFile <- "" ;
    !count <- 1 ;
    [ count <= skipLines ] 
       whileTrue: [ !line <- file getLine else: "" .
                     extendBy: [ !baseLine <- ^self stripBoundingBlanks 
                                     stripChar: newLine ;
                                 !skipIt <- (baseLine isBlank || 
                                              [ baseLine contains: "^#"] );
                               ] ;
                  line skipIt
                  ifFalse: [ :startOfFile <- startOfFile concat: line asSelf ;
                             :count increment  ;
                           ] ;
                  ] ;
    !extraOffset <- 0;
    :extraOffset <- (startOfFile count max: 1 . - 1) asInteger ;

#--- if fieldOrderList not stored, get it from first line of file
    !headers <- ^self getGlobalOption: "fieldOrderList";
    headers isNA
     ifTrue: [ !headerLine <- ^self getHeaderLineNumber else: 1 ;
               !line <- startOfFile asLines at: headerLine ;
               !headers <- line translate: "|	" to: ",";
               ^global GlobalsFeed
                   setGlobalOption: "fieldOrderList" to: headers;
            ];
#--- Turn off the options that would not work with bulk processing.
    ^global GlobalsFeed 
        setGlobalOption: "headerLineNumber"   to: 0 .
        setGlobalOption: "skipTop"            to: 0 .
        setGlobalOption: "skipBottom"         to: 0 .
        setGlobalOption: "maxRecords"         to: 0 .
        setGlobalOption: "asOfDateLineNumber" to: 0 ;

#--- if delimiter is default, unset it so global load doesn't get confused
  !delimiter <- ^self getGlobalOption: "delimiter" . else: "|" ;
  (delimiter contains: "|" ) || (delimiter contains: "	")
      ifTrue: [ ^global GlobalsFeed Globals delete: "delimiter" ] ;

#---  preserve the globals
    ^global GlobalsFeed enableBulkLoadInProgress ;
    !globals <- Globals selectorList
       select: [ asString take: 1 . != "_" ] .
       extendBy: 
          [ !option <- asSelf ;
            !setting <- ^my Globals at: asSelf ;
          ] .
       select: [ setting isntNA ] ;

    newLine print ;
    "    File Size " print: 20 ;   fsize printNL;
    "    ~ Batch Size " print: 20 ; bsize printNL ;
    "    ~ # of Batches " print: 20 ; approxBatches printNL ;
    "    First Batch " print: 20 ; firstBatch printNL ;
    "    Last Batch " print: 20 ; lastBatch printNL ;
    newLine print ;

  #--  iterate through the batches
    !rejectFile <- ^self getGlobalOption: "rejectFileName" . 
        else: [ fname concat: ".rejects" ] ;
    !rejectsFound <- FALSE ;
    approxBatches sequence0 
        select: [ ^self >= ^my firstBatch && ^self <= ^my lastBatch ] .
    iterate: [
      !start <- (^self * ^my bsize + ^my extraOffset + 1) asInteger ;
      !size <- ^my bsize min: (^my fsize - start + 1) . max: 0 . asInteger ;
      !string <- ^my file getString: size at: start . else: "" ;
      !extra <- ^my file getLine else: "" ;
      ^my :extraOffset incrementBy: extra count ;
     string count > 0 
      ifTrue: 
        [ newLine print ;
          "*" fill: 5 . print ;
          "  starting iteration " print ; print ; "  -- " print ;
          ^global Utility UnixSeconds currentTime printNL ;
          "    start     size   string    extra" printNL ;
          start print ; size print ; string count print ; extra count printNL ;

#-- reset globals
         ^global GlobalsFeed clearGlobalOptions ;
         ^my globals
            do: [ ^global GlobalsFeed setGlobalOption: option to: setting ] ;
#        ^global GlobalsFeed showGlobals ;
          ^my feed updateFromString: (string concat: extra) ;
         !status <- ^my feed saveRejectsToFile: ^my rejectFile 
                withLabel: ("Bulk load iteration: " concat: asSelf) ;
          status ifTrue: [ ^my :rejectsFound <- TRUE ] ;
        ] ;
    ];

    file close;
    newLine print ;
    "--->  End of Bulk Loading " print ; ^self whatAmI printNL ;
    "      from file " print ; fname printNL ;
    rejectsFound 
      ifTrue: [ "      rejects in " print; rejectFile printNL ] ;
    "      " print ; ^global Utility UnixSeconds currentTime printNL ;
  
    ^global GlobalsFeed disableBulkLoadInProgress ; 
   ] ;

] ;

GlobalsFeed defineMethod: [ | enableBulkLoadInProgress  | 
  ^self setGlobalOption: "bulkLoadInProgress" to: TRUE .
];

GlobalsFeed defineMethod: [ | disableBulkLoadInProgress | 
  ^self setGlobalOption: "bulkLoadInProgress" to: NA ;
  ^global GlobalsFeed setDefaultGlobals ;
];

DataFeed defineMethod: 
[ | bulkLoadFromFile: fname withFieldList: fieldList
       andBatchSize: bsize fromBatch: bnum to: enum | 
  !feed <- ^self ;

  newLine print ; 
  "--->  Bulk Loading " print ; ^self whatAmI printNL ;
  "      from file " print ; fname printNL ;
  "      " print ; ^global Utility UnixSeconds currentTime printNL ;
  !file <- "file:" concat: fname .
      asOpenVisionChannel
      setTrimFormatToTrailingBlanks ;
  !fsize  <- file byteCount;
  !approxBatches <- (fsize / bsize + 1) asInteger ; 
  !firstBatch <- bnum else: 0 ;
  !lastBatch <- enum else: (approxBatches - 1) . asInteger ;

  newLine print ;
  "    File Size " print: 20 ;   fsize printNL;
  "    ~ Batch Size " print: 20 ; bsize printNL ;
  "    ~ # of Batches " print: 20 ; approxBatches printNL ;
  "    First Batch " print: 20 ; firstBatch printNL ;
  "    Last Batch " print: 20 ; lastBatch printNL ;

  #--  headers supplied in fieldList if present, first line of file otherwise
  !headers <- fieldList isntNA
      ifTrue: [ fieldList ] 
        else: [ file getLine else: "" . translate: "|	" to: "," ] ;   # <---
  !globals <- 
     [ "option	setting	settingType" printNL ;
       "headerLineNumber	0	Integer" printNL ;
       "fieldOrderList	" print ; headers printNL ;
     ] divertOutput ;

  !extraOffset <- fieldList isntNA
     ifTrue: [ 0 ] ifFalse: [ headers count ] ;      # <---

  #--  iterate through the batches
  approxBatches sequence0 
      select: [ ^self >= ^my firstBatch && ^self <= ^my lastBatch ] .
  iterate: [
    !start <- (^self * ^my bsize + ^my extraOffset + 1) asInteger ;
    !size <- ^my bsize min: (^my fsize - start + 1) . max: 0 . asInteger ;
    !string <- ^my file getString: size at: start . else: "" ;
    !extra <- ^my file getLine else: "" ;
    ^my :extraOffset incrementBy: extra count ;
    string count > 0 
    ifTrue: 
      [ newLine print ;
        "*" fill: 5 . print ;
        "  starting iteration " print ; print ; "  -- " print ;
        ^global Utility UnixSeconds currentTime printNL ;
        "    start     size   string    extra" printNL ;
        start print ; size print ; string count print ; extra count printNL ;
        [ ^global GlobalsFeed updateFromString: ^my globals
        ] divertOutput printNL ;     # <---  added print
        ^my feed updateFromString: (string concat: extra) ;
      ] ;
  ];

  file close;
  "--->  End of Bulk Loading " print ; ^self whatAmI printNL ;
  "      from file " print ; fname printNL ;
  "      " print ; ^global Utility UnixSeconds currentTime printNL ;
] ;

#----------------------------------------------------------------------

#--------------------
#  DataFeed Globals
#--------------------
DataFeed define: 'Globals' toBe: Dictionary new ;

DataFeed defineMethod: [ | getGlobalOption: option |
  Globals at: option asString .
     else: [ !id <- Globals 
                at: ("_" concat: option asString toUpper . stripChar: " " );
             Globals at: id
           ] 
] ;

DataFeed defineMethod: [ | showGlobals |
  "--- Current Globals ---" centerNL: 80 . print ;
  Globals selectorList select: [ asString take: 1 . != "_" ] .
  do: [ print ; " :->  " print ; 
        ^my Globals at: asSelf . printNL ;
      ] ;
] ;


#--------------------
#  GlobalsFeed
#    option, value
#--------------------

GlobalsFeed
   defineFixedProperty: 'option' .
   defineFixedProperty: 'setting' .
   defineFixedProperty: 'settingType' .
;

GlobalsFeed defineMethod: [ | getUploadProperties | 
  !feed <- ^self ;
  ("option", "setting", "settingType" ) 
    send: [ ^my feed getMessage: asSelf ] .
    extendBy: [ !triggerKey ] .
] ;

GlobalsFeed defineMethod: [ | displayInfo | 
  option print ; ": " print ; setting print ;
  " as " print ; settingType printNL ;
] ;

GlobalsFeed defineMethod: [ | setGlobalOption: option to: value |
  !mask <- "_" concat: option asString toUpper . stripChar: " " ;
  !id <- Globals at: mask . else: option ;
  Globals at: id asString put: value ;
  Globals at: mask put: option ;
  ^self
] ;

#--------------------
#  Note: Globals dictionary should recreate dictionary
#     instead of selector deletes which do not really cleanup
#--------------------

GlobalsFeed defineMethod: [ | clearGlobalOptions |
  Globals rdelete ;
  ^global DataFeed define: 'Globals' toBe: ^global Dictionary new ;
  ^self
] ;

GlobalsFeed defineMethod: [ | setDefaultGlobals |
  !bulkLoad <- ^self getGlobalOption: "bulkLoadInProgress" ;
  clearGlobalOptions 
     setGlobalOption: "headerLineNumber" to: 1 .
     setGlobalOption: "fieldOrderList" to: NA .
     setGlobalOption: "requiredFields" to: 0 .
     setGlobalOption: "delimiter" to: "	|" .   #- tab or |
     setGlobalOption: "naTest" to: "NA" .
     setGlobalOption: "maxRecords" to: 0 .
     setGlobalOption: "skipTop" to: 0 .
     setGlobalOption: "skipBottom" to: 0 ;
  bulkLoad isTrue ifTrue: [ enableBulkLoadInProgress ] ;
^self
] ;

#--  if headers disabled, ignore header line number
GlobalsFeed defineMethod: [ | getSkipLinesAtTop | 
 usesHeaderForFields
   ifTrue: [ ^super getSkipLinesAtTop ] 
  ifFalse: 
    [ ^self getGlobalOption: "skipTop" . asNumber 
        if: [ ^self > 0 ] then: [ ^self ] .
   #--     else: [ getHeaderLineNumber ] . 
          else: 0 
   ] 
] ;

GlobalsFeed defineMethod: [ | initializeProcessing |
  ^self masterList
     select: [ setting = "setting" || settingType = "settingType" ] .
   do: [ ^self rdelete ] ;
  ^self masterList 
     select: [ setting isntDefault && settingType isntDefault ] .
  do: [ :setting <- setting as: settingType ] ;
] ;

GlobalsFeed defineMethod: [ | runUpdate | 
   setDefaultGlobals ;
  ^self asSelf masterList select: [ option isntDefault ] .
  do: [ ^self setGlobalOption: option to: setting ;
        :isProcessed <- TRUE ;
      ] ;
] ;

GlobalsFeed defineMethod: [ | displayExceptions | 
  !feed <- ^self asSelf ;
  ^super displayExceptions ;
  newLine print ;
  showGlobals ;
] ;

GlobalsFeed defineMethod: [ | displayBadOnes |
  !feed <- ^self asSelf ;
  !badOnes <- feed instanceList select: [ isntProcessed && isntDefault ] ;
  badOnes count > 0 
  ifTrue: [ newLine print ; 
            badOnes count print ; " not processed. " printNL;
            badOnes sortUp: [ option ] . do: [ displayInfo ] ;
          ] ;
] ;

GlobalsFeed defineMethod: [ | resetRunTimeAttributes |
  ^self
] ;

#----------
#  and set defaults
#----------

GlobalsFeed setDefaultGlobals ;

DataFeed 
   define: 'lastUpdateTime' toBe: NA .
   define: 'lastUpdateCount' toBe: 0 .
     enableLocalFields
     enableFieldsFromHeader
     disablePurge
     enableDisplayNewOnes 
     setFieldOrderListTo: List .
     setUpdateBlocks 
     setRequiredFieldsTo: NA .
;




#======================================================================

####################
#  SchemaFeeds
#     These feeds are used to define class and message protocol
#     using the following subclasses
#
#     ClassSetup  - create a new subclass
#     PropertySetup - define properties for a class
#     MessageSetup - set attribute for a message
#     DataFeedSetup - create new DataFeed subclasses
#        MasterFeedSetup - create new MasterFeed subclasses
#        EntityExtenderFeedSetup - create new EntityExtenderFeed subclasses
#        TransactionFeedSetup - create new TransactionFeed subclasses
#        AliasFeedSetup - create new AliasFeed subclasses
#        XRefFeedSetup - create new XRefFeed subclasses
#        MembershipFeedSetup - create new MembershipFeed subclasses
#        RangeGroupFeedSetup - create new RangeGroupFeed subclasses
#
####################

#--------------------
#  SchemaFeeds
#--------------------

SchemaFeeds defineMethod: [ | locateClass: id |
#- use schema so id is looked up in a case-insensitive way
   ^global getMessage: id . 
       isntDefault: [ message evaluate ] else: [ NA ]
] ;

#--------------------
#  ClassSetup
#    classId, parentId, description
#--------------------

ClassSetup
   defineFixedProperty: 'classId' .
   defineFixedProperty: 'parentId' .
   defineFixedProperty: 'description' .
   defineFixedProperty: 'ospace' .

   defineFixedProperty: 'parent' .
   defineFixedProperty: 'newOneCreated' .
;

ClassSetup defineMethod: [ | hasStatusInfo | 
  newOneCreated isTrue
] ;

ClassSetup defineMethod: [ | getUploadProperties | 
  ^super getUploadProperties
    do: [ code = "classId" 
           ifTrue: [ :triggerKey <- 1 ] ;
        ]
] ;

ClassSetup defineMethod: [ | displayInfo | 
  classId print ; " " print ; parentId print ; " " print ;
  ospace isntNA
     ifTrue: [ "( " concat: ospace . concat: " )" . print ] ;
  description printNL ;
] ;


ClassSetup defineMethod: [ | initializeProcessing | 
  !feed <- ^self asSelf ;
  !todo <- feed masterList
  do: [ :underlyingRecord <- ^self locateClass: classId ;
        :parent <- ^self locateClass: parentId ;
      ] .
    select: [ underlyingRecord isNA ] ;

#-- run creation twice to pick up subclasses created on first pass
  feed createClassesFor: todo ;
  :todo <- todo select: [ underlyingRecord isNA ] .
      do: [ :parent <- ^self locateClass: parentId ] ;
  feed createClassesFor: todo ;
] ;

ClassSetup defineMethod: [ | createClassesFor: list |
  list select: [ parent isntNA ] .
    groupedByString: [ classId ] .      #- only create 1 per unique id
    select: [ isntDefault ] .
  iterate:
      [ !record <- groupList at: 1 ;
        !newClass <- record parent 
            createSubclass: asSelf copyString at: ^global CoreWorkspace ;
        record :newOneCreated <- TRUE ;

        !spaceToUse <- record ospace asNumber asInteger ;
        :spaceToUse <- spaceToUse isNumber && [ spaceToUse > 0 ] 
            ifTrue: [ spaceToUse asObjectSpace ] ;
        !space <- spaceToUse isntNA
           ifTrue: [  spaceToUse getSpace ] .
           elseIf: [ newClass isDataRecord ] 
             then: [ ^global Environment DBA DataRecordClasses ] ;
        record :ospace <- space isntNA ifTrue: [ space number ] ;

        space isntNA 
        ifTrue: 
          [ newClass establishResidenceInSpaceOf: space ;
            newClass classDescriptor primaryStringCluster
                  establishResidenceInSpaceOf: space ;
          ] 
        ifFalse:
          [ newClass classDescriptor primaryStringCluster
                  establishResidenceInSpaceOf: ^global Schema ;
          ] ;
        groupList do: [ :underlyingRecord <- ^my newClass ] ;
      ] ;

#-- update CoreWorkspace messages in Schema so case insenstive lookup available
  ^global Schema createMessagesForClass: ^global CoreWorkspace ;

]  ;

ClassSetup defineMethod: [ | runUpdate | 
  ^self asSelf instanceList select: [ underlyingRecord isntNA ] .
  do: [ updateDescription ;
        :isProcessed <- TRUE ;
      ] ;

] ;

ClassSetup defineMethod: [ | updateDescription |
   !cd <- underlyingRecord classDescriptor ;
   description isntDefault
      ifTrue: [ cd setDescriptionTo: description copyString ] ;
   underlyingRecord isEntity && [ cd sampleUsage isNA ] 
   ifTrue: 
     [ !text <- 
         [ "Named " print ; underlyingRecord whatAmI print ;
           " XXX " printNL ;
           "Named " print ; underlyingRecord whatAmI print ; 
           " at: " print ; "XXX" asQuotedString print ; " ." printNL ;
         ] divertOutput ;
       cd setSampleUsageTo: text ;
    ] ;
] ;


ClassSetup defineMethod: [ | displayNewOnes | 
  !newOnes <- ^self instanceList select: [ newOneCreated ] ;
  newOnes count > 0
  ifTrue: [ newLine print ; 
            newOnes count print ; " new subclasses created. " printNL;
            newOnes sortUp: [ classId ] . do: [ displayInfo ] ;
          ] ;
] ;

ClassSetup defineMethod: [ | displayBadOnes | 
  !badOnes <- ^self asSelf instanceList 
      select: [ isntProcessed && isntDefault ] ;
  badOnes count > 0
  ifTrue: [ newLine print ; 
            badOnes count print ; " not processed. " printNL;
            badOnes sortUp: [ classId ] . do: [ displayInfo ] ;
          ] ;
] ;

#======================================================================

#--------------------
#  PropertySetup
#    classId, property, tsFlag, dataType, defaultValue, adjusts, propertyXRef, description
#--------------------

PropertySetup
   defineFixedProperty: 'classId' .
   defineFixedProperty: 'property' .
   defineFixedProperty: 'tsFlag' .
   defineFixedProperty: 'dataType' .
   defineFixedProperty: 'defaultValue' .
   defineFixedProperty: 'adjustments' .
   defineFixedProperty: 'propertyXRef' .
   defineFixedProperty: 'description' .

   defineFixedProperty: 'class' .
   defineFixedProperty: 'convertedType' .
   defineFixedProperty: 'convertedDefault' .
   defineFixedProperty: 'newOneCreated' .
;

PropertySetup defineMethod: [ | hasStatusInfo | 
  newOneCreated isTrue
] ;

PropertySetup defineMethod: [ | getUploadProperties | 
  ^super getUploadProperties
    do: [ :triggerKey <- 
             code = "classId" ifTrue: [ 1 ] .
               elseIf: [ code = "property" ] then: [ 2 ] ;
        ]
] ;

PropertySetup defineMethod: [ | displayInfo | 
  classId print ; " " print ; property print ; 
  tsFlag ifTrue: [ " (TS) " ] else: [ " (FP ) " ] . print ;
  dataType print ; " " print ; defaultValue print ; " " print ; 
  propertyXRef
      ifDefault: [ ] else: [  " (xref " print ; print ; ") " print ] ;
  adjustments else: "" . print ; " " print ; 
  description else: "" . printNL ;
] ;

PropertySetup defineMethod: [ | isAdjusted | adjustments isntDefault ] ;

PropertySetup defineMethod: [ | isApplicationClass | 
  class isEntityOrBridge || class isDataRecord || class isLinkRecord 
     || class isToolKit      #<-- add this test
] ;

PropertySetup defineMethod: [ | initializeProcessing | 
  !feed <- ^self asSelf ;
  !todo <- feed masterList
  do: [ :tsFlag <- tsFlag else: "" . as: ^global Boolean . else: FALSE ;
        property isNA ifTrue: [ :property <- "" ] ;
        property isntDefault && [ isAdjusted ] && [ property take: 1 . != "_" ]
              ifTrue: [ :property <- "_" concat: property ] ;
       :class <- ^self locateClass: classId ;
        !mid <- class getMessage: property ;
        mid isntNA && [ mid messageClassDescriptor class = class ] 
            ifTrue: [ :underlyingRecord <- mid ] ;
        :convertedType <- ^self locateClass: dataType ;
        :convertedDefault <- defaultValue isntNA
        ifTrue: 
          [ defaultValue as: convertedType .
               else: [ ^self locateClass: defaultValue ] .
               else: [ defaultValue as: ^global Number ] . 
               else: [ defaultValue as: ^global Date ]
          ] ;
      ] .
  select: [ underlyingRecord isNA && class isntNA && property isntDefault] .
  select: [ isApplicationClass ] .
  select: [ property asSelector isntNA ] ;

  todo groupedByString: [ classId concat: property ] .    # 1 per unique
  do: [ 
      !record <- groupList at: 1 ;
      !default <- record convertedDefault ;
      record tsFlag 
      ifTrue: [ record class 
                   define: record property withDefault: default ;
              ]
        else: [ record class 
                   defineFixedProperty: record property withDefault: default ;
              ] ;
      groupList do: [ :underlyingRecord <- class getMessage: property ] ;
      record :newOneCreated <- TRUE ;
      record isAdjusted
         ifTrue: [ record processAdjustments ] ;
      ] ;
]  ;

PropertySetup defineMethod: [ | runUpdate | 
  ^self asSelf instanceList select: [ underlyingRecord isntNA ] .
     select: [ underlyingRecord type isProperty ] .    #<---
  do: [ processDefaultValue ;
        processDescription ;
        processPropertyXRef ;
        underlyingRecord enableOkayToUpload ;
        :isProcessed <- TRUE ;
      ] ;
] ;


PropertySetup defineMethod: [ | processAdjustments |
  !text <- adjustments toUpper stripChar: " " ;
  !method <- [
     "[ | " print ; property drop: 1 . print ; " | " printNL ;
     "    " print ; property print ;
     text contains: "CURRENCY" .
        ifTrue: [ " * ^self currencyFactor " print ] ;
     text contains: "PERSHARE" .
        ifTrue: [ " / ^self adjustmentFactor " print ] .
        elseIf: [ text contains: "SHARES" ]
          then: [ " * ^self adjustmentFactor " print ] ;
     newLine print ;
     "]" printNL ;
  ] divertOutput ;
  class defineMethod: method ;
] ;

PropertySetup defineMethod: [ | processDefaultValue |
  convertedType isntNA
     ifTrue: [ underlyingRecord setReturnObjectTypeTo: convertedType ] ;
  convertedDefault isntNA
     ifTrue: [ underlyingRecord setDefaultValueTo: convertedDefault ] ;
  class isEntityOrBridge && 
    [ convertedDefault isDataRecord || convertedDefault isBridge ]
  ifTrue:
    [ convertedDefault setUpdateBlockTo: property asUpdateBlock ;
    ] ;
] ;

PropertySetup defineMethod: [ | processDescription |
  description isntDefault 
    ifTrue: [ underlyingRecord setDescriptionTo: description copyString ] ;
  underlyingRecord setFunctionTypeTo: "Data" ;
] ;

PropertySetup defineMethod: [ | processPropertyXRef |
  !id <- propertyXRef isntDefault ifTrue: [ propertyXRef asSelector ] ;
  id isntNA && 
  [ convertedDefault isntNA ] &&
  [ convertedDefault respondsTo: propertyXRef . not ] 
  ifTrue:
    [ !defaultList <- ^global IndexedList new ;
      tsFlag 
      ifTrue: 
        [ convertedDefault define: id withDefault: defaultList ;
        ]
      else: 
        [ convertedDefault
             defineFixedProperty: id withDefault: defaultList ;
        ] ;
      !text <- "Cross reference for '" concat: property .
         concat: "' at class " . concat: class whatAmI ;
      convertedDefault getMessage: id . 
           setDescriptionTo: text .
           setReturnObjectTypeTo: class .
           setFunctionTypeTo: "Data" ;
    ] ;
] ;

PropertySetup defineMethod: [ | displayNewOnes | 
  !feed <- ^self asSelf ;
  !newOnes <- feed instanceList select: [ newOneCreated ] ;
  newOnes count > 0
  ifTrue: [ newLine print ; 
            newOnes count print ; " new properties created. " printNL;
            newOnes sortUp: [ property ] .sortUp: [ classId ] . 
                do: [ displayInfo ] ;
          ] ;
] ;

PropertySetup defineMethod: [ | displayBadOnes | 
  !badOnes <- ^self asSelf instanceList 
      select: [ isntProcessed && isntDefault ] ;
  badOnes count > 0
  ifTrue: [ newLine print ; 
            badOnes count print ; " not processed. " printNL;
            badOnes sortUp: [ property ] . sortUp: [ classId ] .
            do: [ classId print ; " " print ; property print ;
                  class isNA ifTrue: [ "  - class not found. " print ] .
                  elseIf: [ isApplicationClass not ]
                    then: [ " - class must be an Application class. " print ] .
                  elseIf: [ underlyingRecord isntNA &&
                              [ underlyingRecord type isProperty not ] 
                          ] 
                    then: [ " - already defined as a " print ;
                            underlyingRecord type name print ;
                          ] .
                    else: [ " - bad property name." print ] ;
                  newLine print ;
                ] ;
          ] ;
  !notChanged <- ^self asSelf instanceList 
      select: [ isProcessed && convertedDefault isntNA ] .
      select: [ convertedDefault != underlyingRecord defaultValue ] ;
  notChanged count > 0
  ifTrue: [ newLine print ; 
            notChanged count print ; " defaultValue not changed. " printNL;
            notChanged sortUp: [ property ] . sortUp: [ classId ] .
            do: [ classId print ; " " print ; property print ; " | " print ;
                  " default is " print ; 
                  underlyingRecord defaultValue whatAmI print ;
                  " not changed to " print ; convertedDefault whatAmI printNL ;
                ] ;
          ] ;
] ;


#--------------------
#  MessageSetup
#     classId, message, keyType, returnType, containerType, 
#     level, function, tvFlag, description, brief, plist
#--------------------

MessageSetup
   defineFixedProperty: 'classId' .
   defineFixedProperty: 'message' .
   defineFixedProperty: 'keyType' .
   defineFixedProperty: 'returnType' .
   defineFixedProperty: 'containerType' .
   defineFixedProperty: 'level' .
   defineFixedProperty: 'function' .
   defineFixedProperty: 'tvFlag' .
   defineFixedProperty: 'brief' .
   defineFixedProperty: 'description' .
   defineFixedProperty: 'paramList' .

   defineFixedProperty: 'class' .
   defineFixedProperty: 'convertedReturnType' .
   defineFixedProperty: 'convertedContainer' .
   defineFixedProperty: 'convertedParamList' .
;

MessageSetup defineMethod: [ | getUploadProperties | 
  ^super getUploadProperties
    do: [ :triggerKey <- 
             code = "classId" ifTrue: [ 1 ] .
               elseIf: [ code = "message" ] then: [ 2 ] ;
        ]
] ;

MessageSetup defineMethod: [ | displayInfo | 
  classId print ; " " print ; message print ; 
  tvFlag ifTrue: [ " (TV) " ] else: [ "" ] . print ;
  returnType print ; " " print ; containerType print ; " " print ;
  description else: brief . else: "" . printNL ;
] ;

MessageSetup defineMethod: [ | initializeProcessing | 
  !feed <- ^self asSelf ;
  feed masterList
  do: [ :tvFlag <- tvFlag else: "" . as: ^global Boolean . else: FALSE ;
        message isNA ifTrue: [ :message <- "" ] ;
        :class <- ^self locateClass: classId ;
        !mid <- class getMessage: message ;
        mid isntNA 
            ifTrue: [ :underlyingRecord <- mid ] ;
        :convertedReturnType <- ^self locateClass: returnType ;
        :convertedContainer <- ^self locateClass: containerType ;
        :convertedParamList <- paramList else: "" . breakOn: "," .
             send: [ ^my locateClass: asSelf ] ;
      ] ;
]  ;

MessageSetup defineMethod: [ | runUpdate | 
  ^self asSelf instanceList select: [ underlyingRecord isntNA ] .
  do: [ !mid <- underlyingRecord ;
        !key <- keyType else: "" . toUpper ;
        key = "PARTIAL" ifTrue: [ mid flagAsPartialKeyField ] .
          elseIf: [ keyType = "FULL" ] then: [ mid flagAsKeyField ] ;
        convertedReturnType isntNA
          ifTrue: [ mid setReturnObjectTypeTo: convertedReturnType ] ;
        convertedContainer isntNA
            ifTrue: [ mid setReturnObjectContainerTo: convertedContainer ] ;
        mid type isMethod && tvFlag isTrue
           ifTrue: [ mid setTypeTo: ^global Named MessageType MethodTV ] ;
        description isntDefault 
           ifTrue: [ mid setDescriptionTo: description copyString ] ;
        brief isntDefault 
           ifTrue: [ mid setBriefDescriptionTo: brief copyString ] ;
        level isntDefault ifTrue: [ mid setLevelTo: level ] ;
        function isntDefault
          ifTrue: [ mid setFunctionTypeTo: function ] .
          elseIf: [ mid functionType isDefault ] 
            then: [ mid setFunctionTypeTo: "Data" ] ;
        !params <- convertedParamList numberElements
            select: [ isntDefault ] .
            do: [ ^my mid setParameter: position typeTo: asSelf ] ;
        :isProcessed <- TRUE ;
      ] ;
] ;

MessageSetup defineMethod: [ | displayBadOnes | 
  !badOnes <- ^self asSelf instanceList 
      select: [ isntProcessed && isntDefault ] ;
  badOnes count > 0
  ifTrue: [ newLine print ; 
            badOnes count print ; " not processed. " printNL;
            badOnes sortUp: [ message ] . sortUp: [ classId ] .
            do: [ classId print ; " " print ; message print ;
                  class isNA ifTrue: [ "  - class not found. " print ] .
                    else: [ " - bad message name." print ] ;
                  newLine print ;
                ] ;
          ] ;
] ;

#======================================================================

#--------------------
#  DataFeedSetup
#    feedId, baseClassId 
#--------------------

DataFeedSetup
   defineFixedProperty: 'feedId' .
   defineFixedProperty: 'baseClassId' .

   defineFixedProperty: 'baseClass' .
   defineFixedProperty: 'newOneCreated' .
;

DataFeedSetup defineMethod: [ | setFeedClassTo: type |
  ^self define: 'feedClassType' toBe: type ;
  ^self
] ;

DataFeedSetup defineMethod: [ | getUploadProperties | 
  ^super getUploadProperties
    do: [ code = "feedId" 
           ifTrue: [ :triggerKey <- 1 ] ;
        ]
] ;

DataFeedSetup defineMethod: [ | displayInfo | 
  feedId print ; " (" print ;  ^self feedClassType whatAmI print ; ") " print ;
  baseClassId printNL ;
] ;

DataFeedSetup defineMethod: [ | okayToProcess | baseClass isntNA ] ;

DataFeedSetup defineMethod: [ | getDescription |
   "This feed is used to update instances of the class " 
          concat: baseClass whatAmI . concat: "."
] ;

DataFeedSetup defineMethod: [ | initializeProcessing | 
  !feed <- ^self asSelf ;
  !todo <- feed masterList
    do: [ ^self initializeFeedAttributes ] .
    select: [ okayToProcess ] .
    select: [ underlyingRecord isNA ] ;

  todo
    groupedByString: [ feedId ] .      #- only create 1 per unique id
    select: [ isntDefault ] .
  iterate: 
      [ !record <- groupList at: 1 ;
        !newClass <- record feedClassType
            createSubclass: asSelf copyString at: ^global CoreWorkspace ;
        record :newOneCreated <- TRUE ;
        groupList do: [ :underlyingRecord <- ^my newClass ] ;
      ] ;

#-- update ^global messages in Schema so case insenstive lookup available
  ^global Schema createMessagesForClass: ^global CoreWorkspace ;

]  ;

DataFeedSetup defineMethod: [ | initializeFeedAttributes |
  :underlyingRecord <- ^self locateClass: feedId ;
  :baseClass <- ^self locateClass: baseClassId ;
] ;

DataFeedSetup defineMethod: [ | runUpdate | 
  ^self asSelf instanceList select: [ underlyingRecord isDataFeed ] .
  do: [ !cd <- underlyingRecord classDescriptor ;
        cd setDescriptionTo: ^self getDescription ;
        :isProcessed <- TRUE ;
      ] ;
] ;

DataFeedSetup defineMethod: [ | displayNewOnes | 
  !feed <- ^self asSelf ;
  !newOnes <- feed instanceList select: [ newOneCreated ] ;
  newOnes count > 0
  ifTrue: [ newLine print ; 
            newOnes count print ; " new feed subclasses created. " printNL;
            newOnes sortUp: [ feedId ] . do: [ displayInfo ] ;
          ] ;
] ;

DataFeedSetup defineMethod: [ | displayBadOnes | 
  !feed <- ^self asSelf ;
  !badOnes <- ^self asSelf instanceList 
      select: [ isntProcessed && isntDefault ] ;
  badOnes count > 0
  ifTrue: [ newLine print ; 
            badOnes count print ; " not processed. " printNL;
            badOnes sortUp: [ feedId ] .
            do: [ feedId print ; 
                  baseClass isNA
                  ifTrue:
                    [ "  --> base class " print ; baseClassId print ;
                      " not found." printNL ;
                    ]
                ] ;
          ] ;
] ;

#--------------------
#  MasterFeedSetup
#    feedId, baseClassId
#--------------------
MasterFeedSetup setFeedClassTo: MasterFeed ;

MasterFeedSetup defineMethod: [ | runUpdate | 
  ^super runUpdate ;
  ^self asSelf instanceList select: [ isProcessed ] .
  do: [ underlyingRecord setEntityTypeTo: baseClass ;
      ] ;
] ;

#--------------------
#  EntityExtenderFeedSetup
#    feedId, baseClassId, entityType, freq
#--------------------

EntityExtenderFeedSetup setFeedClassTo: EntityExtenderFeed .
   setFieldOrderListTo: "feedId", "baseClassId", 
         "baseEntityId", "frequency" ;

EntityExtenderFeedSetup
   defineFixedProperty: 'baseEntityId' .
   defineFixedProperty: 'frequency' .

   defineFixedProperty: 'baseEntity' .
;

EntityExtenderFeedSetup defineMethod: [ | displayInfo | 
  feedId print ; 
  " (" print ;  ^self feedClassType whatAmI print ; ") " print ;
  " " print ; baseClassId print ; " for " print ; baseEntityId print ;
  "  freq: " print ; frequency printNL ; 
] ;

EntityExtenderFeedSetup defineMethod: [ | initializeFeedAttributes | 
  ^super initializeFeedAttributes ;
  :baseEntity <- ^self locateClass: baseEntityId ;
  :frequency <- frequency else: "" . as: ^global DateOffset ;
] ;

EntityExtenderFeedSetup defineMethod: [ | runUpdate | 
  ^super runUpdate ;
  ^self asSelf instanceList select: [ isProcessed ] .
  do: [ underlyingRecord 
          setBaseClassTo: baseClass .
          setBaseEntityTo: baseEntity .
          setFrequencyTo: frequency ;
      ] ;
] ;

EntityExtenderFeedSetup defineMethod: [ | displayOtherExceptions | 
  !feed <- ^self asSelf ;
  newLine print ;
  !warnings <- ^self asSelf instanceList 
      select: [ underlyingRecord isntNA ] .
      select: [underlyingRecord baseClass isDataRecord ] .
      select: [ underlyingRecord baseClass updateBlock isNA ] ;
  warnings  count > 0
  ifTrue: [ newLine print ; 
            warnings count  print ;
            " Missing Link From Entity to DataRecord." printNL ;
            "feedId" print: 20 ; 
           "Entity Class" print: 20 ; "DataRecord Class" printNL ;
            warnings send: [ underlyingRecord ] .
              sortUp: [ whatAmI ] .
            do: [ whatAmI print: 20 ;
                  baseEntity whatAmI print: 20 ;
                  baseClass whatAmI printNL ;
                ] ;
          ] ;
] ;


#--------------------
#  TransactionFeedSetup
#    feedId, baseClassId
#--------------------
TransactionFeedSetup setFeedClassTo: TransactionFeed ;

#--------------------
#  AliasFeedSetup
#    feedId, baseClassId
#--------------------
AliasFeedSetup setFeedClassTo: AliasFeed ;

AliasFeedSetup defineMethod: [ | getDescription |
   "This feed is used to add aliases to " 
         concat: baseClass whatAmI . concat: " instances."
] ;

AliasFeedSetup defineMethod: [ | runUpdate | 
  ^super runUpdate ;
  ^self asSelf instanceList select: [ isProcessed ] .
  do: [ underlyingRecord setEntityTypeTo: baseClass ;
      ] ;
] ;

#--------------------
#  XRefFeedSetup
#    feedId, baseClassId
#--------------------
XRefFeedSetup setFeedClassTo: XRefFeed ;

XRefFeedSetup defineMethod: [ | getDescription |
   "This feed is used to add cross reference ids to " 
         concat: baseClass whatAmI . concat: " instances."
] ;

XRefFeedSetup defineMethod: [ | runUpdate | 
  ^super runUpdate ;
  ^self asSelf instanceList select: [ isProcessed ] .
  do: [ underlyingRecord setEntityTypeTo: baseClass ;
      ] ;
] ;



#--------------------
#  MembershipFeedSetup
#    feedId, groupId, groupPath, memberId, memberPath, mode
#--------------------
MembershipFeedSetup setFeedClassTo: MembershipFeed .
   setFieldOrderListTo: "feedId", 
        "groupId", "groupPath", "memberId", "memberPath", "mode" ;

MembershipFeedSetup
   defineFixedProperty: 'groupId' .
   defineFixedProperty: 'groupPath' .
   defineFixedProperty: 'memberId' .
   defineFixedProperty: 'memberPath' .
   defineFixedProperty: 'mode' .

   defineFixedProperty: 'groupType' .
   defineFixedProperty: 'memberType' .
;

MembershipFeedSetup defineMethod: [ | displayInfo | 
  feedId print ; 
  " (" print ;  ^self feedClassType whatAmI print ; ") " print ;
  groupId print ; " " print ; groupPath print ; " " print ;
  memberId print ; " " print ; memberPath print ; " " print ; 
  mode printNL ;
] ;

MembershipFeedSetup defineMethod: [ | initializeFeedAttributes | 
  :underlyingRecord <- ^self locateClass: feedId ;
  :groupType <- ^self locateClass: groupId ;
  :memberType <- ^self locateClass: memberId ;
  :mode <- mode else: "" . toUpper ;
] ;

MembershipFeedSetup defineMethod: [ | okayToProcess | 
  groupType isntNA && groupPath isntDefault && memberType isntNA 
] ;

MembershipFeedSetup defineMethod: [ | getDescription |
   "This feed is used to update the relationship between "
      concat: groupType whatAmI . concat: " and " . 
      concat: memberType whatAmI . concat: "."
] ;

MembershipFeedSetup defineMethod: [ | runUpdate | 
  ^super runUpdate ;
  ^self asSelf instanceList select: [ isProcessed ] .
  do: [ underlyingRecord 
          setGroupTypeTo: groupType withAccessPath: groupPath .
          setMemberTypeTo: memberType withAccessPath: memberPath ;
        mode contains: "APP" . 
           ifTrue: [ underlyingRecord setUpdateModeToAppend ] ;
        mode contains: "REP" . 
           ifTrue: [ underlyingRecord setUpdateModeToReplace ] ;
      ] ;
] ;

MembershipFeedSetup defineMethod: [ | displayNewOnes | 
  !feed <- ^self asSelf ;
  !newOnes <- feed instanceList select: [ newOneCreated ] ;
  newOnes count > 0
  ifTrue: [ newLine print ; 
            newOnes count print ; " new feed subclasses created. " printNL;
            newOnes sortUp: [ feedId ] . do: [ displayInfo ] ;
          ] ;
] ;

MembershipFeedSetup defineMethod: [ | displayBadOnes | 
  !feed <- ^self asSelf ;
  !badOnes <- ^self asSelf instanceList 
      select: [ isntProcessed && isntDefault ] ;
  badOnes count > 0
  ifTrue: [ newLine print ; 
            badOnes count print ; " not processed. " printNL;
            "(must supply groupId, groupPath, and memberId)" 
                     centerNL: 80 . print ;
            badOnes sortUp: [ feedId ] .
            do: [ displayInfo ;
                  "     ---> (" print ;
                  groupType isNA ifTrue: [ "   no group " print ] ;
                  groupPath isNA ifTrue: [ "   no group path " print ] ;
                  memberType isNA ifTrue: [ "  no member " print ] ;
                  " ) " printNL ;
                ] ;
          ] ;
  !warnings <- ^self asSelf instanceList 
      select: [ underlyingRecord isntNA ] .
      select: [groupType getMessage: groupPath . isNA ] ;
  warnings  count > 0
  ifTrue: [ newLine print ; 
            warnings count  print ;
            " Group path not defined for group" printNL ;
            "feedId" print: 20 ; "group" print ; "path" printNL ;
            warnings sortUp: [ underlyingRecord whatAmI ] .
            do: [ underlyingRecord whatAmI print: 20 ;
                  groupType whatAmI print: 20 ;
                  groupPath printNL ;
                ] ;
          ] ;
  !warnings <- ^self asSelf instanceList 
      select: [ underlyingRecord isntNA && memberPath isDefault ] .
      select: [memberType getMessage: memberPath . isntNA ] ;
  warnings count > 0
  ifTrue: [ newLine print ; 
            warnings count  print ;
            " Member path not defined for member type" printNL ;
            "feedId" print: 20 ; "member" print ; "path" printNL ;
            warnings sortUp: [ underlyingRecord whatAmI ] .
            do: [ underlyingRecord whatAmI print: 20 ;
                  memberType whatAmI print: 20 ;
                  memberPath printNL ;
                ] ;
          ] ;
] ;

#--------------------
#  RangeGroupFeedSetup
#   feedId, rangeClassId, updateClassId, updatePath, accessRule
#--------------------
RangeGroupFeedSetup setFeedClassTo: RangeGroupFeed .
   setFieldOrderListTo: "feedId", 
        "rangeClassId", "updateClassId", "updatePath", "accessRule" ;

RangeGroupFeedSetup
   defineFixedProperty: 'rangeClassId' .
   defineFixedProperty: 'updateClassId' .
   defineFixedProperty: 'updatePath' .
   defineFixedProperty: 'accessRule' .

   defineFixedProperty: 'rangeClass' .
   defineFixedProperty: 'updateClass' .
;

RangeGroupFeedSetup defineMethod: [ | displayInfo | 
  feedId print ; 
  " (" print ;  ^self feedClassType whatAmI print ; ") " print ;
  rangeClassId print ; " " print ; updateClassId print ; " " print ;
  updatePath print ; " " print ; accessRule printNL ;
] ;

RangeGroupFeedSetup defineMethod: [ | getDescription |
   "This feed is used to update "
      concat: updateClass whatAmI . concat: " " . concat: updatePath .
      concat: " with " . concat: rangeClass whatAmI . concat: "s." 
] ;

RangeGroupFeedSetup defineMethod: [ | initializeFeedAttributes | 
  :underlyingRecord <- ^self locateClass: feedId ;
  :rangeClass <- ^self locateClass: rangeClassId ;
  :updateClass <- ^self locateClass: updateClassId ;
] ;

RangeGroupFeedSetup defineMethod: [ | okayToProcess | 
  rangeClass isntNA && updateClass isntNA && 
  updatePath isntNA && accessRule isntNA
] ;

RangeGroupFeedSetup defineMethod: [ | runUpdate | 
  ^super runUpdate ;
  ^self asSelf instanceList select: [ isProcessed ] .
  do: [ underlyingRecord 
          setRangeClassTo: rangeClass .
          setBaseClassTo: updateClass .
          setUpdatePropertyTo: updatePath .
          setAccessRuleTo: accessRule ;
      ] ;
] ;

RangeGroupFeedSetup defineMethod: [ | displayNewOnes | 
  !feed <- ^self asSelf ;
  !newOnes <- feed instanceList select: [ newOneCreated ] ;
  newOnes count > 0
  ifTrue: [ newLine print ; 
            newOnes count print ; " new feed subclasses created. " printNL;
            newOnes sortUp: [ feedId ] . do: [ displayInfo ] ;
          ] ;
] ;

RangeGroupFeedSetup defineMethod: [ | displayBadOnes | 
  !feed <- ^self asSelf ;

  !badOnes <- ^self asSelf instanceList 
      select: [ isntProcessed && isntDefault ] ;
  badOnes count > 0
  ifTrue: [ newLine print ; 
            badOnes count print ; " not processed. " printNL;
            "(must supply rangeClass, updateClass, updatePath, accessRule)"
                     centerNL: 80 . print ;
            badOnes sortUp: [ feedId ] .
            do: [ displayInfo ;
                  "     ---> (" print ;
                  rangeClass isNA ifTrue: [ "   no range class " print ] ;
                  updateClass isNA ifTrue: [ "   no update class " print ] ;
                  updatePath isNA ifTrue: [ "   no update path " print ] ;
                  accessRule isNA ifTrue: [ "  no access rule " print ] ;
                  " ) " printNL ;
                ] ;
          ] ;
  !warnings <- ^self asSelf instanceList 
      select: [ underlyingRecord isntNA ] .
      select: [ updateClass getMessage: updatePath . isNA ] ;
  warnings  count > 0
  ifTrue: [ newLine print ; 
            warnings count  print ;
            " Update path not defined." printNL ;
            "feedId" print: 20 ; "updateClass" print ; "path" printNL ;
            warnings sortUp: [ underlyingRecord whatAmI ] .
            do: [ whatAmI print: 20 ;
                  updateClass whatAmI print: 20 ;
                  updatePath printNL ;
                ] ;
          ] ;
] ;


#--------------------
#  XRefFeedSetup
#    feedId, parentId, baseClassId, description
#--------------------
XRefFeedSetup setFeedClassTo: XRefFeed ;



#======================================================================

####################
#  Master Feeds
#     These feeds are used to create and refresh entity instances
#  Subclass Globals:
#     - entityType 
####################

MasterFeed
   defineFixedProperty: 'terminateFlag' .

   defineFixedProperty: 'flaggedAsDuplicate' .
   defineFixedProperty: 'newOneCreated' .
;

MasterFeed defineMethod: [ | hasStatusInfo | 
  newOneCreated isTrue || 
    [ isProcessed &&
        (underlyingCurrency isDefault && [currencyId isntDefault]
            && [ currencyId != "Default" ] 
        )
    ]
] ;

MasterFeed defineMethod: [ | rejectReason | 
   ^self flaggedAsDuplicate isNA
      ifTrue: [ "Bad entityId supplied - " concat: ^self entityId ] 
     ifFalse: [ "Duplicate Feed Record for - " 
                   concat: ^self underlyingRecord code . concat: " " .
                   concat: ^self underlyingRecord name
              ] 
] ;

#----------
#  display
#----------

MasterFeed defineMethod: [ | getUploadProperties | 
  ^super getUploadProperties select: [ code != "date" ] 
] ;

MasterFeed defineMethod: [ | displayFeedConstants | 
!feed <- ^self ;
^super displayFeedConstants ;
newLine print ;
"entityType: " print: 20 ; feed entityType whatAmI printNL ;
] ;

MasterFeed defineMethod: [ | displayInfo | 
  ^self entityType whatAmI print ; " " print ; 
  entityId print ; " " print ; 
  currencyId print ; " " print ; 
  terminateFlag isntDefault
     ifTrue: [ " terminate - " print ; terminateFlag print ] ;
  newLine print ;
] ;

#----------
#  subclass constants
#----------

MasterFeed defineMethod: [ | setEntityTypeTo: etype |
   ^self define: 'entityType' toBe: etype ;
   ^self
] ;

#----------
#  processing support
#----------
MasterFeed defineMethod: [ | enableEntityCreation | 
  ^self define: 'allowEntityCreation' toBe: TRUE ;
  ^self
] ;

MasterFeed defineMethod: [ | disableEntityCreation | 
  ^self define: 'allowEntityCreation' toBe: FALSE ;
  ^self
] ;

MasterFeed defineMethod: [ | enableInternalIds | 
  ^self define: 'autoCreateIds' toBe: TRUE ;
  ^global Named IdSource Internal createXRefFor: ^self entityType ;
  #--  flag as "Data" so it can be seen via navigator
  ^self entityType getMessage: "internalId" .
     setFunctionTypeTo: "Data" ;
  ^self entityType define: 'lastInstanceAssigned' toBe: 0 ;
  ^self
] ;

MasterFeed defineMethod: [ | disableInternalIds | 
  ^self define: 'autoCreateIds' toBe: NA ;
  ^self
] ;

MasterFeed defineMethod: [ | getUpdateClass | 
   ^self entityType
] ;

#----------
#  processing
#----------

MasterFeed defineMethod: [ | initializeProcessing | 
  ^self asSelf instanceList
  do: [ :underlyingRecord <- entityType locateId: entityId ;
      ] ;

^self allowEntityCreation
ifTrue:
  [  ^self asSelf instanceList
       select: [ underlyingRecord isNA ] .
       groupedByString: [ entityId ] .      #- only create 1 per unique id
       select: [ isntDefault ] .
     do: [ !record <- groupList at: 1 ;
           !newEntity <- record entityType 
                 createInstance: record entityId copyString ;
           record :newOneCreated <- TRUE ;
           groupList do: [ :underlyingRecord <- ^my newEntity ] ;
         ] ;
   ] ;
] ;

MasterFeed defineMethod: [ | runUpdate | 
!feed <- ^self asSelf ;
!excludeList <-  feed keyList append: feed localPropertyList ;
!updateBlocks <- feed updateBlockList numberElements 
    select: [ isntNA ] .
    select: [ ^my excludeList excludesElement: position ] .
  extendBy: 
    [ !message <- ^my feed fieldOrderList at: position ;
      !mid <- ^my feed entityType getMessage: message ;
      !dataType <- mid defaultDataType ;
    ] ;

feed purgeRecords ;      #- flag for deletion if terminateFlag isntDefault

#---  check for duplicates
feed instanceList select: [ underlyingRecord isntDefault ] .
   groupedBy: [ underlyingRecord ] .
   select: [ groupList count > 1 ] .
   do: [ !dup <- groupList at: 1 ;
         groupList do: [ :flaggedAsDuplicate <- ^my dup ] ;
       ] ;

feed instanceList select: [ underlyingRecord isntDefault ] .
    select: [ flaggedAsDuplicate isNA ] .
do: [ :underlyingCurrency <- currencyId asCurrency ;
      underlyingCurrency isntDefault 
         ifTrue: [ underlyingRecord setBaseCurrencyTo: underlyingCurrency ] ;
      ^self processMiscItemsUsing: ^my updateBlocks ;
      :isProcessed <- TRUE ;
    ] ;

] ;

MasterFeed defineMethod: [ | processMiscItemsUsing: updateBlocks |
!record <- ^self asSelf ;
!entity <- record underlyingRecord ;
updateBlocks
do: [ !currentValue <- ^my record valueList at: position ;
      !convertValue <- dataType isString
          ifTrue: [ currentValue isntNA 
                     ifTrue: [ ^my entity getClusteredString: currentValue ] 
                  ] .
           elseIf: [ currentValue isntNA || dataType isNumber not ] 
             then: [ currentValue as: dataType ] .
             else: [ NA ] ;
      !newValue <- convertValue isntNA || dataType isntNA
         ifTrue: [ convertValue ] 
        ifFalse:
          [ currentValue isntNA
               ifTrue: [ ^my entity getClusteredString: currentValue ]
          ] ;
      mid type isTimeSeriesProperty
       ifTrue: 
         [ ^my entity send: ^self . updateWith: newValue ;
         ]
       ifFalse: 
         [ ^my entity setProperty: ^self to: newValue ;
         ] ;
     ] ;
] ;

MasterFeed defineMethod: [ | displayBadOnes | 
  !badOnes <- ^self asSelf masterList select: [ isntProcessed ] .
      select: [ underlyingRecord isDefault ] ;
  badOnes count > 0
  ifTrue: [ newLine print ;
            ^self allowEntityCreation
               ifTrue: [ "Bad entityId supplied." print ] 
              ifFalse: [ ">>> Entity Creation Disabled." print ] ;
            badOnes count print ; " not processed.  " printNL ;
            badOnes do: [ displayInfo ] ;
          ] ;
  !dups <- ^self asSelf masterList select: [ flaggedAsDuplicate isntNA ] .
      groupedBy: [ flaggedAsDuplicate ] ;
  dups count > 0
  ifTrue: 
    [ newLine print ;
      "---> Two or more feed records for the same entity: " printNL ;
      dups do: [ newLine print ;
                 "Entity: " print ; underlyingRecord displayInfo ;
                 groupList
                 do: [ "   Feed: " print ; 
                       getValueList do: [ print ; " | " print ; ] ;
                       newLine print ;
                     ] ;
               ] ;
    ] ;
] ;

MasterFeed defineMethod: [ | displayNewOnes | 
  !newOnes <- ^self asSelf instanceList select: [ newOneCreated ] ;
  newOnes count > 0
  ifTrue: [ newLine print ; 
            newOnes count print ; " new " print ;
            ^self entityType whatAmI print ; " instances created. " printNL ;
            newOnes 
            do: [ underlyingRecord code print: 20 ;
                  underlyingRecord name print: 45 ;
                  flaggedAsDuplicate isntNA
                    ifTrue: [ " (duplicates found - not refreshed)" print ] ;
                  newLine print ;
                ] ;
          ] ;
] ;


MasterFeed defineMethod: [ | displayOtherExceptions |
  !deletes <- ^self asSelf instanceList
      select: [ terminateFlag isntDefault ] .
      select: [ terminateFlag else: "" . toUpper != "UNDELETE" ] ;
  deletes count > 0
  ifTrue: [ newLine print ; 
            deletes count print ; " " print ; 
            ^self entityType whatAmI print ; " terminated. " printNL ;
            deletes
            do: [ terminateFlag print ; " -- " print ; 
                  underlyingRecord displayInfo ;
                ] ;
          ] ;
  !noCurrency <- ^self asSelf instanceList select: [ isProcessed ] .
      select: [ currencyId != "Default" ] .
      select: [ underlyingCurrency isDefault && currencyId isntDefault ] ;
  noCurrency count > 0
  ifTrue: [ newLine print ; 
            noCurrency count print ; 
            " records had invalid currencyId values. " printNL ;
            newLine print ;
            "Supplied Id" print: 15 ; "Entity" print: 10 ; " " print: 25 ;
            "Current Currency" printNL ;
            noCurrency sortUp: [ entityId ] . sortUp: [ currencyId ] .
            do: [ currencyId print: 15 ; underlyingRecord code print: 10 ;
                  underlyingRecord name print: 25 ; 
                  underlyingRecord baseCurrency displayInfo ;
                ] ;
          ] ;
] ;

MasterFeed defineMethod: [ | runWrapup | 
  ^super runWrapup ; 
  ^self autoCreateIds isTrue
     ifTrue: [ ^self createInternalIds ] ;
] ;

MasterFeed defineMethod: [ | createInternalIds |
  "...  Creating Internal Ids  -- " print ; 
  !todo <- ^self entityType masterList select: [ internalId isNA ] ;
  todo numberElements
  do: [ !id <- ( lastInstanceAssigned + position ) asInteger asString ;
        ^global Named IdSource Internal addSymbol: id toEntity: asSelf ;
      ] ;
  ^self entityType 
       define: 'lastInstanceAssigned' 
         toBe: (^self entityType lastInstanceAssigned + todo count) asInteger ;
  todo count print ; " created." printNL ;
] ;

MasterFeed defineMethod: [ | resetRunTimeAttributes | 
   ^super resetRunTimeAttributes 
     enableEntityCreation
] ;

#--------------------
#  MasterFeed: methods for purge support
#     You cannot purge the entity instances associated with a master feed
#     but you can flag them for deletion.  For master feeds, this is the
#     same as updating the terminateFlag field.  To undelete a flagged
#     instance, update the terminateFlag field with the value UNDELETE.
#--------------------

MasterFeed defineMethod: [ | purgeRecords | 
!feed <- ^self asSelf currentStore ;
!todo <- feed getRecordsToPurge ;
todo
do: [ terminateFlag else: "" . toUpper = "UNDELETE"
      ifTrue:
        [ underlyingRecord :deletionFlag <- NA ;
          underlyingRecord :deletionDate <- NA ;
          underlyingRecord :deletionReason <- NA ;
        ] 
      ifFalse:
        [ !date <- terminateFlag asDate ;
          underlyingRecord :deletionFlag <- TRUE ;
          date isNA 
          ifTrue:
            [ underlyingRecord :deletionDate <- ^date ;
              terminateFlag isntDefault
              ifTrue: 
                [ underlyingRecord :deletionReason <- 
                     terminateFlag copyString ;
                ] ;
            ]
          ifFalse: [ underlyingRecord :deletionDate <- date ]  ;
        ] ;
    ] ;
feed purgeEnabled ifTrue: [ feed displayPurgeReport ] ;
] ;

MasterFeed defineMethod: [ | getRecordsToPurge |
!feed <- ^self asSelf ;
feed purgeEnabled
ifTrue: 
  [ feed masterList
    do: [ :underlyingRecord <- entityType named at: entityId ; ] .
    select: [ underlyingRecord isntDefault ] .
    do: [ :terminateFlag <- "Purge Enabled" ] 
  ] 
ifFalse:
  [ feed instanceList 
      select: [ underlyingRecord isntDefault && terminateFlag isntDefault ]
  ] 
] ;

MasterFeed defineMethod: [ | displayPurgeReport | 
!feed <- ^self currentStore ;
!notdone <- feed masterList select: [ underlyingRecord isDefault ] ;
!done <- feed instanceList select: [ underlyingRecord isntDefault ] ;

newLine print ;
[ "---  " print ; feed entityType whatAmI print ; 
  " Instances Flagged for Deletion ---" print ;
] divertOutput centerNL: 80 . printNL ;
"Upload Feed:       " print ; feed whatAmI printNL ;
"Total Records:     " print ; feed masterList count printNL ;
"Not Processed:     " print ; notdone count printNL ; 
"Instances Flagged: " print ; done count printNL ;

notdone count > 0 
ifTrue:
  [ newLine print ; 
    notdone count print ; " " print ; 
    feed entityType whatAmI print ; " instances not found. " printNL ;
    notdone sortUp: [ entityId ] .
    do: [ entityId printNL ] ;
  ] ;
done count > 0 
ifTrue:
  [ newLine print ; 
    done count print ; " " print ; 
    feed entityType whatAmI print ; " terminated. " printNL ;
    done
    do: [ terminateFlag print ; " -- " print ; 
          underlyingRecord displayInfo ;
        ] ;
   ] ;

] ;


#--------------------------------------------------

#--------------------
#  CurrencyMaster
#--------------------
CurrencyMaster
   defineFixedProperty: 'underlyingCurrency' .
   defineFixedProperty: 'conversion' .
;

CurrencyMaster defineMethod: [ | initializeLocalAttributes |
  ^super initializeLocalAttributes ;
  :conversion <- conversion asNumber asDouble ;
  ^self
] ;

CurrencyMaster defineMethod: [ | runUpdate | 
  ^super runUpdate ;
  ^self instanceList select: [ isProcessed ] .
  do: [ !parentCurr <- underlyingCurrency asCurrency ;
        parentCurr isntDefault && 
        [ parentCurr != underlyingRecord baseCurrency ] &&
        [ conversion isntNA ] 
            ifTrue: [ underlyingRecord 
                         setUnderlyingCurrencyTo: parentCurr
                         withExchange: conversion ;
                    ] ;
      ] ;
] ;

#--------------------------------------------------

#--------------------
#  UniverseMaster
#--------------------
UniverseMaster
   defineFixedProperty: 'memberType' .
;

UniverseMaster defineMethod: [ | runUpdate | 
  ^super runUpdate ;
  ^self instanceList select: [ isProcessed ] .
  do: [ !type <- memberType else: "" . asClass ;
        type isEntity
          ifTrue: [ underlyingRecord setEntityTypeTo: type ] ;
      ] ;
] ;



#======================================================================

####################
#  EntityExtender Feeds
#     These feeds are used to create and refresh supplemental
#      data for an entity or it's extended data record classes
#  Subclass Globals:
#     - baseClass
#     - baseEntity
#     - frequency
####################

EntityExtenderFeed
   defineFixedProperty: 'newOneCreated' .
   defineFixedProperty: 'newBridgeCreated' .
   defineFixedProperty: 'evaluationDate' .
;

#----------
#  display
#----------

EntityExtenderFeed defineMethod: [ | displayFeedConstants |
  !feed <- ^self ;
  ^super displayFeedConstants ;
  newLine print ;
  "baseClass: " print: 20 ; feed baseClass whatAmI printNL ;
  feed baseClass isDataRecord
  ifTrue:
    [ "baseEntity: " print: 20 ; feed baseEntity whatAmI printNL ;
      "accessPath: " print: 20 ; feed baseClass updateBlock printNL ;
    ] ;
  "frequency: " print: 20 ; feed frequency printNL ;

] ;

EntityExtenderFeed defineMethod: [ | displayInfo | 
   ^self baseClass whatAmI print ; 
   " for " print ; ^self baseEntity whatAmI print ;
   " freq: " print ; ^self frequency printNL ; 
   " " print: 5 ;
   entityId print ; " " print ; date print ; " " print ; currencyId printNL ;
] ;

EntityExtenderFeed defineMethod: [ | rejectReason | 
    "Bad entityId supplied - " concat: ^self entityId 
] ;

#----------
#  subclass constants
#----------

EntityExtenderFeed defineMethod: [ | setBaseClassTo: bclass |
   ^self define: 'baseClass' toBe: bclass ;
   ^self
] ;

EntityExtenderFeed defineMethod: [ | setBaseEntityTo: entity |
   ^self define: 'baseEntity' toBe: entity ;
   ^self
] ;

EntityExtenderFeed
  defineMethod: [ | setFrequencyTo: f |
   ^self define: 'frequency' toBe: f ;
   ^self
] ;

#----------

#----------
#  run time attributes
#----------
EntityExtenderFeed defineMethod: [ | getAdjustmentDate | 
   ^self fieldOrderList any: [ asSelf = "adjustmentDate" ] .
      ifFalse: [ ^self getGlobalOption: "adjustmentDate" . asDate ]
] ;

EntityExtenderFeed defineMethod: [ | enableOnlyUpdateOnChange | 
  ^self define: 'onlyUpdateOnChangeFlag' toBe: TRUE ;
  ^self
] ;

EntityExtenderFeed defineMethod: [ | disableOnlyUpdateOnChange | 
  ^self define: 'onlyUpdateOnChangeFlag' toBe: NA ;
  ^self
] ;

EntityExtenderFeed defineMethod: [ | resetRunTimeAttributes | 
   ^super resetRunTimeAttributes 
   disableOnlyUpdateOnChange ;
  ^self
] ;

EntityExtenderFeed disableOnlyUpdateOnChange ;

#----------
#  processing support
#----------

EntityExtenderFeed defineMethod: [ | getUpdateClass | 
   ^self baseClass
] ;

EntityExtenderFeed defineMethod: [ | updatePathOkay | 
  ^self baseClass isDataRecord && [ ^self baseClass updateBlock isntNA ] 
    ifTrue: 
      [ !accessMessage <- ^self baseClass updateBlock 
            asUndelimitedString stripChar: ": " ;
        ^self baseEntity respondsTo: accessMessage .
      ] . 
    else: [ FALSE ] 
] ;

EntityExtenderFeed defineMethod: [ | entityUpdateBlock |
  ^self baseClass isDataRecord && [ ^self updatePathOkay ] 
     ifTrue: [ ^self baseClass updateBlock ] . 
       else: [ [ asSelf ] ] 
] ;

EntityExtenderFeed defineMethod: [ | recordIsTS |
  ^self baseEntity isEntityOrBridge && [ ^self updatePathOkay ] 
  ifTrue:
     [ ^self baseEntity send: ^self entityUpdateBlock . isTimeSeries ]
] ;

EntityExtenderFeed defineMethod: [ | getEvaluationDate | 
   ^self date else: [ ^self asofDate ] . else: ^date
] ;

#----------
#  processing
#----------

EntityExtenderFeed defineMethod: [ | initializeProcessing | 
#-- if baseClass isEntityOrBridge, data is stored in baseClass properties
#-- otherwise, baseClass updateBlock is used to update; if it returns
#-- if property is t/s then make sure record is present for the date;
#-- if property is fixed, then make sure record isnt default
#-- if baseEntity is a Bridge, make sure instance isnt default
#--
#-- NOTE: this will only work with bridges that point to entities;
#--       bridges that point to bridges will not work with these methods
#--
  !entityOrBridgeClass <- ^self baseEntity else: [ ^self baseClass ] ;
  !entityClass <- entityOrBridgeClass isEntity
     ifTrue: [ entityOrBridgeClass ] .
     elseIf: [ entityOrBridgeClass baseClass isEntity ]
       then: [ entityOrBridgeClass baseClass ] ;

  entityClass isEntity
  ifTrue:
    [
    !dataClass <- ^self baseClass ;
    !updateBlock <- ^self entityUpdateBlock ;
    !isTS <- ^self baseClass isDataRecord ifTrue: [ ^self recordIsTS ] ;
       
    ^self asSelf instanceList
        do: [ :evaluationDate <- getEvaluationDate ] .
    extendBy: [ !entity <- ^my entityClass locateId: entityId ] .
    groupedBy: [ entity ] .
       select: [ isntDefault ] .
    do: [ !updateBlock <- ^my updateBlock ;
           !dataClass <- ^my dataClass ;
           !entityOrBridge <- ^my entityOrBridgeClass isEntity
              ifTrue: [ asSelf ]
             ifFalse:
               [ !bridge <- asSelf 
                   send: ^my entityOrBridgeClass updateBlock . value ;
                 bridge isDefault
                 ifTrue: [ :bridge <- ^my entityOrBridgeClass 
                              createInstance: asSelf ;
                           groupList at: 1 . :newBridgeCreated <- TRUE ;
                         ] ;
                bridge
              ] ;
            ^my isTS
            ifTrue:
              [ groupList groupedBy: [ evaluationDate ] .
                do: [ !record <- groupList at: 1 ;
                      !data <- ^my entityOrBridge
                         send: ^my updateBlock . on: asSelf ;
                      data isDefault ||
                         [ data defaultInstance != ^my dataClass defaultInstance ] 
                      ifTrue: 
                         [ :data <- ^my dataClass currentStore
                              createInstance: ^my entityOrBridge, asSelf ;
                           record :newOneCreated <- TRUE ;
                         ] ;
                      groupList do: [ :underlyingRecord <- ^my data ] ;
                    ] ;
              ] 
            ifFalse:
              [ !record <- groupList at: 1 ;
                !data <- entityOrBridge send: updateBlock . value ;
                data isDefault || 
                   [ data defaultInstance != dataClass defaultInstance ] 
                ifTrue: 
                  [ :data <- dataClass currentStore 
                       createInstance: entityOrBridge, earliestPossibleDate ;
                     record :newOneCreated <- TRUE ;
                  ] ;
                groupList do: [ :underlyingRecord <- ^my data ] ;
              ] 
            else:
              [ groupList do: [ :underlyingRecord <- ^my entityOrBridge ] ;
              ] ;
        ] ;
    ] ;
]  ;

EntityExtenderFeed defineMethod: [ | runUpdate | 
  !feed <- ^self asSelf ;
  !todo <- feed instanceList select: [ underlyingRecord isntDefault ] ;
  todo count > 0
     ifTrue: [ feed updateDataFor: todo ]  ;
feed printTimeWith: "...  after updateDateFor: " ;
  !expirationCheck <- ^self getGlobalOption: "autoExpire" ;
  expirationCheck isDateOffset && 
     [ feed baseEntity isEntityOrBridge ] && [ feed recordIsTS ] 
      ifTrue: [ ^self expireOldDataUsing: expirationCheck ] ;
] ;

EntityExtenderFeed defineMethod: [ | updateDataFor: recs | 
  !feed <- ^self asSelf ;
  !underlyingClass <- recs at: 1 . 
     isntDefault: [ underlyingRecord ] else: [ NA ] ;
  !excludeList <-  feed keyList append: feed localPropertyList ;
  !updateBlocks <- feed updateBlockList numberElements 
      select: [ isntNA ] .
      select: [ ^my excludeList excludesElement: position ] .
      extendBy: [ !message <- ^my feed fieldOrderList at: position ;
                  !mid <- ^my underlyingClass getMessage: message ;
                  !dataType <- mid isntDefault
                     ifTrue: [ mid defaultDataType ] ;
                ] ;
  !adjDate <- feed getAdjustmentDate ;
  recs
  do: [ evaluationDate evaluate:
        [ ^my updateBlocks
          do: [ !currentValue <- ^my valueList at: position ;
                !convertValue <- dataType isString
                ifTrue: 
                  [ currentValue isntNA 
                    ifTrue: [ ^my underlyingRecord 
                                  getClusteredString: currentValue 
                            ] 
                  ] .
                elseIf: [ currentValue isntNA || dataType isNumber not ] 
                  then: [ currentValue as: dataType ] .
                  else: [ NA ] ;
                !newValue <- convertValue isntNA || dataType isntNA
                ifTrue: [ convertValue ] 
                ifFalse:
                   [ currentValue isntNA 
                        ifTrue: [ ^my underlyingRecord 
                                      getClusteredString: currentValue
                                ] 
                   ] ;
               mid type isTimeSeriesProperty 
                   && [^my onlyUpdateOnChangeFlag isTrue]
               ifTrue: 
                 [ ^my underlyingRecord send: ^self . updateWith: newValue ]
               ifFalse: 
                 [ ^my underlyingRecord setProperty: ^self to: newValue ] ;

              ] ;
          :underlyingCurrency <- currencyId asCurrency ;
          underlyingCurrency isntDefault 
          ifTrue: [ underlyingRecord 
                        setBaseCurrencyTo: underlyingCurrency 
                  ] ;
          ^my adjDate isntNA && underlyingRecord isDataRecord
               ifTrue: [ underlyingRecord setAdjustmentDateTo: ^my adjDate ];
          :isProcessed <- TRUE ;
        ] ;
    ] ;
] ;


EntityExtenderFeed defineMethod: [ | expireOldDataUsing: expirationCheck |
  !feed <- ^self asSelf ;
  !updateBlock <- feed entityUpdateBlock else: [ asSelf ] ;
  !cutoff <- feed getAdjustmentDate else: ^date ;
  !duration <- feed frequency else: expirationCheck ;
  feed baseEntity masterList
  extendBy: [ !ts <- ^self send: ^my updateBlock ;
              !lastPoint <- ts lastObservation ;
            ] .
  select: [ lastPoint isntDefault ] .
  select: [ lastPoint date + ^my expirationCheck <= ^my cutoff ] .
  do: [ ts asOf: lastPoint date + ^my duration
            put: lastPoint defaultInstance ;
      ] ;
] ;

EntityExtenderFeed defineMethod: [ | displayNewOnes | 
  !feed <- ^self ;
  !newOnes <- ^self asSelf instanceList
      select: [ newBridgeCreated || newOneCreated ] ;
  newOnes count > 0
  ifTrue: [ newLine print ; 
            newOnes count print ; " new " print ;
            feed baseClass whatAmI print ; " instances created. " printNL ;
            newOnes
            do: [ entityId print ; " " print ; date else: "" . print ;
                  " -- " print ;
                  newBridgeCreated isTrue
                     ifTrue: [ "  New Bridge created." print ] ;
                  newOneCreated isTrue
                    ifTrue: [ "  New DataRecord created." print ] ;
                  newLine print ;
                ] ;
          ] ;
] ;

EntityExtenderFeed defineMethod: [ | displayBadOnes | 
  !feed <- ^self ;

#-- general entity extenders only work if entity-bridge connection
#-- (i.e., specialized methods needed for entity-bridge-bridge updates)
  !entityOrBridgeClass <- ^self baseEntity else: [ ^self baseClass ] ;
  !entityClass <- entityOrBridgeClass isEntity
     ifTrue: [ entityOrBridgeClass ] .
     elseIf: [ entityOrBridgeClass baseClass isEntity ]
       then: [ entityOrBridgeClass baseClass ] ;
  entityClass isEntity
  ifTrue:
    [ !badOnes <- ^self asSelf instanceList 
         select: [ isntProcessed && entityId isntDefault ] ;
      badOnes count > 0
      ifTrue: [ newLine print ; 
                "Bad entityId supplied. " print ;
                badOnes count print ; " not processed." printNL ;
                badOnes groupedByString: [ entityId ] .
                do: [ ^self print: 20 ;
                      groupList count print ; " records skipped." printNL ;
                    ] ;
              ] ;
    ] 
  ifFalse: [ ">>>  Not Defined for this type of Bridge." printNL  ] ;

] ;

EntityExtenderFeed defineMethod: [ | displayOtherExceptions | 
  !feed <- ^self ;
  !expirationCheck <- ^self getGlobalOption: "autoExpire" ;
  expirationCheck isDateOffset && 
      [ feed baseEntity isEntityOrBridge ] && [ feed recordIsTS ] 
  ifTrue: [ newLine print ;
            "Old ones expired using duration of " print ;
            ^self frequency else: expirationCheck . printNL ;
          ] ;
  !noCurrency <- ^self asSelf instanceList select: [ isProcessed ] .
      select: [ currencyId != "Default" ] .
      select: [ underlyingCurrency isDefault && currencyId isntDefault ] ;
  noCurrency count > 0
  ifTrue: [ newLine print ; 
            noCurrency count print ; 
            " records had invalid currencyId values. " printNL ;
            newLine print ;
            "Supplied Id" print: 15 ; "Entity" print: 10 ; " " print: 25 ;
            "Current Currency" printNL ;
            noCurrency sortUp: [ entityId ] . sortUp: [ currencyId ] .
            do: [ currencyId print: 15 ; date print: 15 ;
                  underlyingRecord entity code print: 10 ;
                  underlyingRecord entity name print: 25 ; 
                  underlyingRecord baseCurrency displayInfo ;
                ] ;
          ] ;
] ;

#--------------------
#  EntityExtenderFeed: new/changed methods for purge support
#--------------------

EntityExtenderFeed defineMethod: [ | purgeRecords | 
!feed <- ^self asSelf currentStore ;

#-- Styles:
#--   1) time series properties at entity - delete date in each t/s
#--   2) time series of data record - delete date; flag record
#--   3) fixed data record - delete t/s point within record
#--      if entity = * , then all entities
#--      if date = *, then all dates ; preserve 1/1/1 value
#--      if date contains :, then range of dates

!todo <- feed getRecordsToPurge 
   extendBy:
   [
   !allDates ; !startDate ; !endDate; !singleDate ;
   date isDate 
      ifTrue: [ :singleDate <- date ] .
      elseIf: [ date = "*" ] then: [ :allDates <- TRUE ] .
      elseIf: [ date contains: ":" ] 
        then: [ !range <- date breakOn: ":" ;
                :startDate <- range at: 1 . asDate ;
                :endDate <- range at: 2 . asDate ;
              ] ;
   !hasDate <- allDates, startDate, endDate, singleDate any: [ isntNA ] ;
   ] .
select: [ entities isntNA && hasDate ] ;

todo do: [ :isProcessed <- TRUE ] ;
feed baseClass isEntityOrBridge
    ifTrue: [ feed purgePointsInEntityPropertiesFor: todo ]
   ifFalse: [ feed purgeEntityDataRecordsFor: todo ] ;

!notdone <- feed masterList select: [ isntProcessed ] ;
notdone count > 0 
ifTrue:
   [ newLine print ; 
     notdone count print ; " " print ; 
     " bad instances or dates - not processed. " printNL ;
     notdone sortUp: [ entityId ] .
     do: [ entityId print: 20 ; date printNL ; ] ;
   ] ;

newLine print ; 
"--->  The following deletes were processed: " printNL ;
todo 
do: [ underlyingRecord isntNA
      ifTrue: [ underlyingRecord code print: 10 ; 
                underlyingRecord name print: 25 ; 
              ]
     ifFalse: [ "  *  " print: 10 ; "(All Instances)" print: 25 ; ] ;
     "  --  " print ; 
      allDates isntNA ifTrue: [ "All Dates" printNL ] .
        elseIf: [ singleDate isDate ] then: [ singleDate printNL ] .
        else: 
         [ startDate else: "... " . print ;
           " through " print ; 
           endDate else: " ..." . printNL ;
        ] ;
    ] ;

] ;

EntityExtenderFeed defineMethod: [ | getRecordsToPurge |

!feed <- ^self ;
!entityOrBridgeClass <- feed baseEntity else: [ feed baseClass ] ;
!entityClass <- entityOrBridgeClass isEntity
   ifTrue: [ entityOrBridgeClass ] .
   elseIf: [ entityOrBridgeClass baseClass isEntity ]
     then: [ entityOrBridgeClass baseClass ] ;

!todo <- ^global List ;
entityClass isEntity
ifTrue:
  [ feed instanceList
        do: [ :underlyingRecord <- ^my entityClass locateId: entityId ] ;
    :todo <- feed instanceList
    extendBy: [ !entities <- entityId = "*" 
                   ifTrue: [ ^my entityOrBridgeClass masterList ] .
                  elseIf: [ underlyingRecord isntNA ] 
                  then:
                  [ ^my entityOrBridgeClass isEntity
                       ifTrue: [ underlyingRecord asList ] .
                       elseIf: [ underlyingRecord isEntity && 
                                 ^my entityOrBridgeClass updateBlock isBlock
                               ] 
                         then: [ underlyingRecord send: 
                                  ^my entityOrBridgeClass updateBlock . value
                                  asList
                               ] 
                  ] ;
             ] ;
   ] ;
todo
] ;

EntityExtenderFeed defineMethod: [ | purgePointsInEntityPropertiesFor: todo |
!feed <- ^self asSelf ;
#--   1) time series properties at entity - delete date in each t/s
#--      must supply 1 or more update properties in header; data is ignored
#--      date rules:
#--      if single date, it is deleted, including 1/1/1
#--      if all dates, all are purged and 1/1/1 reinstalled if non-na
#--      if range, all are purged in range and 1/1/1 reinstalled if non-na

!updateBlocks <- feed updateBlockList numberElements 
   select: [ isntNA ] .
   select: [ ^my feed keyList excludesElement: position ] ;

updateBlocks
   select: [ ^my feed baseClass send: asSelf . isTimeSeries ] .
do: [ !block <- ^self ;
      ^my todo
      do: [ !block <- ^my block ;
            entities
            do: [ !ts <- ^self send: ^my block ;
                  !defaultValue <- ts asOf: earliestPossibleDate ;
                  ^my singleDate isDate
                     ifTrue: [ ts delete: ^my singleDate ] . 
                  elseIf: [ ^my allDates isntNA ] 
                  then: [ ts deleteAllPoints ] .
                  else: [ ts from: ^my startDate to: ^my endDate .
                          do: [ ^my ts delete: ^date ] ;
                        ] ;
                  ^my singleDate isNA && defaultValue isntNA 
                  ifTrue: [ ts asOf: earliestPossibleDate 
                               put: defaultValue ;
                          ] ;
                 ] ;
           ] ;
     ] ;
newLine print ;
[ "---  Deletions in " print ; feed baseClass whatAmI print ; 
  " TimeSeries Properties ---" print ;
] divertOutput centerNL: 80 . printNL ;
feed displayExceptionSummary ; 

!nonTS <- updateBlocks
   select: [ ^my feed baseClass send: asSelf . isTimeSeries not ] ;
nonTS count > 0 
ifTrue:
  [ newLine print ;
    "The following fields are not time series properties:" printNL ;
    nonTS 
      do: [ position print: 5 ; " " print: 5 ; asUndelimitedString printNL ] ;
  ] ;

] ;

EntityExtenderFeed defineMethod: [ | purgeEntityDataRecordsFor: todo | 
!feed <- ^self asSelf currentStore ;
feed recordIsTS 
  ifTrue: [ feed purgeEntityTSDataRecordsFor: todo ] 
 ifFalse: [ feed purgeEntityDataRecordTSFor: todo ] ;
] ;

EntityExtenderFeed defineMethod: [ | purgeEntityTSDataRecordsFor: todo | 
#--   2) time series of data record - delete date; flag record; delete record
#--      if single date, it is deleted, excluding 1/1/1
#--      if all dates, all are purged and 1/1/1 reinstalled 
#--      if range, all are purged in range and 1/1/1 reinstalled

!feed <- ^self asSelf ;
!updateBlock <- feed baseClass updateBlock ; 

todo 
do: [ !block <- ^my updateBlock ;
      entities
      do: [ !ts <- ^self send: ^my block ;
            !first <- ts firstDate else: earliestPossibleDate ;
            !last <- ts lastDate else: earliestPossibleDate ;
            ^my singleDate isDate 
            ifTrue: 
               [ :first <- ^my singleDate ; 
                 :last <- ^my singleDate ;
               ] .
            elseIf: [ ^my allDates isNA ] 
              then: [ ^my startDate isDate && [ ^my startDate > first ] 
                           ifTrue: [ :first <- ^my startDate ] ;
                       ^my endDate isDate && [ ^my endDate < last ] 
                           ifTrue: [ :last <- ^my endDate ] ;
                       last < first ifTrue: [ :last <- first ] ;
                     ] ;
            ts select: [ ^date isntDefault ] . from: first to: last .
            do: [ !rec <- ^my ts on: ^date ;
                  rec isntNA ifTrue: [ ^my ts delete: ^date ] ;
                  rec isntDefault
                     ifTrue: [ rec flagForDeletion; rec rdelete ] ;
                ] ;
          ] ;
     ] ;

newLine print ;
"---  Begin Purges for " concat: feed whatAmI . concat: "  ---" .
    centerNL: 80 . print ;
"(Removing Points in " concat: updateBlock . concat: " TimeSeries) " .
    centerNL: 80 . printNL ;
feed displayExceptionSummary ;

] ;

EntityExtenderFeed defineMethod: [ | purgeEntityDataRecordTSFor: todo | 
#--   3) fixed property of data record - delete points from t/s properties
#--      must supply 1 or more update properties in header; data is ignored
#--      date rules:
#--      if single date, it is deleted, including 1/1/1
#--      if all dates, all are purged and 1/1/1 reinstalled if non-na
#--      if range, all are purged in range and 1/1/1 reinstalled if non-na

!feed <- ^self asSelf ;
!drBlock <- feed baseClass updateBlock ; 
:todo <- todo 
  extendBy: [ !recs <- entities send: ^my drBlock . send: [ value ] ] ;
!updateBlocks <- feed updateBlockList numberElements 
   select: [ isntNA ] .
   select: [ ^my feed keyList excludesElement: position ] ;

updateBlocks
   select: [ ^my feed baseClass send: asSelf . isTimeSeries ] .
do: [ !block <- ^self ;
      ^my todo
      do: [ !block <- ^my block ;
            recs 
            do: [ !ts <- ^self send: ^my block ;
                  !defaultValue <- ts asOf: earliestPossibleDate ;
                  ^my singleDate isDate
                     ifTrue: [ ts delete: ^my singleDate ] . 
                  elseIf: [ ^my allDates isntNA ] 
                  then: [ ts deleteAllPoints ] .
                  else: [ ts from: ^my startDate to: ^my endDate .
                          do: [ ^my ts delete: ^date ] ;
                        ] ;
                  ^my singleDate isNA && defaultValue isntNA 
                  ifTrue: [ ts asOf: earliestPossibleDate 
                               put: defaultValue ;
                          ] ;
                 ] ;
           ] ;
     ] ;

newLine print ;
"---  Begin Purges for " concat: feed whatAmI . concat: "  ---" .
    centerNL: 80 . print ;
"(Removing TimeSeries Points in DataRecord: " concat: feed baseClass whatAmI .
    concat: " )" . centerNL: 80 . printNL ;
feed displayExceptionSummary ;
!nonTS <- updateBlocks
   select: [ ^my feed baseClass send: asSelf . isTimeSeries not ] ;
nonTS count > 0 
ifTrue:
  [ newLine print ;
    "The following fields are not time series properties:" printNL ;
    nonTS 
      do: [ position print: 5 ; " " print: 5 ; asUndelimitedString printNL ] ;
    newLine print ;
  ] ;

] ;





#======================================================================

####################
#  Transaction Feeds
#     These feeds are used to create and refresh LinkRecords
#     which reference multiple entities; results are usually stored
#     as lists at one or more of the key classes
####################




#======================================================================

####################
#  MembershipFeeds
#     These feeds are used to specify lists of objects of one class
#     that are associated with a single instance of another class
#     such as Industry, Country, or Universe
#  Subclass Globals:
#     - groupType
#     - groupAccessPath
#     - memberType
#     - memberAccessPath
#     - updateMode
####################

####################
#  Bridge redirections enable data associated with
#  the group and/or member side of a relationship to be stored in
#  a LocalEntity bridge associated with the group or member.  For
#  example, you want to track a relationship between company and
#  industry privately.  You create the bridge classes MyCompany and
#  MyIndustry in your private space and define the properties
#  industry and companyList at these classes to track the relationship.
#
#  This setup requires some manual steps that are not packaged via
#  the Interface class:
#
#--  Manually define the indexed list at the bridge class
#--  use 'define:' or 'defineFixedProperty:' as appropriate
#
#         MyCompany define: 'industry' withDefault: Industry ;
#         MyIndustry define: 'companyList' withDefault: IndexedList new ;
#
#--  Create the Membership Feed directly with an access path that uses
#--  the bridge links ;  
#--  NOTE: Ignore the warning that the group and access path are not set up
#
#       MembershipFeedSetup updateFromString: "
#           feedId | groupId | groupPath | memberId | memberPath
#           MyCompanyToIndustry | Industry | companyList | Company | industry
#       " ;
#
#-- Explicitly set the update block to use the bridge
#-- Note - assume the bridges have already been setup
#        MyCompanyToIndustry setGroupBridgePathTo: "myIndustry" ;
#        MyCompanyToIndustry setMemberBridgePathTo: "myCompany" ;
#
####################

MembershipFeed
   defineFixedProperty: 'groupId' .
   defineFixedProperty: 'memberId' .

   defineFixedProperty: 'group' .
   defineFixedProperty: 'member' .

   defineFixedProperty: 'membershipChange' .
;

#----------
#  display
#----------

MembershipFeed defineMethod: [ | displayFeedConstants |
  !feed <- ^self ;
  ^super displayFeedConstants ;
  newLine print ;
  "groupType: " print: 20 ; feed groupType whatAmI print ;
  "   via: " print: 20 ; feed groupAccessPath printNL ;
  "memberType: " print: 20 ; feed memberType whatAmI print ; 
  "   via: " print: 20 ; feed memberAccessPath printNL ;
  "updateMode: " print;  feed updateMode printNL ;
] ;

MembershipFeed defineMethod: [ | getUploadProperties | 
  !feed <- ^self ;
  ("groupId", "memberId", "date" ) 
    send: [ ^my feed getMessage: asSelf ] . 
    extendBy: 
      [ !triggerKey <- 
           code = "groupId" ifTrue: [ 1 ] .
           elseIf: [ code = "date" ] then: [ 2 ] .
           elseIf: [ code = "memberId" && ^my feed updateMode != "Replace" ]
           then: [ 3 ] ;
      ] .

] ;

MembershipFeed defineMethod: [ | displayInfo | 
  groupId print ; " " print ; memberId print ; " " print ; date printNL ;
] ;

MembershipFeed defineMethod: [ | rejectReason | 
   !message <-  group isNA 
      ifTrue: [ "Bad groupId " concat: groupId . concat: "; " ] 
     ifFalse: [ "" ] ;
   member isNA
   ifTrue: [ :message <- message concat: " Bad memberId " . 
                concat: memberId . concat: "; " ;
           ] ;
   message
] ;

#----------
#  subclass constants
#----------

MembershipFeed defineMethod: [ | setGroupTypeTo: type withAccessPath: path |
   ^self define: 'groupType' toBe: type ;
   ^self define: 'groupAccessPath' toBe: path ;
   ^self
] ;

MembershipFeed defineMethod: [ | setMemberTypeTo: type withAccessPath: path |
   ^self define: 'memberType' toBe: type ;
   ^self define: 'memberAccessPath' toBe: path ;
   ^self
] ;

MembershipFeed defineMethod: [ | setMemberTypeTo: mtype | 
  ^self setMemberTypeTo: mtype withAccessPath: NA
] ;

MembershipFeed defineMethod: [ | setUpdateModeToReplace |
  ^self define: 'updateMode' toBe: "Replace" ;
  ^self
] ;

MembershipFeed defineMethod: [ | setUpdateModeToAppend |
  ^self define: 'updateMode' toBe: "Append" ;
  ^self
] ;

MembershipFeed defineMethod: [ | setGroupBridgePathTo: bpath |
  ^self define: 'groupBridgePath' toBe: bpath ;
  ^self
] ;

MembershipFeed defineMethod: [ | setMemberBridgePathTo: bpath |
  ^self define: 'memberBridgePath' toBe: bpath ;
  ^self
] ;


#----------

#----------
#  run time attributes
#----------

#----------
#  processing support
#----------

MembershipFeed defineMethod: [ | groupAccessBlock | 
  ^self groupAccessPath isntNA 
   ifTrue:
      [ ^self groupBridgePath isntNA
           ifTrue: [ ^self groupBridgePath concat: " " ]
          ifFalse: [ "" ] . concat: ^self groupAccessPath . asBlock
      ]
] ;

MembershipFeed defineMethod: [ | groupUpdateBlock | 
  ^self groupAccessPath isntNA 
   ifTrue:
      [ ^self groupBridgePath isntNA
           ifTrue: [ ^self groupBridgePath concat: " :" ]
          ifFalse: [ ":" ] . concat: ^self groupAccessPath . asBlock
      ]
] ;

MembershipFeed defineMethod: [ | memberAccessBlock | 
  ^self memberAccessPath isntNA 
   ifTrue:
      [ ^self memberBridgePath isntNA
           ifTrue: [ ^self memberBridgePath concat: " " ]
          ifFalse: [ "" ] . concat: ^self memberAccessPath . asBlock
      ]
] ;

MembershipFeed defineMethod: [ | memberUpdateBlock | 
  ^self groupAccessPath isntNA 
   ifTrue:
      [ ^self memberBridgePath isntNA
           ifTrue: [ ^self memberBridgePath concat: " :" ]
          ifFalse: [ ":" ] . concat: ^self memberAccessPath . asBlock
      ]
] ;

MembershipFeed defineMethod: [ | evaluationDate | 
   ^self date else: [ ^self asofDate ] 
] ;

MembershipFeed defineMethod: [ | trackedAtMember | 
  ^self memberAccessBlock isntNA 
] ;

#----------
#  processing
#----------

MembershipFeed defineMethod: [ | initializeProcessing | 
  ^self asSelf instanceList
  do: [ :group <- ^self groupType locateId: groupId ;
        :member <- ^self memberType locateId: memberId ;
      ] ;
  ^self groupBridgePath isntNA
      ifTrue: [ ^self createMissingGroupBridges ] ;
  ^self memberBridgePath isntNA
      ifTrue: [ ^self createMissingMemberBridges ] ;
]  ;

MembershipFeed defineMethod: [ | createMissingGroupBridges |
!path <- ^self groupBridgePath asBlock ;
!bridgeClass <- ^self groupType send: path ;

!todo <-  ^self asSelf instanceList select: [ group isntNA ] .
   groupedBy: [ group ] . send: [ groupList at: 1 ] .
   extendBy: [ !bridge <- group send: ^my path  ] .
   select: [ bridge isDefault ] ;

todo do: [ ^my bridgeClass createInstance: group ] ;

] ;

MembershipFeed defineMethod: [ | createMissingMemberBridges |
!path <- ^self memberBridgePath asBlock ;
!bridgeClass <- ^self memberType send: path ;

!todo <-  ^self asSelf instanceList select: [ member isntNA ] .
   groupedBy: [ member ] . send: [ groupList at: 1 ] .
   extendBy: [ !bridge <- member send: ^my path  ] .
   select: [ bridge isDefault ] ;

todo do: [ ^my bridgeClass createInstance: member ] ;

] ;

MembershipFeed defineMethod: [ | runUpdate | 
  !feed <- ^self asSelf ;
  !groupBlock <- feed groupUpdateBlock ;

  !list <- feed instanceList
     select: [ member isntNA && group isntNA ] .
     extendBy: [ !updateDate <- evaluationDate ;
               ] ;
  feed trackedAtMember
     ifTrue: [ feed updateMembersUsing: list ] 
    ifFalse: [ feed updateGroupsUsing: list ] ;

  list do: [ :isProcessed <- TRUE ] ;
] ;

MembershipFeed defineMethod: [ | updateMembersUsing: list | 
  !feed <- ^self asSelf ;
  !dateList <- list groupedBy: [ updateDate ] ;

  dateList
  do: [ !date <- ^self asSelf ;
        !feed <- ^my feed ;
        feed updateMode = "Replace"
          ifTrue: [ feed deleteGroupAssignmentsOn: date ] ;
        feed updateGroupAssignmentsOn: date using: groupList ;
      ] ;

  feed updateGroupMembershipsOn: dateList ;

] ;

MembershipFeed defineMethod: [ | deleteGroupAssignmentsOn: date |
  !feed <- ^self asSelf ;
  !memberBlock <- feed memberUpdateBlock ;
  !membersAreTimeSeries <- feed memberType send: memberBlock . isTimeSeries ;

  date isntDefault && membersAreTimeSeries
   ifTrue: 
     [ feed memberType instanceList
          do: [ ^self send: ^my memberBlock . delete: ^my date ] ;
     ] ;

] ;

MembershipFeed defineMethod: [ | updateGroupAssignmentsOn: date using: list |
  !feed <- ^self ;

  !evalDate <- date else: earliestPossibleDate ;
  evalDate evaluate:
     [
     !memberBlock <- feed memberUpdateBlock ;
     list select: [ member isntDefault ] .
        do: [ member setProperty: ^my memberBlock to: group ] ;
     ] ;
] ;

MembershipFeed defineMethod: [ | updateGroupMembershipsOn: dateList |
  !feed <- ^self ;
  !memberBlock <- feed memberAccessBlock ;
  !groupBlock <- feed groupUpdateBlock ;
  !defaultList <- feed groupType send: groupBlock . value ;
  !groupMaster <- feed groupType masterList ; 
  ^self groupBridgePath isntNA    # <---
  ifTrue: [ !bridgeBlock <- ^self groupBridgePath asBlock ;
            :groupMaster <- groupMaster 
                extendBy: [ !bridge <- ^self send: ^my bridgeBlock ] .
                select: [ bridge isntDefault ] . send: [ asSelf ] ;
          ] ;
  dateList send: [ asDate else: earliestPossibleDate ] .
  do: [ !date <- ^self ;
        !feed <- ^my feed ;
        !memberBlock <- ^my memberBlock ;
        !groupBlock <- ^my groupBlock ;
        !defaultList <- ^my defaultList ;
        date evaluate:
           [
           feed memberType instanceList 
           extendBy: [ !group <- ^self send: ^my memberBlock ] .
           select: [ group isntDefault ] .
           groupedBy: [ group ] in:  ^my groupMaster .
           do: [ !groupSeries <- ^self send: ^my groupBlock ;
                 !groupOnDate <- groupSeries isTimeSeries
                    ifTrue: [ groupSeries on: ^date ] 
                   ifFalse: [ groupSeries value ] ;
                 groupOnDate isNA || groupOnDate = ^my defaultList
                 ifTrue:
                   [ ^self setProperty: ^my groupBlock
                           to: ^my defaultList clusterNew ;
                   ] ;
                 !members <- groupSeries value ;
                 members do: [ ^my members delete: ^self ] ;
                 groupList do: [ ^my members at: asSelf put: asSelf ] ;
               ] ;
           ] ;
     ] ;
  feed deleteDuplicateMembershipsFor: dateList ;
] ;

MembershipFeed defineMethod: [ | deleteDuplicateMembershipsFor: dlist | 

  !feed <- ^self ;
  !dates <- dlist send: [ asDate ] . 
     select: [ isntDefault ] . sortUp: [ ^self ] ;
  !groupBlock <- feed groupUpdateBlock ;
  !groupIsTimeSeries <- feed groupType send: groupBlock . isTimeSeries;

  groupIsTimeSeries
  ifTrue:
    [
    !defaultList <- feed groupType send: groupBlock . value ;
    feed groupType masterList
    do: [ !groupSeries <- ^self send: ^my groupBlock ;
          !defaultList <- ^my defaultList ;
          ^my dates 
          do: [ !prior <- ^my groupSeries asOf: ^self - 1 days ;
                !current <- ^my groupSeries asOf: ^self ;
                current != ^my defaultList && [ current isEquivalentTo: prior ] 
                ifTrue: [ current rdelete ;
                          ^my groupSeries delete: ^self asDate ;
                        ] ;
              ] ;
        ] ;
    ] ;
] ;


MembershipFeed defineMethod: [ | updateGroupsUsing: list | 
  !feed <- ^self ;
  !groupBlock <- feed groupUpdateBlock ;
  !groupIsTimeSeries <- feed groupType send: groupBlock . isTimeSeries;

  list groupedBy: [ updateDate else: earliestPossibleDate ] .
  do: [ !date <- ^self asDate ;
        !feed <- ^my feed ;
        !groupBlock <- ^my groupBlock ;
        !groupIsTimeSeries <- ^my groupIsTimeSeries ;
        date evaluate:
        [ groupList groupedBy: [ group ] .
          do: [ !groupSeries <- ^self send: ^my groupBlock ;
                !currentDate <- ^my groupIsTimeSeries
                    ifTrue: [ groupSeries effectiveDate ] 
                   ifFalse: [ ^date ] ;
                !listHasChanged <- currentDate != ^date ||
                   [ groupList send: [ member ] . 
                        isntEquivalentTo: groupSeries value
                   ] ;
                listHasChanged || groupSeries value isDefault
                ifTrue:  [
                  ^my groupIsTimeSeries && 
                    [ currentDate != ^date || groupSeries value isDefault ] 
                    ifTrue: [ groupSeries put: groupSeries value clusterNew ] ;
                  !members <- groupSeries value ;
                  ^my feed updateMode = "Replace"
                      ifTrue: [ members do: [ ^my members delete: ^self ] ] ;
                  groupList
                  do: [ ^my members at: member put: member ;
                        :membershipChange <- TRUE ;
                      ] ;
                  ] ;
               ] ;
         ] ;
     ] ;
] ;
MembershipFeed defineMethod: [ | displayNewOnes | 
  !feed <- ^self ;
  "Dates present: " print: 20 ; 
  feed instanceList select: [isProcessed ] .
     groupedBy: [ date ] . do: [ print ; " " print ] ;
] ;

MembershipFeed defineMethod: [ | displayBadOnes | 
  !feed <- ^self ;
  !badOnes <- feed masterList select: [ isntProcessed ] ;
  badOnes count > 0
  ifTrue: [ newLine print ; 
            badOnes count print ; " bad memberships." printNL ;
            newLine print ; 
            badOnes select: [ group isNA ] . 
               groupedByString: [ groupId ] . sortUp: [ asSelf ] .
            do: [ "Group " print ; print ; " not defined  --  has " print ;
                  groupList count asString print ; " members." printNL ;
                ] ;
            badOnes select: [ member isNA ] . 
               groupedByString: [ memberId ] . sortUp: [ asSelf ] .
            do: [ "Member " print ; print ; " not defined --  in  " print ;
                  groupList count asString print ; " groups." printNL ;
                ] ;
            newLine print ;
          ] ;
] ;

#--------------------
#  MembershipFeed: new/changed methods for purge support
#  Styles:
#     1) delete date for member that does track details 
#     2) delete member for universe for date that does not track details
#     3) delete date for universe that does not track details
#--------------------

MembershipFeed defineMethod: [ | purgeRecords | 
!feed <- ^self asSelf currentStore ;
feed initializeProcessing ;

!todo <- feed masterList ;
feed trackedAtMember
ifTrue:
  [ :todo <- todo select: [ member isntNA ] ;
    feed purgeMembersUsing: todo ;
  ]
ifFalse:
  [ :todo <- todo select: [ group isntNA ] ;
    feed purgeGroupsUsing: todo ;
  ] ;

todo do: [ :isProcessed <- TRUE ] ;
newLine print ;
"---  Membership Deletions for " concat: feed whatAmI . concat: " ---" . 
    centerNL: 80 . printNL ;
feed displayExceptionSummary ; 
!notdone <- feed masterList select: [ isntProcessed ] ;
notdone count > 0 
ifTrue:
  [ newLine print ; 
    notdone count print ; " " print ; 
    " bad group, member, or date - not processed. " printNL ;
    notdone sortUp: [ groupId ] .
    do: [ groupId print: 20 ; memberId print: 20 ; date printNL ; ] ;
  ] ;

newLine print ; 
"--->  The following deletes were processed: " printNL ;
todo do: [ group code print: 15 ; member code print: 15 ; date printNL ] ;
] ;

MembershipFeed defineMethod: [ | purgeMembersUsing: list | 
#     1) delete date for member that does track details 
#        if t/s remove point on date ; if fixed, reset to default value

  !feed <- ^self asSelf ;
  !memberBlock <- feed memberUpdateBlock ;
  list 
  do: [ !currentValue <- member send: ^my memberBlock ;
        !default <- currentValue value defaultInstance ;
        currentValue isTimeSeries
        ifTrue: 
          [ !forDate <- date asDate ;
            forDate isDate     #- does not support ranges right now
            ifTrue: 
              [ currentValue delete: forDate ;
                forDate isDefault
                ifTrue: [ currentValue asOf: earliestPossibleDate 
                                        put: default ;
                        ] ;
              ] ;
          ]
        ifFalse: [ currentValue <- default ] ;
      ] ;
  !dateList <- list groupedBy: [ date ] ;
  feed updateGroupMembershipsOn: dateList ;
] ;

MembershipFeed defineMethod: [ | purgeGroupsUsing: list | 
#     2) delete member for universe for date that does not track details
#     3) delete date for universe that does not track details
  !feed <- ^self ;
  !groupBlock <- feed groupUpdateBlock ;
  !groupIsTimeSeries <- feed groupType send: groupBlock . isTimeSeries;

  !hasMembers <- list select: [ member isntNA ] ;
  hasMembers 
  do: [ !asof <- date else: earliestPossibleDate ;
        !groupBlock <- ^my groupBlock ;
        !groupIsTimeSeries <- ^my groupIsTimeSeries ;
        asof evaluate:
        [ !groupSeries <- group send: ^my groupBlock ;
          ^my groupIsTimeSeries not || [ groupSeries on: ^date . isntNA ] 
              ifTrue: [ groupSeries value delete: member ] ;
        ] ;
     ] ;
  !noMembers <- list select: [ member isNA ] ;
  noMembers 
  do: [ !groupBlock <- ^my groupBlock ;
        !groupIsTimeSeries <- ^my groupIsTimeSeries ;
        !groupSeries <- group send: ^my groupBlock ;
        groupIsTimeSeries && date isDate && [ date isntDefault ]
           ifTrue: [ groupSeries delete: date ] .
           elseIf: [ groupIsTimeSeries not || date isDefault ] 
             then: [ !members <- date evaluate: [ groupSeries value ] ;
                     members do: [^my members delete: ^self ] ;
                   ] ;
     ] ;
] ;

#----------------------------------------------------------------------

#--------------------
#  UniverseMembers
#--------------------

UniverseMembers defineMethod: [ | memberType | 
   ^self group else: ^global Universe . entityType
] ;

UniverseMembers defineMethod: [ | setMemberTypeTo: type withAccessPath: path |
#-- don't set type, it's a method derived from the Universe
   ^self define: 'memberAccessPath' toBe: NA ;
   ^self
] ;

UniverseMembers defineMethod: [ | initializeProcessing | 
  ^self asSelf instanceList
  do: [ :group <- ^self groupType locateId: groupId ;
        !type <- ^self memberType ;
        :member <- type named isntNA
           ifTrue: [ type named at: memberId ] ;
      ] ;
]  ;

UniverseMembers defineMethod: [ | resetRunTimeAttributes | 
   ^super resetRunTimeAttributes 
     setUpdateModeToReplace
] ;





#======================================================================

####################
#  AliasFeeds
#     These feeds are used to specify one or more alias for
#     the entity associated with the supplied id.  
#  Subclass Globals:
#     - entityType
####################

AliasFeed
;

AliasFeed defineMethod: [ | displayFeedConstants | 
  !feed <- ^self ;
  ^super displayFeedConstants ;
  newLine print ;
  "entityType: " print: 20 ; feed entityType whatAmI printNL ;
] ;

AliasFeed defineMethod: [ | getUploadProperties | 
    ("oldId", "newId" ) numberElements 
       extendBy: [ !triggerKey <- position ;
                   !code <- asSelf ;
                   !briefDescription <- "" ;
                 ] 
] ;


AliasFeed defineMethod: [ | displayInfo | 
  ^self entityType whatAmI print ;
  valueList select: [ isntDefault ] .  do: [ " | " print ; print ;] ;
  newLine print ;
] ;

AliasFeed defineMethod: [ | rejectReason | 
    "No entity found" 
] ;

#----------

AliasFeed defineMethod: [ | setEntityTypeTo: etype |
   ^self define: 'entityType' toBe: etype ;
   ^self
] ;


AliasFeed defineMethod: [ | initializeProcessing | 
  !feed <- ^self asSelf ;
  feed masterList
  do: [ !feed <- ^my feed ;
        :underlyingRecord <- ^self valueList 
           send: [ ^my feed entityType locateId: asSelf ] .
           select: [ isntDefault ] . at: 1 ;
      ] ;
]  ;

AliasFeed defineMethod: [ | runUpdate | 
  !todo <- ^self asSelf masterList
       select: [ underlyingRecord isntDefault ] ;
  todo 
  do: [ valueList select: [ isntDefault ] .
          do: [ ^my underlyingRecord addAlias: asSelf copyString ] ;
        :isProcessed <- TRUE ;
      ] ;
] ;

AliasFeed defineMethod: [ | displayBadOnes | 
  !badOnes <- ^self asSelf masterList 
       select: [ underlyingRecord isDefault ] ;
  badOnes count > 0
  ifTrue: [ newLine print ; newLine print ; 
            badOnes count print ; " no entity found." printNL ;
            badOnes do: [ displayInfo ] ;
          ] ;
] ;

#--------------------
#  AliasFeed: new/changed methods for purge support
#     For id supplied as first field, delete all aliases supplied
#     as additional fields.
#--------------------

AliasFeed defineMethod: [ | purgeRecords | 
!feed <- ^self asSelf currentStore ;
feed initializeProcessing ;
!todo <- feed masterList
       select: [ underlyingRecord isntDefault ] ;
todo 
do: [ !entity <- underlyingRecord ;
      !id <- valueList at: 1 ;
      !toDel <- valueList select: [ asSelf != ^my id ] .
          select: [ isntNA ] .
          select: [ count > 0 ] .
          groupedByString: [ asSelf ] . send: [ asSelf ] ;
      entity deleteAlias: toDel ;
    ] ;
todo do: [ :isProcessed <- TRUE ] ;

newLine print ;
"---  Aliases Deletions for " concat: feed entityType whatAmI . 
   concat: " ---" . centerNL: 80 . printNL ;
feed displayExceptionSummary ; 
feed displayBadOnes ;
newLine print ;
"--->  The following alias deletes were processed: " printNL ;
todo 
do: [ !key <- valueList at: 1 ;
      underlyingRecord do: [ code print: 10 ; name print: 25 ] ;
      "  --> deleted aliases: " print ; 
      valueList select: [ asSelf != ^my key ] .
      do: [ print ; " | " print ] ;
      newLine print ;
    ] ;
] ;



####################
#  XRefFeeds
#     These feeds are used to specify an identifier for an entity
#         from a specific id source.  The entity and id source must 
#         already exist
#     The source/symbol combinations should be unique ; if the same
#         symbol is supplied, the cross reference will be associated with
#         the first occurrance
#     The entity/source combinations should be unique ; if multiple
#         symbols are supplied, the cross reference will be associated with
#         the first occurrance
#  Subclass Globals:
#     - entityType
####################

XRefFeed
   defineFixedProperty: 'sourceId' .
   defineFixedProperty: 'symbol' .

   defineFixedProperty: 'source' .
   defineFixedProperty: 'xref' .
   defineFixedProperty: 'newOneCreated' .

   defineFixedProperty: 'status' .
   defineFixedProperty: 'recordUsed' .
   defineFixedProperty: 'priorEntity' .
   defineFixedProperty: 'priorSymbol' .
;

XRefFeed defineMethod: [ | displayFeedConstants | 
  !feed <- ^self ;
  ^super displayFeedConstants ;
  newLine print ;
  "entityType: " print: 20 ; feed entityType whatAmI printNL ;
] ;

XRefFeed defineMethod: [ | getUploadProperties | 
  !feed <- ^self ;
  ("entityId", "source", "symbol" ) 
    send: [ ^my feed getMessage: asSelf ] . 
    extendBy: [ !triggerKey ] .
] ;

XRefFeed defineMethod: [ | displayInfo | 
  ^self entityType whatAmI print ; " " print ; 
  entityId print ; " " print ; sourceId print ; " " print ; symbol printNL ;
] ;

XRefFeed defineMethod: [ | hasStatusInfo | 
  newOneCreated isTrue 
] ;

#----------

XRefFeed defineMethod: [ | setEntityTypeTo: etype |
   ^self define: 'entityType' toBe: etype ;
   ^self
] ;


XRefFeed defineMethod: [ | initializeProcessing | 
  !feed <- ^self asSelf ;

  feed masterList
  do: [ :underlyingRecord <- entityType locateId: entityId ;
        :source <- ^global Named IdSource at: sourceId ;
      ] ;

  !todo <- feed masterList 
     select: [ underlyingRecord isntDefault && source isntDefault ] ;

#--  make sure XRef dictionaries have been setup for each source present
  todo
  groupedBy: [ source ] .
  do: [ !xref <- ^self getXRefFor: ^my feed entityType .
           else: [ groupList at: 1 . :newOneCreated <- TRUE ;
                   ^self createXRefFor: ^my feed entityType 
                 ] ;
        groupList do: [ :xref <- ^my xref ] ;
      ] ;

#-- lookup entity currently associated with symbol and current symbol
  todo 
  do: [ :priorEntity <- xref at: symbol ;
        :priorSymbol <- xref getValueFor: underlyingRecord ;
      ] ;

]  ;

XRefFeed defineMethod: [ | runUpdate | 
  !feed <- ^self asSelf ;
  !todo <- feed masterList select: [ xref isntNA ] ;

#--  if same source/symbol for multiple entities, only the first is used
  todo select: [ symbol isntDefault ] .
     groupedByString: [ source code concat: symbol ] .
  do: [ !primary <- groupList at: 1 . asSelf ;
        groupList select: [ asSelf != ^my primary asSelf ] .
        do: [ :status <- "Duplicate" ;
              :recordUsed <- ^my primary ;
            ] ;
      ] ;

#--  if same entity/source for multiple symbols, only the first is used
  todo select: [ status isNA ] .
     groupedByString: [ underlyingRecord code concat: source code ] .
  do: [ !primary <- groupList at: 1 . asSelf ;
        groupList select: [ asSelf != ^my primary asSelf ] .
        do: [ :status <- "Multiple" ;
              :recordUsed <- ^my primary ;
            ] ;
      ] ;

  todo select: [ status isNA ] . 
  do: [ source addSymbol: symbol toEntity: underlyingRecord ;
        :isProcessed <- TRUE ;
      ] ;

] ;

XRefFeed defineMethod: [ | displayBadOnes | 
  !list <- ^self asSelf instanceList select: [ isntProcessed ] ;
  !badOnes <- list
       select: [ underlyingRecord isDefault && entityId isntDefault ] ;
  badOnes count > 0
  ifTrue: [ newLine print ; 
            "Bad entityId supplied. " print ;
            badOnes count print ; " not processed." printNL ;
            badOnes sortUp: [ sourceId ] . sortUp: [ entityId ] . 
               do: [ displayInfo ] ;
          ] ;
  !badOnes <- list
       select: [ underlyingRecord isntDefault && source isDefault ] ;
  badOnes count > 0
  ifTrue: [ newLine print ; 
            "Bad sourceId supplied. " print ;
            badOnes count print ; " not processed." printNL ;
            badOnes sortUp: [ sourceId ] . sortUp: [ entityId ] . 
               do: [ displayInfo ] ;
          ] ;
  !badOnes <- list select: [ status isntNA ] ;
  badOnes count > 0
  ifTrue: [ newLine print ; 
            "Duplicate or Multiple Symbol supplied. " print ;
            badOnes count print ; " not processed." printNL ;
            badOnes sortUp: [ sourceId ] . sortUp: [ entityId ] . 
            do: [ displayInfo ;
                  "   ---> " print ; status print ;
                  ": refer to " print ; recordUsed displayInfo ;
                ] ;
          ] ;
] ;

XRefFeed defineMethod: [ | displayNewOnes | 
  !newOnes <- ^self asSelf instanceList select: [ newOneCreated ] ;
  newOnes count > 0
  ifTrue: [ newLine print ; 
            newOnes count print ; " new XRef's created at " print ;
            ^self entityType whatAmI print ; " XRef " printNL ;
            newOnes do: [ xref whatAmI printNL ] ;
          ] ;
#-- symbol has changed AND 
#-- symbol was non-default or assigned to a different entity ???
 !changes <- ^self asSelf instanceList 
   select: [ isProcessed ] .
   select: [ priorSymbol != symbol && symbol isntDefault
#             && [priorSymbol isntDefault || priorEntity != underlyingRecord ] 
           ] ;
  changes count > 0
  ifTrue: [ newLine print ; 
            changes count print ; " identifiers changed." printNL ;
            changes groupedBy: [ source ] .
            do: [ newLine print ; 
                  "===> " print ; code printNL ;
                  groupList sortUp: [ entityId ] .
                  do: [ symbol print: 20 ; " was " print ; 
                        priorSymbol print: 20 ; " in " print ;
                        underlyingRecord displayInfo ;
                        priorEntity isntDefault &&
                          [ priorEntity != underlyingRecord ] 
                        ifTrue: 
                          [  "   ... " print ; symbol print: 20 ;
                             " removed from " print ; 
                             priorEntity displayInfo ;
                          ] ;
                      ] ;
                ] ;
          ] ;
  !defaults <- ^self asSelf instanceList 
    select: [ isProcessed && symbol isDefault && priorSymbol isntDefault ] ;
  defaults count > 0
  ifTrue: [ newLine print ; 
            defaults count print ; " identifiers deleted." printNL ;
            defaults groupedBy: [ source ] .
            do: [ newLine print ; 
                  "===> " print ; code printNL ;
                  groupList sortUp: [ entityId ] .
                  do: [ " " print: 5 ; underlyingRecord displayInfo ] ;
                ] ;
          ] ;
] ;

#----------



#======================================================================

####################
#  RangeGroupFeeds
#     These feeds are used to specify ranges of values in which
#     to classify underlying data.  Any number of ranges can
#     be specified.  Ranges are assumed to be continugous.  When
#     any data for a specific range is reloaded via a feed, the
#     entire range is recreated.
#  Subclass Globals:
#     - rangeClass
#     - baseClass
#     - updatePropety
#     - accessRule
####################

RangeGroupFeed
   defineFixedProperty: 'startValue' .
   defineFixedProperty: 'endValue' .
   defineFixedProperty: 'label' .
;

#----------
#  display
#----------

RangeGroupFeed defineMethod: [ | displayFeedConstants | 
  !feed <- ^self ;
  ^super displayFeedConstants ;
  newLine print ;
  "rangeClass: " print: 20 ; feed rangeClass whatAmI printNL ;
  "baseClass: " print: 20 ; feed baseClass whatAmI printNL ;
  "  - property: " print: 20 ; feed updateProperty printNL ;
  "  - access rule: " print: 20 ; feed accessRule printNL ;
] ;

RangeGroupFeed defineMethod: [ | getUploadProperties | 
  !feed <- ^self ;
  ("startValue", "endValue", "label" ) 
    send: [ ^my feed getMessage: asSelf ] . 
    extendBy: [ !triggerKey ] .
] ;


RangeGroupFeed defineMethod: [ | displayInfo | 
  ^self rangeClass whatAmI print ; ": " print ; label print ;
  " from " print ; startValue print ;  " to " print ; endValue printNL ;
] ;

#----------
#  subclass constants
#----------

RangeGroupFeed defineMethod: [ | setRangeClassTo: class |
  ^self define: 'rangeClass' toBe: class ;
  ^self
] ;

RangeGroupFeed defineMethod: [ | setBaseClassTo: class |
  ^self define: 'baseClass' toBe: class ;
  ^self
] ;

RangeGroupFeed defineMethod: [ | setUpdatePropertyTo: string |
  !block <- string isString
     ifTrue: [ string asUpdateBlock ] 
    ifFalse: [ string ] ;
  ^self define: 'updateProperty' toBe: [ block ] ;
  ^self
] ;

RangeGroupFeed defineMethod: [ | setAccessRuleTo: string |
  !block <- string isString
     ifTrue: [ string asBlock ]
    ifFalse: [ string ] ;
  ^self define: 'accessRule' toBe: [ block ] ;
  ^self
] ;

#----------

#----------
#  processing
#----------

RangeGroupFeed defineMethod: [ | runUpdate | 
  !feed <- ^self asSelf ;

  feed clearCurrentRangeGroupInstances ;

  feed masterList
    iterate: [     #- use iterate so code gets assigned in serial
       rangeClass createInstance: startValue asNumber, endValue asNumber .
          setNameTo: label ;
        :isProcessed <- TRUE ;
    ] ;

  feed updateCurrentRangeGroups ;

] ;

RangeGroupFeed defineMethod: [ | clearCurrentRangeGroupInstances |
  ^self rangeClass masterList do: [ flagForDeletion ; rdelete ] ;
] ;

RangeGroupFeed defineMethod: [ | updateCurrentRangeGroups |
  !feed <- ^self ;
  feed updateProperty isntNA && feed accessRule isntNA 
  ifTrue:
    [ feed baseClass instanceList collect: feed accessRule .
      do: [ ^self send: ^my feed updateProperty . <- 
               ^my feed rangeClass getClassificationFor: value ;
          ] ;
    ] ;
] ;

RangeGroupFeed defineMethod: [ | displayOtherExceptions | 
  !feed <- ^self ;
  "RangeGroup " print ; feed rangeClass whatAmI printNL ;
  feed rangeClass masterList do: [ displayInfo ] ;
  newLine print ;
  feed updateProperty isntNA && feed accessRule isntNA 
  ifTrue:
    [ "Updates " print ; 
      feed updateProperty asUndelimitedString stripChar: ":" . print ;
      " in class " print ; feed baseClass whatAmI print ;
      " via " print ; feed accessRule printNL ;
      feed baseClass masterList 
          extendBy: [ !rawValue <- ^self send: ^my feed accessRule ;
                      !group <- ^self send: ^my feed updateProperty . value ;
                    ] .
           groupedBy: [ group ] .
      do: [ name print: 30 ; 
            groupList min: [ rawValue ] .  printWithCommas: 12.0 ;
            groupList max: [ rawValue ] .  printWithCommas: 12.0 ;
            groupList count printNL ;
          ] ;
   ] ;
 ] ;





#======================================================================

####################
#  Custom Feeds
#     These feeds are used for custom file processing
####################



#======================================================================

Schema processAllMessages ;

#======================================================================

GlobalsFeed
   setRequiredFieldsTo: 2 .
   disableFieldsFromHeader
   setFieldOrderListTo: "option", "setting", "settingType" ;

GlobalsFeed
do: [ fieldOrderList
      do: [ ^my getMessage: asSelf . enableOkayToUpload ] ;
    ] ;

("classId", "parentId", "description", "ospace")
   do: [ ^my ClassSetup getMessage: asSelf .enableOkayToUpload ] ;

PropertySetup 
   setRequiredFieldsTo: 2 .
;

"classId", "property", "tsFlag", "dataType", "defaultValue", "adjustments", 
"propertyXRef", "description"
   do: [ ^my PropertySetup getMessage: asSelf . enableOkayToUpload ] ;

MessageSetup
   setRequiredFieldsTo: 2 .
;

"classId", "message", "keyType", "returnType", "containerType",
"level", "function", "tvFlag", "description", "brief", "paramList"
   do: [ ^my MessageSetup getMessage: asSelf . enableOkayToUpload ] ;

DataFeedSetup 
   setFeedClassTo: DataFeed .
   setRequiredFieldsTo: 2 .
   disableFieldsFromHeader
   setFieldOrderListTo: "feedId", "baseClassId" .
;
"feedId", "baseClassId"
   do: [ ^my DataFeedSetup getMessage: asSelf . enableOkayToUpload ] ;
"baseEntityId", "frequency"
 do: [ ^my EntityExtenderFeedSetup getMessage: asSelf . enableOkayToUpload ] ;

"groupId", "groupPath", "memberId", "memberPath", "mode"
   do: [ ^my MembershipFeedSetup getMessage: asSelf . enableOkayToUpload ] ;

"rangeClassId", "updateClassId", "updatePath", "accessRule"
   do: [ ^my RangeGroupFeedSetup getMessage: asSelf . enableOkayToUpload ] ;

MasterFeed 
   setEntityTypeTo: Entity .
   disableInternalIds 
   disableLocalFields
   setRequiredFieldsTo: 1 .
   resetRunTimeAttributes ;

"terminateFlag"
   do: [ ^my MasterFeed getMessage: asSelf . enableOkayToUpload ] ;
"underlyingCurrency", "conversion"
   do: [ ^my CurrencyMaster getMessage: asSelf . enableOkayToUpload ] ;
UniverseMaster getMessage: "memberType" . enableOkayToUpload ;

EntityExtenderFeed
   disableLocalFields
   setBaseClassTo: ^global Entity .
   setBaseEntityTo: NA .
   setFrequencyTo: NA .
   disableDisplayNewOnes
   setRequiredFieldsTo: 2 .
;

MembershipFeed
     setGroupTypeTo: ^global Universe withAccessPath: "list" .
     setMemberTypeTo: ^global Entity  withAccessPath: NA .
     setUpdateModeToAppend 
     setRequiredFieldsTo: 2 .
     setGroupBridgePathTo: NA .
     setMemberBridgePathTo: NA .
;
"groupId", "memberId"
   do: [ ^my MembershipFeed getMessage: asSelf . enableOkayToUpload ] ;


AliasFeed
   disableLocalFields
   disableFieldsFromHeader
   setRequiredFieldsTo: 2 .
   setEntityTypeTo: Entity .
;

XRefFeed
   setRequiredFieldsTo: 3 .
   setEntityTypeTo: Entity .
;

XRefFeed getMessage: "sourceId" . enableOkayToUpload ;
XRefFeed getMessage: "symbol" . enableOkayToUpload ;

RangeGroupFeed
     disableFieldsFromHeader
     setRangeClassTo: ^global RangeClassification .
     setBaseClassTo: ^global Entity .
     setUpdatePropertyTo: NA .
     setAccessRuleTo: NA .
     setRequiredFieldsTo: 3 .
     setFieldOrderListTo: "startValue", "endValue", "label" .
;
"startValue", "endValue", "label"
   do: [ ^my RangeGroupFeed getMessage: asSelf . enableOkayToUpload ] ;
"startValue", "endValue"
 do: [ ^my RangeGroupFeed getMessage: asSelf . 
            setReturnObjectTypeTo: ^global Number
     ] ;


#--------------------------------------------------





