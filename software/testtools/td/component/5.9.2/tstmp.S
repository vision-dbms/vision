##################################################
# Time Series Testing
#
# StandAlone/Property
# number of instances in cluster
# patterns of update, access, delete
# deleteColumn
##################################################

Utility echoToFile: (^tmp componentOutput concat:"/tstmp.O");
newLine print; "Beginning file: tstmp.S ..." printNL;


TimeSeries 
defineMethod:
[ | displayStats |
  "Total Points: " print: 20 ; ^self count printNL ;
  "First Point: " print: 20 ; !fd <- ^self firstDate printNL ;
  "First Value: " print: 20 ; ^self firstObservation printNL ;
  "  (actual access): " print: 20 ; 
  fd isDate ifTrue: [ ^self asOf: fd . printNL ] ifFalse: [ newLine print ] ;
  "Last Point: " print: 20 ; !ld <- ^self lastDate printNL ;
  "Last Value: " print: 20 ; ^self lastObservation printNL ;
  "  (actual access): " print: 20 ; 
  ld isDate ifTrue: [ ^self asOf: ld . printNL ] ifFalse: [ newLine print ] ;
  " < 1990 Points: " print: 20 ; 
   ^self select: [ ^date year < 1990 ] . count printNL ;
  "1990 - ... Points: " print: 20 ; 
   ^self select: [ ^date year >= 1990 ] . count printNL ;
  "Has Earliest Point: " print: 20 ; 
   (^self select: [ ^date = earliestPossibleDate ] . count > 0 ) printNL ;
  "Instances in Cluster: " print: 20 ; 
  ^self instanceList count printNL ; 
  "Total: " print: 20 ; ^self select: [ isNumber ] . total printNL ; 
  "Average: " print: 20 ; ^self select: [ isNumber ] . average printNL ; 
  "Maxium: " print: 20 ; ^self select: [ isNumber ] . max printNL ; 
  "Minimum: " print: 20 ; ^self select: [ isNumber ] . min printNL ; 
  "===> Distribution of Points" printNL ;
  ^self groupedBy: [ ^date year ] .
     do: [ print ; groupList count printNL ] ;
  
  newLine print ; 
] .
defineMethod:
[ | displayClusterStats |
  !ts <- ^self new ;
  ^self instanceList
   do: [ !ts <- ^my ts ; 
         ^self do: [ ^my ts put: 0 ]
       ] ;
  ^self instanceList 
    iterate: [ !ts <- ^my ts ;
               ^self do: [ ^my ts put: ( ^my ts value + 1) ] ;
             ] by: 1 ;
  "Total Points: " print: 20 ; ts count printNL ;
  "First Point: " print: 20 ; !fd <- ts firstDate printNL ;
  "Last Point: " print: 20 ; !ld <- ts lastDate printNL ;
  "Instances in Cluster: " print: 20 ; 
  ^self instanceList count printNL ; 
  "===> First/Last Values For Each Instance" printNL  ;
  ^self instanceList runningTotal: [1] .
  do: [ runningTotal print: -5 ;
        "  First: " print ; 
        !fd <- ^self effectiveDateAsOf: ^my fd . print: 12 ;
        fd isDate
          ifTrue: [ ^self asOf: fd . print: 15 ]
         ifFalse: [ " " print: 15 ] ;
        "   Last: " print ; 
        !ld <- ^self effectiveDateAsOf: ^my ld . print: 12 ;
        ld isDate
          ifTrue: [ ^self asOf: ld . print: 15 ]
         ifFalse: [ " " print: 15 ] ;
       newLine print ; 
      ] ;
  "===> Duplicate Dates" printNL ;
  ts select: [ ^self > 1 ] .  
     do: [ ^date print: 15 ; printNL ] ;
  newLine print ; 
  "===> Distribution of Points" printNL ;
  ^self groupedBy: [ ^date year ] .
     do: [ print ; groupList count printNL ] ;
  
  newLine print ; 
] .
;

?g

"
####################
#  Stand Alone Case
####################
" printNL ; 

#---  Create unclustered standalone time series
!tsu1 <- TimeSeries new ;

