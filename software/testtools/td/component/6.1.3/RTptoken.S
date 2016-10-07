#**********************************************************
#**********************************************************
#		Test: RTptoken 
#
#  Tests the RTptoken module:
#  	PART 1 - Collapse
#	PART 2 - Appends
#	PART 3 - Cartesian PTokens
#
#  April 27, 1988	jad
#**********************************************************
#**********************************************************

Utility echoToFile: (^tmp componentOutput concat:"/RTptoken.O");
newLine print; "Beginning file: RTptoken.S ..." printNL;
###########################################################
#		Tests Begin Here ...                      #
###########################################################
?g
?d
"


********************************************************
(PART 1)
--------  Test The Collapse Function:		--------
--------  'rtPTOKEN_CPDCumAdjustments'		--------
********************************************************
" print,

"
-----------------
---  Inserts  ---
-----------------
" print,

"
(PART 1)-------------- Test 1 -----------------
4+4 3+3 2+1 0+2

" print,
"### Not mergable inserts ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
4
4
,#qa ref basePToken addSDCTest
1
3
3
,#qa ref basePToken addSDCTest
1
2
1
,#qa ref basePToken addSDCTest
1
0
2
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 2 -----------------
{4+4 3+3}  {2+1 0+2}

" print,
"### Not mergable inserts",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
2
4
4
3
3
,#qa ref basePToken addSDCTest
2
2
1
0
2
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 3 -----------------
{4+4 2+1 0+2}  6+3

" print,
"### Not mergable inserts",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
3
4
4
2
1
0
2
,#qa ref basePToken addSDCTest
1
6
3
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 4 -----------------
{0+2 3+3}  {2+1 4+4}

" print,
"### Mergable inserts",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
2
0
2
3
3
,#qa ref basePToken addSDCTest
2
2
1
4
4
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 5 -----------------
0+2 0+3 2+1 8+4 

" print,
"### Mergable inserts ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
0
2
,#qa ref basePToken addSDCTest
1
0
3
,#qa ref basePToken addSDCTest
1
2
1
,#qa ref basePToken addSDCTest
1
8
4
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 6 -----------------
{0+2 0+3}  {2+1 8+4}

" print,
"### Mergable inserts ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
2
0
2
0
3
,#qa ref basePToken addSDCTest
2
2
1
8
4
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 7 -----------------
0+2 2+1 0+3 8+4

" print,
"### Mergable inserts ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
0
2
,#qa ref basePToken addSDCTest
1
2
1
,#qa ref basePToken addSDCTest
1
0
3
,#qa ref basePToken addSDCTest
1
8
4
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 8 -----------------
{0+2 2+1}  {0+3 8+4}

" print,
"### Mergable inserts ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
2
0
2
2
1
,#qa ref basePToken addSDCTest
2
0
3
8
4
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 9 -----------------
{2+4 0+2} 2+1 0+3

" print,
"### Mergable inserts ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
2
2
4
0
2
,#qa ref basePToken addSDCTest
1
2
1
,#qa ref basePToken addSDCTest
1
0
3
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 9B -----------------
0+2 2+1 3+3 4+4

" print,
"### Not mergable inserts ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
0
2
,#qa ref basePToken addSDCTest
1
2
1
,#qa ref basePToken addSDCTest
1
3
3
,#qa ref basePToken addSDCTest
1
4
4
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,



"
-----------------
---  Deletes  ---
-----------------
" print,

"
(PART 1)-------------- Test 10 -----------------
8-3 4-1 2-2

" print,
"### Non mergable deletes ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
8
-3
,#qa ref basePToken addSDCTest
1
4
-1
,#qa ref basePToken addSDCTest
1
2
-2
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 11 -----------------
{8-3 4-1}  2-2

" print,
"### Non mergable deletes ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
2
8
-3
4
-1
,#qa ref basePToken addSDCTest
1
2
-2
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 12 -----------------
8-3 2-2 2-1

" print,
"### Non mergable deletes ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
8
-3
,#qa ref basePToken addSDCTest
1
2
-2
,#qa ref basePToken addSDCTest
1
2
-1
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 13 -----------------
8-3  {2-2 2-1}

" print,
"### Non mergable deletes ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
8
-3
,#qa ref basePToken addSDCTest
2
2
-2
2
-1
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 14 -----------------
6-2 6-3

" print,
"### Non mergable deletes ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
6
-2
,#qa ref basePToken addSDCTest
1
6
-3
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 15 -----------------
6-3 5-2

" print,
"### Non mergable deletes ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
6
-3
,#qa ref basePToken addSDCTest
1
5
-2
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 16 -----------------
8-2 6-1

" print,
"### Mergable deletes ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
8
-2
,#qa ref basePToken addSDCTest
1
6
-1
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 17 -----------------
{8-2 6-1}

" print,
"### Mergable deletes ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
2
8
-2
6
-1
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 18 -----------------
8-2 7-1 6-3

" print,
"### Non mergable deletes ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
8
-2
,#qa ref basePToken addSDCTest
1
7
-1
,#qa ref basePToken addSDCTest
1
6
-3
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 19 -----------------
{8-2 7-1}  6-3

" print,
"### Non mergable deletes ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
2
8
-2
7
-1
,#qa ref basePToken addSDCTest
1
6
-3
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 20 -----------------
5-2 7-4

