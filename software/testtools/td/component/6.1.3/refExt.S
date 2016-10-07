##################################################
# Deferred Alignment during ref extract
#
##################################################

Utility echoToFile: (^tmp componentOutput concat:"/refExt.O");
newLine print; "Beginning file: refExt.S ..." printNL;

Company createInstance:"DELL";
Company createInstance:"BUNZ";
Company createInstance:"CSCO";
Company createInstance:"INTC";
Company createInstance:"YHOO";

Named Company BUNZ rdelete;

companyList do:[^self print:10;];
newLine print;
companyEList do:[^self print:10; company printNL];

companyEList at:1 . :company <- (Company createInstance:"TXN");
companyEList do:[^self print:10; company printNL];

###########################################################
#		End of tests ...			  #
###########################################################
newLine print; "End of file: refExt.S" printNL; "" printNL;
?g
?t
Utility stopEchoToFile;
?g
