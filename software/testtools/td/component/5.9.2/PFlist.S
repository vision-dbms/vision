#**********************************************************
#**********************************************************
#		Test: PFlist 
#
#  Tests for the PFlist module
#	PART 1 - NumericList Primitive
#	PART 2 - CellAtPosition Primitive
#
#  7/11/88	jad
#**********************************************************
#**********************************************************


Utility echoToFile: (^tmp componentOutput concat:"/PFlist.O");
newLine print; "Beginning file: PFlist.S ..." printNL;
###########################################################
#		Tests Begin Here ...                      #
###########################################################
"


******************************************************
(PART 1)
--------  Numeric Lists                       --------
******************************************************
" printNL;

"
### Test scalar-mode sequence generator (+ receipient)
" printNL;
!l <- 10 sequence0;
l count print; l do: [^self print: 3]; "" printNL;

!l1 <- 10 sequence;
l1 count print; l1 do: [^self print: 3]; "" printNL;

"
### Test scalar-mode sequence generator (0 receipient)
" printNL;
!ll <- 0 sequence0;
ll count print; ll do: [^self print: 3]; "" printNL;

!ll1 <- 0 sequence; 
ll1 count print; ll1 do: [^self print: 3]; "" printNL;

"
### Test scalar-mode sequence generator (- receipient)
" printNL;
!lll <- -10 sequence0;
lll count print; lll do: [^self print: 3]; "" printNL;

!lll1 <- -10 sequence;
lll1 count print; lll1 do: [^self print: 3]; "" printNL;

"
### Test non-scalar-mode sequence generator
" printNL;
10, 0, -10, 5 do:
[
    ^self sequence0 basicSend:
	[count print; ^self do: [^self print: 3]; "" printNL];
    ^self sequence basicSend:
	[count print; ^self do: [^self print: 3]; "" printNL];
];

"


******************************************************
(PART 2)
--------  CellAtPosition - List Subscripts    --------
******************************************************
" printNL;
"
### setup
" printNL;

!list <- 20, 11, 52, 23, 24, 55;
!list1 <- 21, 12, 53, 24, 25, 56;

"
###  Scalar Subscripts
" printNL;

###
#  Try 1st, middle, last, first-1, first-2, last+1, last+2 slots
###

l base0CellAtPosition: 10 . printNL;
l base0CellAtPosition: 0  . printNL;
l base0CellAtPosition: 5  . printNL;
l base0CellAtPosition: 9  . printNL;
l base0CellAtPosition: -1 . printNL;
l base0CellAtPosition: -2 . printNL;
l base0CellAtPosition: 11 . printNL;
 "" printNL;

l1 base1CellAtPosition: 11 . printNL;
l1 base1CellAtPosition: 1 . printNL;
l1 base1CellAtPosition: 6 . printNL;
l1 base1CellAtPosition: 9 . printNL;
l1 base1CellAtPosition: 0 . printNL;
l1 base1CellAtPosition: -1 . printNL;
l1 base1CellAtPosition: 12 . printNL;
 "" printNL;

list base0CellAtPosition: 0 . printNL;
list base0CellAtPosition: 1 . printNL;
list base0CellAtPosition: 4 . printNL;
list base0CellAtPosition: 5 . printNL;
list base0CellAtPosition: -1 . printNL;
list base0CellAtPosition: -2 . printNL;
list base0CellAtPosition: 6 . printNL;
list base0CellAtPosition: 7 . printNL;
 "" printNL;

list1 base1CellAtPosition: 0 . printNL;
list1 base1CellAtPosition: 1 . printNL;
list1 base1CellAtPosition: 4 . printNL;
list1 base1CellAtPosition: 5 . printNL;
list1 base1CellAtPosition: -1 . printNL;
list1 base1CellAtPosition: -2 . printNL;
list1 base1CellAtPosition: 6 . printNL;
list1 base1CellAtPosition: 7 . printNL;
 "" printNL;

"
### Scalar Subscript in Empty List
" printNL;

ll base0CellAtPosition: 0 . printNL;
ll base0CellAtPosition: 1 . printNL;
ll base0CellAtPosition: -1 . printNL;
 "" printNL;

