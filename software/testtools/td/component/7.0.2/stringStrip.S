#**********************************************************
#**********************************************************
#		Test: stringstripPrims 
#
#  Test String Primitives: 
#	strip
#
#  July 21, 2004	jck
#**********************************************************
#**********************************************************


Utility echoToFile: (^tmp componentOutput concat:"/stringStrip.O");
newLine print; "Beginning file: stringStrip.S ..." printNL;
###########################################################
#		Tests Begin Here ...                      #
###########################################################



" ---  Scalar normal input tests  ---" printNL ; 
"abcdefg" stripLeading: 	"ag" . printNL ; 
"abcdefg" stripTrailing:	"ag" . printNL ; 
"abcdefg" stripBounding:	"ag" . printNL ; 

"abcdefg" stripLeadingAlwaysCopy: 	"ag" . printNL ; 
"abcdefg" stripTrailingAlwaysCopy:	"ag" . printNL ; 
"abcdefg" stripBoundingAlwaysCopy:	"ag" . printNL ; 

" ---  Scalar extended recipient tests  ---" printNL ; 
"abcdefg" extendBy: [].	stripLeading: 	"ag" . printNL ; 
"abcdefg" extendBy: [].	stripTrailing:	"ag" . printNL ; 
"abcdefg" extendBy: [].	stripBounding:	"ag" . printNL ; 

"abcdefg" extendBy: [].	stripLeadingAlwaysCopy: 	"ag" . printNL ; 
"abcdefg" extendBy: [].	stripTrailingAlwaysCopy:	"ag" . printNL ; 
"abcdefg" extendBy: [].	stripBoundingAlwaysCopy:	"ag" . printNL ; 

" ---  Scalar extended parameter tests  ---" printNL ; 
"abcdefg" stripLeading: 	("ag" extendBy: []) . printNL ; 
"abcdefg" stripTrailing:	("ag" extendBy: []) . printNL ; 
"abcdefg" stripBounding:	("ag" extendBy: []) . printNL ; 

"abcdefg" stripLeadingAlwaysCopy: 	("ag" extendBy: []) . printNL ; 
"abcdefg" stripTrailingAlwaysCopy:	("ag" extendBy: []) . printNL ; 
"abcdefg" stripBoundingAlwaysCopy:	("ag" extendBy: []) . printNL ; 

" ---  Scalar both operands extended  ---" printNL ; 
"abcdefg" extendBy: []. stripLeading: 	("ag" extendBy: []) . printNL ; 
"abcdefg" extendBy: []. stripTrailing:	("ag" extendBy: []) . printNL ; 
"abcdefg" extendBy: []. stripBounding:	("ag" extendBy: []) . printNL ; 

"abcdefg" extendBy: []. stripLeadingAlwaysCopy: 	("ag" extendBy: []) . printNL ; 
"abcdefg" extendBy: []. stripTrailingAlwaysCopy:	("ag" extendBy: []) . printNL ; 
"abcdefg" extendBy: []. stripBoundingAlwaysCopy:	("ag" extendBy: []) . printNL ; 

" ---  Scalar invalid parameter  ---" printNL ; 
"abcdefg" extendBy: []. stripLeading: 	10 . printNL ; 
"abcdefg" extendBy: []. stripTrailing:	10.0 . printNL ; 
"abcdefg" extendBy: []. stripBounding:	(NA extendBy: []) . printNL ; 

"abcdefg" extendBy: []. stripLeadingAlwaysCopy: 	^current . printNL ; 
"abcdefg" extendBy: []. stripTrailingAlwaysCopy:	6 asFloat . printNL ; 
"abcdefg" extendBy: []. stripBoundingAlwaysCopy:	List new . printNL ; 

" --- List tests with single store block strings as parameters ---" printNL; 
!stripStrings <- "d", "e", "de", "1234", "   ";
stripStrings count  sequence do:[
	!stripString <- ^my stripStrings at: ^self; 
	"dddeee" stripLeading: stripString . printNL
]; 

" --- List tests with single store block strings as parameters (reordered) ---" printNL;
stripStrings count  sequence do:[
	!stripString <- ^my stripStrings at: (((^self + 3) asInteger mod: clusterCardinality) +1) asInteger; 
	"dddeee" stripLeading: stripString . printNL
]; 

" --- List tests with multi store lstore strings as parameters ---" printNL; 
!stripStrings <- "d" fixed, "e" fixed;
stripStrings count  sequence do:[
	!stripString <- ^my stripStrings at: ^self; 
	"dddeee" stripLeading: stripString . printNL
]; 

" --- List tests with invalid parameters ---" printNL; 
!stripStrings <- 10 sequence,"d"; 
stripStrings count  sequence do:[
	!stripString <- ^my stripStrings at: ^self; 
	"dddeee" stripLeading: stripString . printNL
]; 

!stripStrings <- 1000000 sequence; 
stripStrings count  sequence do:[
	!stripString <- ^my stripStrings at: ^self; 
	"dddeee" stripLeading: stripString . 
]; 
!stripStrings <- NA;

###########################################################
#		End of tests ...			  #
###########################################################
newLine print; "End of file: stringstripPrims.S" printNL; "" printNL;
?g
?t
Utility stopEchoToFile;
?g

