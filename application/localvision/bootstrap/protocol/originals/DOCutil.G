Utility getMessage: "UnixSeconds" .
  setDescriptionTo: "Returns the Utility UnixSeconds class." . 
  setReturnObjectTypeTo: Utility UnixSeconds . 
  setFunctionTypeTo: "Constant" . 
;

Utility getMessage: "asSelf" .
  setDescriptionTo: "Returns the default Utility instance." . 
  setReturnObjectTypeTo: Utility . 
;

Utility getMessage: "basicUpdateNetwork" .
  setDescriptionTo: "Primitive version of object space update message." . 
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
;

Utility getMessage: "classDescriptor" .
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

Utility getMessage: "collectGarbage" .
  setDescriptionTo: "Initiates network Garbage Collection process." .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
;
Utility getMessage: "collectSessionGarbage" .
  setDescriptionTo: "Initiates transient Garbage Collection process." .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
;

Utility getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" . 
;

Utility getMessage: "echoToFile:" .
  setSelectorTo: "echoToFile: fileName " . 
  setDescriptionTo: 
"Opens the supplied file name for output.  All subsequent output is
displayed on standard output and in this file until the 'stopEchoToFile'
message is encountered." . 
  setFunctionTypeTo: "Utility" . 
;

Utility getMessage: "faultSeverities" .
  setFunctionTypeTo: "Primitive" . 
  setLevelTo: "Internal" . 
;

Utility getMessage: "isUtility" .
  setDescriptionTo: 
"Returns the value TRUE when sent to Utility." . 
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

Utility getMessage: "restartSession" .
  setDescriptionTo: 
"Running this primitive will replace the current session with a
fresh session started with the same options as the original. The
process id will be the same. The input and output channels will be the
same. All uncommitted modifications will be lost. Because the NDF
will be reread, the latest version of the network will be
accessed after this primitive is executed, unless batchvision was
invoked with the -v option. In that case, the same version of the
object network will be accesed.  " .
  setFunctionTypeTo: "Utility" . 
;

Utility getMessage: "stopEchoToFile" .
  setDescriptionTo: 
"Closes the file currently open for output.  All output continues to
display on the standard output." . 
  setFunctionTypeTo: "Utility" . 
;

Utility getMessage: "systemErrorCount" .
  setFunctionTypeTo: "Inquiry" . 
  setLevelTo: "Advanced" . 
  setDescriptionTo: 
"Returns Number of 'hard' errors encountered during current session." . 
  setReturnObjectTypeTo: Integer .
;

Utility getMessage: "systemWarningCount" .
  setFunctionTypeTo: "Inquiry" . 
  setLevelTo: "Advanced" . 
  setDescriptionTo: 
"Returns Number of 'warning' errors encountered during current session." . 
  setReturnObjectTypeTo: Integer .
;

Utility getMessage: "updateNetwork" .
  setDescriptionTo: 
"Updates the network with data and protocol defined during this session.
Depending on type of user, the update may be global or private." . 
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
;

Utility getMessage: "whatAmI" .
setDescriptionTo: "Returns the name of the recipient's class." .
setReturnObjectTypeTo: String.
;


Utility UnixSeconds  getMessage: "!=" .
  setSelectorTo: "!= object " . 
  setDescriptionTo: 
"Returns TRUE if recipient is not equal to supplied object." . 
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Object.
;

Utility UnixSeconds  getMessage: "<" .
  setSelectorTo: "< object " . 
  setDescriptionTo: 
"Returns TRUE if recipient is less than supplied object." . 
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Object.
;

Utility UnixSeconds  getMessage: "<=" .
  setSelectorTo: "<= object " . 
  setDescriptionTo: 
"Returns TRUE if recipient is less than or equal to supplied object." . 
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Object.
;

Utility UnixSeconds  getMessage: "=" .
  setSelectorTo: "= object " . 
  setDescriptionTo: 
"Returns TRUE if recipient is equal to supplied object." . 
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Object.
;

Utility UnixSeconds  getMessage: ">" .
  setSelectorTo: "> object " . 
  setDescriptionTo: 
"Returns TRUE if recipient is greater than supplied object." . 
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Object.
;

