#**********************************************************
#**********************************************************
#		Test: stringPrims 
#
#  Test String Primitives: 
#	fill, take, drop, concat, and sort.
#
#  9/10/87		jad 
#**********************************************************
#**********************************************************


Utility echoToFile: "stringPrims.O";
newLine print; "Beginning file: stringPrims.S ..." printNL;
###########################################################
#		Tests Begin Here ...                      #
###########################################################

"


********************************************************
(PART 1)
--------  Test the 'ToDouble' String Primitive  --------
********************************************************
" print;

"
------------------------------------------------------
(1A)
---  Test the 'ConvertStringToDouble' macro to make 
---  sure that it works for all string values.
------------------------------------------------------
" print;

### Test valid numbers ...
"1" toDouble printNL;
"4.2" toDouble printNL;
"1e2" toDouble printNL;
"123.456" toDouble printNL;
"62790" toDouble printNL;
"123456.0789" toDouble print: 20.4;
".1234" toDouble print: 20.4;

"+1" toDouble printNL;
"+4.2" toDouble printNL;
"+1e2" toDouble printNL;
"+123.456" toDouble printNL;
"+62790" toDouble printNL;
"+123456.0789" toDouble print: 20.4;
"+.1234" toDouble print: 20.4;

"-1" toDouble printNL;
"-4.2" toDouble printNL;
"-1e2" toDouble printNL;
"-123.456" toDouble printNL;
"-62790" toDouble printNL;
"-123456.0789" toDouble print: 20.4;
"-.1234" toDouble print: 20.4;

### test invalid numbers ...
"" toDouble printNL;
"a" toDouble printNL;
"abc" toDouble printNL;
"12d3" toDouble printNL;
"12*2" toDouble printNL;
"12.3T" toDouble printNL;
"12eH3" toDouble printNL;
"------" toDouble printNL;

"+" toDouble printNL;
"+a" toDouble printNL;
"+abc" toDouble printNL;
"+12d3" toDouble printNL;
"+12*2" toDouble printNL;
"+12.3T" toDouble printNL;
"+12eH3" toDouble printNL;
"+------" toDouble printNL;

"-" toDouble printNL;
"-a" toDouble printNL;
"-abc" toDouble printNL;
"-12d3" toDouble printNL;
"-12*2" toDouble printNL;
"-12.3T" toDouble printNL;
"-12eH3" toDouble printNL;
"-------" toDouble printNL;

"
------------------------------------------------------
(1B)
---  Test the scalar 'blockString' case.
------------------------------------------------------
" print;

### NA value
"***" toDouble printNL;

### real value
"3405.67" toDouble printNL;

"
------------------------------------------------------
(1C)
---  Test the scalar 'lstoreString' case.
------------------------------------------------------
" print;

### NA value
"***" fixed toDouble printNL;

### real value
"3405.67" fixed toDouble printNL;

"
------------------------------------------------------
(1D)
---  Test the non-scalar 'blockString' case.
------------------------------------------------------
" print;

### all real values
"1","402.2","203.23","1e2",".34" 
        do: [^self toDouble printNL];"" printNL;

### all NAs
"","[[[","1.dd","fs1" 
        do: [^self toDouble printNL];"" printNL;

### mixed
"","##123","2.3","444","2d4" 
        do: [^self toDouble printNL];"" printNL;
"1.2", "xx2","3.2","x","x","34","2" 
        do: [^self toDouble printNL];"" printNL;
"","1","" 
        do: [^self toDouble printNL];"" printNL;
"","","","2" 
        do: [^self toDouble printNL];"" printNL;
"1","2","" 
        do: [^self toDouble printNL];"" printNL;

### single element list - real value
"42" asList do: [^self toDouble printNL];

### single element list - NA
"x" asList do: [^self toDouble printNL];

"
------------------------------------------------------
(1E)
---  Test the non-scalar 'lstoreString' case.
------------------------------------------------------
" print;

### all real values
"1","402.2","203.23","1e2",".34" send: [^self fixed] .
        do: [^self toDouble printNL];"" printNL;

### all NAs
"","[[[","1.dd","fs1" send: [^self fixed] .
         do: [^self toDouble printNL];"" printNL;

### mixed
"","##123","2.3","444","2d4" send: [^self fixed] .
        do: [^self toDouble printNL];"" printNL;
"1.2", "xx2","3.2","x","x","34","2" send: [^self fixed] .
        do: [^self toDouble printNL];"" printNL;
"","1","" send: [^self fixed] . 
        do: [^self toDouble printNL];"" printNL;
"","","","2" send: [^self fixed] . 
        do: [^self toDouble printNL];"" printNL;
"1","2","" send: [^self fixed] . 
        do: [^self toDouble printNL];"" printNL;

### single element list - real value
"42" fixed asList do: [^self toDouble printNL];

### single element list - NA
"x" fixed asList do: [^self toDouble printNL];

"
------------------------------------------------------
(1F)
---  Test polymorphic cases.
     (expect 1 >>> Selector 'toDouble' Not Found <<<)
------------------------------------------------------
" print;

"1","2" fixed,"3" fixed,"4","x" do: [^self toDouble printNL];"" printNL;

"1",42,"2" fixed,"4" do: [^self toDouble printNL];"" printNL;



