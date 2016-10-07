#**********************************************************
#**********************************************************
#		Test: dateRange 
#
#  Test date ranges 
#
#  2/1/88	jad
#**********************************************************
#**********************************************************


Utility echoToFile: "dateRange.O";
newLine print; "Beginning file: dateRange.S ..." printNL;
###########################################################
#		Tests Begin Here ...                      #
###########################################################

  
"


********************************************************
(PART 1)
--------  Test Date Ranges to make sure         --------
--------  that they work.                       --------
********************************************************
" print;

"
------------------------------------------------------
(1A)
---  Test the 'ForDatesInRangeEvaluate' Primitive  ---
------------------------------------------------------
" print;

"
-----------------------------------
(1A1)
---  Test the two converses:    --- 
---  'by:to:evaluate:' and      ---
---  'by:from:evaluate:'        ---
-----------------------------------
" print;

### Make an extended date
!xdate <- 19841231 asDate extendBy: [];

### Test 'by:to:evaluate:' with valid input
1 years from: xdate to: 19851231 asDate evaluate: [^date printNL];
"" printNL;

### Try to send the converse to an invalid date -> selector not found
1 years from: "I'm not a date" to: 19851231 asDate evaluate: [];

### Test 'by:from:evaluate:' with valid input
1 years from: 19841231 asDate to: xdate evaluate: [^date printNL];
"" printNL;

### Try to send the converse to an invalid date -> selector not found
1 years from: 19841231 asDate to: "I'm a bad date" evaluate: [];

### Test both converses
1 years from: xdate to: xdate evaluate: [^date printNL];
"" printNL;

"
-----------------------------------
(1A2)
---  Test the scalar case:      --- 
-----------------------------------
" print;

19811231 asDate to: 19851231 asDate by: 1 yearEnds . evaluate: [^date printNL];
"" printNL;

19851231 asDate to: 19811231 asDate by: 1 yearEnds . evaluate: [^date printNL];
"" printNL;

"
-----------------------------------
(1A3)
---  Test the non-scalar case:  --- 
-----------------------------------
" print;

### Test the simple case where every date range in the list is identical
!shortList <- Named Company send: [GM, IBM, XON, F];
shortList do: [19811231 asDate to: 19851231 asDate by: 2 yearEnds .
                evaluate: [^date print: 20; sales printNL]];
"" printNL;


### Test a case where every date range in the list is not identical
!cmlx <- Company masterList select: [compustat date isntNA && sales > 8000];
cmlx count printNL;
cmlx do: [name printNL; 
          compustat date to: compustat date - 6 years by: 1 years . 
                evaluate: ["  " print; ^date print: 20; sales printNL]];
"" printNL;

### Test a one element list
!list <- Named Company IBM asList;
list do: [19840101 asDate to: 19860203 asDate by: 1 years . 
                evaluate: [^date print: 20; sales printNL]];
"" printNL;


"
-----------------------------------
(1A4)
---  Test every Date Increment  ---
---  type			---
-----------------------------------
" print;

### days
19811231 asDate to: 19821231 asDate by: 100 days .
	 evaluate: [^date printNL];
"" printNL;

### bdays
19811231 asDate to: 19821231 asDate by: 100 businessDays . 
	evaluate: [^date printNL];
"" printNL;

### month
19811231 asDate to: 19821231 asDate by: 4 months .
	 evaluate: [^date printNL];
"" printNL;

### monthbegin
19811231 asDate to: 19821231 asDate by: 4 monthBeginnings .
	 evaluate: [^date printNL];
"" printNL;

### monthend
19811231 asDate to: 19821231 asDate by: 4 monthEnds .
	 evaluate: [^date printNL];
"" printNL;

### quarter
19811231 asDate to: 19821231 asDate by: 2 quarters .
	 evaluate: [^date printNL];
"" printNL;

### quarterbegin
19811231 asDate to: 19821231 asDate by: 2 quarterBeginnings .
	 evaluate: [^date printNL];
"" printNL;

### quarterend
19811231 asDate to: 19821231 asDate by: 2 quarterEnds .
	 evaluate: [^date printNL];
"" printNL;

### year
19811231 asDate to: 19831231 asDate by: 2 years .
	 evaluate: [^date printNL];
"" printNL;

### yearbegin
19811231 asDate to: 19831231 asDate by: 2 yearBeginnings .
	 evaluate: [^date printNL];
"" printNL;

### yearend
19811231 asDate to: 19831231 asDate by: 2 yearEnds .
	 evaluate: [^date printNL];
"" printNL;




"
-----------------------------------
(1A5)
---  Test polymorphic lists:	---
-----------------------------------
" print;

#---!myCompany <- Company newPrototype specialized;
Company newPrototype specializeAs: "myCompany" ;
myCompany defineFixedProperty: 'date1';
myCompany defineFixedProperty: 'date2';
#myCompany defineFixedProperty: 'name';

