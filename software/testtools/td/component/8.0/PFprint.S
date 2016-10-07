#**********************************************************
#**********************************************************
#		Test: nicePrintOf:	
#
#
#  April 8, 2011	cqi
#**********************************************************
#**********************************************************


Utility echoToFile: (^tmp componentOutput concat:"/nicePrint.O");
newLine print; "Beginning file: PFprint.S ..." printNL;
###########################################################
#		Tests Begin Here ...                      #
###########################################################

"
*********************************************************
--------  IntegerPrintOf and DoublePrintOf  	 --------
--------  Primitives. 				 --------
*********************************************************
" printNL;

"
-----------------------------
-----  Integer PrintOf  -----
-----------------------------
" printNL;

"
-----  Test each scalar data type 

###  Character
" printNL;
### not reached


"
### Double
" printNL;
123.456 nicePrintNL: 10;
-123.456 nicePrintNL: -10;
123.456 nicePrintNL: 4;
123.456 nicePrintNL: 0;

"
### float
" printNL;
# not used

"
### lstore string
" printNL;
"hello there" fixed nicePrintNL: 15;
"hello there" fixed nicePrintNL: -15;
"hello there" fixed nicePrintNL: 4;
"hello there" fixed nicePrintNL: 0;

"
### block string
" printNL;
"hello there" nicePrintNL: 15;
"hello there" nicePrintNL: -15;
"hello there" nicePrintNL: 4;
"hello there" nicePrintNL: 0;

"
### selector
" printNL;
### 'print:' is defined to be 'print' for selectors - must use 'nicePrintOf:' to
### really reach the primitive.
10 nicePrintOf: 'value';"" printNL;
-10 nicePrintOf: 'value';"" printNL;
4 nicePrintOf: 'value';"" printNL;
0 nicePrintOf: 'value';"" printNL;


"
### integer
" printNL;
1234 nicePrintNL: 10;
-1234 nicePrintNL: -10;
1234 nicePrintNL: 4;
1234 nicePrintNL: 3;
1234 nicePrintNL: 0;
11111111111 nicePrintNL: 15;


"
-----  Test each list data type   -----

### character
" printNL;
### not reached


"
### double
" printNL;
123.456,-10234.1,10.0 do: [^self nicePrintNL: 10];"" printNL;
123.456,-10234.1,10.0 do: [^self nicePrintNL: -10];"" printNL;
123.456,-10234.1,10.0 do: [^self nicePrintNL: 7];"" printNL;
123.456,-10234.1,10.0 do: [^self nicePrintNL: 2];"" printNL;
123.456,-10234.1,10.0 do: [^self nicePrintNL: 0];"" printNL;

10,-10,4,0 do: [123.45 nicePrintNL: ^self];"" printNL;


"
### double, packed with specification of 10 decimal places
" printNL;
123.456,-10234.1,10.0,88log do: [^self nicePrint: 20 with: 10];"" printNL;

"
### float, packed with specification of 10 decimal places
" printNL;
123.456,-10234.1,10.0,88log do: [asFloat nicePrint: 20 with: 10];"" printNL;

"
### multi-type, packed with specification of 10 decimal places
" printNL;
123,-10234.1,10.0,"88log" do: [^self nicePrint: 20 with: 10];"" printNL;


"
### float
not accessed
" printNL;

"
### block strings
" printNL;

"hello", "there","everyone" do: [^self nicePrintNL: 10];"" printNL;
"hello", "there","everyone" do: [^self nicePrintNL: -10];"" printNL;
"hello", "there","everyone" do: [^self nicePrintNL: -4];"" printNL;
"hello", "there","everyone" do: [^self nicePrintNL: 0];"" printNL;

10,-10,4,5,0 do: ["hello" nicePrintNL: ^self];"" printNL;

"
### lstore strings
" printNL;

!l <- "hello","there","everyone" send: [^self fixed];
l do: [^self nicePrintNL: 10];"" printNL;
l do: [^self nicePrintNL: -10];"" printNL;
l do: [^self nicePrintNL: 4];"" printNL;
l do: [^self nicePrintNL: -4];"" printNL;
l do: [^self nicePrintNL: 0];"" printNL;