Utility UnixSeconds  getMessage: ">=" .
  setSelectorTo: ">= object " . 
  setDescriptionTo: 
"Returns TRUE if recipient is greater than or equal to supplied object." . 
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Object.
;

Utility UnixSeconds  getMessage: "asInteger" .
  setDescriptionTo: 
"Converts UnixSeconds into an Integer." . 
  setFunctionTypeTo: "Conversion" . 
;

Utility UnixSeconds  getMessage: "asSelf" .
setDescriptionTo: "Returns the original object, stripped of extensions.".
setReturnObjectTypeTo: Utility UnixSeconds .
setFunctionTypeTo: "Conversion".
;

Utility UnixSeconds  getMessage: "classDescriptor" .
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

Utility UnixSeconds  getMessage: "currentTime" .
  setDescriptionTo: "Returns the current time." . 
  setReturnObjectTypeTo: Utility UnixSeconds .
  setFunctionTypeTo: "Inquiry" . 
;

Utility UnixSeconds  getMessage: "defaultInstance" .
  setDescriptionTo: 
"Returns the default instance of the recipient object's class" . 
  setFunctionTypeTo: "Schema" . 
  setReturnObjectTypeTo: Utility UnixSeconds .
;

Utility UnixSeconds  getMessage: "formatAsGreenwichMeanTime" .
  setDescriptionTo: "Formats timestamp in greenwich mean time." . 
  setFunctionTypeTo: "Format" . 
;

Utility UnixSeconds  getMessage: "formatAsLocalTime" .
  setDescriptionTo: "Formats timestamp in local time." . 
  setFunctionTypeTo: "Format" . 
;

Utility UnixSeconds  getMessage: "isUnixSeconds" .
  setSelectorTo: "isUnixSeconds" . 
  setDescriptionTo: "Returns TRUE when sent to a UnixSeconds object." . 
  setFunctionTypeTo: "Inquiry" . 
;

Utility UnixSeconds  getMessage: "passCurrentTimeToCollector:" .
  setFunctionTypeTo: "Primitive" . 
  setLevelTo: "Internal" . 
;

Utility UnixSeconds  getMessage: "passDecodedGreenwichMeanTimeToCollector:" .
  setFunctionTypeTo: "Primitive" . 
  setLevelTo: "Internal" . 
;

Utility UnixSeconds  getMessage: "passDecodedLocalTimeToCollector:" .
  setFunctionTypeTo: "Primitive" . 
  setLevelTo: "Internal" . 
;

Utility UnixSeconds  getMessage: "print" .
  setDescriptionTo: 
"Prints UnixSeconds in 'Wed Jan 20 14:25:50 EST 1993' format" . 
  setFunctionTypeTo: "Display" .
;

Utility UnixSeconds  getMessage: "printDecodedGreenwichMeanTime" .
  setDescriptionTo: "Prints UnixSeconds details in greenwich mean time." .
  setFunctionTypeTo: "Display" .
;

Utility UnixSeconds  getMessage: "printDecodedLocalTime" .
  setDescriptionTo: "Prints UnixSeconds details in local time." .
  setFunctionTypeTo: "Display" .
;

Utility UnixSeconds  getMessage: "whatAmI" .
setDescriptionTo: "Returns the name of the recipient's class." .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;


#--------------------------------------------------

Utility getMessage: "SessionAttribute" .
setDescriptionTo: 
 "Supports parameterized control of session update properties." .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
;

Utility getMessage: "createSpaceInNetworkWithAnnotation:" .
setDescriptionTo: 
"Creates new object space in network and saves the supplied string in the ndf." .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
  setParameter: 1 typeTo: String .
;

Utility getMessage: "updateNetworkWithAnnotation:" .
setDescriptionTo: 
"Performs network update and saves the supplied string in the ndf." .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
  setParameter: 1 typeTo: String .
;

Utility getMessage: "updateAndCompactNetworkWithAnnotation:" .
setDescriptionTo: 
"Performs network update and the copy phase of the compaction process,
saving the supplied annotation in the NDF." .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
  setParameter: 1 typeTo: String .
;