"...  New Unclustered Time Series" printNL ;
tsu1 displayStats ;
"...  Yearly (89-92) Points Integers: Added in Scalar" printNL ;
tsu1 asOf: 90 put: 90 ;
tsu1 asOf: 92 put: 92 ;
tsu1 asOf: 91 put: 91 ;
tsu1 asOf: 89 put: 89 ;
tsu1 displayStats ;
"...  Monthly points (8806-8905) Integers: Added in drange" printNL ;
8806 to: 8905 by: 1 monthEnds .
   evaluate: [ tsu1 put: ^date asInteger ] ;
tsu1 displayStats ;
"...  Random points Integers Added in Scalar" printNL ; 
tsu1 asOf: 900112 put: 900112 ;
tsu1 asOf: 930301 put: 930301 ;
tsu1 displayStats ;
"...  Random points (Non-Integer) Added in Scalar" printNL ;
tsu1 asOf: 880711 put: 880711.0 ;
tsu1 asOf: 900101 put: Entity ;
tsu1 asOf: 910414 put: List new ;
tsu1 displayStats ;


#---  Create clustered standalone time series
!tsc1 <- TimeSeries new ;
!tsc2 <- tsc1 clusterNew ; 

"...  New Clustered Time Series" printNL ;
"TSC1: " printNL ; tsc1 displayStats ; 
"TSC2: " printNL ; tsc2 displayStats ;
"...  Yearly (89-92) Points Integers: Added in Scalar" printNL ;
tsc1 asOf: 90 put: 90 ;
tsc1 asOf: 92 put: 92 ;
tsc1 asOf: 91 put: 91 ;
tsc1 asOf: 89 put: 89 ;
"TSC1: " printNL ; tsc1 displayStats ; 
"TSC2: " printNL ; tsc2 displayStats ;
"...  Monthly points (8806-8905) Integers: Added in drange" printNL ;
8806 to: 8905 by: 1 monthEnds .
   evaluate: [ tsc1 put: ^date asInteger ] ;
"TSC1: " printNL ; tsc1 displayStats ; 
"TSC2: " printNL ; tsc2 displayStats ;
"...  Random points Integers Added in Scalar" printNL ;
tsc1 asOf: 900112 put: 900112 ;
tsc1 asOf: 930301 put: 930301 ;
"TSC1: " printNL ; tsc1 displayStats ; 
"TSC2: " printNL ; tsc2 displayStats ;
"...  Random points (Non-Integer) Added in Scalar" printNL ; 
tsc1 asOf: 880711 put: 880711.0 ;
tsc1 asOf: 900101 put: Entity ;
tsc1 asOf: 910414 put: List new ;
"TSC1: " printNL ; tsc1 displayStats ; 
"TSC2: " printNL ; tsc2 displayStats ;

"...  Daily points in tsc2 (Integer) Added in drange" printNL ; 
870101 to: 901231 by: 1 days .
  evaluate: [ tsc2 asOf: ^date put: ^date asInteger ] ;
"TSC1: " printNL ; tsc1 displayStats ; 
"TSC2: " printNL ; tsc2 displayStats ;

"...  Change subset of values in tsc1 to string" printNL ; 
"...  Change subset of values in tsc2 to float" printNL ; 
8803 to: 8910 by: 2 months . 
  evaluate: [ tsc1 asOf: ^date put: (tsc1 asOf: ^date . asString) ] ;
861115 to: 870115 by: 1 days .
  evaluate: [ tsc2 asOf: ^date put: (tsc2 asOf: ^date . asFloat) ] ;
"TSC1: " printNL ; tsc1 displayStats ; 
"TSC2: " printNL ; tsc2 displayStats ;

"*** Cluster Stats" printNL ;
tsu1 displayClusterStats ;
tsc1 displayClusterStats ;


"
####################
#  Property Case
####################
" printNL ; 

!class1 <- Object specializeClass ;
class1 __define: "tsp" ;

!c1a <- class1 new ;
"...  New Time Series in class of 1 instance" printNL ;
c1a :tsp displayStats ;
"...  Yearly (89-92) Points Integers: Added in Scalar" printNL ;
c1a :tsp asOf: 90 put: 90 ;
c1a :tsp asOf: 92 put: 92 ;
c1a :tsp asOf: 91 put: 91 ;
c1a :tsp asOf: 89 put: 89 ;
c1a :tsp displayStats ;
"...  Monthly points (8806-8905) Integers: Added in drange" printNL ;
8806 to: 8905 by: 1 monthEnds .
   evaluate: [ c1a :tsp put: ^date asInteger ] ;
