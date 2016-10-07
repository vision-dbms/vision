"
############################
####  Utility Protocol  ####
############################
"
?g


##################################################
#  Note: basicUpdateNetwork already defined in CREATE.bi
##################################################


####################
#  Primitives and Converses
####################

Utility
  define: 'collectGarbage' toBe: 1 asPrimitive .          #CollectGarbage
  define: 'collectSessionGarbage' toBe: 4 asPrimitive .
  define:'restartSession' toBe: 3 asPrimitive .           #RestartSession
  define: 'echoToFile:'    toBe: 88 asPrimitive .          #EchoToFile
  define: 'stopEchoToFile' toBe: 89 asPrimitive .          #StopEchoToFile

  define: 'basicUpdateNetwork' toBe: 115 asPrimitive .
  define: 'faultSeverities'    toBe: 371 asPrimitive .
defineMethod:
[ | updateNetwork |
   systemErrorCount > 0 
   ifTrue: 
     [
     ">>>  System Faults In Session Prevent Network Update  <<<" printNL;
     -1
     ] 
   ifFalse: 
     [
     ^self basicUpdateNetwork
     ]
].

defineMethod:
[ | systemErrorCount |
  faultSeverities instanceList select: [^self = "Error"]. count
].

defineMethod:
[ | systemWarningCount |
  faultSeverities instanceList select: [^self = "Warning"]. count
].

;

####################
#  SessionAttributes - saves
####################

Utility

defineMethod:
[ | updateNetworkWithAnnotation: aString |
    SessionAttribute updateAnnotation <- aString;
    ^self updateNetwork
].

defineMethod:
[ | updateAndCompactNetworkWithAnnotation: aString |
    SessionAttribute compactOnUpdate <- TRUE;
    !result <- ^self updateNetworkWithAnnotation: aString;
    SessionAttribute compactOnUpdate <- FALSE;
    result
].

defineMethod:
[ | updateAndCompact | 
  ^self updateAndCompactNetworkWithAnnotation: "updateAndCompact"
] .

defineMethod:
[ | updateNetworkAsBaseVersionWithAnnotation: aString |
    SessionAttribute makeBaseVersionOnUpdate <- TRUE;
    !result <- ^self updateNetworkWithAnnotation: aString;
    SessionAttribute makeBaseVersionOnUpdate <- FALSE;
    result
].

defineMethod:
[ | updateAndCompactNetworkAsBaseVersionWithAnnotation: aString |
    SessionAttribute compactOnUpdate <- TRUE;
    SessionAttribute makeBaseVersionOnUpdate <- TRUE;
    !result <- ^self updateNetworkWithAnnotation: aString;
    SessionAttribute compactOnUpdate <- FALSE;
    SessionAttribute makeBaseVersionOnUpdate <- FALSE;
    result
].

defineMethod:
[ | fullCompact |
#---  touch all spaces and turn trace on
  !att <- SessionAttribute ;
  1024 sequence
     do: [ ^my att updateFlagOfSpace: ^self . <- TRUE ];
  att traceCompaction <- TRUE ;
#--  run copy and base version components of save/compact
    !result <- ^self
	updateAndCompactNetworkAsBaseVersionWithAnnotation: "Full Compaction";
    att traceCompaction <- FALSE;
    result
] .

defineMethod:
[ | fullCompactOf: list |
  #-- list is list of numbers of spaces
  !att <- SessionAttribute ;
  1024 sequence
    do: [ ^my att compactionFlagOfSpace: ^self . <- FALSE ] ;
  list send: [ asInteger ] . select: [isntNA && ^self > 0 ] .
    do: [ ^my att compactionFlagOfSpace: ^self . <- TRUE ;
          ^my att updateFlagOfSpace: ^self . <- TRUE ;
          "  ...  Including " print ; printNL ;
        ] ;
  att traceCompaction <- TRUE ;
#--  run copy and base version components of save/compact
  !mess <- [ "Full Compact Of " print ; 
             list do: [ ^self print: 4 ] ;
           ] divertOutput ;
  !result <- ^self updateAndCompactNetworkAsBaseVersionWithAnnotation: mess ;
  att traceCompaction <- FALSE;
  result

] .

defineMethod:
[ | fullCompactExcept: list |
  #-- list is list of numbers of spaces
  !att <- SessionAttribute ;
  1024 sequence
    do: [ ^my att compactionFlagOfSpace: ^self . <- TRUE;
          ^my att updateFlagOfSpace: ^self . <- TRUE ;
        ] ;
  list send: [ asInteger ] . select: [isntNA && ^self > 0 ] .
    do: [ ^my att compactionFlagOfSpace: ^self . <- FALSE ;
          "  ...  Omitting " print ; printNL ;
        ] ;
  att traceCompaction <- TRUE ;
#--  run copy and base version components of save/compact
  !mess <- [ "Full Compact Except " print ; 
             list do: [ ^self print: 4 ] ;
           ] divertOutput ;
  !result <- ^self updateAndCompactNetworkAsBaseVersionWithAnnotation: mess ;
  att traceCompaction <- FALSE;
  result
] .
;

