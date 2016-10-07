#**********************************************************
#**********************************************************
#		Test: RTdictionary
#
#  June 6, 2008	jck
#**********************************************************
#**********************************************************


Utility echoToFile: (^tmp componentOutput concat:"/RTdictionary.O");
newLine print; "Beginning file: RTdictionary.S ..." printNL;
###########################################################
#		Tests Begin Here ...                      #
###########################################################

"This test creates a test store and fills its dictionary to the breaking point" printNL ;
"It should generate a Warning and then an Error ..." printNL ;

!store <- Object basicSpecialized;
!count <- 0; 
[TRUE] whileTrue: [
   120 sequence do: [^global store __define: ("_" concat: ^my count .fill: 900 + ^self)];
   :count <- count + 1
];
?g
?t
newLine print;
" ... and then, a small addition should succeed with a warning. " printNL ; 
# ... in 7.1.3, it would fail with an error because the above code would have corrupted the dictionary.
# ... any earlier version would have been corrupted with no error or warning ...

store define:'z' toBe: 3;
?g

" and we should see the '3' we stored at 'z' ... not an SNF!!" printNL ; 
store z printNL;

!store <- NA;
###########################################################
#		End of tests ...			  #
###########################################################
newLine print; "End of file: RTdictionary.S" printNL; "" printNL;
?g
?t
Utility stopEchoToFile;
?g

