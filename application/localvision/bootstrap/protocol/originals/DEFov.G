################
####  OpenVision
################

OpenVision

################
#  Process Information
################

define: 'HostName'		toBe: 510 asPrimitive.

defineMethod:
[ | ActiveChannel |
    ^self __ActiveChannel asPointerTo: Channel
].
define: '__ActiveChannel'	toBe: 512 asPrimitive.


################
#  'rexec'
################

defineMethod:
[ | rexec: command onHost: host asUser: username withPassword: password |
    !errorService <- "inet/stream,p:0" asOpenVisionChannel;
    !errorPort    <- errorService socketName breakOn: ":". at: 2;

    !primaryChannel <- "inet/stream:" concat: host
    . concat: ":exec"
    . asOpenVisionChannel;

    primaryChannel do: [
	^self putString: ^my errorPort; ^self putByte: 0;
	^self putString: ^my username ; ^self putByte: 0;
	^self putString: ^my password ; ^self putByte: 0;
	^self putString: ^my command  ; ^self putByte: 0;
    ];

    !errorChannel <- errorService acceptConnection;
    [ errorChannel isReady isFalse && errorService isReady ] whileTrue: [
	errorService wait;
	:errorChannel <- errorService acceptConnection;
    ];
    errorService close;

    primaryChannel setErrorInputTo: errorChannel;
    errorChannel release;

    !status <- primaryChannel getByte;
    [ status isNA && [ primaryChannel isntAtEndOfInput ] ] whileTrue: [
	primaryChannel wait;
	:status <- primaryChannel getByte;
    ];

    status = 0 ifTrue: [
	primaryChannel
    ] ifFalse: [
	!message <- primaryChannel getString else: "";
	[ primaryChannel isntAtEndOfInput ] whileTrue: [
	    primaryChannel wait;
	    :message <- message concat: (primaryChannel getString else: "");
	];
	primaryChannel close;
	message
    ]
].


################
####  Vision 'rexec'
################

defineMethod:
[ | rexecVisionOnHost: host asUser: user withPassword: password |
    ^self rexecVision: "/vision/bin/batchvision"
               onHost: host
               asUser: user
         withPassword: password
           andOptions: ""
].

defineMethod:
[ | rexecVisionOnHost: host asUser: user withPassword: password andOptions: options |
    ^self rexecVision: "/vision/bin/batchvision"
               onHost: host
               asUser: user
         withPassword: password
           andOptions: options
].

defineMethod:
[ | rexecVision: executable
           onHost: host
           asUser: user
     withPassword: password
       andOptions: options |

    !channelOrMessage <- ^self
             rexec: (executable concat: " ". concat: options)
            onHost: host
            asUser: user
      withPassword: password
    ;

    channelOrMessage isOpenVisionChannel ifTrue: [
	channelOrMessage consumeVisionPrompt
    ];

    channelOrMessage
].
;


################
####  OpenVision Channel
################

OpenVision Channel
defineMethod:
[ | asOpenVisionChannel |
    ^self
].

define: 'isOpenVisionChannel' toBe: TRUE.

define: 'whatAmI' toBe: "OpenVision Channel".

################
#  Creation
################

defineMethod:
[ | acceptConnection |
    ^self __acceptConnection asPointerTo: super
].
define: '__acceptConnection'	toBe: 531 asPrimitive.

################
#  Enumeration
################

defineMethod:
[ | instanceList |
    __instanceListCount sequence send: [
	^self asOpenVisionChannel
    ]. select: [index > 0]
].

define: '__instanceListCount'	toBe: 513 asPrimitive.


################
#  Display
################

defineMethod:
[ | displayInfo |
    ^self print; " " print;
    ^self channelType   print: 12;
    ^self tcpNoDelay    print: -2;# added
    ^self binaryFormat  print: 12;
    ^self promptFormat  print: 12;
    ^self state         print: 12;
    ^self status        print: 12;
    ^self lastError     printNL;
].

defineMethod:
[ | print |
    "OpenVisionChannel_" concat: [^self index print] divertOutput stripBoundingBlanks. print;
    ^self
].


################
#  Wait
################

defineMethod:
[ | wait |
    ^self __wait: -1
].

defineMethod:
[ | wait: aNumberOfSeconds |
    ^self __wait: (aNumberOfSeconds * 1000000) asInteger
].

define: '__wait:'		toBe: 535 asPrimitive.

################
#  Shutdown
################

define: 'endTransmission'	toBe: 536 asPrimitive.
define: 'endReception'		toBe: 537 asPrimitive.

################
#  Close
################

define: 'close'			toBe: 538 asPrimitive.


################
#  Index Query
################

define: 'index'			toBe: 540 asPrimitive.


################
#  State Query
################

defineMethod:
[ | state |
    ^self __state asPointerTo: State
].