c1a :tsp displayStats ;
"...  Random points Integers Added in Scalar" printNL ;
c1a :tsp asOf: 900112 put: 900112 ;
c1a :tsp asOf: 930301 put: 930301 ;
c1a :tsp displayStats ;
"...  Random points (Non-Integer) Added in Scalar" printNL ;
c1a :tsp asOf: 880711 put: 880711.0 ;
c1a :tsp asOf: 900101 put: Entity ;
c1a :tsp asOf: 910414 put: List new ;
c1a :tsp displayStats ;

!c1b <- class1 new ;
"...  New Clustered Time Series" printNL ;
"c1a :tsp: " printNL ; c1a :tsp displayStats ; 
"c1b :tsp:: " printNL ; c1b :tsp displayStats ;
"...  Yearly (89-92) Points Integers: Added in Scalar" printNL ;
c1a :tsp asOf: 90 put: 90 ;
c1a :tsp asOf: 92 put: 92 ;
c1a :tsp asOf: 91 put: 91 ;
c1a :tsp asOf: 89 put: 89 ;
"c1a :tsp: " printNL ; c1a :tsp displayStats ; 
"c1b :tsp:: " printNL ; c1b :tsp displayStats ;
"...  Monthly points (8806-8905) Integers: Added in drange" printNL ;
8806 to: 8905 by: 1 monthEnds .
   evaluate: [ c1a :tsp put: ^date asInteger ] ;
"c1a :tsp: " printNL ; c1a :tsp displayStats ; 
"c1b :tsp:: " printNL ; c1b :tsp displayStats ;
"...  Random points Integers Added in Scalar" printNL ;
c1a :tsp asOf: 900112 put: 900112 ;
c1a :tsp asOf: 930301 put: 930301 ;
"c1a :tsp: " printNL ; c1a :tsp displayStats ; 
"c1b :tsp:: " printNL ; c1b :tsp displayStats ;
"...  Random points (Non-Integer) Added in Scalar" printNL ; 
c1a :tsp asOf: 880711 put: 880711.0 ;
c1a :tsp asOf: 900101 put: Entity ;
c1a :tsp asOf: 910414 put: List new ;
"c1a :tsp: " printNL ; c1a :tsp displayStats ; 
"c1b :tsp:: " printNL ; c1b :tsp displayStats ;

"...  Daily points in c1b :tsp (Integer) Added in drange" printNL ; 
870101 to: 901231 by: 1 days .
  evaluate: [ c1b :tsp asOf: ^date put: ^date asInteger ] ;
"c1a :tsp: " printNL ; c1a :tsp displayStats ; 
"c1b :tsp:: " printNL ; c1b :tsp displayStats ;

"...  Change subset of values in c1a :tsp to string" printNL ; 
"...  Change subset of values in c1b :tsp to float" printNL ; 
8803 to: 8910 by: 2 months . 
  evaluate: [ c1a :tsp asOf: ^date put: (c1a :tsp asOf: ^date . asString) ] ;
861115 to: 870115 by: 1 days .
  evaluate: [ c1b :tsp asOf: ^date put: (c1b :tsp asOf: ^date . asFloat) ] ;
"c1a :tsp: " printNL ; c1a :tsp displayStats ; 
"c1b :tsp:: " printNL ; c1b :tsp displayStats ;

"*** Cluster Stats" printNL ;
c1a :tsp displayClusterStats ;
c1a :tsp displayClusterStats ;

"
####################
#  DeleteColumn test
####################
" printNL ;

!Class <- Object basicSpecialized ;
Class define: 'ts' ; 
!c1 <- Class new ; 
"...  Assign Values" printNL ;
930101 to: ^today - 1 days by: 1 days .
evaluate: [ c1 :ts put: ^date asInteger ; 
          ] ;
"...  Delete Column" printNL ;
930101 evaluate:
   [ Class :ts deleteColumn  ] ;
"...  Align Store " printNL ;
Class :ts alignStore ;
"...  New Instance" printNL ;
!c2 <- Class new ; 
"... Update" printNL ;
c2 :ts put: 99 ;


###########################################################
#		End of tests ...			  #
###########################################################
newLine print; "End of file: tstmp.S" printNL; "" printNL;
?g
?t
Utility stopEchoToFile;
?g