!a <- myCompany createInstance: "a" ;
!b <- myCompany createInstance: "b" ;
!c <- myCompany createInstance: "c" ;
!d <- myCompany createInstance: "d" ;
!e <- myCompany createInstance: "e" ;
!f <- myCompany createInstance: "f" ;
!g <- myCompany createInstance: "g" ;

!x <- 19841231 asDate extendBy: [];

a :date1 <- 19810101 asDate;
a :date2 <- x;
b :date1 <- "a string";
b :date2 <- 19801231 asDate;
c :date1 <- x;
c :date2 <- x;
d :date1 <- 19820505 asDate;
d :date2 <- "another string";
e :date1 <- 19841231 asDate;
e :date2 <- 19811231 asDate;
f :date1 <- 19810204 asDate;
f :date2 <- 19820404 asDate;
g :date1 <- 19861210 asDate;
g :date2 <- 19841010 asDate;

!list <- a,b,c,d,e,f,g;
list do: [name printNL;
          date1 to: date2 by: 1 years . 
                evaluate: ["   " print; ^date printNL]];
"" printNL;

list do: [name printNL;
          date1 to: date2 by: 1 yearEnds . 
                evaluate: ["   " print; ^date printNL]];
"" printNL;

list do: [name printNL;
          date1 to: date2 by: 3 months .
                evaluate: ["   " print; ^date printNL]];
"" printNL;


"

-----------------------------------------------------------------
(1B)
---  Test the underlying DATE_EnumerateDatesInRange function  ---
-----------------------------------------------------------------
" print;

"
------------------------------------
(1B1)
---  Check for Illegal Increment ---
---  multiple (0)		 ---
------------------------------------
" print;

0 yearEnds from: 19811231 asDate to: 19841231 asDate 
	evaluate: [^date printNL];


"
-----------------------------------
(1B2)
---  Test the Remember Last 	---
---  Range Feature.		---
-----------------------------------
" print;

### the same date range 3 times
19810101 asDate to: 19840101 asDate by: 1 years . evaluate: [^date printNL];
19810101 asDate to: 19840101 asDate by: 1 years . evaluate: [^date printNL];
19810101 asDate to: 19840101 asDate by: 1 years . evaluate: [^date printNL];
"" printNL;

### the same except for the startdate
19820101 asDate to: 19840101 asDate by: 1 years . evaluate: [^date printNL];
"" printNL;

### the same except for the enddate
19820101 asDate to: 19830101 asDate by: 1 years . evaluate: [^date printNL];
"" printNL;

### the same except for the increment multiple
19820101 asDate to: 19830101 asDate by: 2 years . evaluate: [^date printNL];
"" printNL;

### the same except for the increment type
19820101 asDate to: 19830101 asDate by: 2 quarters . evaluate: [^date printNL];
"" printNL;

### test a case where the number of dates is greater than the size of dvector
1 days from: 19800101 asDate to: 19820101 asDate evaluate: [^date printNL];
1 days from: 19800101 asDate to: 19820101 asDate evaluate: [^date printNL];
"" printNL;

"
-----------------------------------
(1B3)
---  Test backwards ranges	---
-----------------------------------
" print;

### startdate > enddate
19840101 asDate to: 19810101 asDate by: 1 yearEnds . 
	evaluate: [^date printNL];
"" printNL;

### startdate > enddate && -1 yearEnds
19840101 asDate to: 19810101 asDate by: -1 yearEnds . 
	evaluate: [^date printNL];
"" printNL;

### startdate < enddate && -1 yearEnds -> forward date
19811010 asDate to: 19841010 asDate by: -1 yearEnds . 
	evaluate: [^date printNL];

"


------------------------------------------------------
(1B4)
--------  Test the date low level routines    --------
--------  to guarentee that they are          --------
--------  returning the correct values for    --------
--------  all incrementTypes and increment    --------
--------  values.			      --------
------------------------------------------------------
" print;


"
...............  Month Tests ....................
" printNL;

19830131 asDate to: 19840131 asDate by: 1 months . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19830131 asDate by: 1 months . 
	evaluate: [^date printNL];
"" printNL;

19830303 asDate to: 19840303 asDate by: 1 months . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19830303 asDate by: 1 months . 
	evaluate: [^date printNL];
"" printNL;

19831231 asDate to: 19841231 asDate by: 1 months . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19831231 asDate by: 1 months . 
	evaluate: [^date printNL];
"" printNL;




19830131 asDate to: 19840131 asDate by: 1 monthEnds . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19830131 asDate by: 1 monthEnds . 
	evaluate: [^date printNL];
"" printNL;

19830303 asDate to: 19840303 asDate by: 1 monthEnds . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19830303 asDate by: 1 monthEnds . 
	evaluate: [^date printNL];
"" printNL;

19831231 asDate to: 19841231 asDate by: 1 monthEnds . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19831231 asDate by: 1 monthEnds . 
	evaluate: [^date printNL];
"" printNL;




19830131 asDate to: 19840131 asDate by: 1 monthBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19830131 asDate by: 1 monthBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19830303 asDate to: 19840303 asDate by: 1 monthBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19830303 asDate by: 1 monthBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19831231 asDate to: 19841231 asDate by: 1 monthBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19831231 asDate by: 1 monthBeginnings . 
	evaluate: [^date printNL];