" print,
"### Non mergable deletes ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
5
-2
,#qa ref basePToken addSDCTest
1
7
-4
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 21 -----------------
5-2 4-1 5-2 4-1

" print,
"### Non mergable deletes ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
5
-2
,#qa ref basePToken addSDCTest
1
4
-1
,#qa ref basePToken addSDCTest
1
5
-2
,#qa ref basePToken addSDCTest
1
4
-1
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 22 -----------------
{9-2 4-1}  6-3

" print,
"### Non mergable deletes ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
2
9
-2
4
-1
,#qa ref basePToken addSDCTest
1
6
-3
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 23 -----------------
6-2 4-1 6-3

" print,
"### Non mergable deletes ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
6
-2
,#qa ref basePToken addSDCTest
1
4
-1
,#qa ref basePToken addSDCTest
1
6
-3
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 24 -----------------
6-2 6-2 6-2

" print,
"### Non mergable deletes ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
6
-2
,#qa ref basePToken addSDCTest
1
6
-2
,#qa ref basePToken addSDCTest
1
6
-2
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 25 -----------------
6-2 6-2 6-3

" print,
"### Non mergable deletes ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
6
-2
,#qa ref basePToken addSDCTest
1
6
-2
,#qa ref basePToken addSDCTest
1
6
-3
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 26 -----------------
{6-2 6-2}  6-3

" print,
"### Non mergable deletes ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
2
6
-2
6
-2
,#qa ref basePToken addSDCTest
1
6
-3
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,





"
-----------------------
---  DeleteInserts  ---
-----------------------
" print,

"
(PART 1)-------------- Test 27 -----------------
6-3 3+3 3+2

" print,
"### DeleteInserts never cancel ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
6
-3
,#qa ref basePToken addSDCTest
1
3
3
,#qa ref basePToken addSDCTest
1
3
2
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 28 -----------------
{6-3 3+3}  3+2

" print,
"### DeleteInserts never cancel ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
2
6
-3
3
3
,#qa ref basePToken addSDCTest
1
3
2
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 29 -----------------
6-3 {3+3 3+2}

" print,
"### DeleteInserts never cancel ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
6
-3
,#qa ref basePToken addSDCTest
2
3
3
3
2
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 30 -----------------
6-3 3+3 4+2

" print,
"### DeleteInserts never cancel ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
6
-3
,#qa ref basePToken addSDCTest
1
3
3
,#qa ref basePToken addSDCTest
1
4
2
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 31 -----------------
3-2 5-3 1+2 4+1 

" print,
"### DeleteInserts never cancel ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
3
-2
,#qa ref basePToken addSDCTest
1
5
-3
,#qa ref basePToken addSDCTest
1
1
2
,#qa ref basePToken addSDCTest
1
4
1
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 32 -----------------
{3-2 5-3} {1+2 4+1}

" print,
"### DeleteInserts never cancel ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
2
3
-2
5
-3
,#qa ref basePToken addSDCTest
2
1
2
4
1
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 33 -----------------
3-2 {1+2 7-3 4+1}

" print,
"### DeleteInserts never cancel ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
3
-2
,#qa ref basePToken addSDCTest
3
1
2
7
-3
4
1
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 34 -----------------
{3-2 1+2 7-3} 4+1

" print,
"### DeleteInserts never cancel ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
3
3
-2
1
2
7
-3
,#qa ref basePToken addSDCTest
1
4
1
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 35 -----------------
3-2 1+2 7-3 4+1

" print,
"### DeleteInserts never cancel ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
3
-2
,#qa ref basePToken addSDCTest
1
1
2
,#qa ref basePToken addSDCTest
1
7
-3
,#qa ref basePToken addSDCTest
1
4
1
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 36 -----------------
7-3 4+1 3-2 1+2

" print,
"### DeleteInserts never cancel ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
7
-3
,#qa ref basePToken addSDCTest
1
4
1
,#qa ref basePToken addSDCTest
1
3
-2
,#qa ref basePToken addSDCTest
1
1
2
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 37 -----------------
{7-3 4+1 3-2}  1+1 1+5

" print,
"### DeleteInserts never cancel ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
3
7
-3
4
1
3
-2
,#qa ref basePToken addSDCTest
1
1
1
,#qa ref basePToken addSDCTest
1
1
5
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,





"
-----------------------
---  InsertDeletes  ---
-----------------------
" print,

"
(PART 1)-------------- Test 38 -----------------
10+2 2-1 4+3 10-2

" print,
"### InsertDeletes non merge cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
10
2
,#qa ref basePToken addSDCTest
1
2
-1
,#qa ref basePToken addSDCTest
1
4
3
,#qa ref basePToken addSDCTest
1
10
-2
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 39 -----------------
{10+2 2-1} 4+3 10-2

" print,
"### InsertDeletes non merge cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
2
10
2
2
-1
,#qa ref basePToken addSDCTest
1
4
3
,#qa ref basePToken addSDCTest
1
10
-2
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 40 -----------------
{10+2 2-1} {4+3 10-2}

" print,
"### InsertDeletes non merge cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
2
10
2
2
-1
,#qa ref basePToken addSDCTest
2
4
3
10
-2
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 41 -----------------
10+2 {2-1 4+3 10-2}

