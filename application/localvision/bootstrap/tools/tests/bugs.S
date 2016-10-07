#**********************************************************
#**********************************************************
#		Test: bugs 
#
#  Test for previously found bugs.
#
#  10/14/87	jad
#**********************************************************
#**********************************************************


Utility echoToFile: "bugs.O";
newLine print; "Beginning file: bugs.S ..." printNL;
###########################################################
#		Tests Begin Here ...                      #
###########################################################


###
#  Bug #1  Oct 16, 1987.
#	The lstore routines that count elements did not align the lstore
#	before accessing the breakpoint array via a private pointer. Therfore,
#	unaligned lstores when presented with a request for a count would
#	answer randomly. The following sets up a time series with an unaligned
#	lstore.		-jck
###

	"Bug 1" printNL;
	!Bug1_Class <- Object specialized;
	Bug1_Class __define: 'x';
	Bug1_Class :x asOf:10101 put:0;
	Bug1_Class new;
	Bug1_Class new;
	Bug1_Class new;
	!x <- Bug1_Class new;
	!y <- Bug1_Class new;
	Bug1_Class new;
	Bug1_Class new;
	!z <- Bug1_Class new;

	!Bug1_Class_ <- Object specialized;
	Bug1_Class_ __define: 'x';
	Bug1_Class_ :x asOf:10101 put:0;
	Bug1_Class_ new;
###
#  End of setup for Bug1
###
	Bug1_Class,x,y,z do:[:x count print];
	"" printNL;
	Bug1_Class_ :x count print;

###
#  End of Bug1 edge condition test
###

###
#  Bug #2  Nov 3, 1987.
#	The simple case optimization for rtVECTOR_LCExtract cannot work
#	if any of the values to be extracted are the 'referenceNil' value.
#	If this is true the new vector constructor needs to have an undefined
#	usegment.  This bug showed up because an integer 0 was returned 
#	instead of an undefined value.  The fix was to only go thru the 
#	simple case if 1) the source vector has only 1 pmap entry, AND 
#	2) there are no 'referenceNil' values in the extraction linkc.
#			jad
###
	"" printNL; 
	"Bug 2" printNL;
	Company define: 'foo' ;
	Named Company IBM send: [foo ] . printNL ;
	Named Company IBM asList send: [ foo printNL ] ;
	Named Company GM :foo asOf: 8709 put: 1 ;
	Named Company IBM send: [foo ] . printNL ;
# this next value must be 'NA' not 0 
	Named Company IBM asList send: [ foo printNL ] ;
	Named Company GM send: [foo ] . printNL ;
	Named Company GM asList send: [ foo printNL ] ;



###
#  Bug #3  Nov 11, 1987.
#	The scalar lstore element counting routine mishandled the reference
#	nil case. The following sets up a time series referenced by the
#	referenceNil.		-jck
###

	"Bug 3" printNL;
	!Bug3_Class <- Object specializeClass;
	Bug3_Class __define: 'x';

###
#  End of setup for Bug3
###
	Bug3_Class instanceList count print:10;
	Bug3_Class :x count printNL:10;
	Bug3_Class new;
	Bug3_Class instanceList count print:10;
	Bug3_Class :x count printNL:10;

###
#  End of Bug3 edge condition test
###

###
#  Bug #4  Dec 14, 1987.
#	Index lookups failed to align their underlying lstores. This showed up
#	when rows in a time series were deleted. The following sets up an 
#	unaligned time series.		-jck
###

	"Bug 4" printNL;
	!bug4_Class <- Object specialized;
	bug4_Class __define: 'history';
	bug4_Class new;
	bug4_Class new;
	bug4_Class new;
	bug4_Class new;
	bug4_Class new;
	bug4_Class new;
	bug4_Class new;
	bug4_Class new;
	bug4_Class new;
	bug4_Class new;
	1 asReferenceTo: bug4_Class. :history asOf:^today put:3;
	2 asReferenceTo: bug4_Class. :history asOf:^today put:3;
	3 asReferenceTo: bug4_Class. :history asOf:^today put:3;
	4 asReferenceTo: bug4_Class. :history asOf:^today put:3;
	5 asReferenceTo: bug4_Class. :history asOf:^today put:3;
	6 asReferenceTo: bug4_Class. :history asOf:^today put:3;
	7 asReferenceTo: bug4_Class. :history asOf:^today put:3;
	8 asReferenceTo: bug4_Class. :history asOf:^today put:3;