####################
#  Licensing - saves
#
#  Note: these new methods and the modification to 'updateNetwork' must
#        be installed or your updates will be rejected; this patch assumes
#        that you have loaded your license file in this session as well.
#        Please contact Insyte if you need a license file.  Be prepared
#        to supply the host name and/or IP address of your Vision server
#        machine.
####################

Utility define: 'UpdateLicense' toBe: NA ;

Utility defineMethod: [ | setUpdateLicenseTo: license |
    ^self define: 'UpdateLicense' toBe: license;
    ^self
] ;

Utility SessionAttribute defineMethod: [ | licenseItemAt: i |
    (127 + i) asInteger asPointerTo: super
] ;

#----------
#  modify updateNetwork to load the current license settings so
#  that an update can happen -  Note: this is a change to an existing
#  method that must be made if you want to use release 6.1.1 and future
#  releases to perform updates
#----------

Utility defineMethod: [ | createSpaceInNetworkWithAnnotation: note | 
   ^self UpdateLicense isList 
   ifTrue: 
     [ ^self UpdateLicense numberElements 
        do: [ 
              ^my SessionAttribute licenseItemAt: position. <- ^self asInteger
            ] ;
     ] ;
   systemErrorCount > 0 
   ifTrue: 
     [
       ">>>  System Faults In Session Prevent Network Update  <<<" printNL;
       -1
     ]
   ifFalse: 
     [ ^global objectSpace = 0     #-- only create a new space
          ifTrue: 
            [ SessionAttribute updateAnnotation <- 
                   (note else: "New Space Created" ) ;
              ^self basicUpdateNetwork 
            ]
         ifFalse: 
           [ ">>>  Running in Object Space " print ; 
             ^global objectSpace asString print ;
             ".  New Space Not Created. <<<" printNL ;
             -1
           ] 
     ] 
] ;

Utility defineMethod: [ | updateNetwork |
   ^self UpdateLicense isList 
   ifTrue: 
     [ ^self UpdateLicense numberElements 
        do: [ 
              ^my SessionAttribute licenseItemAt: position. <- ^self asInteger
            ] ;
     ] ;
   systemErrorCount > 0 
   ifTrue: 
     [
       ">>>  System Faults In Session Prevent Network Update  <<<" printNL;
       -1
     ]
   ifFalse: 
     [ ^global objectSpace != 0     #-- do not create a new space
          ifTrue: [ ^self basicUpdateNetwork ]
         ifFalse: 
           [ ">>>  Object Space Not Set. Save Aborted.  <<<" printNL ;
             -1
           ] 
     ] 
] ;


####################
#  SessionAttributes
####################

Utility SessionAttribute
define: '<-'
  toBe: 2 asPrimitive.

define: 'updateAnnotation'
  toBe: (0 asPointerTo: Utility SessionAttribute).

define: 'compactOnUpdate'
  toBe: (1 asPointerTo: Utility SessionAttribute).

define: 'makeBaseVersionOnUpdate'
  toBe: (2 asPointerTo: Utility SessionAttribute).

define: 'traceCompaction'
  toBe: (3 asPointerTo: Utility SessionAttribute).

define: 'traceUpdate'
  toBe: (4 asPointerTo: Utility SessionAttribute).

define: 'validatingChecksums'
  toBe: (5 asPointerTo: Utility SessionAttribute).

defineMethod:
[ | updateFlagOfSpace: space |
    (space * 65536 + 0) asInteger asPointerTo: super
].

defineMethod:
[ | compactionFlagOfSpace: space |
    (space * 65536 + 1) asInteger asPointerTo: super
].

defineMethod:
[ | mssOverrideOfSpace: space |
    (space * 65536 + 2) asInteger asPointerTo: super
].

defineMethod:
[ | copyCoefficientOfSpace: space |
    (space * 65536 + 3) asInteger asPointerTo: super
].

defineMethod:
[ | reclaimCoefficientOfSpace: space |
    (space * 65536 + 4) asInteger asPointerTo: super
].

defineMethod:
[ | maxCompactionSegmentsOfSpace: space |
    (space * 65536 + 5) asInteger asPointerTo: super
].

defineMethod:
[ | targetSegmentSizeOfSpace: space |
    (space * 65536 + 6) asInteger asPointerTo: super
].

;

Utility SessionAttribute defineMethod: [ | largeContainerSizeOfSpace: space |
    (space * 65536 + 7) asInteger asPointerTo: super
] ;