" print,
"### InsertDeletes non merge cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
10
2
,#qa ref basePToken addSDCTest
3
2
-1
4
3
10
-2
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 42 -----------------
5+3 2-1 0+2 14-2

" print,
"### InsertDeletes non merge cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
5
3
,#qa ref basePToken addSDCTest
1
2
-1
,#qa ref basePToken addSDCTest
1
0
2
,#qa ref basePToken addSDCTest
1
14
-2
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 43 -----------------
{5+3 2-1} {0+2 14-2}

" print,
"### InsertDeletes non merge cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
2
5
3
2
-1
,#qa ref basePToken addSDCTest
2
0
2
14
-2
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 44 -----------------
9+2 7-2 4+1 3-3

" print,
"### InsertDeletes non merge cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
9
2
,#qa ref basePToken addSDCTest
1
7
-2
,#qa ref basePToken addSDCTest
1
4
1
,#qa ref basePToken addSDCTest
1
3
-3
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 45 -----------------
{9+2 7-2 4+1} 3-3

" print,
"### InsertDeletes non merge cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
3
9
2
7
-2
4
1
,#qa ref basePToken addSDCTest
1
3
-3
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 46 -----------------
{9+2 7-2 4+1 3-3}

" print,
"### InsertDeletes non merge cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
4
9
2
7
-2
4
1
3
-3
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 46B -----------------
3+3 7-4

" print,
"### InsertDeletes merge cases ",
"### cancel entire insert plus element after it ",
"### This corresponds to case InsertDelete4 ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
3
3
,#qa ref basePToken addSDCTest
1
7
-4
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 46C -----------------
3+3 7-3

" print,
"### InsertDeletes merge cases ",
"### cancel top end of insert plus element after it",
"### This corresponds to case InsertDelete2 ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
3
3
,#qa ref basePToken addSDCTest
1
7
-3
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 47 -----------------
3+3 6-3

" print,
"### InsertDeletes merge cases ",
"### cancel totally ",
"### This corresponds to case InsertDelete5 ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
3
3
,#qa ref basePToken addSDCTest
1
6
-3
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 48 -----------------
3+3 6-2

" print,
"### InsertDeletes merge cases ",
"### cancel top end of insert ",
"### This corresponds to case InsertDelete3 ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
3
3
,#qa ref basePToken addSDCTest
1
6
-2
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 49 -----------------
3+4 5-2

" print,
"### InsertDeletes merge cases ",
"### cancel bottom end of insert ",
"### This corresponds to case InsertDelete3 ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
3
4
,#qa ref basePToken addSDCTest
1
5
-2
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 50 -----------------
3+3 6-4

" print,
"### InsertDeletes merge cases ",
"### cancel all of insert plus element before it ",
"### This corresponds to case InsertDelete4 ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
3
3
,#qa ref basePToken addSDCTest
1
6
-4
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 51 -----------------
3+3 5-4

" print,
"### InsertDeletes merge cases ",
"### cancel bottom end of insert plus elements before it ",
"### This corresponds to case InsertDelete1 ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
3
3
,#qa ref basePToken addSDCTest
1
5
-4
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 52 -----------------
9+3 13-4 6+4 11-6 3+1 4-1 1+2 3-3

" print,
"### InsertDeletes merge cases ",
"### This hits the InsertDelete4&5 cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
9
3
,#qa ref basePToken addSDCTest
1
13
-4
,#qa ref basePToken addSDCTest
1
6
4
,#qa ref basePToken addSDCTest
1
11
-6
,#qa ref basePToken addSDCTest
1
3
1
,#qa ref basePToken addSDCTest
1
4
-1
,#qa ref basePToken addSDCTest
1
1
2
,#qa ref basePToken addSDCTest
1
3
-3
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 53 -----------------
{9+3 6+4 3+1 1+2} {3-3 3-1 10-6 10-4}

" print,
"### InsertDeletes merge cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
4
9
3
6
4
3
1
1
2
,#qa ref basePToken addSDCTest
4
3
-3
3
-1
10
-6
10
-4
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,





"
(PART 1)-------------- Test 54 -----------------
{9+3 6+4} {3+1 1+2} {3-3 3-1} {10-6 10-4}

" print,
"### InsertDeletes merge cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
2
9
3
6
4
,#qa ref basePToken addSDCTest
2
3
1
1
2
,#qa ref basePToken addSDCTest
2
3
-3
3
-1
,#qa ref basePToken addSDCTest
2
10
-6
10
-4
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 55 -----------------
{3+1 1+2} 9+4 {16+3 6-1} 13-6 {3-3 10-4}

" print,
"### InsertDeletes merge cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
2
3
1
1
2
,#qa ref basePToken addSDCTest
1
9
4
,#qa ref basePToken addSDCTest
2
16
3
6
-1
,#qa ref basePToken addSDCTest
1
13
-6
,#qa ref basePToken addSDCTest
2
3
-3
10
-4
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 56 -----------------
9+2 10-3 5+3 9-3 3+2 5-1 1+2 2-1