"" printNL;


"
...............  Quarter Tests ....................
" printNL;


19830131 asDate to: 19840131 asDate by: 1 quarters . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19830131 asDate by: 1 quarters . 
	evaluate: [^date printNL];
"" printNL;

19830303 asDate to: 19840303 asDate by: 1 quarters . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19830303 asDate by: 1 quarters . 
	evaluate: [^date printNL];
"" printNL;

19831231 asDate to: 19841231 asDate by: 1 quarters . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19831231 asDate by: 1 quarters . 
	evaluate: [^date printNL];
"" printNL;




19830131 asDate to: 19840131 asDate by: 1 quarterEnds . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19830131 asDate by: 1 quarterEnds . 
	evaluate: [^date printNL];
"" printNL;

19830303 asDate to: 19840303 asDate by: 1 quarterEnds . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19830303 asDate by: 1 quarterEnds . 
	evaluate: [^date printNL];
"" printNL;

19831231 asDate to: 19841231 asDate by: 1 quarterEnds . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19831231 asDate by: 1 quarterEnds . 
	evaluate: [^date printNL];
"" printNL;




19830131 asDate to: 19840131 asDate by: 1 quarterBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19830131 asDate by: 1 quarterBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19830303 asDate to: 19840303 asDate by: 1 quarterBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19830303 asDate by: 1 quarterBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19831231 asDate to: 19841231 asDate by: 1 quarterBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19831231 asDate by: 1 quarterBeginnings . 
	evaluate: [^date printNL];
"" printNL;



"
...............  Year Tests ....................
" printNL;


19810131 asDate to: 19840131 asDate by: 1 years . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19810131 asDate by: 1 years . 
	evaluate: [^date printNL];
"" printNL;

19810303 asDate to: 19840303 asDate by: 1 years . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19810303 asDate by: 1 years . 
	evaluate: [^date printNL];
"" printNL;

19811231 asDate to: 19841231 asDate by: 1 years . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19811231 asDate by: 1 years . 
	evaluate: [^date printNL];
"" printNL;




19810131 asDate to: 19840131 asDate by: 1 yearEnds . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19810131 asDate by: 1 yearEnds . 
	evaluate: [^date printNL];
"" printNL;

19810303 asDate to: 19840303 asDate by: 1 yearEnds . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19810303 asDate by: 1 yearEnds . 
	evaluate: [^date printNL];
"" printNL;

19811231 asDate to: 19841231 asDate by: 1 yearEnds . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19811231 asDate by: 1 yearEnds . 
	evaluate: [^date printNL];
"" printNL;




19810131 asDate to: 19840131 asDate by: 1 yearBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19810131 asDate by: 1 yearBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19810303 asDate to: 19840303 asDate by: 1 yearBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19810303 asDate by: 1 yearBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19811231 asDate to: 19841231 asDate by: 1 yearBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19811231 asDate by: 1 yearBeginnings . 
	evaluate: [^date printNL];
"" printNL;




"
...............  Day Tests ....................
" printNL;

19880112 asDate to: 19880120 asDate by: 1 days .
	evaluate: [^date printNL];
"" printNL;

19880120 asDate to: 19880112 asDate by: 1 days .
	evaluate: [^date printNL];
"" printNL;

19880127 asDate to: 19880203 asDate by: 1 days .
	evaluate: [^date printNL];
"" printNL;

19880203 asDate to: 19880127 asDate by: 1 days .
	evaluate: [^date printNL];
"" printNL;



"
...............  BDay Tests ....................
" printNL;

19880112 asDate to: 19880120 asDate by: 1 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880120 asDate to: 19880112 asDate by: 1 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880127 asDate to: 19880203 asDate by: 1 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880203 asDate to: 19880127 asDate by: 1 businessDays .
	evaluate: [^date printNL];
"" printNL;



"
--- Test ranges that end and begin with weekend days ---.
" printNL;

### the first is a weekend the second is ok
19880123 asDate to: 19880126 asDate by: 1 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880126 asDate to: 19880123 asDate by: 1 businessDays .
	evaluate: [^date printNL];
"" printNL;


### both are weekends
19880123 asDate to: 19880131 asDate by: 1 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880131 asDate to: 19880123 asDate by: 1 businessDays .
	evaluate: [^date printNL];
"" printNL;


### both are weekends 
19880123 asDate to: 19880124 asDate by: 1 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880124 asDate to: 19880123 asDate by: 1 businessDays .
	evaluate: [^date printNL];
"" printNL;



### 1 businessDays range
19880123 asDate to: 19880125 asDate by: 1 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880125 asDate to: 19880123 asDate by: 1 businessDays .
	evaluate: [^date printNL];
"" printNL;




### the first is ok the second is a weekend
19880118 asDate to: 19880124 asDate by: 1 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880124 asDate to: 19880118 asDate by: 1 businessDays .
	evaluate: [^date printNL];
