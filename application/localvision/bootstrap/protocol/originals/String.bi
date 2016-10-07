"
##################
####  String  ####
##################
"
?g

##############################################################################
#	The 'print' primitive message to 'String's is defined during system
#	bootstrap.  It is repeated here for consistency.
##############################################################################

####################
#  Printing and Standard Messages
####################

Prototypical String
;

####################
#  Primitives and Converses
####################

Prototypical String

  define: 'asString'                    toBe: 11 asPrimitive .     #ReturnCurrent

  define: 'print'      toBe: 125 asPrimitive .    #PrintString
  defineMethod: [ | print: aFormat | aFormat printOf: super ] .
  defineMethod:     [ | sprint: aFormat | aFormat sprintOf: super ] .

  define: 'locateOrAddInDictionaryOf:'  toBe: 111 asPrimitive .    #LocateOrAddEntryInDictionaryOf
  define: 'locateInDictionaryOf:'       toBe: 112 asPrimitive .    #LocateEntryInDictionaryOf
  define: 'deleteFromDictionaryOf:'     toBe: 113 asPrimitive .    #DeleteFromDictionaryOf

  define: 'replacementUpdateDumpOf:'    toBe: 117 asPrimitive .    #DBUpdateReplaceDump
  define: 'additionUpdateDumpOf:'       toBe: 118 asPrimitive .    #DBUpdateAppendDump

  define: 'count'                       toBe: 350 asPrimitive .    #StringSize

  define: 'canonicalizedForSort'        toBe: 351 asPrimitive .    #EncodeStringAsDouble
  defineMethod:
  [ | orderUsingDictionary: dictionary |
     "." concat: super. locateOrAddInDictionaryOf: dictionary
  ].

  define: 'fixed'      toBe: 352 asPrimitive .    #ToLStoreString
  define: 'basicFill:'      toBe: 353 asPrimitive .    #StringFill
  defineMethod: [ | fill: n | ^self basicFill: (n asInteger else: 0 ) ] .
  define: 'basicTake:'      toBe: 354 asPrimitive .    #StringTake
  defineMethod: [ | take: n | ^self basicTake: (n asInteger else: 0 ) ] .
  define: 'basicDrop:'      toBe: 355 asPrimitive .    #StringDrop
  defineMethod: [ | drop: n | ^self basicDrop: (n asInteger else: 0 ) ] .
  define: 'basicConcat:'    toBe: 356 asPrimitive .    #StringConcat
defineMethod: 
[ | concat: string | 
  string isString 
     ifTrue: [ ^self basicConcat: string asString ] 
    ifFalse: [ ^self basicConcat: string asString stripBoundingBlanks ] 
] .

  defineMethod: [ | prefixWith: aString | aString concat: super ] .  #StringConcat

  define: 'toDouble'   toBe: 357 asPrimitive .     #ToDouble

  define: '<'          toBe: 360 asPrimitive .    #StringLessThan
  define: '<='         toBe: 361 asPrimitive .    #StringLessThanOrEqual
  define: '='         toBe: 362 asPrimitive .    #StringEqual
  defineMethod: [ | > aString | aString asString < super ] .
  defineMethod: [ | >= aString |  aString asString <= super ] . 
  defineMethod:     [ | != aString | (aString = super) not] .

  define: 'evaluateWithCompilationErrorHandler:in:'  toBe: 35 asPrimitive .
  defineMethod:
  [ | evaluateIn: anObject |
    ^self
    evaluateWithCompilationErrorHandler:
    [ | :string :errorLine :errorCharacter |
        "Error at line/character [" print;
        errorLine print; "/" print; errorCharacter print;
        "] of <" print;
        string print;
        ">" printNL;
    ]  in: anObject
  ]  .
  defineMethod: [ | evaluate | ^self evaluateIn: ^global ] .