" print,
"### InsertDeletes merge cases ",
"### This hits the InsertDelete1&2&3 cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
9
2
,#qa ref basePToken addSDCTest
1
10
-3
,#qa ref basePToken addSDCTest
1
5
3
,#qa ref basePToken addSDCTest
1
9
-3
,#qa ref basePToken addSDCTest
1
3
2
,#qa ref basePToken addSDCTest
1
5
-1
,#qa ref basePToken addSDCTest
1
1
2
,#qa ref basePToken addSDCTest
1
2
-1
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
(PART 1)-------------- Test 57 -----------------
9+2 {10-3 5+3} {9-3 3+2} {5-1 1+2} 2-1

" print,
"### InsertDeletes merge cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
9
2
,#qa ref basePToken addSDCTest
2
10
-3
5
3
,#qa ref basePToken addSDCTest
2
9
-3
3
2
,#qa ref basePToken addSDCTest
2
5
-1
1
2
,#qa ref basePToken addSDCTest
1
2
-1
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,



"
(PART 1)-------------- Test 58 -----------------
5+3 9-3 1+2 2-1 4+2 6-1 11+2 12-3

" print,
"### InsertDeletes merge cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
5
3
,#qa ref basePToken addSDCTest
1
9
-3
,#qa ref basePToken addSDCTest
1
1
2
,#qa ref basePToken addSDCTest
1
2
-1
,#qa ref basePToken addSDCTest
1
4
2
,#qa ref basePToken addSDCTest
1
6
-1
,#qa ref basePToken addSDCTest
1
11
2
,#qa ref basePToken addSDCTest
1
12
-3
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,



"
(PART 1)-------------- Test 59 -----------------
9+2 5+3 3+2 1+2 2-1 6-1 11-3 12-3

" print,
"### InsertDeletes merge cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
9
2
,#qa ref basePToken addSDCTest
1
5
3
,#qa ref basePToken addSDCTest
1
3
2
,#qa ref basePToken addSDCTest
1
1
2
,#qa ref basePToken addSDCTest
4
2
-1
6
-1
11
-3
12
-3
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,



"
(PART 1)-------------- Test 60 -----------------
9+2 5+3 1+2 5+2 7-1 2-1 15-3 11-3

" print,
"### InsertDeletes merge cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
9
2
,#qa ref basePToken addSDCTest
1
5
3
,#qa ref basePToken addSDCTest
1
1
2
,#qa ref basePToken addSDCTest
1
5
2
,#qa ref basePToken addSDCTest
1
7
-1
,#qa ref basePToken addSDCTest
1
2
-1
,#qa ref basePToken addSDCTest
1
15
-3
,#qa ref basePToken addSDCTest
1
11
-3
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"
------------------------------
---  Misc. Assorted Cases  ---
------------------------------
" print,

"
(PART 1)-------------- Test 61 -----------------
10+3 3-2 1+1 1+2 8-2 8+1

" print,
"### Misc. assorted cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
10
3
,#qa ref basePToken addSDCTest
1
3
-2
,#qa ref basePToken addSDCTest
1
1
1
,#qa ref basePToken addSDCTest
1
1
2
,#qa ref basePToken addSDCTest
1
8
-2
,#qa ref basePToken addSDCTest
1
8
1
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,








"
(PART 1)-------------- Test 62 -----------------
10+3 {3-2 1+1} {1+2 8-2 8+1}

" print,
"### Misc. assorted cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
10
3
,#qa ref basePToken addSDCTest
2
3
-2
1
1
,#qa ref basePToken addSDCTest
3
1
2
8
-2
8
1
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,








"
(PART 1)-------------- Test 63 -----------------
4+4 11+1 8+2 11-3 1-1

" print,
"### Misc. assorted cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
4
4
,#qa ref basePToken addSDCTest
1
11
1
,#qa ref basePToken addSDCTest
1
8
2
,#qa ref basePToken addSDCTest
1
11
-3
,#qa ref basePToken addSDCTest
1
1
-1
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,








"
(PART 1)-------------- Test 64 -----------------
{4+4 11+1} 8+2 {11-3 1-1}

" print,
"### Misc. assorted cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
2
4
4
11
1
,#qa ref basePToken addSDCTest
1
8
2
,#qa ref basePToken addSDCTest
2
11
-3
1
-1
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,








"
(PART 1)-------------- Test 65 -----------------
8-1 7-2 1+2 0+1 10+3

" print,
"### Misc. assorted cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
8
-1
,#qa ref basePToken addSDCTest
1
7
-2
,#qa ref basePToken addSDCTest
1
1
2
,#qa ref basePToken addSDCTest
1
0
1
,#qa ref basePToken addSDCTest
1
10
3
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,








"
(PART 1)-------------- Test 66 -----------------
{8-1 7-2 1+2 0+1} 10+3

" print,
"### Misc. assorted cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
4
8
-1
7
-2
1
2
0
1
,#qa ref basePToken addSDCTest
1
10
3
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,








"
(PART 1)-------------- Test 67 -----------------
10-10 0+10

" print,
"### Misc. assorted cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
10
-10
,#qa ref basePToken addSDCTest
1
0
10
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,








"
(PART 1)-------------- Test 68 -----------------
10-10 0+5 2+5

" print,
"### Misc. assorted cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
10
-10
,#qa ref basePToken addSDCTest
1
0
5
,#qa ref basePToken addSDCTest
1
2
5
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,








"
(PART 1)-------------- Test 69 -----------------
10-5 5-5 0+10

