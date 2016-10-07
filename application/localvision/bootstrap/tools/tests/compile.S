#**********************************************************
#**********************************************************
#		Test: compile 
#
#  Comment about the kind of tests ...
#
#  10/10/88		jck 
#**********************************************************
#**********************************************************


Utility echoToFile: "compile.O";
newLine print; "Beginning file: compile.S ..." printNL;
###########################################################
#		Tests Begin Here ...                      #
###########################################################

"
#----------------------------------------------------------
#  Setup (compile)
#----------------------------------------------------------
" printNL;




"
#----------------------------------------------------------
#  Normal Input (compile)
#----------------------------------------------------------
" printNL;




"
#----------------------------------------------------------
#  Erroneous Input (compile)
#----------------------------------------------------------
" printNL;




"
#----------------------------------------------------------
#  Edge Conditions (compile)
#----------------------------------------------------------
" printNL;
?g

"
#----------------------
#  Account setup
#----------------------
" printNL ; 
Entity createSubclass: "Account" ; 
Account createInstance: "NEF" ;

?g

## List of statements with semi-colon missing. Makes a larger than expected
## keyword message ....

Named Account NEF navRankFromLatestDate: (^today - 1 monthEnds) 
     length: 1 sortBy: [ rank ]

Named Account NEF navRankFromLatestDate: (^today - 1 monthEnds)
        length: 1 sortBy: [ rank ]

## Complete list of companies, ranked by contribution - YTD
Named Account NEF navRankFromLatestDate: (^today - 1 monthEnds) 
     length: YTD sortBy: [ rank ]

## Complete List of Companies, ranked by alphabetically - 1 month
Named Account NEF navRankFromLatestDate: (^today - 1 monthEnds) 
     length: 1 sortBy: [ name ]

## Complete list of companies, ranked alphabetically - YTD
Named Account NEF navRankFromLatestDate: (^today - 1 monthEnds) 
     length: YTD sortBy: [ name ]

## Ten best & worst contributors, ranked by contribution - 1 month
Named Account NEF navBWFromLatestDate: (^today - 1 monthEnds) 
     length: 1 cutoff: 10  

## Ten best & worst contributors, ranked by contribution - YTD
Named Account NEF navBWFromLatestDate: (^today - 1 monthEnds) 
     length: YTD cutoff: 10  

## Grouped by Industry, sorted by contribution - 1 month
Named Account NEF navRankFromLatestDate: (^today - 1 monthEnds) 
     length: 1 groupedBy: [ fundIndustry ] sortBy: [ rank ]

## Grouped by Industry, sorted by contribution - YTD
Named Account NEF navRankFromLatestDate: (^today - 1 monthEnds) 
     length: YTD groupedBy: [ fundIndustry ] sortBy: [ rank ]

## Grouped by Sector, sorted by contribution - 1 month
Named Account NEF navRankFromLatestDate: (^today - 1 monthEnds) 
     length: 1 groupedBy: [ fundSector ] sortBy: [ rank ]

## Grouped by Sector, sorted by contribution - YTD
Named Account NEF navRankFromLatestDate: (^today - 1 monthEnds) 
     length: YTD groupedBy: [ fundSector ] sortBy: [ rank ]

;

##  Now attempt to compile a larger than ordinary integer:
111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
printNL;

##  and a larger than ordinary real number:
1111111.11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
printNL;

#  and a keyword message with a large parameter:
^current define:
'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'
;


##  and a larger than expected block parameter:
[|
:aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
|] printNL;

##  decompile a block with a large keyword in it's selector:

[| navRankFromLatestDate: x
     length: y sortByffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff: rank| ]
printNL;


"
#----------------------------------------------------------
#  Cleanup (compile)
#----------------------------------------------------------
" printNL;


###########################################################
#		End of tests ...			  #
###########################################################
newLine print; "End of file: compile.S" printNL; newLine print;

Utility stopEchoToFile;
?g

