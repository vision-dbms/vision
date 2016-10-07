#**********************************************************
#**********************************************************
#		Test: performance 
#
#  Tests code which was changed to improve the system's
#  performance.
#
#  10/14/87		jad
#**********************************************************
#**********************************************************


Utility echoToFile: "performance.O";
newLine print; "Beginning file: performance.S ..." printNL;
###########################################################
#		Tests Begin Here ...                      #
###########################################################

"
#----------------------------------------------------------
#  Setup (performance)
#----------------------------------------------------------
" printNL;




"
#----------------------------------------------------------
#  Normal Input (performance)
#----------------------------------------------------------
" printNL;


#######  Test changes made to RTvector.c on 10/14/87.  
#######  LCExtract and RefExtract now do a binary search for the
#######  correct pmap entry instead of a linear search.
Named Company IBM, Named Company GM, Named Company XON, Named Company F
select: [sales > 10000] . do: [name print: 30; sales printNL;];

#######  Test changes made to RTvector.c on 10/21/87.
#######  Special cased vectors with a single pmap entry in
#######  LCExtract.  (Added a SimpleLCExtract debuger counter
#######  in #RTYPE_C_Vector displayCounts.)
Named Company IBM, Named Company GM, Named Company XON, Named Company F
do: [sales/income printNL;];

"
#----------------------------------------------------------
#  Erroneous Input (performance)
#----------------------------------------------------------
" printNL;




"
#----------------------------------------------------------
#  Edge Conditions (performance)
#----------------------------------------------------------
" printNL;




"
#----------------------------------------------------------
#  Cleanup (performance)
#----------------------------------------------------------
" printNL;


###########################################################
#		End of tests ...			  #
###########################################################
newLine print; "End of file: performance.S" printNL; newLine print;
?g
?t
Utility stopEchoToFile;
?g