defineMethod:
[ | isUnused |
    state = State Unused
].

defineMethod:
[ | isReady |
    state = State Ready
].

defineMethod:
[ | isStarting |
    state = State Starting
].

defineMethod:
[ | isRestarting |
    state = State Restarting
].

defineMethod:
[ | isRunning |
    state = State Running
].

defineMethod:
[ | isClosing |
    state = State Closing
].

define: '__state'		toBe: 541 asPrimitive.


################
#  Status Query
################

defineMethod:
[ | status |
    ^self __status asPointerTo: Status
].

defineMethod:
[ | isntAtEndOfInput |
    status != Status Closed
].

defineMethod:
[ | isAtEndOfInput |
    status = Status Closed
].

define: '__status'		toBe: 542 asPrimitive.


################
#  Last Error Query
################

defineMethod:
[ | lastError |
    ^self __lastError asPointerTo: Error
].

define: '__lastError'		toBe: 543 asPrimitive.


################
#  Secondary Channel Query and Maintenance
################

define: 'errorOutput'		toBe: 545 asPrimitive.
define: 'errorInput'		toBe: 549 asPrimitive.

define: 'retain'		toBe: 560 asPrimitive.
define: 'release'		toBe: 561 asPrimitive.

define: 'setErrorOutputTo:'	toBe: 565 asPrimitive.
define: 'setErrorInputTo:'	toBe: 569 asPrimitive.

defineMethod:
[ | becomeErrorOutputOf: primaryChannel |
    primaryChannel setErrorOutputTo: super
].


################
#  Channel Type Query
################

defineMethod:
[ | channelType |
    ^self __channelType asPointerTo: ChannelType
].

defineMethod:
[ | isAFileChannel |
    channelType = ChannelType File
].

defineMethod:
[ | isAServiceChannel |
    channelType = ChannelType Service
].

defineMethod:
[ | isAStreamChannel |
    channelType = ChannelType Stream    
].

define: '__channelType'		toBe: 550 asPrimitive.


################
#  Socket Name/Peer Name Query
################

define: 'socketName'		toBe: 551 asPrimitive.
define: 'peerName'		toBe: 552 asPrimitive.


################
#  General Option Query And Maintenance
################

defineMethod:
[ | option |
    ^self __option asPointerTo: Option
].

define: '__option'		toBe: 546 asPrimitive.
define: '__optionValue'		toBe: 547 asPrimitive.

define: 'setOptionTo:'		toBe: 566 asPrimitive.
define: 'setOptionValueTo:'	toBe: 567 asPrimitive.


################
#  Prompt Format Query And Maintenance
################

defineMethod:
[ | promptFormat |
    ^self setOptionToPromptFormat __optionValue asPointerTo: PromptFormat
].

defineMethod:
[ | setPromptFormatToNormal |
    ^self setPromptFormatTo: PromptFormat Normal
].

defineMethod:
[ | setPromptFormatToEditor |
    ^self setPromptFormatTo: PromptFormat Editor
].

defineMethod:
[ | setPromptFormatTo: value |
    ^self setOptionToPromptFormat setOptionValueTo: value
].

defineMethod:
[ | setOptionToPromptFormat |
    ^self setOptionTo: Option PromptFormat
].


################
#  Binary Format Query And Maintenance
################

defineMethod:
[ | binaryFormat |
    ^self setOptionToBinaryFormat __optionValue asPointerTo: BinaryFormat
].

defineMethod:
[ | setBinaryFormatToUntranslated |
    ^self setBinaryFormatTo: BinaryFormat Untranslated
].

defineMethod:
[ | setBinaryFormatToPARisc |
    ^self setBinaryFormatTo: BinaryFormat PARisc
].

defineMethod:
[ | setBinaryFormatToSparc |
    ^self setBinaryFormatTo: BinaryFormat Sparc
].

defineMethod:
[ | setBinaryFormatToIntel |
    ^self setBinaryFormatTo: BinaryFormat Intel
].

defineMethod:
[ | setBinaryFormatTo: value |
    ^self setOptionToBinaryFormat setOptionValueTo: value
].

defineMethod:
[ | setOptionToBinaryFormat |
    ^self setOptionTo: Option BinaryFormat
].


################
#  Trim Format Query And Maintenance
################

defineMethod:
[ | trimFormat |
    ^self setOptionToTrimFormat __optionValue asPointerTo: TrimFormat
].

defineMethod:
[ | setTrimFormatToUntrimmed |
    ^self setTrimFormatTo: TrimFormat Untrimmed
].

defineMethod:
[ | setTrimFormatToLeadingBlanks |
    ^self setTrimFormatTo: TrimFormat TrimLeadingBlanks
].

defineMethod:
[ | setTrimFormatToTrailingBlanks |
    ^self setTrimFormatTo: TrimFormat TrimTrailingBlanks
].