#----------
#  associative clusters
#----------
  define: 'basicNewAssociativeCluster' toBePrimitive: 91 withControlValue: 1 .

defineMethod: [ | newAssociativeCluster |
    ^self fixed basicNewAssociativeCluster
] .

  define: 'isAnAssociativeCluster'toBePrimitive: 497 withControlValue: 51 .

  define: 'insertIntoCluster:'  toBePrimitive: 151 .
  define: 'locateInCluster:'    toBePrimitive: 152 .
  define: 'deleteFromCluster:'  toBePrimitive: 153 .
#----------
#  Associative Cluster Converses -- 
#  NOTE: Do Not Change Message Names Without Modifying Backend
#----------
defineMethod: [ | clusterInsert: aString |
    aString insertIntoCluster: super
] .
defineMethod: [ | clusterLocate: aString |
    aString locateInCluster: super
] .
defineMethod: [ | clusterDelete: aString |
    aString deleteFromCluster: super
] .

#----------
#  basic string searches and manipulations
#----------

  define: 'toUpper'      toBePrimitive: 359 withControlValue: 1 .
  define: 'toLower'      toBePrimitive: 359 withControlValue: 2 .
  define: 'toggleCase'   toBePrimitive: 359 withControlValue: 3 .
  define: 'capitalize'   toBePrimitive: 359 withControlValue: 4 .
  define: 'lowercase'    toBePrimitive: 359 withControlValue: 5 .

  define:  'basicContains:'   toBe: 366 asPrimitive.
  defineMethod: [ | matchesString: aString | aString basicContains: super] .
  defineMethod: [ | contains: string | ^self basicContains: string asString ] .

  define: 'breakOn:'	toBe: 369 asPrimitive.
  define: 'cbreakOn:'	toBe: 370 asPrimitive.
  defineMethod: [ | breakString: aString | aString breakOn: super ] .
  defineMethod: [ | cbreakString: aString | aString cbreakOn: super ] .
  define: 'tokenizeOn:'          toBePrimitive: 369 withControlValue: 1 .
  define: 'ctokenizeOn:'         toBePrimitive: 370 withControlValue: 1 .
  define: 'breakOnString:'       toBePrimitive: 389 .
  define: 'tokenizeOnString:'    toBePrimitive: 389 withControlValue: 1 .
  define: 'breakOnPattern:'      toBePrimitive: 390 .
  define: 'tokenizeOnPattern:'   toBePrimitive: 390 withControlValue: 1 .
  defineMethod: [ | breakTokenize: aString | aString tokenizeOn: super ] .
  defineMethod: [ | cbreakTokenize: aString | aString cbreakTokenize: super ].
  defineMethod: [ | stringBreak: aString | aString breakOnString: super ] .
  defineMethod: [ | stringTokenize: aString | aString tokenizeOnString: super].
  defineMethod: [ | patternBreak: aString | aString breakOnPattern: super ] .
defineMethod: [ | patternTokenize: aString | aString tokenizeOnPattern: super].

   define: 'filterOutputOf:'      toBe: 81 asPrimitive.
   define: 'substituteOutputOf:'  toBe: 82 asPrimitive.
   define: 'appendOutputOf:'      toBe: 83 asPrimitive.

   define: 'reverse'             toBe: 358 asPrimitive.
   define: 'prefixSpannedBy:'    toBe: 367 asPrimitive.
   define: 'prefixNotSpannedBy:' toBe: 368 asPrimitive.
defineMethod:
[ | prefixSpannedIn: aString |
   aString prefixSpannedBy: super
] .
defineMethod:
[ | prefixNotSpannedIn: aString |
    aString prefixNotSpannedBy: super
] .

################
#  Substring Search - added as part of open vision module 5.9
################

define: 'basicFindSubstringOrigin:'	toBe: 380 asPrimitive.
defineMethod: 
[ | findSubstringOrigin: string | 
  ^self basicFindSubstringOrigin: string asString 
] .