Utility SessionAttribute
   define: 'visionMaxWriteChunk' 
     toBe: (6 asPointerTo: Utility SessionAttribute) .
   define: 'visionMappingLimit' 
     toBe: (256 asPointerTo: Utility SessionAttribute) .
   define: 'visionAddressThreshold' 
     toBe: (257 asPointerTo: Utility SessionAttribute) .
   define: 'visionNSyncRetries' 
     toBe: (258 asPointerTo: Utility SessionAttribute) .
   define: 'visionSSyncRetries' 
     toBe: (259 asPointerTo: Utility SessionAttribute) .
   define: 'visionSOpenRetries' 
     toBe: (260 asPointerTo: Utility SessionAttribute) .
   define: 'visionSORetryDelay' 
     toBe: (261 asPointerTo: Utility SessionAttribute) .
   define: 'visionNetOpenTrace' 
     toBe: (262 asPointerTo: Utility SessionAttribute) .
   define: 'visionLargeTaskSize' 
     toBe: (263 asPointerTo: Utility SessionAttribute) .
   define: 'visionStackDump' 
     toBe: (264 asPointerTo: Utility SessionAttribute) .
   define: 'visionGRMEnabled' 
     toBe: (265 asPointerTo: Utility SessionAttribute) .
   define: 'visionGRMTrace' 
     toBe: (266 asPointerTo: Utility SessionAttribute) .
   define: 'visionAttemptDefragment' 
     toBe: (267 asPointerTo: Utility SessionAttribute) .
   define: 'visionRestartOnError' 
     toBe: (268 asPointerTo: Utility SessionAttribute) .
   define: 'visionExitOnError' 
     toBe: (269 asPointerTo: Utility SessionAttribute) .
   define: 'verboseSelectorNotFound' 
     toBe: (270 asPointerTo: Utility SessionAttribute) .
;

#--------------------
#  SessionAttribute covers 
#--------------------

Utility SessionAttribute defineMethod: [ | enableVerboseSelectorNotFound |
  verboseSelectorNotFound <- TRUE ;
  ^self
] ;

Utility SessionAttribute defineMethod: [ | disableVerboseSelectorNotFound |
  verboseSelectorNotFound <- FALSE ;
  ^self
] ;

####################
#  Miscellaneous primitive to access active session information
####################

Utility

   define: 'hostname' toBePrimitive: 510 .
   define: 'username' toBePrimitive: 510 withControlValue: 1 .
   define: 'hostId'   toBePrimitive: 513 withControlValue: 1 .

   define: 'accessedNetworkVersion' toBePrimitive: 602 .
   define: 'currentNetworkVersion' toBePrimitive: 603 .
   define: 'processId' toBePrimitive: 604 .

;

#--------------------
#  Cover methods
#--------------------

?g


"
############################
####  Utility UnixSeconds Protocol  ####
############################
"
?g

Utility UnixSeconds
  define: 'passCurrentTimeToCollector:' toBe: 15 asPrimitive .
  define: 'passDecodedLocalTimeToCollector:' toBe: 16 asPrimitive .
  define: 'passDecodedGreenwichMeanTimeToCollector:' toBe: 17 asPrimitive .
  define: 'formatAsLocalTime' toBe: 18 asPrimitive .
  define: 'formatAsGreenwichMeanTime' toBe: 19 asPrimitive .
  define: 'asInteger' toBe: 33 asPrimitive .

defineMethod:
[ | currentTime |
    ^self passCurrentTimeToCollector: [ | :seconds :microseconds |
     seconds asPointerTo: ^my Utility UnixSeconds
    ]
] .

defineMethod:
[ | printDecodedLocalTime |
    ^self passDecodedLocalTimeToCollector: [
     | :year :mon :mday :hour :min :sec :yday :wday :isdst |
     year print: 4; mon print: 3; mday print: 3;
     hour print: 3; min print: 3; sec print: 3;
     yday print: 4; wday print: 2; isdst print: 2
    ];
    ^self
] .

defineMethod:
[ | printDecodedGreenwichMeanTime |
    ^self passDecodedGreenwichMeanTimeToCollector: [
     | :year :mon :mday :hour :min :sec :yday :wday :isdst |
     year print: 4; mon print: 3; mday print: 3;
     hour print: 3; min print: 3; sec print: 3;
     yday print: 4; wday print: 2; isdst print: 2
    ];
    ^self
].

defineMethod:
[ | print |
    ^self formatAsLocalTime print; ^self
].

defineMethod:
[ |  < b | ^self asInteger  < b asInteger ].

defineMethod:
[ | <= b | ^self asInteger <= b asInteger ].

defineMethod:
[ |  = b | ^self asInteger  = b asInteger ].

defineMethod:
[ | != b | ^self asInteger != b asInteger ].

defineMethod:
[ | >= b | ^self asInteger >= b asInteger ].

defineMethod:
[ |  > b | ^self asInteger  > b asInteger ].
;


?g