defineMethod:
[ | setTrimFormatToBoundingBlanks |
    ^self setTrimFormatTo: TrimFormat TrimBoundingBlanks
].

defineMethod:
[ | setTrimFormatTo: value |
    ^self setOptionToTrimFormat setOptionValueTo: value
].

defineMethod:
[ | setOptionToTrimFormat |
    ^self setOptionTo: Option TrimFormat
].

################
#  tcp no delay
################

defineMethod:
[ | tcpNoDelay |
    ^self setOptionToTCPNoDelay __optionValue 
].

defineMethod:
[ | turnTCPNoDelayOn |
    ^self setTCPNoDelayTo: 1
].

defineMethod:
[ | turnTCPNoDelayOff |
    ^self setTCPNoDelayTo: 0
].

defineMethod:
[ | setTCPNoDelayTo: value |
    ^self setOptionToTCPNoDelay setOptionValueTo: value
].

defineMethod:
[ | setOptionToTCPNoDelay |
    ^self setOptionTo: Option TCPNoDelay
].




################
#  Seek Offset Query and Maintenance
################

define: 'seekOffset'		toBe: 548 asPrimitive.
define: 'setSeekOffsetTo:'	toBe: 568 asPrimitive.


################
#  Handler Query And Maintenance
################

define: 'handler'		toBe: 544 asPrimitive.

define: 'enableHandler'		toBe: 562 asPrimitive.
define: 'disableHandler'	toBe: 563 asPrimitive.

define: 'setHandlerTo:'		toBe: 564 asPrimitive.

defineMethod:
[ | setBoundHandlerTo: block |
    ^self setHandlerTo: (block asMethod asClosure: ^self)
].


################
#  Input
################

define: 'byteCount'		toBe: 570 asPrimitive.

define: 'getByte'		toBe: 571 asPrimitive.
define: 'getUnsignedByte'	toBe: 572 asPrimitive.
define: 'getShort'		toBe: 573 asPrimitive.
define: 'getUnsignedShort'	toBe: 574 asPrimitive.
define: 'getLong'		toBe: 575 asPrimitive.
define: 'getFloat'		toBe: 576 asPrimitive.
define: 'getDouble'		toBe: 577 asPrimitive.

define: 'getString'		toBe: 580 asPrimitive.
define: 'getString:'		toBe: 581 asPrimitive.
define: 'getLine'		toBe: 582 asPrimitive.


defineMethod:
[ | getByteAt: offset |
    ^self setSeekOffsetTo: offset asInteger. getByte
].

defineMethod:
[ | getUnsignedByteAt: offset |
    ^self setSeekOffsetTo: offset asInteger. getUnsignedByte
].

defineMethod:
[ | getShortAt: offset |
    ^self setSeekOffsetTo: offset asInteger. getShort
].

defineMethod:
[ | getUnsignedShortAt: offset |
    ^self setSeekOffsetTo: offset asInteger. getUnsignedShort
].

defineMethod:
[ | getLongAt: offset |
    ^self setSeekOffsetTo: offset asInteger. getLong
].

defineMethod:
[ | getFloatAt: offset |
    ^self setSeekOffsetTo: offset asInteger. getFloat
].

defineMethod:
[ | getDoubleAt: offset |
    ^self setSeekOffsetTo: offset asInteger. getDouble
].

defineMethod:
[ | getStringAt: offset |
    ^self setSeekOffsetTo: offset asInteger. getString
].

defineMethod:
[ | getString: ofLength at: offset |
    ^self setSeekOffsetTo: offset asInteger. getString: ofLength
].

defineMethod:
[ | getLineAt: offset |
    ^self setSeekOffsetTo: offset asInteger. getLine
].


################
#  Output
################

define: 'syncOutput'		toBe: 590 asPrimitive.

defineMethod:
[ | putByte: byte |
    byte putByteTo: super
].

defineMethod:
[ | putShort: short |
    short putShortTo: super
].

defineMethod:
[ | putLong: long |
    long putLongTo: super
].

defineMethod:
[ | putFloat: float |
    float putFloatTo: super
].

defineMethod:
[ | putDouble: double |
    double putDoubleTo: super
].

defineMethod:
[ | putString: string |
    string putStringTo: super
].

defineMethod:
[ | putStringNL: string |
    [string printNL] divertOutput putStringTo: super
].


defineMethod:
[ | putByte: byte at: offset |
    ^self setSeekOffsetTo: offset asInteger. putByte: byte
].

defineMethod:
[ | putShort: short at: offset |
    ^self setSeekOffsetTo: offset asInteger. putShort: short
].

defineMethod:
[ | putLong: long at: offset |
    ^self setSeekOffsetTo: offset asInteger. putLong: long
].

defineMethod:
[ | putFloat: float at: offset |
    ^self setSeekOffsetTo: offset asInteger. putFloat: float
].