"" printNL;

### over a month bondary

19880131 asDate to: 19880206 asDate by: 1 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880206 asDate to: 19880131 asDate by: 1 businessDays .
	evaluate: [^date printNL];
"" printNL;


"
...............  Month Tests ....................
" printNL;

19830131 asDate to: 19840131 asDate by: 3 months . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19830131 asDate by: 3 months . 
	evaluate: [^date printNL];
"" printNL;

19830303 asDate to: 19840303 asDate by: 3 months . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19830303 asDate by: 3 months . 
	evaluate: [^date printNL];
"" printNL;

19831231 asDate to: 19841231 asDate by: 3 months . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19831231 asDate by: 3 months . 
	evaluate: [^date printNL];
"" printNL;



19830131 asDate to: 19840131 asDate by: 3 monthEnds . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19830131 asDate by: 3 monthEnds . 
	evaluate: [^date printNL];
"" printNL;

19830303 asDate to: 19840303 asDate by: 3 monthEnds . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19830303 asDate by: 3 monthEnds . 
	evaluate: [^date printNL];
"" printNL;

19831231 asDate to: 19841231 asDate by: 3 monthEnds . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19831231 asDate by: 3 monthEnds . 
	evaluate: [^date printNL];
"" printNL;




19830131 asDate to: 19840131 asDate by: 3 monthBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19830131 asDate by: 3 monthBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19830303 asDate to: 19840303 asDate by: 3 monthBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19830303 asDate by: 3 monthBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19831231 asDate to: 19841231 asDate by: 3 monthBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19831231 asDate by: 3 monthBeginnings . 
	evaluate: [^date printNL];
"" printNL;


"
...............  Quarter Tests ....................
" printNL;


19830131 asDate to: 19840131 asDate by: 3 quarters . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19830131 asDate by: 3 quarters . 
	evaluate: [^date printNL];
"" printNL;

19830303 asDate to: 19840303 asDate by: 3 quarters . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19830303 asDate by: 3 quarters . 
	evaluate: [^date printNL];
"" printNL;

19831231 asDate to: 19841231 asDate by: 3 quarters . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19831231 asDate by: 3 quarters . 
	evaluate: [^date printNL];
"" printNL;




19830131 asDate to: 19840131 asDate by: 3 quarterEnds . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19830131 asDate by: 3 quarterEnds . 
	evaluate: [^date printNL];
"" printNL;

19830303 asDate to: 19840303 asDate by: 3 quarterEnds . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19830303 asDate by: 3 quarterEnds . 
	evaluate: [^date printNL];
"" printNL;

19831231 asDate to: 19841231 asDate by: 3 quarterEnds . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19831231 asDate by: 3 quarterEnds . 
	evaluate: [^date printNL];
"" printNL;




19830131 asDate to: 19840131 asDate by: 3 quarterBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19830131 asDate by: 3 quarterBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19830303 asDate to: 19840303 asDate by: 3 quarterBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19830303 asDate by: 3 quarterBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19831231 asDate to: 19841231 asDate by: 3 quarterBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19831231 asDate by: 3 quarterBeginnings . 
	evaluate: [^date printNL];
"" printNL;



"
...............  Year Tests ....................
" printNL;


19810131 asDate to: 19840131 asDate by: 3 years . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19810131 asDate by: 3 years . 
	evaluate: [^date printNL];
"" printNL;

19810303 asDate to: 19840303 asDate by: 3 years . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19810303 asDate by: 3 years . 
	evaluate: [^date printNL];
"" printNL;

19811231 asDate to: 19841231 asDate by: 3 years . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19811231 asDate by: 3 years . 
	evaluate: [^date printNL];
"" printNL;




19810131 asDate to: 19840131 asDate by: 3 yearEnds . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19810131 asDate by: 3 yearEnds . 
	evaluate: [^date printNL];
"" printNL;

19810303 asDate to: 19840303 asDate by: 3 yearEnds . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19810303 asDate by: 3 yearEnds . 
	evaluate: [^date printNL];
"" printNL;

19811231 asDate to: 19841231 asDate by: 3 yearEnds . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19811231 asDate by: 3 yearEnds . 
	evaluate: [^date printNL];
"" printNL;




19810131 asDate to: 19840131 asDate by: 3 yearBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19810131 asDate by: 3 yearBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19810303 asDate to: 19840303 asDate by: 3 yearBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19810303 asDate by: 3 yearBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19811231 asDate to: 19841231 asDate by: 3 yearBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19811231 asDate by: 3 yearBeginnings . 
	evaluate: [^date printNL];
"" printNL;




"
...............  Day Tests ....................
" printNL;

19880112 asDate to: 19880120 asDate by: 3 days .
	evaluate: [^date printNL];
"" printNL;

19880120 asDate to: 19880112 asDate by: 3 days .
	evaluate: [^date printNL];
"" printNL;

19880127 asDate to: 19880203 asDate by: 3 days .
	evaluate: [^date printNL];
"" printNL;

