#**********************************************************
#**********************************************************
#		Test: cleanDictionary
#
#  Test for previously found bugs.
#
#  02/08/2014    jwang
#**********************************************************
#**********************************************************


Utility echoToFile: (^tmp componentOutput concat:"/cleanDictionary.O");
newLine print; "Beginning file: cleanDictionary.S ..." printNL;
###########################################################
#		Tests Begin Here ...                      #
###########################################################


"Size before cleanDictionary:" printNL;
!bSize <- ( Object asPOP getNthPOP:4 .getNthPOP:0 .getNthPOP:2 .containerSize )
?g
Object cleanDictionary;
?g
"" printNL;
"Size after cleanDictionary:" printNL;
!aSize <- ( Object asPOP getNthPOP:4 .getNthPOP:0 .getNthPOP:2 .containerSize )
?g


###########################################################
#		End of tests ...			  #
###########################################################

newLine print; "End of file: cleanDictionary.S" printNL; newLine print;
Utility stopEchoToFile;
?g



