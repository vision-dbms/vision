####################################################
#  Define core protocol for DataFeed module.
####################################################


####
# Methods for option control.  Set the default as well.
#
# This is a non-sticky option

####
DataFeed defineMethod: [ | enableOmitEmptyRecords | 
  ^self define: 'omitEmptyRecordsFlag' toBe: TRUE;
  ^self
] ;

DataFeed defineMethod: [ | disableOmitEmptyRecords | 
  ^self define: 'omitEmptyRecordsFlag' toBe: FALSE;
  ^self
] ;

DataFeed defineMethod: [|resetRunTimeAttributes|
  CoreWorkspace GlobalsFeed setDefaultGlobals; 
  ^self disablePurge; 
  ^self disableOmitEmptyRecords;
  ^self
];

DataFeed disableOmitEmptyRecords;


####
# A Utility method for checking if records are empty.  Also some properties
# for customizing the behavior of this method.
#
# A record is empty if it only contains keys (ex: id, date).
####

####
# Return a List of positions of fields that are uploaded.  If they're all 
# empty, we can omit a record.  This is a feed level method.
####

 DataFeed defineMethod: [|omitEmptyFields| 
  !self <- ^self;
  self updateBlocksToDo 
    select: [^my self keyList excludesElement: position] .
    send: [ position ]
];

DataFeed defineMethod: [| isEmptyRecord: fields |
  !feed <- ^self asSelf; 

  !positions <- feed defaultInstance sendOnce: [
      !feed <- ^my feed;
      feed omitEmptyFields
    ];

  positions all: [
      !feed <- ^my feed;
      !value <- ^my fields at: ^self;
      value isDefault || [ ^my feed naTest any: [^my value send: asSelf]]
    ]
];


####
# Local method to re-define what empty means for a record.  Default to 
# false.
####

DataFeed defineMethod: [| isEmptyRecordLocal: fields |
  FALSE
];


####
# The hook into the DataFeed update stream.  This hook is here to avoid
# modifying updateFromString
####

DataFeed defineMethod: [| createInstanceFrom: string| 
  !feed <- ^self asSelf; 
  !fields <- string fields;


  feed omitEmptyRecordsFlag ifTrue: [
    !empty <- (feed isEmptyRecord: fields) || 
      [ feed isEmptyRecordLocal: fields ];
    empty 
      ifTrue: [ feed defaultInstance; ]
      else:   [ ^self createInstance: fields ]
  ]
  else: [
    ^self createInstance: fields
  ]
];


#####-----
# From patch_102.vis
#####-----


DataFeed defineMethod: [ | cleanupAllSubclassInstances |
  ^self subclassList
    select: [ ^self currentStore masterList count > 0 ] .
 do: [ "... cleaning feed instances for: " print; ^self whatAmI print: 30 ;
         ^self currentStore masterList count printNL ;
         ^self cleanupCurrentInstances ;
     ] . count print ; " feed classes required cleanup." printNL ;
] ;

#####-----
# From patch_103.vis
#####-----