19880203 asDate to: 19880127 asDate by: 3 days .
	evaluate: [^date printNL];
"" printNL;



"
...............  BDay Tests ....................
" printNL;

19880112 asDate to: 19880120 asDate by: 3 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880120 asDate to: 19880112 asDate by: 3 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880127 asDate to: 19880203 asDate by: 3 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880203 asDate to: 19880127 asDate by: 3 businessDays .
	evaluate: [^date printNL];
"" printNL;



"
--- Test ranges that end and begin with weekend days ---.
" printNL;

### the first is a weekend the second is ok
19880123 asDate to: 19880126 asDate by: 3 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880126 asDate to: 19880123 asDate by: 3 businessDays .
	evaluate: [^date printNL];
"" printNL;


### both are weekends
19880123 asDate to: 19880131 asDate by: 3 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880131 asDate to: 19880123 asDate by: 3 businessDays .
	evaluate: [^date printNL];
"" printNL;


### both are weekends 
19880123 asDate to: 19880124 asDate by: 3 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880124 asDate to: 19880123 asDate by: 3 businessDays .
	evaluate: [^date printNL];
"" printNL;



### 1 businessDays range
19880123 asDate to: 19880125 asDate by: 3 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880125 asDate to: 19880123 asDate by: 3 businessDays .
	evaluate: [^date printNL];
"" printNL;




### the first is ok the second is a weekend
19880118 asDate to: 19880124 asDate by: 3 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880124 asDate to: 19880118 asDate by: 3 businessDays .
	evaluate: [^date printNL];
"" printNL;

### over a month bondary

19880131 asDate to: 19880206 asDate by: 3 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880206 asDate to: 19880131 asDate by: 3 businessDays .
	evaluate: [^date printNL];
"" printNL;





"
---  Test increment skips over year-month-day bondaries ---
" printNL;

"
...............  Month Tests ....................
" printNL;

19830131 asDate to: 19840131 asDate by: 23 months . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19830131 asDate by: 23 months . 
	evaluate: [^date printNL];
"" printNL;

19830303 asDate to: 19840303 asDate by: 23 months . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19830303 asDate by: 23 months . 
	evaluate: [^date printNL];
"" printNL;

19831231 asDate to: 19841231 asDate by: 23 months . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19831231 asDate by: 23 months . 
	evaluate: [^date printNL];
"" printNL;




19830131 asDate to: 19840131 asDate by: 23 monthEnds . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19830131 asDate by: 23 monthEnds . 
	evaluate: [^date printNL];
"" printNL;

19830303 asDate to: 19840303 asDate by: 23 monthEnds . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19830303 asDate by: 23 monthEnds . 
	evaluate: [^date printNL];
"" printNL;

19831231 asDate to: 19841231 asDate by: 23 monthEnds . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19831231 asDate by: 23 monthEnds . 
	evaluate: [^date printNL];
"" printNL;




19830131 asDate to: 19840131 asDate by: 23 monthBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19830131 asDate by: 23 monthBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19830303 asDate to: 19840303 asDate by: 23 monthBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19830303 asDate by: 23 monthBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19831231 asDate to: 19841231 asDate by: 23 monthBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19831231 asDate by: 23 monthBeginnings . 
	evaluate: [^date printNL];
"" printNL;


"
...............  Quarter Tests ....................
" printNL;


19830131 asDate to: 19840131 asDate by: 23 quarters . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19830131 asDate by: 23 quarters . 
	evaluate: [^date printNL];
"" printNL;

19830303 asDate to: 19840303 asDate by: 23 quarters . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19830303 asDate by: 23 quarters . 
	evaluate: [^date printNL];
"" printNL;

19831231 asDate to: 19841231 asDate by: 23 quarters . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19831231 asDate by: 23 quarters . 
	evaluate: [^date printNL];
"" printNL;




19830131 asDate to: 19840131 asDate by: 23 quarterEnds . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19830131 asDate by: 23 quarterEnds . 
	evaluate: [^date printNL];
"" printNL;

19830303 asDate to: 19840303 asDate by: 23 quarterEnds . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19830303 asDate by: 23 quarterEnds . 
	evaluate: [^date printNL];
"" printNL;

19831231 asDate to: 19841231 asDate by: 23 quarterEnds . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19831231 asDate by: 23 quarterEnds . 
	evaluate: [^date printNL];
"" printNL;




19830131 asDate to: 19840131 asDate by: 23 quarterBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19830131 asDate by: 23 quarterBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19830303 asDate to: 19840303 asDate by: 23 quarterBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19830303 asDate by: 23 quarterBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19831231 asDate to: 19841231 asDate by: 23 quarterBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19831231 asDate by: 23 quarterBeginnings . 
	evaluate: [^date printNL];
"" printNL;



"
...............  Year Tests ....................
" printNL;


19810131 asDate to: 19840131 asDate by: 23 years . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19810131 asDate by: 23 years . 
	evaluate: [^date printNL];
"" printNL;