" print,
"### Misc. assorted cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
10
-5
,#qa ref basePToken addSDCTest
1
5
-5
,#qa ref basePToken addSDCTest
1
0
10
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,








"
(PART 1)-------------- Test 70 -----------------
5-5 5-5 0+10

" print,
"### Misc. assorted cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
5
-5
,#qa ref basePToken addSDCTest
1
5
-5
,#qa ref basePToken addSDCTest
1
0
10
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,








"
(PART 1)-------------- Test 71 -----------------
10+1 8+1 2+1 5+1 8+1

" print,
"### Misc. assorted cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
10
1
,#qa ref basePToken addSDCTest
1
8
1
,#qa ref basePToken addSDCTest
1
2
1
,#qa ref basePToken addSDCTest
1
5
1
,#qa ref basePToken addSDCTest
1
8
1
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,








"
(PART 1)-------------- Test 72 -----------------
10-1 8-1 2-1 3-1 4-1

" print,
"### Misc. assorted cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
10
-1
,#qa ref basePToken addSDCTest
1
8
-1
,#qa ref basePToken addSDCTest
1
2
-1
,#qa ref basePToken addSDCTest
1
3
-1
,#qa ref basePToken addSDCTest
1
4
-1
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,








"
(PART 1)-------------- Test 73 -----------------
10-10

" print,
"### Misc. assorted cases ",

#qa set: (#RTYPE_C_PToken new: 10),
#qa ref addSDCTest
1
10
-10
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,








"
(PART 1)-------------- Test 74 -----------------
0+5 5+5 3-2

" print,
"### Start with empty ptoken ",

#qa set: (#RTYPE_C_PToken new: 0),
#qa ref addSDCTest
1
0
5
,#qa ref basePToken addSDCTest
1
5
5
,#qa ref basePToken addSDCTest
1
3
-2
,
"       
---  Shift: ---
" print,
#qa ref display,
"
--- Result: ---
" print,
#qa ref collapse,




"


********************************************************
(PART 2)
--------  Test The Append Function:		--------
--------  'rtPTOKEN_AppendAdjustments' 		--------
********************************************************
" print,

"
(PART 2)-------------- 1 ------------------
" print,

#qa set: (#RTYPE_C_PToken new: 10),
#qb set: (#RTYPE_C_PToken new: 10),
#qi set: (#RTYPE_C_IntUV new: #qa referencing: #qb),
#qi ref stepValues,

#qa ref addSDCTest
2
8
1
7
-1

,
#qi ref print,
#qa ref display,


"
(PART 2)-------------- 2 ------------------
" print,

#qa set: (#RTYPE_C_PToken new: 10),
#qb set: (#RTYPE_C_PToken new: 10),
#qi set: (#RTYPE_C_IntUV new: #qa referencing: #qb),
#qi ref stepValues,

#qa ref addSDCTest
2
8
1
7
1

,
#qi ref print,
#qa ref display,


"
(PART 2)-------------- 3 ------------------
" print,

#qa set: (#RTYPE_C_PToken new: 10),
#qb set: (#RTYPE_C_PToken new: 10),
#qi set: (#RTYPE_C_IntUV new: #qa referencing: #qb),
#qi ref stepValues,

#qa ref addSDCTest
2
8
1
8
-1

,
#qi ref print,
#qa ref display,


"
(PART 2)-------------- 4 ------------------
" print,

#qa set: (#RTYPE_C_PToken new: 10),
#qb set: (#RTYPE_C_PToken new: 10),
#qi set: (#RTYPE_C_IntUV new: #qa referencing: #qb),
#qi ref stepValues,

#qa ref addSDCTest
2
8
1
8
1

,
#qi ref print,
#qa ref display,


"
(PART 2)-------------- 5 ------------------
" print,

#qa set: (#RTYPE_C_PToken new: 10),
#qb set: (#RTYPE_C_PToken new: 10),
#qi set: (#RTYPE_C_IntUV new: #qa referencing: #qb),
#qi ref stepValues,

#qa ref addSDCTest
2
8
1
9
-1

,
#qi ref print,
#qa ref display,


"
(PART 2)-------------- 6 ------------------
" print,

#qa set: (#RTYPE_C_PToken new: 10),
#qb set: (#RTYPE_C_PToken new: 10),
#qi set: (#RTYPE_C_IntUV new: #qa referencing: #qb),
#qi ref stepValues,

#qa ref addSDCTest
2
8
1
9
1

,
#qi ref print,
#qa ref display,


"
(PART 2)-------------- 7 ------------------
" print,

#qa set: (#RTYPE_C_PToken new: 10),
#qb set: (#RTYPE_C_PToken new: 10),
#qi set: (#RTYPE_C_IntUV new: #qa referencing: #qb),
#qi ref stepValues,

#qa ref addSDCTest
2
8
1
10
-1

,
#qi ref print,
#qa ref display,


"
(PART 2)-------------- 8 ------------------
" print,

#qa set: (#RTYPE_C_PToken new: 10),
#qb set: (#RTYPE_C_PToken new: 10),
#qi set: (#RTYPE_C_IntUV new: #qa referencing: #qb),
#qi ref stepValues,

#qa ref addSDCTest
2
8
1
10
1

,
#qi ref print,
#qa ref display,