#################
# Description:
#    Revised methods:
#    - DataFeed 'bulkLoadFromFile: fname withConfig: cfgName andBatchSize: bsize fromBatch: bnum to: enum'
#    - DataFeed 'initializeDate'
#
# Description:
#    - DataFeed 'bulkLoadFromFile: fname withConfig: cfgName andBatchSize: bsize fromBatch: bnum to: enum'
#
# DataFeed 'bulkLoadFromFile: fname withConfig: cfgName andBatchSize: bsize fromBatch: bnum to: enum'
#       . If batch size passed in is 0, reset batch size to file size.
#       . The 'headerLineNumber', 'skipTop' and 'asofDateLineNumber' parameters are 
#         expected to be  assigned line numbers which do not include accounting for 
#         preceding or embedded blank or comment lines (lines prefaced by #). Revised 
#         logic to ensure the 'startOfFile' accounts for preceding and embedded blank 
#         and comment line content. The line numbers for 'headerLineNumber and
#         'asofDateLineNumber' are re-derived to account for blank and comment lines.
#       . Fixed logic for 'asofDateLineNumber' to be preserved for 'asofDate' before
#         before re-initialized to 0.
#       . If an empty file, break out altogether and do not attempt batch processing.
#         Examples: 
#           - A 'headerLineNumber' of 1 is correct for a raw file where 4 blank lines 
#             precede the header line.
#           - A 'headerLineNumber' of 1 and an 'asofDateLineNumber' of 2 are correct 
#             where the first line in the raw files is the hader line, followed by two
#             new lines, followed by an as of date line.
#           - A 'skipTop' of 2 is correct for a raw file where the first 3 lines are 
#             comment lines, followed by 2 lines which are copyright lines not prefaced 
#             with a #. 
#           - A 'skipTop' of 4 is correct for a raw file where the first two lines are
#	      copyright lines not prefaced by a #, and where the 'headerLineNumber' is 
#             3 and the 'asofDateLineNumber' is 4.
#          NOTE: The 'skipTop' assignment should be inclusive of the 'headerLineNumber'
#             and the 'asofDateLineNumber' if these latter two parameter values are
#             available in the raw file.
#          NOTE: This method contains two tabs which should be preserved.
#
# DataFeed 'initializeDate'
#       . Revised to support YYYY-MMM-DD and DD-MMM-YYYY formats.
#         Note that MMM can be upper case or lower case or a combination of upper
#         and lower case, i.e., 2009-Feb-28 or 2009-feb-28 or 2009-FEB-28 will all be supported.
#         MMM must be known in the Date MonthDescriptor XRef in order to be resolved.
#
####################