###
#  End of setup for Bug4
###

	4 asReferenceTo: bug4_Class   . history printNL;
	4 asReferenceTo: bug4_Class   . delete;
	7 asReferenceTo: bug4_Class . history;
	7 asReferenceTo: bug4_Class . :history asOf:^today put:5 . printNL;

###
#  End of Bug4 normal input test
###

###
#  Bug #5  Dec 30, 1987.
#	Some of the 'RTindex' routines failed to 'realize' the key argument
#	before checking to see if the key was scalar.  Unfortunately, if the
#	key was a 'future' key, it did the wrong thing and segmentation 
#	faulted.  This bug has always been there but apparently this particular
#	path thru 'RTindex' was never used before.  The re-implementation
#	of date ranges seems to cause this new path to be used.
#		- jad
###
	"Bug 5" printNL;

 	!dr <- 86 to:84 by:1 yearEnds;
	!ibm <- Named Company IBM;
	dr evaluate:[
	    ibm sales < 50000 
		ifTrue: [ibm sales print; ^date printNL;]];

###
#  End of Bug5 normal input test
###

###
#  Bug #6  Dec 30, 1987.
#	'rtINDEX_DeleteEmptyColumns' had a potentially serious defect in its
#	computation of p-token adjustments.  Basically, the routine maintains
#	a 'totalAdjustment' used to compute p-token adjustment origins.  That
#	total adjustment should have been adjusted ONLY when new p-token shifts
#	were created; in fact, 'totalAdjustment' was adjusted by the amount of
#	the last adjustment for each RRD processed and not just those which 
#	really made an adjustment.  If the routine was lucky, only one RRD 
#	created an adjustment, or every RRD created one.  If, a span of RRDs 
#	failed to create p-token shifts after a p-token shift was created, 
#	the next p-token shift was wrong.  This resulted in either the WRONG 
#	answer (VERY,VERY,VERY BAD) or a 'AppendAdjustment' error from p-token.
#	
#	This bug was discovered courtesy of an 'AppendAdjustment' error.  
#	Because this routine affected the 'delete:' operations of indexed 
#	lists and time series and because the pattern of use of these 
#	operations to date has been safe (i.e., single point deletion, 
#	all point deletion), the likelihood of corrupted data is negligible.
#		- mjc
###

	"Bug 6" printNL;
	!bug6_il1 <- Prototypical IndexedList new;
	!bug6_il2 <- bug6_il1 clusterNew;
	!bug6_il3 <- bug6_il1 clusterNew;

###
#  End of setup for bug6
###

	0, 2, 3, 7
	do:
	[
	    ^my bug6_il1 at: ^self put: ^self
	];

	1, 4, 5, 6, 8
	do:
	[
	    ^my bug6_il2 at: ^self put: ^self;
	    ^my bug6_il3 at: ^self put: ^self
	];

	bug6_il1 instanceList do:
	    [count printNL; ^self do: [printNL]; newLine print];

	bug6_il1 delete printNL;
	bug6_il2 delete: 99;		# Nothing magic about '99'

###
#  End of bug 6 normal input test
###

###
#  Bug #7  Jan 8, 1988.
#	
#	Bug which appeared when doing a select on single element timeseries.
#	The bug was in PFlist.c. 
#	IICSubsetContinuation calls LoadAWithNewIndex
#	which calls EmptyKeyValueOfType which creates a new set uvector
#	based on the rtype and ptokens of the 'ListIteratorTOTSCKey'.
#	Unfortunately, EmptyKeyValueOfType did not expect a scalar key
#	and caused a segmentation fault when one appeared.  
#			- jad
###

	"Bug 7" printNL;
	Company define: 'xyzzy';
	Named Company GM :xyzzy asOf: 19841231 put: 10;

###
#  End of setup for bug7
###

	Named Company GM :xyzzy select: [];

###
#  End of bug 7 normal input test
###

###
#  Bug #8  Mar 25, 1988.
#	
#	A decompiler bug. ':' was dropped in certain chains of unary
#	messages.  Also, the keyword expression closing mark, '.', 
#	was incorrectly placed if the final parameter of the expression
#	was a number. 
#	 - jck
###

	"Bug 8" printNL;
	^current __defineMethod: 
		[|testDecompiler|
			!a <- b :c;
			1 a: 2 b: 3 . :c;
		];

###
#  End of setup for bug 8
###

	:testDecompiler printNL;

###
#  End of bug 8 normal input test
###