"
(PART 2)-------------- 9 ------------------
" print,

#qa set: (#RTYPE_C_PToken new: 10),
#qb set: (#RTYPE_C_PToken new: 10),
#qi set: (#RTYPE_C_IntUV new: #qa referencing: #qb),
#qi ref stepValues,

#qa ref addSDCTest
2
8
-1
6
-1

,
#qi ref print,
#qa ref display,


"
(PART 2)-------------- 10 ------------------
" print,

#qa set: (#RTYPE_C_PToken new: 10),
#qb set: (#RTYPE_C_PToken new: 10),
#qi set: (#RTYPE_C_IntUV new: #qa referencing: #qb),
#qi ref stepValues,

#qa ref addSDCTest
2
8
-1
6
1

,
#qi ref print,
#qa ref display,


"
(PART 2)-------------- 11 ------------------
" print,

#qa set: (#RTYPE_C_PToken new: 10),
#qb set: (#RTYPE_C_PToken new: 10),
#qi set: (#RTYPE_C_IntUV new: #qa referencing: #qb),
#qi ref stepValues,

#qa ref addSDCTest
2
8
-1
7
-1

,
#qi ref print,
#qa ref display,


"
(PART 2)-------------- 12 ------------------
" print,

#qa set: (#RTYPE_C_PToken new: 10),
#qb set: (#RTYPE_C_PToken new: 10),
#qi set: (#RTYPE_C_IntUV new: #qa referencing: #qb),
#qi ref stepValues,

#qa ref addSDCTest
2
8
-1
7
1

,
#qi ref print,
#qa ref display,


"
(PART 2)-------------- 13 ------------------
" print,

#qa set: (#RTYPE_C_PToken new: 10),
#qb set: (#RTYPE_C_PToken new: 10),
#qi set: (#RTYPE_C_IntUV new: #qa referencing: #qb),
#qi ref stepValues,

#qa ref addSDCTest
2
8
-1
8
-1

,
#qi ref print,
#qa ref display,


"
(PART 2)-------------- 14 ------------------
" print,

#qa set: (#RTYPE_C_PToken new: 10),
#qb set: (#RTYPE_C_PToken new: 10),
#qi set: (#RTYPE_C_IntUV new: #qa referencing: #qb),
#qi ref stepValues,

#qa ref addSDCTest
2
8
-1
8
1

,
#qi ref print,
#qa ref display,


"
(PART 2)-------------- 15 ------------------
" print,

#qa set: (#RTYPE_C_PToken new: 10),
#qb set: (#RTYPE_C_PToken new: 10),
#qi set: (#RTYPE_C_IntUV new: #qa referencing: #qb),
#qi ref stepValues,

#qa ref addSDCTest
2
8
-1
9
-1

,
#qi ref print,
#qa ref display,


"
(PART 2)-------------- 16 ------------------
" print,

#qa set: (#RTYPE_C_PToken new: 10),
#qb set: (#RTYPE_C_PToken new: 10),
#qi set: (#RTYPE_C_IntUV new: #qa referencing: #qb),
#qi ref stepValues,

#qa ref addSDCTest
2
8
-1
9
1

,
#qi ref print,
#qa ref display,
#RTYPE_C_PToken tracesOff,





"


********************************************************
(PART 3)
--------  Test Cartesian PTokens: 		--------
--------  Updating them ...			--------
********************************************************
" print,

"
(PART 3)------------------ 1 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 3),
#qb set: (#RTYPE_C_PToken new: 4),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 20),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qa ref addSDCTest
2
0
1
3
-1
,
#qi ref print,
#qa ref display,


"
(PART 3)------------------ 2 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 3),
#qb set: (#RTYPE_C_PToken new: 4),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 20),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qa ref addSDCTest
1
1
2

,
#qi ref print,
#qa ref display,


"
(PART 3)------------------ 3 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 3),
#qb set: (#RTYPE_C_PToken new: 4),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 20),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qb ref addSDCTest
1
3
-1

,
#qi ref print,
#qb ref display,



"
(PART 3)------------------ 4 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 3),
#qb set: (#RTYPE_C_PToken new: 4),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 20),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qb ref addSDCTest
2
0
1
3
-1

,
#qi ref print,
#qb ref display,



"
(PART 3)------------------ 5 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 3),
#qb set: (#RTYPE_C_PToken new: 4),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 20),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qa ref addSDCTest
1
1
2

,
#qb ref addSDCTest
2
0
1
3
-1

,
#qi ref print,
#qa ref display,
#qb ref display,


"
(PART 3)------------------ 6 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 3),
#qb set: (#RTYPE_C_PToken new: 4),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 20),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qa ref addSDCTest
2
0
1
3
-1

,
#qb ref addSDCTest
1
3
-1

,
#qi ref print,
#qa ref display,
#qb ref display,


"
(PART 3)------------------ 7 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 3),
#qb set: (#RTYPE_C_PToken new: 4),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 20),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qa ref addSDCTest
2
0
1
3
1

,
#qb ref addSDCTest
1
0
1

,
#qi ref print,
#qa ref display,
#qb ref display,



"
(PART 3)------------------ 8 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 7),
#qb set: (#RTYPE_C_PToken new: 4),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 40),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qa ref addSDCTest
2
6
-2
4
1