"


********************************************************
(PART 2)
--------  Test the 'fill', 'take', 'drop',         -----
--------  'concat', and 'sort' String Primitives   -----
********************************************************
" print;

"
------- Single String Fill -----------
" print;
!x <- "hello" fill: 10;
"@" print; x print; "@" print; ^global newLine print;

!x <- "hello" fill: -10;
"@" print; x print; "@" print; ^global newLine print;

!x <- "hello" fill: 0;
"@" print; x print; "@" print; ^global newLine print;

"
------- Single String Take ---------
" print;
!x <- "hello" take: 3;
"@" print; x print; "@" print; ^global newLine print;

!x <- "hello" take: -3;
"@" print; x print; "@" print; ^global newLine print;

!x <- "hello" take: 10;
"@" print; x print; "@" print; ^global newLine print;

!x <- "hello" take: -10;
"@" print; x print; "@" print; ^global newLine print;

!x <- "hello" take: 0;
"@" print; x print; "@" print; ^global newLine print;

"
--------- Single String Drop ----------
" print;
!x <- "hello" drop: 3;
"@" print; x print; "@" print; ^global newLine print;

!x <- "hello" drop: -3;
"@" print; x print; "@" print; ^global newLine print;

!x <- "hello" drop: 10;
"@" print; x print; "@" print; ^global newLine print;

!x <- "hello" drop: -10;
"@" print; x print; "@" print; ^global newLine print;

!x <- "hello" drop: 0;
"@" print; x print; "@" print; ^global newLine print;

"
-------- List Fill -----------
" print;

!list <- "a","very","large","ant";

!x <- list send: [^self fill: 10];
x do: ["@" print; ^self print; "@ " print];
" " printNL;

!x <- list send: [^self fill: -10];
x do: ["@" print; ^self print; "@ " print];
" " printNL;

!x <- list send: [^self fill: 2];
x do: ["@" print; ^self print; "@ " print];
" " printNL;

!x <- list send: [^self fill: -2];
x do: ["@" print; ^self print; "@ " print];
" " printNL;

!x <- list send: [^self fill: 0];
x do: ["@" print; ^self print; "@ " print];
" " printNL;

"
------------ List Take --------------
" print;

!x <- list send: [^self take: 10];
x do: ["@" print; ^self print; "@ " print];
" " printNL;

!x <- list send: [^self take: -10];
x do: ["@" print; ^self print; "@ " print];
" " printNL;

!x <- list send: [^self take: 2];
x do: ["@" print; ^self print; "@ " print];
" " printNL;

!x <- list send: [^self take: -2];
x do: ["@" print; ^self print; "@ " print];
" " printNL;

!x <- list send: [^self take: 0];
x do: ["@" print; ^self print; "@ " print];
" " printNL;

"
---------- List Drop ---------------
" print;

!x <- list send: [^self drop: 10];
x do: ["@" print; ^self print; "@ " print];
" " printNL;

!x <- list send: [^self drop: -10];
x do: ["@" print; ^self print; "@ " print];
" " printNL;

!x <- list send: [^self drop: 2];
x do: ["@" print; ^self print; "@ " print];
" " printNL;

!x <- list send: [^self drop: -2];
x do: ["@" print; ^self print; "@ " print];
" " printNL;

!x <- list send: [^self drop: 0];
x do: ["@" print; ^self print; "@ " print];
" " printNL;

"
===========  test invalid args to fill, take , & drop ===========
" print;

"
.... fill tests ...
     (expect 2 >>> Selector 'fill:' Not Found <<<)

" print;

"hello" fill: 10.2;
!aNum <- 25;
aNum fill: 10;
25 fill: 10.2;

!list <- "a","very","big","ant";
!x <- list send: [^self fill: 10.2];

"
.... take tests ...
     (expect 2 >>> Selector 'take:' Not Found <<<)
" print;
"hello" take: 10.2;
!aNum <- 25;
aNum take: 10;
25 take: 10.2;
!x <- list send: [^self take: 10.2];

"
.... drop tests ...
     (expect 2 >>> Selector 'drop:' Not Found <<<)
" print;
"hello" drop: 10.2;
aNum drop: 10;
25 drop: 10.2;
!x <- list send: [^self drop: 10.2];

"
=============== test concat =================
" print;
"hello" concat: " there" . printNL;

!list <- "hello", "to", "you";
!x <- list send: [^self concat: "@"];
x do: [^self print; " " print];
" " printNL;

"
..... test bad arg .......
" print;
"hello" concat: 10 . printNL;

!list <- "hello", "to", "you";
!x <- list send: [^self concat: 10];

"
......... more concat tests ...........
" print;
"hello" concat: "@" . printNL;

!list <- "hello", "to", "you";
!x <- list send: [^self concat: " there"];
x do: [^self print; " " print];
" " printNL;

"
--------------- test string sort ... ------------------
" print;
!l <- "dog","habits","fine","a","jolly","enjoys","colored",
"kings","gourmet","imitating","brown";
l sortUp: [^self]. do: [^self print; " " print];
" " printNL;


###########################################################
#		End of tests ...			  #
###########################################################
newLine print; "End of file: stringPrims.S" printNL; "" printNL;
?g
?t
Utility stopEchoToFile;
?g