DataFeed defineMethod: [ | bulkLoadFromFile: fname withConfig: cfgName andBatchSize: bsize fromBatch: bnum to: enum |
!feed <- ^self;
newLine print;
"--->  Bulk Loading " print; ^self whatAmI printNL;
"      from file " print; fname printNL;
"      using config " print; cfgName printNL;
"      " print; CoreWorkspace Utility UnixSeconds currentTime printNL;
!file <- "file:" concat: fname . asOpenVisionChannel setTrimFormatToUntrimmed;
!fsize <- file byteCount;

fsize printNL;  
fsize = 0
ifTrue: [ ">>> Empty file or file not found." printNL ].
elseIf: [ cfgName isntDefault && [ cfgName asFileContents isNA ] ]
then: [  ">>> Config file " concat: cfgName .concat: " not found." . printNL; ] .

else: 
    [

    (bsize = 0 || bsize isNA)  ### <=== Account for 0 batch size passed in.
    ifTrue: [ "--->  Batch Size Not Available or 0. Reset. " printNL; :bsize <- fsize; ];  

    !approxBatches <- (fsize / bsize + 1) asInteger;  

    !firstBatch <- bnum else: 0;  
    !lastBatch <- enum else: (approxBatches - 1) .asInteger;  
    ^self loadConfig: cfgName;
    !skipLines <- ^self getSkipLinesAtTop;

    ## -- If a 'fieldOrderList' exists and a header line exists, the 'fieldOrderList' will
    ##    take precedence over the header line.
    ## -- A header line is expected to be available if no 'fieldOrderList' exists.
    ## -- The default value for the headerLineNumber is 1. The 'skipTop' and 
    ##    'asofDateLineNumber' default values are 0.
    ## -- Note that blank lines and comment lines (prefaced with #) which precede the
    ##    'headerLineNumber', the 'asofDateLineNumber' and 'skipTop' should be ignored
    ##    when determining line number assignments for these parameters. This behavior is 
    ##    consistent with DataFeed 'loadFromFile:'. 

    !headerLine <- ^self getHeaderLineNumber else: 1;     ### <=== Moved from below.

    !startOfFile <- "";  
    !count <- 1; 
    !lineIncrement <- 0;

    [ (startOfFile count < fsize) && [ count <= skipLines ] ]  ### <=== Revised
    whileTrue: [

               ## -- If 'skipTop' is set, 'fieldOrderList' should be available.

              !nextLine <- file getLine else: "";
 	      !line <- nextLine 
              extendBy: [
                        !baseLine <- ^self stripBoundingBlanks stripChar: newLine;
                        !skipIt <- (baseLine isBlank || [baseLine contains: "^#"]);
                        ];

              line skipIt 
              ifTrue: [ 
                        :startOfFile <- startOfFile concat: line asSelf;
                        ## -- Increase row number for header line derivation and as of date line derivation.
                        :lineIncrement <- (lineIncrement + 1) asInteger;    ### <=== Added
                      ]
              ifFalse: [
                       :startOfFile <- startOfFile concat: line asSelf; 
                       :count increment;
                       ];
               ];

    !extraOffset <- (startOfFile count max: 1 .- 1) asInteger;
  
    !headerExists <- TRUE;    ### <=== Added
    ^self getGlobalOption: "fieldOrderList" .
    isNA ifTrue: 
              [
                ### <=== Revised block
                :headerLine <- (headerLine + lineIncrement) asInteger;

                ## -- This will address empty files.
                ##    A header line must be available to process any file. If it is not available, exit.
                !line <- startOfFile asLines at: headerLine;  
                line isNA
                ifTrue: [ "--->  Possible empty file: headerLine not available. " printNL;
                          :headerExists <- FALSE;
                        ]; 

                headerExists
                ifTrue: [ 
                        !headers <- line translate: "|	" to: ",";  ## -- Tab.
                        CoreWorkspace GlobalsFeed setGlobalOption: "fieldOrderList" to: headers;
                        ]
              ];

    headerExists
    ifTrue: [

            ### <=== Account for 'asofDate'. Capture it before re-setting 'asOfDateLineNumber' to 0.
            !asofLine <- ^self getAsofDateLineNumber else: 0;
            asofLine > 0
            ifTrue: [ :asofLine <- (asofLine + lineIncrement) asInteger; 
                      !asof <- startOfFile asLines at: asofLine;
                      asof isntNA
                      ifTrue: [ CoreWorkspace GlobalsFeed setGlobalOption: "asofDate" to: asof ]
                    ]; 

	    CoreWorkspace GlobalsFeed setGlobalOption: "headerLineNumber" to: 0 .setGlobalOption: "skipTop" 
	    to: 0 .setGlobalOption: "skipBottom" 
	    to: 0 .setGlobalOption: "maxRecords" 
	    to: 0 .setGlobalOption: "asOfDateLineNumber"
            to: 0;


	    !delimiter <- ^self getGlobalOption: "delimiter" .else: "|"; 
	    (delimiter contains: "|") || [delimiter contains: "	"]  ## -- Tab.
	    ifTrue: [CoreWorkspace GlobalsFeed Globals delete: "delimiter"]; 

	    CoreWorkspace GlobalsFeed enableBulkLoadInProgress; 

	    !globals <- Globals selectorList select: [asString take: 1 .!= "_"].
	    extendBy: [
		        !option <- asSelf; 
		        !setting <- ^my Globals at: asSelf; 
		       ].select: [setting isntNA]; 

	    newLine print; 
	    "    File Size " print: 20; fsize printNL; 
	    "    ~ Batch Size " print: 20; bsize printNL; 
	    "    ~ # of Batches " print: 20; approxBatches printNL; 
	    "    First Batch " print: 20; firstBatch printNL; 
	    "    Last Batch " print: 20; lastBatch printNL; 
	    newLine print; 
	    !rejectFile <- ^self getRejectFileNameFor: fname; 
	    !rejectsFound <- FALSE; 
	    approxBatches sequence0 iterate: [

	        !start <- (^self * ^my bsize + ^my extraOffset + 1) asInteger; 
	        !size <- ^my bsize min: (^my fsize - start + 1) .max: 0 .
	        asInteger; 

	        !string <- (^my file getString: size at: start .else: "") concat: (!extra <- ^my file getLine else: ""); 

                ^my :extraOffset incrementBy: extra count;  

        	newLine print; 
	        "Iteration: " print; ^self print: -5; 
	        " totNetAllocSize: " print; 
	        CoreWorkspace AdminTools 
	        totalNetworkAllocation printWithCommasNL: 20.000000; 
	        ^self >= ^my firstBatch && [
	            ^self <= ^my lastBatch]&& [string count > 0]ifTrue: [
	            newLine print; 
	            "*" fill: 5 .print; 
	            "  starting iteration " print; print; "  -- " print; 
	            CoreWorkspace Utility UnixSeconds currentTime printNL; 
	            "    start     size   string    extra" printNL; 
        	    start print; size print; string count print; extra count 
	            printNL; 
        	    CoreWorkspace GlobalsFeed clearGlobalOptions; 
	            ^my globals 
        	    do: [CoreWorkspace GlobalsFeed setGlobalOption: option to: setting]; 
	            ^my feed updateFromString: :string; 
        	    :string cleanStore; 
	            !status <- ^my feed saveRejectsToFile: ^my rejectFile withLabel: ("Bulk load iteration: " concat: 
        	    asSelf); 
	            status ifTrue: [^my :rejectsFound <- TRUE]; 
        	   ]; 
	       ]; 

	    file close; 
	    newLine print; 
	    "--->  End of Bulk Loading " print; ^self whatAmI printNL; 
	    "      from file " print; fname printNL; 
	    rejectsFound ifTrue: ["      rejects in " print; rejectFile printNL]; 
	    "      " print; CoreWorkspace Utility UnixSeconds currentTime printNL; 
	    CoreWorkspace GlobalsFeed disableBulkLoadInProgress; 
	    ^self isEntityExtenderFeed ifTrue: [ ^self resetRunTimeAttributes ]; 

            ]
ifFalse: [ ### <=== Added for headerExists isFalse
           file close;
           ^self isEntityExtenderFeed ifTrue: [ ^self resetRunTimeAttributes ]; 
         ]
   ]
]; 