defineMethod:
[ | putDouble: double at: offset |
    ^self setSeekOffsetTo: offset asInteger. putDouble: double
].

defineMethod:
[ | putString: string at: offset |
    ^self setSeekOffsetTo: offset asInteger. putString: string
].

defineMethod:
[ | putStringNL: string at: offset |
    ^self setSeekOffsetTo: offset asInteger. putStringNL: string
].


################
#  Line Prompt Support
################

defineMethod:
[ | putString: request expectingLinePrompt: prompt |
    ^self putString: request;

    !lines <- request breakOn: newLine;
    !lineCount <- lines count;
    lines at: lineCount. count > 0
	ifTrue: [ ^self putString: newLine ]
       ifFalse: [ :lineCount <- lineCount - 1 ]
    ;

    !reply <- ^self getString else: "";
    [
 	[ reply containsSubstring: prompt ] whileTrue: [
	    :lineCount <- lineCount - 1;
	    :reply <- reply drop: (reply findSubstringSuffix: prompt);
	];
	lineCount > 0 && [ ^self isntAtEndOfInput ]
    ] whileTrue: [
	^self wait;
	:reply <- reply concat: (^self getString else: "");
    ];
].

defineMethod:
[ | getStringThroughPrompt: prompt |
    !string <- ^self getString else: "";
    [ string take: prompt count negated. != prompt && [ ^self isntAtEndOfInput ]
    ] whileTrue: [
	^self wait;
	:string <- string concat: (^self getString else: "");
    ];
    string
].


################
#  Vision Prompt Handshaking
################

defineMethod:
[ | submitVisionRequest: request |
    !prompt <- "V> ";

    ^self putString: request expectingLinePrompt: prompt;
    ^self putStringNL: "?g";
    ^self getStringThroughPrompt: prompt. drop: prompt count negated
].

defineMethod:
[ | consumeVisionPrompt |
    ^self getStringThroughPrompt: "V> ";
    ^self
].
;


################
####  OpenVision Constant
################

OpenVision Constant
defineMethod:
[ | print: aWidth |
    ^self description take: aWidth. print;
    ^self
].

defineMethod:
[ | print |
    ^self description print;
].

defineMethod:
[ | nameInstances: count |
    !ConstantClass <- ^self;
    count sequence0 do: [
        !instance <- ^self asPointerTo: ^my ConstantClass;
        instance description contains: "\\?". ifFalse: [
            ^my ConstantClass
                define: [instance description asWords
				  do: [^self print]] divertOutput
                  toBe: instance
        ]
    ];
    ^self
].
;


################
####  OpenVision Channel State
################

OpenVision Channel State
define: 'description'	toBe: 520 asPrimitive.
define: 'whatAmI'	toBe: "OpenVision Channel State".

nameInstances: 16
;

################
####  OpenVision Channel Status
################

OpenVision Channel Status
define: 'description'	toBe: 521 asPrimitive.
define: 'whatAmI'	toBe: "OpenVision Channel Status".

nameInstances: 16
;

################
####  OpenVision Channel Error
################

OpenVision Channel Error
define: 'description'	toBe: 522 asPrimitive.
define: 'whatAmI'	toBe: "OpenVision Channel Error".

nameInstances: 1
;

################
####  OpenVision Channel ChannelType
################

OpenVision Channel ChannelType
define: 'description'	toBe: 523 asPrimitive.
define: 'whatAmI'	toBe: "OpenVision Channel ChannelType".

nameInstances: 16
;


################
####  OpenVision Channel Option
################

OpenVision Channel Option
define: 'description'	toBe: 524 asPrimitive.
define: 'whatAmI'	toBe: "OpenVision Channel Option".

defineMethod:
[ | becomeOptionOf: channel |
    channel setOptionTo: super
].

nameInstances: 16
;

################
####  OpenVision Channel OptionValue
################

OpenVision Channel OptionValue

defineMethod:
[ | becomeOptionValueOf: channel |
    channel setOptionValueTo: super
].
;


################
####  OpenVision Channel PromptFormat
################

OpenVision Channel PromptFormat
define: 'description'	toBe: 525 asPrimitive.
define: 'whatAmI'	toBe: "OpenVision Channel PromptFormat".

nameInstances: 16
;

################
####  OpenVision Channel BinaryFormat
################

OpenVision Channel BinaryFormat
define: 'description'	toBe: 526 asPrimitive.
define: 'whatAmI'	toBe: "OpenVision Channel BinaryFormat".

nameInstances: 16
;

################
####  OpenVision Channel TrimFormat
################

OpenVision Channel TrimFormat
define: 'description'	toBe: 527 asPrimitive.
define: 'whatAmI'	toBe: "OpenVision Channel TrimFormat".

nameInstances: 16
;