ll base1CellAtPosition: 1 . printNL;
ll base1CellAtPosition: 2 . printNL;
ll base1CellAtPosition: 0 . printNL;
ll base1CellAtPosition: -1 . printNL;
 "" printNL;

"
###
# Non-scalar Subscripts
###
" printNL;

!test <- -2,-1,0,1,5,9,10,11;
!test1 <- -1,0,1,2,6,10,11,12;
!na <- -3,20,20,30;

###
#  Produce 3 types of results:
#       all valid values, all NAs, and mixed
###

l do: [^my l base0CellAtPosition: ^self . printNL]; "" printNL;
test do: [^my l base0CellAtPosition: ^self . printNL]; "" printNL;
na do: [^my l base0CellAtPosition: ^self . printNL]; "" printNL;
 "" printNL;

(list count) sequence0 do: [^my list base0CellAtPosition: ^self . printNL]; "" printNL;
test do: [^my list base0CellAtPosition: ^self . printNL]; "" printNL;
na do: [^my list base0CellAtPosition: ^self . printNL]; "" printNL;
 "" printNL;

### Non-scalar Subscript in Empty List
(ll count) sequence0 do: [^my ll base0CellAtPosition: ^self . printNL]; "" printNL;
test do: [^my ll base0CellAtPosition: ^self . printNL]; "" printNL;
na do: [^my ll base0CellAtPosition: ^self . printNL]; "" printNL;
 "" printNL;

"
### Base 1 Non-scalar Tests
" printNL;

l1 do: [^my l1 base1CellAtPosition: ^self . printNL]; "" printNL;
test1 do: [^my l1 base1CellAtPosition: ^self . printNL]; "" printNL;
na do: [^my l1 base1CellAtPosition: ^self . printNL]; "" printNL;
 "" printNL;

(list1 count) sequence do: [^my list1 base1CellAtPosition: ^self . printNL];
 "" printNL;
test1 do: [^my list1 base1CellAtPosition: ^self . printNL]; "" printNL;
na do: [^my list1 base1CellAtPosition: ^self . printNL]; "" printNL;
 "" printNL;

### Non-scalar Subscript in Empty List
(ll1 count) sequence do: [^my ll1 base1CellAtPosition: ^self . printNL];
 "" printNL;
test1 do: [^my ll1 base1CellAtPosition: ^self . printNL]; "" printNL;
na do: [^my ll1 base1CellAtPosition: ^self . printNL]; "" printNL;
 "" printNL;

"
###
#  Test Multiple lists in one LStore
###
" printNL;

!l1 <- 0,1,2;
!l2 <- l1 clusterNew;
!l3 <- l1 clusterNew;
l2, 20, 41;
l3, 40, 21, 52, 53;

### List selector is a range 0-2
!m <- Prototypical List new, l1, l2, l3;
m do: [^self base0CellAtPosition: 5 . printNL]; "" printNL;
m do: [^self base0CellAtPosition: 0 . printNL]; "" printNL;
m do: [^self base0CellAtPosition: 2 . printNL]; "" printNL;

### List selector is repeat,repeat,range:  0*2,1*2,2
!mm <- Prototypical List new, l1, l2, l2, l1, l3;
mm do: [^self base0CellAtPosition: 5 . printNL]; "" printNL;
mm do: [^self base0CellAtPosition: 0 . printNL]; "" printNL;
mm do: [^self base0CellAtPosition: 2 . printNL]; "" printNL;

### List selector is range, repeat: 0+2,2*2
!mmm <- Prototypical List new, l1, l2, l3, l3;
mmm do: [^self base0CellAtPosition: 5 . printNL]; "" printNL;
mmm do: [^self base0CellAtPosition: 0 . printNL]; "" printNL;
mmm do: [^self base0CellAtPosition: 2 . printNL]; "" printNL;



"

#####
##### Test IndexList interface
#####
" printNL;
!il <- Prototypical IndexedList new;
il base0CellAtPosition: 0 . printNL;
il at: 0 put: 10;
il at: 1 put: 11;
il at: 2 put: 12;
!x <- il count sequence0;
"
###  The results of the next tests may vary from run to run
###  The following SEVEN sets must be identical (within one run).

     ONE:
" printNL;
il do: [printNL];

"
     TWO:
" printNL;
x do: [^my il base0CellAtPosition: ^self . value printNL];

"
     THREE:
" printNL;
x do: 
        [!pos <- (^self + 1) asInteger;
         ^my il base1CellAtPosition: pos . value printNL];

"
###
#  Test valid converse and covering messages at IndexedList
###

     FOUR:
" printNL;
il base0CellAtPosition: (0 extendBy: []) . value printNL;
1 base0CellPositionOf: il . value printNL;
2 base0CellPositionOf: il . value printNL;
"
     FIVE:
" printNL;
0,1,2 do: [^self base0CellPositionOf: ^my il . value printNL];

"
     SIX:
" printNL;
1 base1CellPositionOf: il . value printNL;
2 base1CellPositionOf: il . value printNL;
3 base1CellPositionOf: il . value printNL;

"
     SEVEN:
" printNL;
1,2,3 do: [^self base1CellPositionOf: ^my il . value printNL];



"

####
#### Test TimeSeries Interface
####
" printNL;
!ts <- Prototypical TimeSeries new;
ts base0CellAtPosition: 0 . printNL;
ts at: 19801231 asDate put: 40;
ts at: 19811231 asDate put: 41;
ts at: 19821231 asDate put: 42;
!x <- ts count sequence0;
"
###  The results of the next tests may vary from run to run
###  The following SEVEN sets must be identical (within one run).

     ONE:
" printNL;

ts do: [printNL];
"
     TWO:
" printNL;
x do: [^my ts base0CellAtPosition: ^self . value printNL];

"
     THREE:
" printNL;
x do: 
        [!pos <- (^self + 1) asInteger;
         ^my ts base1CellAtPosition: pos . value printNL];
"
###
#  Test valid converse and covering messages at TimeSeries
###

     FOUR:
" printNL;
ts base0CellAtPosition: (0 extendBy: []) . value printNL;
1 base0CellPositionOf: ts . value printNL;
2 base0CellPositionOf: ts . value printNL;

"
     FIVE:
" printNL;
0,1,2 do: [^self base0CellPositionOf: ^my ts . value printNL]; 

"
     SIX:
" printNL;
1 base1CellPositionOf: ts . value printNL;
2 base1CellPositionOf: ts . value printNL;
3 base1CellPositionOf: ts . value printNL;

"
     SEVEN:
" printNL;
1,2,3 do: [^self base1CellPositionOf: ^my ts . value printNL];



" 
###
#  Test binary converse 
###

###  Bogus message
" printNL;

!ll <- 10 sequence0;
ll base0CellAtPosition: "hello" . printNL;

"
###
#  Test valid converse and covering messages at List
###
" printNL;

### valid converse
ll base0CellAtPosition: (0 extendBy: []) . printNL;"" printNL;

### covering messages
3 base0CellPositionOf: ll . printNL;
1,2,3 do: [^self base0CellPositionOf: ^my ll . printNL]; "" printNL;

3 base1CellPositionOf: ll . printNL;
1,2,3 do: [^self base1CellPositionOf: ^my ll . printNL]; "" printNL;


###
#  Test 'iterate:by:'
###

1,2,3 iterate: [print];
1,2,3 iterate: [print] by:5;
1,2,3 iterate: [print] by:5.0;


ts iterate: [print];
ts iterate: [print] by:5;
ts iterate: [print] by:5.0;


1,2,NA iterate:[ 1,2,3 iterate:[print] by:^self;
		"" printNL;
		^my ts iterate:[print] by:^self;
		"" printNL;
		];

1,2,NA do:[ 1,2,3 iterate:[print] by:5;
		"" printNL;
	   ^my ts iterate:[print] by:5;
		"" printNL;
	   ];


###########################################################
#		End of tests ...			  #
###########################################################
newLine print; "End of file: PFlist.S" printNL; newLine print;
?g
?t
Utility stopEchoToFile;
?g