19810303 asDate to: 19840303 asDate by: 23 years . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19810303 asDate by: 23 years . 
	evaluate: [^date printNL];
"" printNL;

19811231 asDate to: 19841231 asDate by: 23 years . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19811231 asDate by: 23 years . 
	evaluate: [^date printNL];
"" printNL;




19810131 asDate to: 19840131 asDate by: 23 yearEnds . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19810131 asDate by: 23 yearEnds . 
	evaluate: [^date printNL];
"" printNL;

19810303 asDate to: 19840303 asDate by: 23 yearEnds . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19810303 asDate by: 23 yearEnds . 
	evaluate: [^date printNL];
"" printNL;

19811231 asDate to: 19841231 asDate by: 23 yearEnds . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19811231 asDate by: 23 yearEnds . 
	evaluate: [^date printNL];
"" printNL;




19810131 asDate to: 19840131 asDate by: 23 yearBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19810131 asDate by: 23 yearBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19810303 asDate to: 19840303 asDate by: 23 yearBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19810303 asDate by: 23 yearBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19811231 asDate to: 19841231 asDate by: 23 yearBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19811231 asDate by: 23 yearBeginnings . 
	evaluate: [^date printNL];
"" printNL;




"
...............  Day Tests ....................
" printNL;

19880112 asDate to: 19880120 asDate by: 23 days .
	evaluate: [^date printNL];
"" printNL;

19880120 asDate to: 19880112 asDate by: 23 days .
	evaluate: [^date printNL];
"" printNL;

19880127 asDate to: 19880203 asDate by: 23 days .
	evaluate: [^date printNL];
"" printNL;

19880203 asDate to: 19880127 asDate by: 23 days .
	evaluate: [^date printNL];
"" printNL;



"
...............  BDay Tests ....................
" printNL;

19880112 asDate to: 19880120 asDate by: 23 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880120 asDate to: 19880112 asDate by: 23 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880127 asDate to: 19880203 asDate by: 23 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880203 asDate to: 19880127 asDate by: 23 businessDays .
	evaluate: [^date printNL];
"" printNL;



"
--- Test ranges that end and begin with weekend days ---.
" printNL;

### the first is a weekend the second is ok
19880123 asDate to: 19880126 asDate by: 23 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880126 asDate to: 19880123 asDate by: 23 businessDays .
	evaluate: [^date printNL];
"" printNL;


### both are weekends
19880123 asDate to: 19880131 asDate by: 23 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880131 asDate to: 19880123 asDate by: 23 businessDays .
	evaluate: [^date printNL];
"" printNL;


### both are weekends 
19880123 asDate to: 19880124 asDate by: 23 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880124 asDate to: 19880123 asDate by: 23 businessDays .
	evaluate: [^date printNL];
"" printNL;



### 1 businessDays range
19880123 asDate to: 19880125 asDate by: 23 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880125 asDate to: 19880123 asDate by: 23 businessDays .
	evaluate: [^date printNL];
"" printNL;




### the first is ok the second is a weekend
19880118 asDate to: 19880124 asDate by: 23 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880124 asDate to: 19880118 asDate by: 23 businessDays .
	evaluate: [^date printNL];
"" printNL;

### over a month bondary

19880131 asDate to: 19880206 asDate by: 23 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880206 asDate to: 19880131 asDate by: 23 businessDays .
	evaluate: [^date printNL];
"" printNL;



"
------------------------------------------------
---   Multi element Ranges with large skips  ---
------------------------------------------------
" print,
"
...............  Month Tests ....................
" printNL;

19700131 asDate to: 19840131 asDate by: 25 months . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19700131 asDate by: 25 months . 
	evaluate: [^date printNL];
"" printNL;

19700303 asDate to: 19840303 asDate by: 25 months . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19700303 asDate by: 25 months . 
	evaluate: [^date printNL];
"" printNL;

19701231 asDate to: 19841231 asDate by: 25 months . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19701231 asDate by: 25 months . 
	evaluate: [^date printNL];
"" printNL;




19700131 asDate to: 19840131 asDate by: 25 monthEnds . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19700131 asDate by: 25 monthEnds . 
	evaluate: [^date printNL];
"" printNL;

19700303 asDate to: 19840303 asDate by: 25 monthEnds . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19700303 asDate by: 25 monthEnds . 
	evaluate: [^date printNL];
"" printNL;

19701231 asDate to: 19841231 asDate by: 25 monthEnds . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19701231 asDate by: 25 monthEnds . 
	evaluate: [^date printNL];
"" printNL;




19700131 asDate to: 19840131 asDate by: 25 monthBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19700131 asDate by: 25 monthBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19700303 asDate to: 19840303 asDate by: 25 monthBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19700303 asDate by: 25 monthBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19701231 asDate to: 19841231 asDate by: 25 monthBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19701231 asDate by: 25 monthBeginnings . 
	evaluate: [^date printNL];
"" printNL;


"
...............  Quarter Tests ....................
" printNL;


19650131 asDate to: 19840131 asDate by: 25 quarters . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19650131 asDate by: 25 quarters . 
	evaluate: [^date printNL];