###
#  Bug 9
#	This bug showed up on the Apollo. Its cause was an uninitialized
#  descriptor. The fault lay in the RTdsc.c which did not insert a value in
#  the store slot of the descriptor if the value should have been Nil. The
#  problem did not show up on the HP as it initializes automatic variables to
#  zero.
###

	"Bug 9" printNL;
	Named Company GM :sales :value :value :value asOf: 85 . printNL;

###
#  End of Bug 9 normal input test
###

###
#  Bug10
#	rtLINK_AlignConstructor. Inappropriately did nothing if the
#	only modifications to the positional state are additions to the end
#	(caused, for example, by 'new') and the link does not contain the
#	reference Nil. (Sending the x message causes alignment to happen).
###
	"Bug 10" printNL;
	Bug3_Class new;
	Bug3_Class new;
	!tmp <- Bug3_Class new;
	tmp x;
	(0 asReferenceTo: Bug3_Class), (1 asReferenceTo: Bug3_Class) 
		do: [ new ; x];

###
#  End of Bug10 edge condition test
###

###
#  Bug #11   March 1989
#	The 'asPointerTo:' primitive mishandled parameters obtained
#  via the '^my' magic word in certain instances.
#  (used the TOTSC ptoken rather than the parameter's own ptoken)
###

	"Bug 11" printNL;
	!Bug11 <- Object specialized;
#!!!!!	Named Company GM,
#!!!!!
#!!!!!	Named Company IBM do:[^self printNL:(3 asPointerTo: ^my Bug11)];
#!!!!!  (causes DSC_DistributePointer: Inconsistent Descriptor Distribution)
#!!!!!

###
#  End of Bug 11 normal input test
###

###
#  Bug #12   April 1989
#	Virtual machine previously did incomplete bookkeeping regarding
#  parameter counts. If the number of parameters passed did not match the
#  methods implementation, or exceeded the virtual machine's maximum,
#  one of several different errors would occur. The interpreter had
#  essentially relied on the compiler to prevent mismatch errors. However, at
#  least two means are available for circumventing the compiler's control. One
#  is escaping the colon in a user defined selector. The second is the use of
#  the <EvaluateFor...>primitives which can be defined with any number of
#  parameters that the user wishes.
###

	"Bug 12" printNL;

2 defineMethod: [|plus: x  plus: y  plus: z  plus: a  plus: b|
			x + y + z + a + b];

2 defineMethod: [|plus: x  plus: y  plus: z  plus: a  plus: b  plus: c
		  plus: x1 plus: y1 plus: z1 plus: a1 plus: b1 plus: c1
		  plus: x2 plus: y2 plus: z2 plus: a2 plus: b2 plus: c2|
	x + y + z + a + b + c + x1 + y1 + z1 + a1 + b1 + c1
	 + x2 + y2 + z2 + a2 + b2 + c2];

2 plus: 3 plus: 4 plus: 5 plus: 6 plus: 7 . printNL;

"value
"print;
(2 plus: 3 plus: 4 plus: 5 plus: 6 :plus: 7) value printNL;

"valueWith:and:and:and:and:and:
"print;
(2 plus: 3 plus: 4 plus: 5 plus: 6 :plus: 7)
	 valueWith: 2 and: 3 and: 4 and: 5 and: 6 and: 7 
. printNL;

"valueWith:and:and:and:and:
"print;
(2 plus: 3 plus: 4 plus: 5 plus: 6 :plus: 7)
	 valueWith: 2 and: 3 and: 4 and: 5 and: 6
. printNL;

"valueWith:and:and:
"print;
(2 plus: 3 plus: 4 plus: 5 plus: 6 :plus: 7) valueWith: 2 and: 3 and: 4
.printNL;

2 plus: 3 plus: 4 plus: 5 plus: 6 plus: 7 plus: 8 plus: 9 plus: 10 plus: 11
  plus: 12 plus: 13 plus: 14
  plus: 12 plus: 13 plus: 14 
  plus: 12 plus: 13 plus: 14 . printNL;

# non-primitive with an escaped colon
printNL\:;
# primitives with an escaped colon(s)
(1,2) do\:;
(1,2) rankUp\:usingCollector\:;
(1,2) rankDown\:usingCollector:[];