defineMethod:
[ | containsSubstring: substring |
    ^self findSubstringOrigin: substring. < ^self count
].

defineMethod:
[ | findSubstringExtent: substring |
    !substringOrigin <- ^self findSubstringOrigin: substring;
    substringOrigin < ^self count
        ifTrue: [ substring count ]
       ifFalse: [ 0 ]
].

defineMethod:
[ | findSubstringSuffix: substring |
#----------------------------------------------------------------#
#   (
#        (^self findSubstringOrigin: substring)
#     +
#        (^self findSubstringExtent: substring)
#   ) asInteger
#----------------------------------------------------------------#
    !substringOrigin <- ^self findSubstringOrigin: substring;
    substringOrigin < ^self count
        ifTrue: [ (substringOrigin + substring count) asInteger ]
       ifFalse: [ ^self count ]
].

################
#  Regular Expression Search
################

#--define: 'containsPattern:'	toBe: 366 asPrimitive.	# ...same as 'contains:'

define: 'basicFindPatternOrigin:'	toBe: 381 asPrimitive.
defineMethod:
[ | findPatternOrigin: string | 
  ^self basicFindPatternOrigin: string asString
] .

define: 'basicFindPatternExtent:'	toBe: 382 asPrimitive.
defineMethod:
[ | findPatternExtent: string |
  ^self basicFindPatternExtent: string asString
] .

defineMethod:
[ | findPatternSuffix: pattern |
    (
        (^self findPatternOrigin: pattern)
      +
        (^self findPatternExtent: pattern)
    ) asInteger
].

defineMethod: [ | convertToCaseInsensitiveRegEx |
  [ toList
    do: [ !u <- toUpper ; 
          !l <- toLower ;
          u != l
          ifTrue:
            [ "[" print; u print; l print; "]" print ;
            ] .
          else: [ print ]
        ] ;
  ] divertOutput
] .

################
#  Converses
################

defineMethod:
[ | substringOriginIn: string |
    string findSubstringOrigin: super
].
defineMethod:
[ | patternOriginIn: string |
    string findPatternOrigin: super
].
defineMethod:
[ | patternExtentIn: string |
    string findPatternExtent: super
].


;


?g

##################################################
#    String Definitions
#
#   1) formatting
#   2) standard notifications
#   3) misc
#
##################################################

####################
#  formatting
####################

Prototypical String

defineMethod: 
[ | center: aWidth |
  [ !blanks <- ( (aWidth - count) / 2) asInteger;
    blanks > 0
    ifTrue: [ 
            " " print: blanks;
            ^self print: (aWidth - 2 * blanks) ;
            " " print: blanks;
            ]
   ifFalse: [ ^self print: aWidth ];
  ] divertOutput  
] .

defineMethod: 
[ | centerNL: aWidth |
  ^self center: aWidth . concat: newLine
] .

#----------

defineMethod: [ | asNumber|
  !d <- toDouble; 
  ^self containsSubstring: "." .
     ifTrue: [ d ]
    ifFalse: [d asInteger] . else: [ d ]
] .

defineMethod: [ | asDate | 
  ^self asNumber isInteger
  ifTrue:
    [ count = 4 ifTrue: [ asNumber asDateFromYYMM ] .
      elseIf: [ count = 6 ] then: [ asNumber asDateFromYYMMDD ] .
      else: [ asNumber asDate ]
    ] .
  elseIf: [^self howManyOf: "-/" .= 2]
    then: 
     [ !fields <- ^self breakOn: "/-"; 
       !m <- fields at: 1 .asNumber asInteger ; 
       !d <- fields at: 2 .asNumber asInteger ; 
       !y <- fields at: 3 .asNumber asInteger ; 
       m isInteger && [ d isInteger ] && [ y isInteger ] 
         ifTrue: [ y asDateFromYearForMonth: m andDay: d ] 
     ] .
    elseIf: 
     [ ^self contains: "[dD]ays" .
       || [^self contains: "month"]
       || [^self contains: "quarter"]
       || [^self contains: "year"]
     ]
   then: [^date + ^self evaluate] . 
   elseIf: [toLower contains: "today"]
     then: [^today] . 
   elseIf: [toLower contains: "yesterday"]
     then: [^today - 1 days].
] .