"" printNL;

19650303 asDate to: 19840303 asDate by: 25 quarters . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19650303 asDate by: 25 quarters . 
	evaluate: [^date printNL];
"" printNL;

19651231 asDate to: 19841231 asDate by: 25 quarters . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19651231 asDate by: 25 quarters . 
	evaluate: [^date printNL];
"" printNL;




19650131 asDate to: 19840131 asDate by: 25 quarterEnds . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19650131 asDate by: 25 quarterEnds . 
	evaluate: [^date printNL];
"" printNL;

19650303 asDate to: 19840303 asDate by: 25 quarterEnds . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19650303 asDate by: 25 quarterEnds . 
	evaluate: [^date printNL];
"" printNL;

19651231 asDate to: 19841231 asDate by: 25 quarterEnds . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19651231 asDate by: 25 quarterEnds . 
	evaluate: [^date printNL];
"" printNL;




19650131 asDate to: 19840131 asDate by: 25 quarterBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19650131 asDate by: 25 quarterBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19650303 asDate to: 19840303 asDate by: 25 quarterBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19650303 asDate by: 25 quarterBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19651231 asDate to: 19841231 asDate by: 25 quarterBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19651231 asDate by: 25 quarterBeginnings . 
	evaluate: [^date printNL];
"" printNL;



"
...............  Year Tests ....................
" printNL;


19250131 asDate to: 19840131 asDate by: 25 years . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19250131 asDate by: 25 years . 
	evaluate: [^date printNL];
"" printNL;

19250303 asDate to: 19840303 asDate by: 25 years . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19250303 asDate by: 25 years . 
	evaluate: [^date printNL];
"" printNL;

19251231 asDate to: 19841231 asDate by: 25 years . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19251231 asDate by: 25 years . 
	evaluate: [^date printNL];
"" printNL;




19250131 asDate to: 19840131 asDate by: 25 yearEnds . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19250131 asDate by: 25 yearEnds . 
	evaluate: [^date printNL];
"" printNL;

19250303 asDate to: 19840303 asDate by: 25 yearEnds . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19250303 asDate by: 25 yearEnds . 
	evaluate: [^date printNL];
"" printNL;

19251231 asDate to: 19841231 asDate by: 25 yearEnds . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19251231 asDate by: 25 yearEnds . 
	evaluate: [^date printNL];
"" printNL;




19250131 asDate to: 19840131 asDate by: 25 yearBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19840131 asDate to: 19250131 asDate by: 25 yearBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19250303 asDate to: 19840303 asDate by: 25 yearBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19840303 asDate to: 19250303 asDate by: 25 yearBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19251231 asDate to: 19841231 asDate by: 25 yearBeginnings . 
	evaluate: [^date printNL];
"" printNL;

19841231 asDate to: 19251231 asDate by: 25 yearBeginnings . 
	evaluate: [^date printNL];
"" printNL;




"
...............  Day Tests ....................
" printNL;

19880112 asDate to: 19880316 asDate by: 25 days .
	evaluate: [^date printNL];
"" printNL;

19880316 asDate to: 19880112 asDate by: 25 days .
	evaluate: [^date printNL];
"" printNL;

19880127 asDate to: 19880427 asDate by: 25 days .
	evaluate: [^date printNL];
"" printNL;

19880427 asDate to: 19880127 asDate by: 25 days .
	evaluate: [^date printNL];
"" printNL;



"
...............  BDay Tests ....................
" printNL;

19880112 asDate to: 19880316 asDate by: 25 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880316 asDate to: 19880112 asDate by: 25 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880127 asDate to: 19880427 asDate by: 25 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880427 asDate to: 19880127 asDate by: 25 businessDays .
	evaluate: [^date printNL];
"" printNL;



"
--- Test ranges that end and begin with weekend days ---.
" printNL;

### the first is a weekend the second is ok
19880123 asDate to: 19880522 asDate by: 25 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880522 asDate to: 19880123 asDate by: 25 businessDays .
	evaluate: [^date printNL];
"" printNL;


### both are weekends
19880123 asDate to: 19880528 asDate by: 25 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880528 asDate to: 19880123 asDate by: 25 businessDays .
	evaluate: [^date printNL];
"" printNL;


### both are weekends - adjacent weekend days
19880123 asDate to: 19880124 asDate by: 25 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880124 asDate to: 19880123 asDate by: 25 businessDays .
	evaluate: [^date printNL];
"" printNL;



### 1 businessDays range
19880123 asDate to: 19880125 asDate by: 25 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880125 asDate to: 19880123 asDate by: 25 businessDays .
	evaluate: [^date printNL];
"" printNL;




### the first is ok the second is a weekend
19880118 asDate to: 19880430 asDate by: 25 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880430 asDate to: 19880118 asDate by: 25 businessDays .
	evaluate: [^date printNL];
"" printNL;

### over a month bondary

19880131 asDate to: 19880206 asDate by: 25 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880206 asDate to: 19880131 asDate by: 25 businessDays .
	evaluate: [^date printNL];
