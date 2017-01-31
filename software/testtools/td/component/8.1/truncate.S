#**********************************************************
#**********************************************************
#		Test: truncate
#
#  Test for previously found bugs.
#
#  01/28/2014    jwang
#**********************************************************
#**********************************************************


Utility echoToFile: (^tmp componentOutput concat:"/truncate.O");
newLine print; "Beginning file: truncate.S ..." printNL;
###########################################################
#		Tests Begin Here ...                      #
###########################################################


"1" print; someMethod1 printNL;
"2" print; someMethod2 printNL;
"3" print; someMethod3 printNL;
"4" print; someMethod4 printNL;
"5" print; someMethod5 printNL;
"6" print; someMethod6 printNL;


###########################################################
#		End of tests ...			  #
###########################################################

newLine print; "End of file: truncate.S" printNL; newLine print;
Utility stopEchoToFile;
?g



