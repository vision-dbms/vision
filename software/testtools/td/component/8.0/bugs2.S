#**********************************************************
#**********************************************************
#		Test: bugs2
#
#  Test for previously found bugs.
#
#  11/20/2012    jck
#**********************************************************
#**********************************************************


Utility echoToFile: (^tmp componentOutput concat:"/bugs2.O");
newLine print; "Beginning file: bugs2.S ..." printNL;
###########################################################
#		Tests Begin Here ...                      #
###########################################################


###
#  Bug #23  Nov 20, 2012.
#
#  The following caused a segmentation fault in early versions of 7.2
#
###

"We do not want a segmentation fault here!" printNL; 
1,1 do:	[1 asFloat asInteger];
?g
###
#  End of Bug 23 test
###


###########################################################
#		End of tests ...			  #
###########################################################

newLine print; "End of file: bugs2.S" printNL; newLine print;
Utility stopEchoToFile;
?g



