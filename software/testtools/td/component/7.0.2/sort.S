#**********************************************************
#**********************************************************
#		Test: sort
#
#  Test Sortings: 
#	strip
#
#  October 3, 2006	jck
#**********************************************************
#**********************************************************


Utility echoToFile: (^tmp componentOutput concat:"/sort.O");
newLine print; "Beginning file: sort.S ..." printNL;
###########################################################
#		Tests Begin Here ...                      #
###########################################################
?d
#RTYPE_C_Vector displayParameters,
QUIT

"Test default Sort behavior:	" printNL ;

Utility SessionAttribute visionPre7Ordering <- FALSE ;

1,2,3,NA,NA,4,-2.0,-5 sortUp:[^self] .do:[printNL]; "-" fill:10 . printNL;
1,2,3,NA,NA,4,-2.0,-5 sortDown:[^self] .do:[printNL]; "-" fill:10 . printNL;
"abc", "this", "is" fixed, -5, 10, 1700 sortUp: [ ^self ] . do: [ printNL; ]; "-" fill:10 . printNL;
"abc", "this", "is" fixed, -5, 10, 1700 sortDown: [ ^self ] . do: [ printNL; ]; "-" fill:10 . printNL;

!dts <- 20051231 asDate, 20041231 asDate, 19990101 asDate,
        "20060606" fixed, NA asDate, "20010131", 19841231;

dts sortUp: [ ^self ] . do: [ ^self printNL; ]; "-" fill:10 . printNL;
dts sortDown: [ ^self ] . do: [ ^self printNL; ]; "-" fill:20 . printNL;
	
"Test pre7 Sort behavior:	" printNL ;

Utility SessionAttribute visionPre7Ordering <- TRUE ;

1,2,3,NA,NA,4,-2.0,-5 sortUp:[^self] .do:[printNL]; "-" fill:10 . printNL;
1,2,3,NA,NA,4,-2.0,-5 sortDown:[^self] .do:[printNL]; "-" fill:10 . printNL;
"abc", "this", "is" fixed, -5, 10, 1700 sortUp: [ ^self ] . do: [ printNL; ]; "-" fill:10 . printNL;
"abc", "this", "is" fixed, -5, 10, 1700 sortDown: [ ^self ] . do: [ printNL; ]; "-" fill:10 . printNL;

dts sortUp: [ ^self ] . do: [ ^self printNL; ]; "-" fill:10 . printNL;
dts sortDown: [ ^self ] . do: [ ^self printNL; ]; "-" fill:20 . printNL;
	
# restore default sort behavior:	
Utility SessionAttribute visionPre7Ordering <- FALSE ;

###########################################################
#		End of tests ...			  #
###########################################################
newLine print; "End of file: sort.S" printNL; "" printNL;
?g
?t
Utility stopEchoToFile;
?g