defineMethod: [ | asSelector | 
!stripStrayChars <-
    [ asSelf cbreakOn: 
         "0123456789_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" .
      do: [ print ]
    ] divertOutput ;
stripStrayChars count > 0
ifTrue: 
  [ !escape <- "\\ " take: 1 ;
    stripStrayChars at: 1 . asNumber isNumber
      ifTrue: [ :stripStrayChars <- escape concat: stripStrayChars ] ;
    "'" concat: stripStrayChars .concat: "'" .asBlock value
  ] 
] .

defineMethod:
[ | asBlock | 
  ("[ " concat: ^self . concat: " ] " ) evaluate
] .

defineMethod:
[ | asBlockIn: object | 
  ("[ " concat: ^self . concat: " ] " ) evaluateIn: object
] .

defineMethod: [ | asUpdateBlock | 
  ^self contains: ":" . 
      ifTrue: [ ^self ] ifFalse: [ ":" concat: ^self ] . asBlock 
] .

#----------

defineMethod:
[ | printPRN |
"\"" print;
^self print ;
"\"" print;
^self 
] .

defineMethod:
[ | printPRN: aFormat |
"\"" print;
^self print: aFormat ;
"\"" print;
^self 
] ;

#----------
#  copyString   returns string in new cluster
#  from: to:    returns substring from pos1 to pos2; if pos1 > pos2
#               returns substring from pos2 to pos1 (i.e, reversed)
#  from: for:   returns substring from pos for num characters; if
#               num is negative, pos is last character is returned
#               substring; otherwise pos is first char
#  at: n        returns nth character in string 
#  at: n put: s inserts s at character n in recipient
#----------

Prototypical String

defineMethod: [ | copyString |
  ^self drop: 0
] .

defineMethod:
[ | from: pos1 to: pos2 |
  !start <- pos1 max: 1 ; 
  !end <- pos2 max: 1 ; 
  !count <- (end - start) absoluteValue + 1 ;
  !drop <- start min: end . - 1 ;
  !result <- ^self drop: drop . take: count ;
  start <= end ifTrue: [ result ] ifFalse: [ result reverse ] 
] .

defineMethod:
[ | from: pos for: count | 
  !start <- pos max: 1 ;
  !end <- count > 0
     ifTrue: [ pos + count - 1 ] ifFalse: [ pos + count + 1 ] ;
  start < end 
    ifTrue: [ ^self from: start to: end ] 
   ifFalse: [ ^self from: end to: start ] 
] .

defineMethod:
[ | from: pos |
  pos isNumber 
    ifTrue: [ ^self from: pos to: ^self count ] 
  ifFalse: [ ^self drop: (^self findSubstringOrigin: pos asString ) ] 
] .

defineMethod:
[ | to: pos |
  pos isNumber 
    ifTrue: [ ^self from: 1 to: pos ] 
  ifFalse: [ ^self take: (^self findSubstringSuffix: pos asString ) ] 
] .

defineMethod:
[ | at: n | 
  n isNumber && n > 0 && n <= ^self count
     ifTrue: [ ^self drop: n - 1 . take: 1 ]
    ifFalse: [ "" ] 
] .

defineMethod:
[ | at: n put: s | 
  n isNumber 
  ifTrue:     # insert point must be >= 0 and <= count + 1
    [ !prefix <- n <= 1 
        ifTrue: [ 0 ] .
        elseIf: n > count
          then: [ count ] .
          else: [ (n - 1 ) asInteger ] ;
      ^self take: prefix . concat: s asString . concat: (^self drop: prefix) 
     ]
  ifFalse: [ ^self ]
] .       