Utility getMessage: "updateNetworkAsBaseVersionWithAnnotation:" .
setDescriptionTo: 
"Performs network update and the base version phase of the compaction process,
saving the supplied annotation in the NDF." .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
  setParameter: 1 typeTo: String .
;

Utility getMessage: "updateAndCompactNetworkAsBaseVersionWithAnnotation:" .
setDescriptionTo: 
"Performs network update and the copy and base version phases of the
compaction process, saving the supplied annotation in the NDF." .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
  setParameter: 1 typeTo: String .
;

Utility getMessage: "updateAndCompact" .
setDescriptionTo: 
"Performs the update and the copy phase of the compaction process,
with no special annotation." .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
;

Utility getMessage: "fullCompact" .
setDescriptionTo: 
"Compacts all spaces performing the copy and base version phases
and generating all traces." .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
;

Utility getMessage: "fullCompactOf:" .
setSelectorTo: "fullCompactOf: spaceList" .
setDescriptionTo: 
"Compacts all spaces in supplied list performing the copy and base version 
phases and generating all traces." .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
;

Utility getMessage: "fullCompactExcept:" .
setSelectorTo: "fullCompactExcept: spaceList" .
setDescriptionTo: 
"Compacts all spaces except those in supplied list performing the copy and 
base version phases and generating all traces." .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
;

Utility SessionAttribute getMessage: "updateAnnotation" .
setDescriptionTo: 
"Session attribute property is assigned a string containg unique 
annotation that describes the save transaction.  The value of this 
attribute is erased by a successful update and threfore must be
reset prior to the next update.  " .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
;

Utility SessionAttribute getMessage: "compactOnUpdate" .
setDescriptionTo: 
"While this attribute is TRUE, the default behavior of an update is
to peform the copy phase of the compaction on all object spaces modfied
and writeable as part of the update.  This flag can be set explicitly,
or access via one of the update covers methods.  If accessed via a cover 
method, it applies only to the update performd by the cover method.  If 
set explicitly, this attribute applies to all subsequent updates until
it is either cleared explicitly or rest by a cover method." .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
;

Utility SessionAttribute getMessage: "makeBaseVersionOnUpdate" .
setDescriptionTo: 
"Updates peformed while this attribute is TRUE, create base versions.
Creating a base version causes th generation of MSS files for each space.
This attribute can be set explicitly or access via one of the update
cover methods.  If set explicitly, this attribute applies to all subsequent
updates until it is either cleared explicitly or rest by a cover method." .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
;

Utility SessionAttribute getMessage: "traceCompaction" .
setDescriptionTo: 
"This attribute causes the compactor to generate a report of its activity." .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
;

Utility SessionAttribute getMessage: "traceUpdate" .
#setDescriptionTo: 
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
;

Utility SessionAttribute getMessage: "validatingChecksums" .
setDescriptionTo: 
"A segment's checksum is computed as th segment is created and is
validated by re-reading the segment immediately after the operating
system reports that its contents are on disk.  If the checksum fails
validation, the update is aborted.  By default, checksum validation
is enabled.  Becasue validation is potentially expensive, it can be
controlled.  This attribute can be set to FALSE to disable the
checksum." .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
;

Utility SessionAttribute getMessage: "updateFlagOfSpace:" .
setDescriptionTo: 
"Forces an unmodified space to be included in compaction if set to TRUE." .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
  setParameter: 1 typeTo: Integer .
;

Utility SessionAttribute getMessage: "compactionFlagOfSpace:" .
setDescriptionTo: 
"Forces a modified space to be excluded from compaction if set to FALSE." .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
  setParameter: 1 typeTo: Integer .
;

Utility SessionAttribute getMessage: "mssOverrideOfSpace:" .
setDescriptionTo: 
"Sets MSS override for supplied object space to integer value
provided (e.g., Utility SessionAttribute mssOverrideOfSpace: 3 . <- 1234).
If the segment is invalid for the specified object space, the
request is ignored and replaced by the segment chosen by the
compactor.
Forces a modified space to be excluded from compaction if set to FALSE." .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
  setParameter: 1 typeTo: Integer .
;

