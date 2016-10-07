#**********************************************************
#**********************************************************
#		Test: stringRefNil
#
#  Test StringRefNil: 
#
#  Februrary 13, 2009	jck
#**********************************************************
#**********************************************************


Utility echoToFile: (^tmp componentOutput concat:"/stringRefNil.O");
newLine print; "Beginning file: stringRefNil.S ..." printNL;
###########################################################
#		Tests Begin Here ...                      #
###########################################################

"Ref nil compared to empty string .... scalar case .. should be TRUE" printNL;
(-1 asReferenceTo: "" fixed . = "") printNL ; 

"Ref nil compared to non empty string .... scalar case .. should be FALSE" printNL;
(-1 asReferenceTo: "" fixed . = "hey there") printNL ; 

"Ref nil compared to empty string .... single element type list  case .. should be TRUE" printNL;
1,2 send: [(-1 asReferenceTo: "" fixed . = "") printNL ]. do:[printNL];

"Ref nil compared to non empty string .... single element type list  case .. should be FALSE" printNL;
1,2 send: [(-1 asReferenceTo: "" fixed . = "hi there") printNL]. do:[printNL];

"Ref nil compared to non empty and strings .... single element type list  case .. should be mixed" printNL;
1,3,0,2 send: [(-1 asReferenceTo: "" fixed . = ((^self > 1) ifTrue:["ho there"] ifFalse:[""])) printNL ]. do:[printNL];

"Ref nil compared to empty string .... two element type list  case .. should be TRUE" printNL;
1,2.0 send: [(-1 asReferenceTo: "" fixed . = "") printNL ]. do:[printNL];

"Ref nil compared to non empty string .... two element type list  case .. should be FALSE" printNL;
1,2.0 send: [(-1 asReferenceTo: "" fixed . = "ho there") printNL ]. do:[printNL];

"Ref nil compared to non empty and empty strings .... two element type list  case .. should be mixed" printNL;
1,2.0,0,2 send: [(-1 asReferenceTo: "" fixed . = ((^self > 1) ifTrue:["ho there"] ifFalse:[""])) printNL ]. do:[printNL];

###########################################################
#		End of tests ...			  #
###########################################################
newLine print; "End of file: stringRefNil.S" printNL; "" printNL;
?g
?t
Utility stopEchoToFile;
?g