defineMethod: [ | howManyOf: char | 
  (^self breakOn: char . count - 1 ) asInteger
]  .
;

#----------
#  translate: to:     changes all occurrences of any supplied char
#  translateString: to:  change all occurence of supplied string
#  translatePattern: to: change all occurences of supplied patter
#----------

Prototypical String defineMethod: [ | translate: char to: string | 
  [ ^self breakOn: char . do: [ print ; ^my string print; ] 
  ] divertOutput drop: string count * -1
] ;

#----------

Prototypical String defineMethod: [ | translateString: string to: replacement |
   [ ^self breakOnString: string. do: [print; ^my replacement print]
   ] divertOutput drop: replacement count * -1
] ;

#----------

Prototypical String
defineMethod: [ | translatePattern: pattern to: replacement |
   [ ^self breakOnPattern: pattern. do: [print; ^my replacement print]
   ] divertOutput drop: replacement count * -1
] ;

#------------------------------

####################
#  standard notifications
####################
Prototypical String 
defineMethod: 
[ | notifyAsError |
  "   >>>   " print;
  print;
  "   <<<" print ;
  newLine print ; 

] .

#----------

defineMethod:
[ | notifyAsWarning |
  "   ==> WARNING:  " print;
  print;
  newLine print;
] .

#----------

defineMethod:
[ | notifyAsComment |
  "   ...  " print;
  print;
  newLine print;
] ;

####################
#  misc
####################

Prototypical String
defineMethod:
[ | asInteger |
  ^self asNumber asInteger
] .

;

#--------------------
#  Strings Manipulations
#--------------------

Prototypical String

defineMethod: [ | isDefault | ^self count = 0 ] .

defineMethod:
[ | stripLeadingBlanks |
    ^self drop: (^self prefixSpannedBy: " ")
].

defineMethod:
[ | stripTrailingBlanks |
    ^self reverse stripLeadingBlanks reverse
].

defineMethod:
[ | stripBoundingBlanks |
  !original <- ^self ;
  ^self stripLeadingBlanks stripTrailingBlanks
     send: [ count = ^my count
                ifTrue: [ ^my original ] ifFalse: [ ^self ]
           ] 
].

defineMethod:
[ | isBlank |
  " " take: (^self count) . = ^self asSelf
] .

defineMethod:
[ | isBlankFor: n |
  ^self take: n . = (" " take: n)
] .

defineMethod: [ | fileExists| 
  !ovc <- "file:" concat: ^self .asOpenVisionChannel; 
  !result <- ovc status != ovc status Failed ;
  ovc close ;
  result
] .

defineMethod: [ | asFileContents |
  !ovc <- "file:" concat: ^self .asOpenVisionChannel; 
  !result <- ovc status != ovc status Failed
    ifTrue: [ !result <- ovc getString ;
              result isNA ifTrue: [ "" ] ifFalse: [ result ] 
            ]  ;
  ovc close ;
  result
] .

defineMethod: [ | unixFileContents | 
    ["cat -s " concat: ^self . filterOutputOf: []] divertOutput
].

defineMethod:
[ | asLines |
  ^self contains: "" .
     ifTrue: [ ^self stripChar: "" ]
    ifFalse: [ ^self ] .
  send: [ ^self take: -1 . = newLine1
           ifTrue: [ ^self drop: -1 ] ifFalse: [ ^self ] .
                breakOn: newLine1
        ] 
].

defineMethod:
[ | asWords |        # breaks on blank, tab, and new line
  !boundary <- newLine concat: " " . concat: "	" ;
  ^self breakOn: boundary . select: [count > 0 ] 
].

defineMethod:
[ | toList | 
   !tokens <- ^self ctokenizeOn: "";
   ^self count sequence send: [^my tokens at: (^self * 2)asInteger]
] .