DataFeed defineMethod: [ | initializeDate |
!realDate <- date isString 
ifTrue: [
    date toUpper = "DEFAULT" ifTrue: [ earliestPossibleDate ]
    ifFalse: 
        [
        !format <- getDateFormat else: "" .stripBoundingBlanks; 
        !paddedDate <- "0" fill: format count .
        concat: date .take: -1 * format count; 
        format = "MMYYYY" || format = "MMYY" 
        ifTrue: [
                !mm <- paddedDate take: 2 .asNumber asInteger; 
                !year <- paddedDate drop: 2 .asNumber asInteger; 
                year isNumber && mm isNumber 
                ifTrue: [ year asDateFromYearForMonth: mm  andDay: 1 . + 0 monthEnds]
                ] .
        elseIf: [ format = "MMDDYY" || format = "MMDDYYYY" ]
        then: [
              !mm <- paddedDate take: 2 .asNumber asInteger; 
              !dd <- paddedDate drop: 2 .take: 2 .asNumber asInteger; 
              !year <- paddedDate drop: 4 .asNumber asInteger; 
              year isNumber && mm isNumber && dd isNumber 
              ifTrue: [ year asDateFromYearForMonth: mm andDay: dd ]
              ] .
        elseIf: [ format = "YYYYMM" || format = "YYYYMMXX" ]
        then: [
              !yyyy <- paddedDate take: 4 .asNumber asInteger; 
              !mm <- paddedDate drop: 4 .take: 2 .asNumber asInteger; 
              yyyy isNumber && mm isNumber 
              ifTrue: [yyyy asDateFromYearForMonth: mm andDay: 1 . + 0 monthEnds ] 
              ] .
        elseIf: [ format = "MM/YYYY" ]
        then: [
              !fields <- paddedDate breakOn: "/-"; 
              !mm <- fields at: 1 .asNumber asInteger; 
              !year <- fields at: 2 .asNumber asInteger; 
              year isNumber && mm isNumber 
              ifTrue: [ year asDateFromYearForMonth: mm andDay: 1 . + 0 monthEnds ]
              ] .
        elseIf: [ format = "DD-MMM-YYYY" || format = "YYYY-MMM-DD" ]
        then: [
              !fields <- paddedDate breakOn: "/-" ;
              !dd; !mm; !yyyy;
              !mmMonth <- CoreWorkspace Date MonthDescriptor XRef at: (fields at: 2 . toUpper) ;
              mmMonth isntNA ifTrue: [ :mm <- mmMonth month ];

              (format = "DD-MMM-YYYY")
              ifTrue: [
                      :dd <- fields at: 1 . asNumber asInteger;
                      :yyyy <- fields at: 3 . asNumber asInteger;
                      ]
              else: [
                    :dd <- fields at: 3 . asNumber asInteger;
                    :yyyy <- fields at: 1 . asNumber asInteger;
                    ];

              mm isNumber && [ dd isNumber ] && [ yyyy isNumber ]
              ifTrue: [ yyyy asDateFromYearForMonth: mm andDay: dd ]
              ] . 
        elseIf: [ ^self purgeEnabled not ]
        then: [ date asDate ] .
        elseIf: [ date contains: ":" . not ]
        then: [ date asDate ] .
        ]
      ]; 
:date <- realDate isDate || realDate isTime ifTrue: [ realDate ]
ifFalse: [ ^self purgeEnabled 
           ifTrue: [ date ]
           ifFalse: [ ^self asofDate ]
         ]; 
^self
]; 



