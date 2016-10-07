#**********************************************************
#**********************************************************
#		Test: basics
#
#  Some Simple Basic Tests 
#
#  9/4/87		jad
#**********************************************************
#**********************************************************


Utility echoToFile: "basics.O";
newLine print; "Beginning file: basics.S ..." printNL;
###########################################################
#		Tests Begin Here ...                      #
###########################################################

"
#----------------------------------------------------------
#  1) Leslie's 5 favorite tests
#----------------------------------------------------------
" printNL;

10 printNL;
10 do: [^self printNL;];
10 asList do: [^self printNL;];
10,10 do: [^self printNL;];
10,"hello" do: [^self printNL;];

"
#----------------------------------------------------------
#  2) Section on homogenious lists
#----------------------------------------------------------
" printNL;

!list <- 1, 2, 3, 4;
list do: [^self printNL;];
" " printNL;

list do: [(^self + 2) printNL;];
" " printNL;

list do: [^my list do: [^self printNL];];
" " printNL;

# TRY A BINARY MESSAGE
!list <- 5.3, 6.2, 7.1;
list do: [^self printOf: 4.1234; " " printNL;];
list do: [^self print: 9.3; " " printNL;];

"
#----------------------------------------------------------
#  3) Section on homomorphic lists
#----------------------------------------------------------
" printNL;

!mlist <- 1,"hello", 2, 3, "there";
mlist do: [^self printNL;];
" " printNL;

mlist do: [^my mlist do: [^self print]; " " printNL;];
" " printNL;

!mlist <- 1,"it",2,3;
mlist send: [^self printNL;];

# TRY A BINARY MESSAGE
!list <- 5.3, 4, 6, 6.2, 7.1;
list do: [^self printOf: 4.1234; " " printNL;];
list do: [^self print: 9.3; " " printNL;];

"
#----------------------------------------------------------
#  4) Section on date arithmetic
#----------------------------------------------------------
" printNL;

[
	!d <- 19861231 asDate;
	!xd <- 19830617 asDate extendBy:[];

	!i <- 1 monthEnds;
	!xi <- 62 days extendBy: [];

	!X <- Object newPrototype specializeClass;
	X __defineFixedProperty: 'd'.
	  __defineFixedProperty: 'i'.
	  __defineMethod: [|print| " " print ; d print: 11; i print; ^self];

	X new do: [:d <- ^my  d; :i <- ^my  i];
	X new do: [:d <- ^my  d; :i <- ^my xi];
	X new do: [:d <- ^my xd; :i <- ^my  i];
	X new do: [:d <- ^my xd; :i <- ^my xi];

        "... (d + i) " printNL ; 
	X instanceList do: [(d + i) print: 10; printNL];
        "... (i increment: d )" printNL ;
	X instanceList do: [(i increment: d) print: 10; printNL];

        "... (d - i) " printNL ;
	X instanceList do: [(d - i) print: 10; printNL];
        "... (i decrement: d)" printNL ; 
	X instanceList do: [(i decrement: d) print: 10; printNL];
] value;



###########################################################
#		End of tests ...			  #
###########################################################
newLine print; "End of file: basics.S" printNL; "" printNL;
?g
?t
Utility stopEchoToFile;
?g
