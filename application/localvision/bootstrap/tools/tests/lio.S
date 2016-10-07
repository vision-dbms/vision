#**********************************************************
#**********************************************************
#		Test: lio 
#
#  Test the lio module 
#  PART 1 A - Prints
#
#  7/5/88	jad
#**********************************************************
#**********************************************************


Utility echoToFile: "lio.O";
newLine print; "Beginning file: lio.S ..." printNL;
###########################################################
#		Tests Begin Here ...                      #
###########################################################

"
#----------------------------------------------------------
#  Setup (lio)
#----------------------------------------------------------
" printNL;

### create a 60 element list60
!list60 <- 1,2,3,4,5,6,7,8,9,10,1,2,3,4,5,6,7,8,9,10,1,2,3,4,5,6,7,8,9,10,
1,2,3,4,5,6,7,8,9,10,1,2,3,4,5,6,7,8,9,10,1,2,3,4,5,6,7,8,9,10;



"
#----------------------------------------------------------
#  Normal Input (lio)
#----------------------------------------------------------
" printNL;

"

-------------------------------------------------------
-------------------------------------------------------
------  PART 1 A - Test LIO Print Functions
------
-------------------------------------------------------
-------------------------------------------------------

### 1) PrintCurrentAsPointer: PrintAValue (LIO_printf)
### char, double, float, int
" printNL;
'a', 'b', 'c', 'd' do: [printNL];
1.2,8.9e307 asDouble,1234563231.11,2.333 do: [^self printNL];
1e25 asFloat, 323432214.122 asFloat, 2.33 asFloat do: [^self printNL];
11111111111,2,3,4 do: [^self printNL];


"

### 2) PrintOf List (LIO_puts)
" printNL;
344343, 122.333, 1234.56, 0, 101, 334.23 do:
	[^self print: 15.3; "" printNL];
1,2,3,4,5 do: ["-" fill: 300 . print: 305];

"

### 3A&B) PrintAAsString: Print string or user selector (LIO_printf)
###	A) handle block string
" printNL;
"Hello There World","Hello There World" do: [printNL];
"Hello There World","Hello There World" do: [print12];
"Hello There World","Hello There World" do: [print24];

"

###	b) handle lstore string
" printNL;
"Hello There World","Hello There World" fixed do: [printNL];
"Hello There World","Hello There World" fixed do: [print12];
"Hello There World","Hello There World" fixed do: [print24];

"

### 4) PrintAASKSI (LIO_puts)
" printNL;
'value', '+', '=' do: [printNL];


"

### 5) PrintAASPrimfn <> (LIO_printf)
" printNL;

Object defineMethod: 
[|pDisplay|
# Display all selectors which are methods along with their implementation
!class <- ^self ;
class selectorList do:
[ !selector <- ^self ;
  selector locateInDictionaryOf: ^my class . 
   do: [ (value whatAmI = "Method"  ||  value whatAmI = "aMethod" ||
	  value whatAmI = "Primitive"  ||  value whatAmI = "aPrimitive")
                ifTrue: [ value printNL;
                          newLine print; ]
       ]
]];
"" pDisplay;


"

### 6) PrintAAsBlockClosureInd  (LIO_broadcast)
" printNL;
#-- !list <- Company, Analyst, Prototypical SequencedCollectin,
!list <- Company, ClassDescriptor, Prototypical SequencedCollection,
	NA, Prototypical List, 10 asDate, Company, "", 42, "";"" printNL;
list do: [^self :print printNL];

"
### 7) PrintDate (LIO_printf)
" printNL;
19831231 asDate, 19830221 asDate, 19800101 asDate do:
	[^self printNL];"" printNL;
?g
?t

"

###############################
###############################
###  Now test large prints
###############################
###############################

### 8
### elements longer than 256 - strings
" printNL;
list60 do: ["+" fill: 300 . printNL];
"" printNL;
?g
?t

"
### 9
### force it to realloc - doubles
" printNL;
list60 do:
	[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20 do:
		[12.2 print: 16.3]; ^global newLine print];
"" printNL;
?g
?t



###########################################################
#		End of tests ...			  #
###########################################################
newLine print; "End of file: lio.S" printNL; newLine print;
?g
?t
Utility stopEchoToFile;
?g