"" printNL;



"
--------------------
---  Leap Years  ---
--------------------
" print;

19870131 asDate to: 19880331 asDate by: 1 monthEnds .
	evaluate: [^date printNL];
"" printNL;

19880331 asDate to: 19870131 asDate by: 1 monthEnds .
	evaluate: [^date printNL];
"" printNL;

19870131 asDate to: 19880331 asDate by: 1 months .
	evaluate: [^date printNL];
"" printNL;

19880331 asDate to: 19870131 asDate by: 1 months .
	evaluate: [^date printNL];
"" printNL;



19870231 asDate to: 19880331 asDate by: 1 quarters .
	evaluate: [^date printNL];
"" printNL;

19880331 asDate to: 19870231 asDate by: 1 quarters .
	evaluate: [^date printNL];
"" printNL;



19870228 asDate to: 19890228 asDate by: 1 years .
	evaluate: [^date printNL];
"" printNL;

19890228 asDate to: 19870228 asDate by: 1 years .
	evaluate: [^date printNL];
"" printNL;



19880222 asDate to: 19880302 asDate by: 1 days .
	evaluate: [^date printNL];
"" printNL;

19880302 asDate to: 19880222 asDate by: 1 days .
	evaluate: [^date printNL];
"" printNL;

19880222 asDate to: 19880302 asDate by: 1 businessDays .
	evaluate: [^date printNL];
"" printNL;

19880302 asDate to: 19880222 asDate by: 1 businessDays .
	evaluate: [^date printNL];
"" printNL;


"


**********************************************************
(PART 2)
--------  Test Date Ranges with specific timings  --------
--------  primitive with some specific timings    -------- 
--------  to make sure that it is not slower.     --------
**********************************************************
" print;

### setup
#---!cmlx <- Company masterList select: [compustat date isntNA && sales > 8000];
#---!cml <- Company masterList select: [compustat date isntNA];
!cmlx <- Named Company send: [ IBM, F, GM, C, DEC, HWP ] ;
!cml <- Company masterList ;
"Small list count: " print; cmlx count printNL;
"Large list count: " print; cml count printNL;
?g
?t

"
---------------------------------------------------
(2A)
---  Time using a small list 			---
---------------------------------------------------
" print;

"
-----------------------------------
(2A1)
---  Use a small list with the	---
---  same non-daily date-range  ---
---  for every element		---
-----------------------------------
" print;

cmlx do: 
	[19861231 asDate to: 19861231 asDate - 6 years by: 1 years .
		evaluate: [sales]];
"" printNL;
?g
?t

"
-----------------------------------
(2A2)
---  Use a small list with a	---
---  different non-daily 	---
---  date-range for every 	---
---  element			---
-----------------------------------
" print;

cmlx do: 
	[compustat date to: compustat date - 6 years by: 1 years .
		evaluate: [sales]];
"" printNL;
?g
?t

"
-----------------------------------
(2A3)
---  Use a small list with the	---
---  same daily date-range 	---
---  for every element		---
-----------------------------------
" print;

cmlx do:
	[19840101 asDate to: 19840101 asDate - 6 days by: 1 days .
		evaluate: [sales]];
"" printNL;
?g
?t

"
-----------------------------------
(2A4)
---  Use a small list with a 	---
---  different daily date-range ---
---  for every element		---
-----------------------------------
" print;

cmlx do:
	[compustat date to: compustat date - 6 days by: 1 days .
		evaluate: [sales]];
"" printNL;
?g
?t

"
---------------------------------------------------
(2B)
---  Time using a large list 			---
---------------------------------------------------
" print;

"
-----------------------------------
(2B1)
---  Use a large list with the	---
---  same non-daily date-range  ---
---  for every element		---
-----------------------------------
" print;

cml do: 
	[19861231 asDate to: 19861231 asDate - 6 years by: 1 years .
		evaluate: [sales]];
"" printNL;
?g
?t

"
-----------------------------------
(2B2)
---  Use a large list with a	---
---  different non-daily 	---
---  date-range for every 	---
---  element			---
-----------------------------------
" print;

cml do: 
	[compustat date to: compustat date - 6 years by: 1 years .
		evaluate: [sales]];
"" printNL;
?g
?t

"
-----------------------------------
(2B3)
---  Use a large list with the	---
---  same daily date-range 	---
---  for every element		---
-----------------------------------
" print;

cml do:
	[19840101 asDate to: 19840101 asDate - 6 days by: 1 days .
		evaluate: [sales]];
"" printNL;
?g
?t

"
-----------------------------------
(2B4)
---  Use a large list with a 	---
---  different daily date-range ---
---  for every element		---
-----------------------------------
" print;

cml do:
	[compustat date to: compustat date - 6 days by: 1 days .
		evaluate: [sales]];
"" printNL;
?g
?t



###########################################################
#		End of tests ...			  #
###########################################################
newLine print; "End of file: dateRange.S" printNL; newLine print;
?g
?t
Utility stopEchoToFile;
?g

