#**********************************************************
#**********************************************************
#		Test: circRef
#
#  Test StringRefNil: 
#
#  Februrary 13, 2009	jck
#**********************************************************
#**********************************************************


Utility echoToFile: (^tmp componentOutput concat:"/circRef.O");
newLine print; "Beginning file: circRef.S ..." printNL;
###########################################################
#		Tests Begin Here ...                      #
###########################################################
	
?od+
?d
#M sessionGCTrace on,
QUIT

!count <- 50;
[count > 0] whileTrue: [!x <- ^current; !y <- 25000000 sequence; : count <- (count -1) asInteger];

###########################################################
#		End of tests ...			  #
###########################################################
newLine print; "End of file: circRef.S" printNL; "" printNL;
?g
?t
Utility stopEchoToFile;
?g
?d
#M sessionGCTrace off,
QUIT
^tmp define:'collectSessionGarbage' toBe: 4 asPrimitive;
^tmp collectSessionGarbage;
?g
