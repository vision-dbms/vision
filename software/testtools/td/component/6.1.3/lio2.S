#**********************************************************
#**********************************************************
#		Test: lio 
#
#  Test the lio module
#  PART 1 B - Prints continued 
#
#  7/5/88	jad
# 10/17/02      jck   Added test ## 14
#**********************************************************
#**********************************************************


Utility echoToFile: (^tmp componentOutput concat:"/lio2.O");
newLine print; "Beginning file: lio2.S ..." printNL;
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
?g
?t

"
#----------------------------------------------------------
#  Normal Input (lio)
#----------------------------------------------------------
" printNL;


"

-------------------------------------------------------
-------------------------------------------------------
------  PART 1 B - Test LIO Print Functions
------
-------------------------------------------------------
-------------------------------------------------------

###############################
###############################
###  continue testing large prints
###############################
###############################

### 10
### force it to realloc - KSIs
" printNL;
list60 do:
	[1,2,3 do:
		[1,2,3,4,5,6,7,8,9,10 do:
			['value' print]]];
"" printNL;
?g
?t

"
### 11
### One huge element
" printNL;

1 asList do: ["-" fill: 40000 . print];
"" printNL;
?g
?t

"
### 12
### force it to realloc several times
" printNL;
list60 do: ["+" fill: 600 . print];
"" printNL;
?g
?t


"
### 13
### largest double 
" printNL;
-179769313486231571000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.00
asList do: [printNL];

?g
?t

## 14
### divertOutput/substituteOutputOf unable to open file

[
"hello" printNL;
!result1 <- "dogfood/kibble" substituteOutputOf:[
"This is a test of the emergency broadcast system" printNL;
];
"If this had been an actual emergency, you would have been instructed to ..." printNL;
!result2 <- "dogfood/kibble" appendOutputOf:[
"...seek shelter in the wings of a prayer."printNL;
"This concludes our test of the emergency broadcast system." printNL;
];
"Thank you" printNL;
result1 print; result2 printNL
] divertOutput -> !x;
x printNL;




###########################################################
#		End of tests ...			  #
###########################################################
newLine print; "End of file: lio2.S" printNL; newLine print;
?g
?t
Utility stopEchoToFile;
?g

