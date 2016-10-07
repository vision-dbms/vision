#**********************************************************
#**********************************************************
#		Test: mksfile
#
#  Test for previously found bugs.
#
#  02/19/2014    jwang
#**********************************************************
#**********************************************************


Utility echoToFile: (^tmp componentOutput concat:"/mksfile.O");
newLine print; "Beginning file: mksfile.S ..." printNL;
###########################################################
#		Tests Begin Here ...                      #
###########################################################


"grep CoreWorkspace" filterOutputOf:[displayInheritance]
?G
"grep -v CoreWorkspace" filterOutputOf:[displayInheritance]
?G
"grep BuiltInWorkspace" filterOutputOf:[displayInheritance]
?G


###########################################################
#		End of tests ...			  #
###########################################################

newLine print; "End of file: mksfile.S" printNL; newLine print;
Utility stopEchoToFile;
?g