defineMethod:
[ | stripCommas |
  [ ^self breakOn: "," .
    do: [ print ]
  ] divertOutput
] .

defineMethod:
[ | convertToNumber |
  !prefix <- ^self prefixNotSpannedBy: "0123456789.-+"; 
  !string <- ^self drop: prefix; 
  !suffix <- string reverse prefixNotSpannedBy: "0123456789.-+"; 
  :string <- string drop: (-1 * suffix) asInteger; 
  string contains: "\\." .
     ifTrue: [ string stripCommas asNumber asDouble ] 
    ifFalse: [ string stripCommas asNumber ] 
] .

defineMethod:
[ | startingPositionsOf: subString | 
  !searchString <- ^self ; 
  searchString contains: subString .
  ifTrue:
    [
    !firstChar <- subString take: 1 ;
    !length <- subString count ;
    !entryPoints <- searchString breakOn: firstChar .
       runningTotal: [ 1 + ^self count ] .
       extendBy: 
         [ !startingPosition <- (runningTotal - 1 )  asInteger ;
           !matchString <- 
               ^my searchString drop: startingPosition . take: ^my length ; 
         ] ;
    !matches <- entryPoints select: [ matchString = ^my subString ] ;
    matches send: [ (startingPosition + 1) asInteger]
    ]
] .

defineMethod:
[ | startingPositionOf: subString | 
  !matches <- ^self startingPositionsOf: subString ;
  matches isntNA ifTrue: [ matches at: 1 ] 
]  .

defineMethod:
[ | stripChar: char | 
  [ ^self breakOn: char . do: [ print ] 
  ] divertOutput
] .

defineMethod: [ | stripLeading: chars |
  ^self drop: (^self prefixSpannedBy: chars)
] .

defineMethod:
[ | pad: n |
  [ ^self print: n ] divertOutput
] .

defineMethod:
[ | asQuotedString | 
  "\"" concat: ^self . concat: "\""
] .

#--------------------
#  asRecords
#     converts a file into lines and removes any empty lines
#--------------------

defineMethod:
[ | asRecords | 
  ^self asFileContents asLines select: [ count > 0 ] 
] .

#--------------------
#  asCSVRecords
#    converts a standard excel comma-separated-value format to
#      a tab-separated file; actual , values included in quoted strings
#      remain as , and the " characters removed
#--------------------

defineMethod:
[ | asCSVRecords | 
  ^self asFileContents translate: "," to: "	" .    #- translate , to tab
       asLines select: [ count > 0 ]  .
  send: [
        !quotes <- ^self breakOn: "\"" . numberElements ;
          [ quotes
            do: [   #- odd lines get printed, even lines are quoted strings
                position mod: 2 . = 1
                    ifTrue: [ print ]
                  ifFalse: [ ^self translate: "	" to: "," . print ] ;
               ] ;
          ] divertOutput
       ] .
  extendBy: [ !fields <- ^self breakOn: "	" ] 
] .

#--------------------
# asCellsOn: delimiter
# asCells
#    returns list of lines extended by cells in line where cells are
#    defined by delimiter character (tab or 
#--------------------

defineMethod:
[ | asCells | 
  !delimiter <- "	ÿ" ;      #- tab or vision delimiter
  ^self asCellsOn: delimiter
] .

defineMethod:
[ | asCellsOn: delimiter |
  ^self breakOn: newLine .
      select: [ count > 0 ] .
      select: [ ^self contains: "@@@" . not ] .     #--- remove formatter
  extendBy: 
    [ !cells <- ^self breakOn: ^my delimiter .
          select: [ count > 0 ] .
          select: [ ^self contains: "" . not ] ;    #-- remove printer
    ] .
  select: [ cells count > 0 ]
] .