Utility SessionAttribute getMessage: "copyCoefficientOfSpace:" .
setDescriptionTo: 
"Sets the copy coeffecient for the compaction selection algorithm to
supplied number 
   (e.g., Utility SessionAttribute copyCoefficientOfSpace: 3 . <- 1.5).
The default value is 1.  Decreasing the copy coefficient favors
reclaiming space and should generate larger MSS values.  Increasing
the copy coefficient favors leaving objects where they are and should
generate smaller MSS values." .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
  setParameter: 1 typeTo: Integer .
;

Utility SessionAttribute getMessage: "reclaimCoefficientOfSpace:" .
setDescriptionTo: 
"Sets the reclaim coeffecient for the compaction selection algorithm to
supplied number 
   (e.g., Utility SessionAttribute copyCoefficientOfSpace: 3 . <- .75).
The default value is 1.  Increasing the reclaim coefficient favors
reclaiming space and should generate larger MSS values.  Decreasing
the reclaim coefficient favors leaving objects where they are and should
generate smaller MSS values." .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
  setParameter: 1 typeTo: Integer .
;

Utility SessionAttribute getMessage: "maxCompactionSegmentsOfSpace:" .
setDescriptionTo: 
"This attribute is used to limit the number of compaction segments
generated in a space during a single update.  For example:

    Utility SessionAttribute maxCompactionSegmentsOfSpace: 3 . <- 10 ;

limits to 10 the number of segments of 'targetSegmentSizeOfSpace:'
that will be created to hold compacted data in subsequent compactions
of space 3 performed in this session.  The compactor uses this
statistic by first computing an MSS in the normal way.  If more than
the product of 'targetSegmentSize' and 'maxCompactionSegments' bytes
would be be copied using the normal MSS, the MSS will be adjusted
downward until the amount copied is less than this product.  Note
that this rule implies that it is impossible to copy part of a
segment during a compaction.  The default value of this attribute is
2 billion.  The 'VisionMaxCompSegs' environment variable overrides
this default for all spaces.  This attribute does not apply to the
stand-alone compactor or if a specific MSS has been selected.
" .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
  setParameter: 1 typeTo: Integer .
;

Utility SessionAttribute getMessage: "targetSegmentSizeOfSpace:" .
setDescriptionTo: 
"This attribute is used to control the size of segments created by updates.
The attribute is object space specific.  For example:

    Utility SessionAttribute targetSegmentSizeOfSpace: 3 . <- 1000000 ;

sets the target size of new object space 3 segments created in this
session.  This example sets that size to 1 megabyte.  If more than
1MB of data must be written to space 3 by this session in a single
update, it will be divided into as many 1MB segments as are needed to
accomodate the update.  The value set by this tuning parameter is a
target and not a maximum.  Individual segments can exceed this value
to accomodate the last structure written to the segment.  The default
value of this attribute is 2GB.  The 'VisionMaxSegSize' environment
variable overrides this default value for all spaces.  " .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
  setParameter: 1 typeTo: Integer .
;

Utility SessionAttribute getMessage: "isSessionAttribute" .
setDescriptionTo: "Returns TRUE if recipient object is a SessionAttribute" .
  setFunctionTypeTo: "Inquiry" . 
;

Utility SessionAttribute getMessage: "traceUpdate" .
setDescriptionTo: "" .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
;

Utility SessionAttribute getMessage: "largeContainerSizeOfSpace:" .
setDescriptionTo: "" .
  setFunctionTypeTo: "DBAdmin" . 
  setLevelTo: "Advanced" . 
;

Utility SessionAttribute getMessage: "visionMaxWriteChunk" .
   setDescriptionTo: "" .
   setFunctionTypeTo: "DBAdmin" .
   setLevelTo: "Advanced" .
;
Utility SessionAttribute getMessage: "visionMappingLimit" .
   setDescriptionTo: "" .
   setFunctionTypeTo: "DBAdmin" .
   setLevelTo: "Advanced" .
;
Utility SessionAttribute getMessage: "visionAddressThreshold" .
   setDescriptionTo: "" .
   setFunctionTypeTo: "DBAdmin" .
   setLevelTo: "Advanced" .
;
Utility SessionAttribute getMessage: "visionNSyncRetries" .
   setDescriptionTo: "" .
   setFunctionTypeTo: "DBAdmin" .
   setLevelTo: "Advanced" .