#####-----
# From patch_105.vis
# NOTE: INIfeeds.G must be installed first to create the IndexedDataRecord and IndexedDataRecordFeed classes
#####-----

#--------------------
#  Add new method to create an update property that is flagged as
#     local to the feed
#--------------------
DataFeed defineMethod: [ | addLocalProperty: property withDefault: d |
  ^self defineFixedProperty: property withDefault: d ;
  ^self getMessage: property asString . enableOkayToUpload ;
  ^self
] ;


#----------
#  redefine prints
#----------
IndexedDataRecord defineMethod: [ | print | 
  ^self entity code concat: "-" . concat: ^self index code  . print ;
  ^self
]  ;

IndexedDataRecord defineMethod: [ | print: f | 
 ^self entity code concat: "-" . concat: ^self index code . print: f ;
 ^self
]  ;

#--------------------
#  initialization methods
#--------------------

IndexedDataRecord defineMethod: [ | initializeKeys: keys |
:entity <- keys at: 1 . asSelf ;
:index <- keys at: 2 . asSelf ;
:recordValue <- keys at: 3 . asSelf ;
:baseCurrency <- keys at: 4 . asSelf ;
^self
] ;

IndexedDataRecord defineMethod: [ | initializeLocalAttributes |
#--  don't run the ^super initializeLocalAttributes - it will
#--    use the updateBlock and wipe out the list
#-- ^super initializeLocalAttributes ;

:recordDate <- earliestPossibleDate ;

#--  don't do this next step until we are sure that the
#--  indexed list is a non-default instance, created once
#--^self updateBlock isntNA && index isntDefault
#--    ifTrue: [ entity send: ^self updateBlock . value 
#--                 at: index asSelf put: ^self asSelf
#--            ] ;
 ^self
] ;

IndexedDataRecordFeed defineMethod: [ | initializeProcessing |
!feed <- ^self currentStore ;

#--  initialize local properties
feed masterList
do: [ :entity <- entityId as: baseEntity ;
      :indexEntity <- indexId as: baseClass indexType ;
      :evaluationDate <- getEvaluationDate ;
    ] ;

#--  Create missing IndexedLists as needed and initialize underlyingRecord
!todo <- feed masterList 
    select: [ entity isntDefault && indexEntity isntDefault ] ;
!drecClass <- ^self baseClass ;
!updateBlock <- drecClass updateBlock ;
!defaultXRef <- ^self baseEntity send: updateBlock . value ;
todo groupedBy: [ entity ] .
do: [ !xref <- ^self send: ^my updateBlock . value ;
      xref  = ^my defaultXRef
         ifTrue: [ :xref <- ^my defaultXRef clusterNew ;
                   ^self send: ^my updateBlock . <- xref ;
                 ] ;
      !drecClass <- ^my drecClass ;
      groupList groupedBy: [ indexEntity ] .
      do: [ !rec <- ^my xref at: ^self asSelf .
               else: [ !newDR <- ^my drecClass currentStore
                         createInstance:^my asSelf, ^self asSelf ;
                       ^my xref at: ^self asSelf put: newDR ;
                       newDR
                     ] ;
            groupList do: [ :underlyingRecord <- ^my rec ] ;
          ] ;
     ] ;
] ;


####################
#   IndexedDataRecordFeed protocol
####################

#--------------------
#  Initialization protocol for IndexedDataRecordFeed
#--------------------

IndexedDataRecordFeed addLocalProperty: 'indexId' withDefault: "" ;