,
#qb ref addSDCTest
2
1
-1
0
2

,
#qi ref print,
#qa ref display,
#qb ref display,



"
========== MORE COLUMNS =============
" print,
"
(PART 3)------------------ 9 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 3),
#qb set: (#RTYPE_C_PToken new: 5),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 40),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qb ref addSDCTest
4
5
-1
3
1
3
-1
1
-1

,
#qi ref print,
#qb ref display,



"
(PART 3)------------------ 10 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 3),
#qb set: (#RTYPE_C_PToken new: 5),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 40),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qb ref addSDCTest
3
5
1
3
2
2
-1

,
#qi ref print,
#qb ref display,



"
(PART 3)------------------ 11 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 3),
#qb set: (#RTYPE_C_PToken new: 5),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 40),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qb ref addSDCTest
2
3
2
2
-2

,
#qi ref print,
#qb ref display,



"
(PART 3)------------------ 12 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 3),
#qb set: (#RTYPE_C_PToken new: 5),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 40),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qb ref addSDCTest
1
3
-3

,
#qi ref print,
#qb ref display,



"
(PART 3)------------------ 13 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 3),
#qb set: (#RTYPE_C_PToken new: 5),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 40),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qb ref addSDCTest
1
5
-2

,
#qi ref print,
#qb ref display,



"
(PART 3)------------------ 14 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 3),
#qb set: (#RTYPE_C_PToken new: 5),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 40),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qb ref addSDCTest
2
5
5
5
-5

,
#qi ref print,
#qb ref display,



"
========== MORE ROWS =============
" print,
"
(PART 3)------------------ 15 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 3),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 40),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qa ref addSDCTest
4
5
-1
3
1
3
-1
1
-1

,
#qi ref print,
#qa ref display,



"
(PART 3)------------------ 16 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 3),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 40),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qa ref addSDCTest
3
5
1
3
2
2
-1

,
#qi ref print,
#qa ref display,



"
(PART 3)------------------ 17 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 3),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 40),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qa ref addSDCTest
2
3
2
2
-2

,
#qi ref print,
#qa ref display,



"
(PART 3)------------------ 18 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 3),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 40),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qa ref addSDCTest
1
3
-3

,
#qi ref print,
#qa ref display,



"
(PART 3)------------------ 19 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 3),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 40),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qa ref addSDCTest
1
5
-2

,
#qi ref print,
#qa ref display,



"
(PART 3)------------------ 20 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 3),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 40),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qa ref addSDCTest
2
5
5
5
-5

,
#qi ref print,
#qa ref display,



"
========== MORE BOTH =============
" print,
"
(PART 3)------------------ 21 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 5),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 40),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qa ref addSDCTest
4
5
-1
3
1
3
-1
1
-1

,

#qb ref addSDCTest
4
5
-1
3
1
3
-1
1
-1

,
#qi ref print,
#qa ref display,
#qb ref display,



"
(PART 3)------------------ 22 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 5),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 40),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qa ref addSDCTest
3
5
1
3
2
2
-1

,

#qb ref addSDCTest
3
5
1
3
2
2
-1

,
#qi ref print,
#qa ref display,
#qb ref display,



"
(PART 3)------------------ 23 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 5),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 40),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qa ref addSDCTest
2
3
2
2
-2

,

#qb ref addSDCTest
2
3
2
2
-2

,
#qi ref print,
#qa ref display,
#qb ref display,



"
(PART 3)------------------ 24 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 5),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 40),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qa ref addSDCTest
1
3
-3

,

#qb ref addSDCTest
1
3
-3

,
#qi ref print,
#qa ref display,
#qb ref display,



"
(PART 3)------------------ 25 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 5),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 40),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qa ref addSDCTest
1
5
-2

,

#qb ref addSDCTest
1
5
-2

,
#qi ref print,
#qa ref display,
#qb ref display,



"
(PART 3)------------------ 26 ---------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 5),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 40),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qa ref addSDCTest
2
5
5
5
-5

,

#qb ref addSDCTest
2
5
5
5
-5

,
#qi ref print,
#qa ref display,
#qb ref display,


"
----------------------------------------------------------
========== UPDATE CARTESIAN PTOKENS -- EMPTY =============
----------------------------------------------------------
" print,

"
(PART 3 EMPTY)------------ 1 --------------
" print,
#qa set: (#RTYPE_C_PToken new: 0),
#qb set: (#RTYPE_C_PToken new: 0),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 50),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qa ref addSDCTest
1
0
5

,
#qi ref print,
"
" print,
#qa ref display,
#qb ref display,

"
(PART 3 EMPTY)------------ 2 --------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 0),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 50),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qa ref addSDCTest
1
5
-5

,
#qi ref print,
"
" print,
#qa ref display,
#qb ref display,

"
(PART 3 EMPTY)------------ 3 --------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 0),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 50),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qa ref addSDCTest
1
5
-1

,
#qi ref print,
"
" print,
#qa ref display,
#qb ref display,

"
(PART 3 EMPTY)------------ 4 --------------
" print,
#qa set: (#RTYPE_C_PToken new: 0),
#qb set: (#RTYPE_C_PToken new: 0),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 50),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qb ref addSDCTest
1
0
2