10,-10,4,5,0 do: ["hello" fixed nicePrintNL: ^self];"" printNL;

"
### selectors
" printNL;

'aselector','another' do: [10 nicePrintOf: ^self;"" printNL];"" printNL;
'aselector','another' do: [-10 nicePrintOf: ^self;"" printNL];"" printNL;
'aselector','another' do: [4 nicePrintOf: ^self;"" printNL];"" printNL;
'aselector','another' do: [-5 nicePrintOf: ^self;"" printNL];"" printNL;
'aselector','another' do: [5 nicePrintOf: ^self;"" printNL];"" printNL;
'aselector','another' do: [0 nicePrintOf: ^self;"" printNL];"" printNL;

10,-10,4,5,0 do: [^self nicePrintOf: 'value';"" printNL];"" printNL;


"
### integers
" printNL;
12345,-12345,0,10,11111111111 do: [^self nicePrintNL: 15];"" printNL;
12345,-12345,0,10,11111111111 do: [^self nicePrintNL: -10];"" printNL;
12345,-12345,0,10,11111111111 do: [^self nicePrintNL: -4];"" printNL;
12345,-12345,0,10,11111111111 do: [^self nicePrintNL: 5];"" printNL;
12345,-12345,0,10,11111111111 do: [^self nicePrintNL: 0];"" printNL;

10,-10,4,5,0 do: [12345 nicePrintNL: ^self];"" printNL;

"
----------------------------
-----  Double PrintOf  -----
----------------------------
" printNL;

"
-----  Test each scalar data type 

###  Character
" printNL;
### not reached

"
### Double
" printNL;
-123.456 nicePrintNL: 10.1;
123.456 nicePrintNL: -110.101;
123.456 nicePrintNL: 4.0;
123.456 nicePrintNL: 0.0;

"
### float
" printNL;
# not used

"
### lstore string
" printNL;
"hello there" fixed nicePrintNL: 15.3;
"hello there" fixed nicePrintNL: -15.0;
"hello there" fixed nicePrintNL: 4.2;
"hello there" fixed nicePrintNL: 0.0;

"
### block string
" printNL;
"hello there" nicePrintNL: 15.3;
"hello there" nicePrintNL: -15.2;
"hello there" nicePrintNL: 4.0;
"hello there" nicePrintNL: 0.0;

"
### selector
" printNL;
### 'print:' is defined to be 'print' for selectors - must use 'nicePrintOf:' to
### really reach the primitive.
10.2 nicePrintOf: 'value';"" printNL;
-10.0 nicePrintOf: 'value';"" printNL;
4.22 nicePrintOf: 'value';"" printNL;
0.0 nicePrintOf: 'value';"" printNL;

"
### integer
" printNL;
-1234 nicePrintNL: 10.33;
1234 nicePrintNL: -10.0;
1234 nicePrintNL: 4.1;
1234 nicePrintNL: 3.0;
1234 nicePrintNL: 0.0;
11111111111 nicePrintNL: 15.55;


"
-----  Test each list data type   -----

### character
" printNL;
### not reached

"
### double
" printNL;
123.456,-10234.1,10.0 do: [^self nicePrintNL: 10.1];"" printNL;
123.456,-10234.1,10.0 do: [^self nicePrintNL: -25.11];"" printNL;
123.456,-10234.1,10.0 do: [^self nicePrintNL: 7.3];"" printNL;
123.456,-10234.1,10.0 do: [^self nicePrintNL: 4.22];"" printNL;
123.456,-10234.1,10.0 do: [^self nicePrintNL: 0.0];"" printNL;

10.2,-40.11,4.1,0.0 do: [123.45 nicePrintNL: ^self];"" printNL;


"
### float
not accessed
" printNL;

"
### block strings
" printNL;

"hello", "there","everyone" do: [^self nicePrintNL: 10.0];"" printNL;
"hello", "there","everyone" do: [^self nicePrintNL: -10.9];"" printNL;
"hello", "there","everyone" do: [^self nicePrintNL: -4.0];"" printNL;
"hello", "there","everyone" do: [^self nicePrintNL: 0.0];"" printNL;