;
Utility SessionAttribute getMessage: "visionSSyncRetries" .
   setDescriptionTo: "" .
   setFunctionTypeTo: "DBAdmin" .
   setLevelTo: "Advanced" .
;
Utility SessionAttribute getMessage: "visionSOpenRetries" .
   setDescriptionTo: "" .
   setFunctionTypeTo: "DBAdmin" .
   setLevelTo: "Advanced" .
;
Utility SessionAttribute getMessage: "visionSORetryDelay" .
   setDescriptionTo: "" .
   setFunctionTypeTo: "DBAdmin" .
   setLevelTo: "Advanced" .
;
Utility SessionAttribute getMessage: "visionNetOpenTrace" .
   setDescriptionTo: "" .
   setFunctionTypeTo: "DBAdmin" .
   setLevelTo: "Advanced" .
;
Utility SessionAttribute getMessage: "visionLargeTaskSize" .
   setDescriptionTo: "" .
   setFunctionTypeTo: "DBAdmin" .
   setLevelTo: "Advanced" .
;
Utility SessionAttribute getMessage: "visionStackDump" .
   setDescriptionTo: "" .
   setFunctionTypeTo: "DBAdmin" .
   setLevelTo: "Advanced" .
;
Utility SessionAttribute getMessage: "visionGRMEnabled" .
   setDescriptionTo: "" .
   setFunctionTypeTo: "DBAdmin" .
   setLevelTo: "Advanced" .
;
Utility SessionAttribute getMessage: "visionGRMTrace" .
   setDescriptionTo: "" .
   setFunctionTypeTo: "DBAdmin" .
   setLevelTo: "Advanced" .
;
Utility SessionAttribute getMessage: "visionAttemptDefragment" .
   setDescriptionTo: "" .
   setFunctionTypeTo: "DBAdmin" .
   setLevelTo: "Advanced" .
;
Utility SessionAttribute getMessage: "visionRestartOnError" .
   setDescriptionTo: "" .
   setFunctionTypeTo: "DBAdmin" .
   setLevelTo: "Advanced" .
;
Utility SessionAttribute getMessage: "visionExitOnError" .
   setDescriptionTo: "" .
   setFunctionTypeTo: "DBAdmin" .
   setLevelTo: "Advanced" .
;
Utility SessionAttribute getMessage: "verboseSelectorNotFound" .
   setDescriptionTo: "" .
   setFunctionTypeTo: "DBAdmin" .
   setLevelTo: "Advanced" .
;

Utility SessionAttribute getMessage: "enableVerboseSelectorNotFound" .
   setDescriptionTo: "enables verbose selector not found reporting." .
   setFunctionTypeTo: "Diagnostic" .
;
Utility SessionAttribute getMessage: "disableVerboseSelectorNotFound" .
   setDescriptionTo: "disables verbose selector not found reporting." .
   setFunctionTypeTo: "Diagnostic" .
;

Utility getMessage: "hostname" .
  setDescriptionTo: "returns symbolic name of host machine" .
  setFunctionTypeTo: "Descriptive" .
  setReturnObjectTypeTo: String .
;
Utility getMessage: "username" .
  setDescriptionTo: "" .
  setFunctionTypeTo: "Descriptive" .
  setReturnObjectTypeTo: String .
;
Utility getMessage: "hostId" .
  setDescriptionTo: "" .
  setFunctionTypeTo: "Descriptive" .
  setReturnObjectTypeTo: Integer .
;
Utility getMessage: "accessedNetworkVersion" .
  setDescriptionTo: "NDF version number at start of session" .
  setFunctionTypeTo: "Descriptive" .
  setReturnObjectTypeTo: Integer .
;
Utility getMessage: "currentNetworkVersion" .
  setDescriptionTo: "NDF version number of last save performed in this session" .
  setFunctionTypeTo: "Descriptive" .
  setReturnObjectTypeTo: Integer .
;
Utility getMessage: "processId" .
  setDescriptionTo: "operating system process id of this session" .
  setFunctionTypeTo: "Descriptive" .
  setReturnObjectTypeTo: Integer .
;