defineMethod:
[ | formatForPRN |     #---  assumes output is delimited
  [
  ^self asCells
  do: [ cells 
        do: [ !number <- ^self convertToNumber ;
              number isNumber
                ifTrue: [ number print ; " " print ] 
               ifFalse: 
                 [ "\"" print ; 
                   ^self stripBoundingBlanks = "NA"
                      ifTrue: [ " " print ] ifFalse: [ print ] ;
                   "\"" print ; " " print ; 
                 ] ;
            ] ;
       newLine print ; 
     ] ;
  ] divertOutput
] .

defineMethod:
[ | formatForExcel |
  [
  ^self asCells
  do: [ cells 
        do: [ ^self stripBoundingBlanks = "NA"
               ifTrue: [ "" print ] ifFalse: [ print] ; 
              "	" print ;
            ] ;
        newLine print ;
      ] ;
  ] divertOutput
]  .

;


?g

####################
#  String as: type  
#  Object convertFrom: string 
#
#  The generic "as:" message at String will try convert the string
#     into an object of the supplied type.  If the type is a string
#     that is not the default String, the supplied type will be evaluated
#     to determine its class.  The implementation of convertFrom: defined
#     for that class will be used to convert the string to the correct
#     type.  The version at Object looks the string up in the class'
#     default naming dictionary if it exists.  If the string contains the
#     "," character, this method returns a list of objects of supplied 
#     type.
####################

Prototypical String defineMethod: [ | as: type |
  !isList <- ^self contains: "," ;
  !toType <- type isString && [ type isntDefault ] 
    ifTrue: [ type evaluate ] ifFalse: [ type ] ;
  isList
  ifTrue: [ ^self breakOn: "," .
              send: [ ^my toType convertFrom: ^self ] 
          ] 
  ifFalse: [ toType convertFrom: ^self ] 
] ;

Prototypical String defineMethod: [ | convertFrom: string | 
  string asString
] ;

Prototypical String defineMethod: [ | asClass |
  !id <- ^self stripChar: " " ;
  !capId <- id capitalize ;
  ^global respondsTo: id .
    ifTrue: [ id evaluate ] .
    elseIf: [ ^global respondsTo: capId ] then: [ capId evaluate ] .
      else: [ ^global getMessage: id .
                 isntDefault: [ message evaluate ] 
                 else: [ ^global getMessage: ^my capId .
                            isntDefault: [ message evaluate ]  else: [ NA ] 
                       ]
            ] 
] ;

Prototypical String defineMethod: [ | asProgramWith: inputBlock |
  !error ;
  !block <- inputBlock else: [ ] ;
  !out <- [ :error <- asSelf filterOutputOf: block ] divertOutput ;
  error = 0 
    ifTrue: [ out ] 
   ifFalse: 
     [ [ ">>> Error " print ; error asString printNL ; 
         "    Processing Program: " print ; printNL; 
         inputBlock isntNA
            ifTrue: [ "    with " print ; inputBlock printNL ] ;
         out isntDefault
         ifTrue:
           [ "... Output: " printNL ;
             out printNL ;
           ] ;
       ] divertOutput
     ]
] ;

Prototypical String defineMethod: [ | asProgram | 
  asSelf asProgramWith: NA
] ;

Prototypical String defineMethod: [ | validateAsExpressionIn: object |
!string <- ^self ;
!okay <- string contains: ":" . not ;

!messages <- string asWords select: [ isntDefault ] ;
!count <- 1 ;
[ count <= messages count && okay ] whileTrue:
    [ !message <- messages at: count ;
      object respondsTo: message .
        ifTrue: [ :object <- message evaluateIn: object ] 
       ifFalse: [ :okay <- FALSE ] ;
      :count <- (count + 1 ) asInteger ;
    ] ;
okay
] ;


?g

####################
#  redefine newLine using take to make sure there is a one-character version
#    that will work for unix or pc machines
####################
Prototypical Object define: 'newLine1' toBe: (newLine take: -1) ;