10.2,-10.0,4.9,5.2,0.0 do: ["hello" nicePrintNL: ^self];"" printNL;

"
### lstore strings
" printNL;

!l <- "hello","there","everyone" send: [^self fixed];
l do: [^self nicePrintNL: 10];"" printNL;
l do: [^self nicePrintNL: -10.0];"" printNL;
l do: [^self nicePrintNL: -4.2];"" printNL;
l do: [^self nicePrintNL: 0.0];"" printNL;

10.3,-10.0,4.4,5.0,0.0 do: ["hello" fixed nicePrintNL: ^self];"" printNL;

"
### selectors
" printNL;

'aselector','another' do: [10.0 nicePrintOf: ^self;"" printNL];"" printNL;
'aselector','another' do: [-10.4 nicePrintOf: ^self;"" printNL];"" printNL;
'aselector','another' do: [4.22 nicePrintOf: ^self;"" printNL];"" printNL;
'aselector','another' do: [0.0 nicePrintOf: ^self;"" printNL];"" printNL;

10.0,-10.3,4.1,5.22,0.0 do: [^self nicePrintOf: 'value';"" printNL];"" printNL;

"
### integers
" printNL;
12345,-12345,0,10,11111111111 do: [^self nicePrintNL: 15.0];"" printNL;
12345,-12345,0,10,11111111111 do: [^self nicePrintNL: -10.5];"" printNL;
12345,-12345,0,10,11111111111 do: [^self nicePrintNL: 4.99];"" printNL;
12345,-12345,0,10,11111111111 do: [^self nicePrintNL: 5.0];"" printNL;
12345,-12345,0,10,11111111111 do: [^self nicePrintNL: 0.0];"" printNL;

10.2,-10.0,4.99,5.4,5.2,0.0 do: [12345 nicePrintNL: ^self];"" printNL;


"
--------------------------------
-----  Test PrintOf limits  ----
--------------------------------
" printNL;

###
#  Test Max Double as the value
###
8.9e307 nicePrintNL: 10;
8.9e307 nicePrintNL: 400;
8.9e307 nicePrintNL: 10.10;
-8.9e307 nicePrintNL: 400.21;
10,400 do: [8.9e307 asDouble nicePrintNL: ^self];"" printNL;
10.2,400.1 do: [8.9e307 asDouble nicePrintNL: ^self];"" printNL;

###
#  Test the Maximum Formats
###

### Test RealizeFields 4 cases:
102.34 nicePrintNL: 8.9e307;
102.34 nicePrintNL: 15.11;
102.34 nicePrintNL: -8.9e307;
102.34 nicePrintNL: -10000.7;
102.34 nicePrintNL: -15.11;
102.34 nicePrintNL: -20.555555555;

### Test ProduceDoubleFormatForDouble:
### withCommas:
1102.34 printWithCommas: -15000.11;"" printNL;
1102.34 printWithCommas: -15.0;"" printNL;
1102.34 printWithCommas: 40.11;"" printNL;
1102.34 printWithCommas: -40.25;"" printNL;
### withoutCommas:
1102.34 nicePrintNL: -15.0;
1102.34 nicePrintNL: 40.11;
1102.34 nicePrintNL: -40.25;

### test exactly the correct field width
1234 nicePrintNL: 4;
1234.5 nicePrintNL: -6.1;

### test for the right number of commas
8.9e307 printWithCommas: 425.1; "" printNL;

### test printing factorial results
3 defineMethod: 
	[|factorial|
	    ^self > 1 ifTrue: [ ^self * ((^self - 1) asInteger factorial) ]
		      ifFalse: [1]
	];

150,140,130,120,110,100,90,80,70,60,50,40,30,20,10,0
	do: [factorial printNL; factorial/^self nicePrintNL:400];



###########################################################
#		End of tests ...			  #
###########################################################
newLine print; "End of file: PFprint.S" printNL; "" printNL;
?g
?t
Utility stopEchoToFile;
?g