,
#qi ref print,
"
" print,
#qa ref display,
#qb ref display,

"
(PART 3 EMPTY)------------ 5 --------------
" print,
#qa set: (#RTYPE_C_PToken new: 0),
#qb set: (#RTYPE_C_PToken new: 0),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 50),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qb ref addSDCTest
1
0
2

,
#qa ref addSDCTest
1
0
5

,
#qi ref print,
"
" print,
#qa ref display,
#qb ref display,

"
(PART 3 EMPTY)------------ 6 --------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 0),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 50),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qb ref addSDCTest
1
0
2

,
#qa ref addSDCTest
1
5
-5

,
#qi ref print,
"
" print,
#qa ref display,
#qb ref display,


"
(PART 3 EMPTY)------------ 7 --------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 0),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 50),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qb ref addSDCTest
1
0
2

,
#qi ref print,
"
" print,
#qa ref display,
#qb ref display,

"
(PART 3 EMPTY)------------ 8 --------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 0),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 50),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qb ref addSDCTest
1
0
2

,
#qa ref addSDCTest
1
5
-1

,
#qi ref print,
"
" print,
#qa ref display,
#qb ref display,

"
(PART 3 EMPTY)------------ 9 --------------
" print,
#qa set: (#RTYPE_C_PToken new: 0),
#qb set: (#RTYPE_C_PToken new: 2),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 50),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qb ref addSDCTest
1
2
-2

,
#qi ref print,
"
" print,
#qa ref display,
#qb ref display,

"
(PART 3 EMPTY)------------ 10 --------------
" print,
#qa set: (#RTYPE_C_PToken new: 0),
#qb set: (#RTYPE_C_PToken new: 2),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 50),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qb ref addSDCTest
1
2
-2

,
#qa ref addSDCTest
1
0
5

,
#qi ref print,
"
" print,
#qa ref display,
#qb ref display,

"
(PART 3 EMPTY)------------ 11 --------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 2),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 50),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qb ref addSDCTest
1
2
-2

,
#qa ref addSDCTest
1
5
-5

,
#qi ref print,
"
" print,
#qa ref display,
#qb ref display,

"
(PART 3 EMPTY)------------ 12 --------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 2),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 50),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qb ref addSDCTest
1
2
-2

,
#qi ref print,
"
" print,
#qa ref display,
#qb ref display,

"
(PART 3 EMPTY)------------ 13 --------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 2),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 50),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qb ref addSDCTest
1
2
-2

,
#qa ref addSDCTest
1
5
-1

,
#qi ref print,
"
" print,
#qa ref display,
#qb ref display,

"
(PART 3 EMPTY)------------ 14 --------------
" print,
#qa set: (#RTYPE_C_PToken new: 0),
#qb set: (#RTYPE_C_PToken new: 2),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 50),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qa ref addSDCTest
1
0
5

,
#qi ref print,
"
" print,
#qa ref display,
#qb ref display,

"
(PART 3 EMPTY)------------ 15 --------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 2),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 50),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qa ref addSDCTest
1
5
-5

,
#qi ref print,
"
" print,
#qa ref display,
#qb ref display,

"
(PART 3 EMPTY)------------ 16 --------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 2),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 50),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qa ref addSDCTest
1
5
-1

,
#qi ref print,
"
" print,
#qa ref display,
#qb ref display,

"
(PART 3 EMPTY)------------ 17 --------------
" print,
#qa set: (#RTYPE_C_PToken new: 0),
#qb set: (#RTYPE_C_PToken new: 2),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 50),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qb ref addSDCTest
1
2
1

,

#qi ref print,
"
" print,
#qa ref display,
#qb ref display,

"
(PART 3 EMPTY)------------ 18 --------------
" print,
#qa set: (#RTYPE_C_PToken new: 0),
#qb set: (#RTYPE_C_PToken new: 2),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 50),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qb ref addSDCTest
1
2
1

,
#qa ref addSDCTest
1
0
5

,
#qi ref print,
"
" print,
#qa ref display,
#qb ref display,

"
(PART 3 EMPTY)------------ 19 --------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 2),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 50),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qb ref addSDCTest
1
2
1

,
#qa ref addSDCTest
1
5
-5

,
#qi ref print,
"
" print,
#qa ref display,
#qb ref display,

"
(PART 3 EMPTY)------------ 20 --------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 2),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 50),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qb ref addSDCTest
1
2
1

,
#qi ref print,
"
" print,
#qa ref display,
#qb ref display,

"
(PART 3 EMPTY)------------ 21 --------------
" print,
#qa set: (#RTYPE_C_PToken new: 5),
#qb set: (#RTYPE_C_PToken new: 2),
#qc set: (#RTYPE_C_PToken newRow: #qa col: #qb),
#qd set: (#RTYPE_C_PToken new: 50),

#qi set: (#RTYPE_C_IntUV new: #qc referencing: #qd),
#qi ref stepValues,

#qb ref addSDCTest
1
2
1

,
#qa ref addSDCTest
1
5
-1

,
#qi ref print,
"
" print,
#qa ref display,
#qb ref display,







"
" print,
" 
" print,
QUIT

###########################################################
#		End of tests ...			  #
###########################################################
newLine print; "End of file: RTptoken.S" printNL; newLine print;
Utility stopEchoToFile;
?g