1,2.0,3
do:[
2 plus: 3 plus: 4 plus: 5 plus: 6 plus: 7 . printNL;

"value
"print;
(2 plus: 3 plus: 4 plus: 5 plus: 6 :plus: 7) value printNL;

"valueWith:and:and:and:and:and:
"print;
(2 plus: 3 plus: 4 plus: 5 plus: 6 :plus: 7)
	 valueWith: 2 and: 3 and: 4 and: 5 and: 6 and: 7 
. printNL;

"valueWith:and:and:and:and:
"print;
(2 plus: 3 plus: 4 plus: 5 plus: 6 :plus: 7)
	 valueWith: 2 and: 3 and: 4 and: 5 and: 6
. printNL;

"valueWith:and:and:
"print;
(2 plus: 3 plus: 4 plus: 5 plus: 6 :plus: 7) valueWith: 2 and: 3 and: 4
.printNL;

# Too many parameters
2 plus: 3 plus: 4 plus: 5 plus: 6 plus: 7 plus: 8 plus: 9 plus: 10 plus: 11
  plus: 12 plus: 13 plus: 14
  plus: 12 plus: 13 plus: 14 
  plus: 12 plus: 13 plus: 14 . printNL;

# non-primitive with an escaped colon
printNL\:;
# primitive with an escaped colon
(1,2) do\:;
];

###
#  End of Bug 12 normal input test
###

###
#  Bug #13   April 1989
#	Align store erroneously implemented for string stores. 
#  Test alignStore on the various types.
###

	"Bug 13" printNL;
	#  String Stores
	Environment DB Compustat stringStore alignStore printNL;
	#  Value Store (and also Vector)
	Environment DB Compustat staticData  alignStore printNL;
	#  Indexed List (and also List Store and Vector)
	Company CompustatData :history alignStore printNL;

###
#  End of Bug 13 normal input test
###

###
#  Bug #14 January 2000
#	Lstore subscript operations don't work correctly after
#  deletion of list from lstore.
###

	"Bug 14" printNL;
[
!class <- Object specialized;
!c1 <- class new;
!c2 <- class new;
!c3 <- class new;
!c4 <- class new;
!c5 <- class new;
!c6 <- class new;
class defineFixedProperty:'list';
class instanceList numberElements do:[:list <- ^global List new, position];
class cleanStore;

c1 list delete;
c1 list at:1 . printNL;
"The 2 values on the next line should be the same." printNL;
c2 list at:1 . print;
c2 list at:1 . printNL;

class list delete;
"The next two lines should be identical." printNL;
class instanceList do:[ list at:1 .print;];newLine print;
class instanceList do:[ list at:1 .print;];newLine print;
] value;

###
#  End of Bug 14 normal input test
###


###
#  Bug #15 January 2000
#	Make sure result of the log of 0 is NA
#  
###

	"Bug 15" printNL;
0 log printNL;
0 log:10 .printNL;
5 sequence0 do:[log print];
newLine print;
?g
?t
###
#  End of Bug 15 normal input test
###

###
#  Bug #16 February 2000
#	Check descriptor type when comparing forced references
#  
###
	"Bug 16" printNL;

"Following Error should be usage error rather than bus error
 or segmentation fault" printNL;
?g
0,0 do:[ -1 asReferenceTo:^self . = ^self];
newLine print;
?g
###
#  End of Bug 16 normal input test
###

###
#  Bug #17 March 2000
#	Ilist key deletion bug fixed in version 5.9.3 (June, 1996)
#  
###

	"Bug 17" printNL;
[
!class <- Object specialized;
!inst1 <- class new;


!IList <- IndexedList new;
IList at: inst1 put: 3;

inst1 delete;

#  This operation should produce the error; however, the alignment
#  that invalidates the structure is performed after the validity
#  test...
IList at: inst1 . printNL;

#  Now that the structure has been invalidated, an error happens
#  every time...
IList at: inst1 . printNL;
newLine print;
] value;

###
#  End of Bug 17 normal input test
###

###
#  Bug #18 December 2002
#	caused REP error in 6.2 and early versions of 7
#  
###

	"Bug 18" printNL;

# 
1000000 sequence rankUp:[^self]; 

###
#  End of Bug 18 normal input test
###

###
#  Bug #19 December 2003
#	A variation on Bug 18 ... 
#       VListEnumerationController::returnOrder: Fragmentation Unsupported
###

	"Bug 19" printNL;

# 
1000 sequence do:[
    1000 sequence, 2.0 basicRunningTotal:[1 asDouble] usingCollector: [|:x| ^current]
]; 

###
#  End of Bug 18 normal input test
###

###########################################################
#		End of tests ...			  #
###########################################################
?g
?t
newLine print; "End of file: bugs.S" printNL; newLine print;
Utility stopEchoToFile;
?g



