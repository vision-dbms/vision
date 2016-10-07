"
######################
####  TimeSeries  ####
######################
"
?g

####################
#  Print and Other Standard Messages
####################

Prototypical TimeSeries
;

####################
#  Primitives and Converses
####################

Prototypical TimeSeries

  define: 'value'           toBe: 40 asPrimitive .     #EvaluateForValue
  define: 'valueCell'       toBe: 140 asPrimitive .    #LocateCurrentDate
  define: 'newValueCell'    toBe: 141 asPrimitive .    #DefineCurrentDate
  define: 'intervalStart'   toBe: 142 asPrimitive .    #EffectiveDate
  define: 'intervalEnd'     toBe: 143 asPrimitive .    #NextEffectiveDate
  define: 'deletePoint'     toBe: 144 asPrimitive .    #DeleteCurrentDate
  define: 'new'             toBe: 145 asPrimitive .    #NewTimeSeries
  define: 'deleteColumn'    toBe: 146 asPrimitive .    #DeleteDate
  define: 'toList'          toBe: 162 asPrimitive .    #IndexedListToSequencedList
  define: 'count'           toBe: 170 asPrimitive .    #CountListElements
  define: 'iterate:by:'     toBe: 179 asPrimitive .    #Iterate
  define: 'do:'             toBe: 180 asPrimitive .    #IterateInContext
  define: 'send:'           toBe: 181 asPrimitive .    #IterateInContextForValue
  define: 'extendBy:'       toBe: 182 asPrimitive .    #IterateInContextForEnvironment
  define: 'select:'         toBe: 183 asPrimitive .    #IterateInContextForSubset
  define: 'groupedBy:usingCollector:'
    toBe: 184 asPrimitive .        #IterateInContextForGrouping
  define: 'basicSortUp:'    toBe: 185 asPrimitive .    #IterateInContextForAscendingSort
  define: 'basicSortDown:'  toBe: 186 asPrimitive .    #IterateInContextForDescendingSort

  define: 'basicTotal:'          toBe: 190 asPrimitive .    #IterateInContextForNumericTotal
  define: 'basicAverage:'        toBe: 191 asPrimitive .    #IterateInContextForNumericAverage
  define: 'basicMin:'            toBe: 192 asPrimitive .    #IterateInContextForNumericMinimum
  define: 'basicMax:'            toBe: 193 asPrimitive .    #IterateInContextForNumericMaximum
  define: 'ncount:'         toBe: 194 asPrimitive .    #IterateInContextForNumericCount
  define: 'basicRankUp:usingCollector:'
    toBe: 195 asPrimitive .                            #IterateInContextForAscendingRank
  define: 'basicRankDown:usingCollector:' 
    toBe: 196 asPrimitive .                            #IterateInContextForDescendingRank
  define: 'basicProduct:'        toBe: 197 asPrimitive .    #IterateInContextForNumericProduct
  define: 'basicRunningTotal:usingCollector:'
     toBe: 198 asPrimitive .                           #IterateInContextForRunningTotal

  defineMethod: [ | valueCell: aDate | aDate evaluate: [valueCell] ] .
  defineMethod: [ | newValueCell: aDate | aDate evaluate: [newValueCell] ] .
;


####################
#  Time Series point/date Access/Update
####################

Prototypical TimeSeries

  defineMethod: [ | asOf: aDate | aDate evaluate: super ] .
  defineMethod:	[ | put: aValue | newValueCell <- aValue ] .
  defineMethod: [ | delete: aDate | aDate evaluate: [deletePoint] ] .

defineMethod:	
[ | asOf: aDate put: aValue |  
  aDate evaluate: [newValueCell <- aValue]
] .

defineMethod:
[ | asOf: aDate assign: aValue |
  aDate evaluate: [valueCell <- aValue]
] .

defineMethod: 
[ | <- aValue |
  !fdate <- firstDate ; 
  fdate isNA ifTrue: [ :fdate <- ^date + 1 days ] ;
  ^date < fdate
  ifTrue: [ ^self asOf: 10101 basicAsDate put: aValue ]
  ifFalse: [ valueCell <- aValue ]
] .

defineMethod:
[| effectiveDateAsOf: aDate |
  aDate evaluate: [intervalStart + 0 days] .
     send: [ isInitialDate ifFalse: [ ^self ] ]
] .
defineMethod: [ | effectiveDate | super effectiveDateAsOf: ^date ] .

defineMethod:
[ | nextDateAsOf: aDate |
   aDate evaluate:[intervalEnd + 0 days] .
     send: [ isInitialDate ifFalse: [ ^self ] ]
] .
defineMethod: [ | nextDate | super nextDateAsOf: ^date ] .

defineMethod:
[ | deleteColumnForDate: aDate |
   aDate evaluate:[deleteColumn]
] .

defineMethod:
[ | deleteAllPoints |

!ts <- ^self ;
ts do: [ ^my ts delete: ^date ] ;

] .

defineMethod: 
[ | firstObservation | 
!date <- firstDate ;
date isDate ifTrue: [ date evaluate: super super ]
] .

defineMethod: 
[ | lastObservation | 
  99991231 basicAsDate evaluate: super  
] .

defineMethod: 
[ | firstDate|
  ^self effectiveDateAsOf: earliestPossibleDate . isntNA
    ifTrue: [ earliestPossibleDate ] 
   ifFalse: [ super nextDateAsOf: earliestPossibleDate ]
] .

defineMethod: [ | lastDate | super effectiveDateAsOf: 99991231 basicAsDate ] .
;

#----------
#  updateWith: object
#     only update the t/s if the supplied object is different from
#     the current t/s value (as of ^date).  If value is new, update
#     t/s as of ^date; if t/s has a point as of earliest date and
#     its value is default or NA and this is the first real point in
#     the time series, set the earliest value to the object as well
#----------

Prototypical TimeSeries
defineMethod:
[ | updateWith: object | 
  !ts <- ^self ; 
  !currentValue <- ts value ;
  currentValue asSelf != object asSelf
  ifTrue: 
    [ ts firstDate = earliestPossibleDate && 
      ts count = 1 && ts value isDefault
        ifTrue: [ ts asOf: earliestPossibleDate put: object ] ;
      ts put: object asSelf ;
    ] ;
  ts
] .

defineMethod: [ | changeTo: newValue |
!changed <- FALSE ;
!currentValue <- ^self value ;
currentValue !== newValue
   ifTrue: [ :changed <- TRUE ; ^self put: newValue ] ;
changed
] .
;

#----------
#  from: to:
#  from:
#  to:
#----------

Prototypical TimeSeries

defineMethod: 
[ | from: date1 to: date2 | 
!d1 <- date1 asDate; 
!d2 <- date2 asDate; 
d1 isDate && [ d2 isDate ] && [ d2 < d1 ]
  ifTrue: [ !tmp <- d2 ; :d2 <- d1 ; :d1 <- tmp ] ;

d1 isNA  ifTrue: [ ^self to: d2 ] .
 elseIf: [ d2 isNA ] then: [ ^self from: d1 ] .
 else: [ ^self select: [^my d1 <= ^date && ^date <= ^my d2]
       ]
] .

defineMethod:
[ | from: aDate | 
  !date <- aDate asDate ;
  date isDate
     ifTrue: [ ^self select: [^my date <= ^date] ] 
    ifFalse: [ ^self ] 
] .

defineMethod:
[ | to: aDate | 
  !date <- aDate asDate ;
  date isDate
     ifTrue: [ ^self select: [^my date >= ^date] ] 
    ifFalse: [ ^self ] 
] .

defineMethod: [ | doReverse: block |
  ^self asReverseList
    do: [ date evaluate: [ ^self basicSend: ^my block  ] ] ;
  ^self
] .

defineMethod: [ | asReverseList |
  ^self extendBy: [ !date <- ^date ] . sortDown: [ date ]
] .

;

#----------
#  t/s exact match on date
#----------

Prototypical TimeSeries
defineMethod:
[ | on: aDate | 
  !date <- aDate asDate ;
  date isntNA && [ ^self effectiveDateAsOf: date . = date ] 
     ifTrue: [ ^self asOf: date ]
] .
;
   
#------------------------------

####################
#  relative date: TS
####################

Prototypical TimeSeries

defineMethod: [ | latest | super asOf: ^today ] .

#----------

defineMethod: [ | lag: anOffset | super asOf: ^date - anOffset ] .

#----------

defineMethod:
[ | changeLag: anOffset |
(super value) - (super lag: anOffset)
] .

#----------

defineMethod:
[ |pctChangeLag: anOffset|
(super value) pctChangeTo: (super lag: anOffset)
] .

#----------

defineMethod: [ | lead: anOffset | super asOf: ^date + anOffset ] .

#----------

defineMethod:
[ | changeLead: anOffset |
(super lead: anOffset) - (super value) 
] .

#----------

defineMethod:
[ | pctChangeLead: anOffset |
(super lead: anOffset) pctChangeTo: (super value)
] ;

#------------------------------


####################
#  stats : TS
####################

Prototypical TimeSeries

defineMethod:
[ | extractForDateRange: dr |

!ts <- ^global Prototypical TimeSeries new;
dr evaluate: [ ts put: value ];
ts

] .

defineMethod:
[ | extractExactValuesForDateRange: dr |
  !ts <- ^global Prototypical TimeSeries new ;
  !default <- ^self asOf: earliestPossibleDate ;
  dr evaluate:
   [ ts put: default ; 
     ^self effectiveDateAsOf: ^date . = ^date
         ifTrue: [ ts put: value ]
   ]; 
  ts
] .

#----------

defineMethod:
[ | aveForDateRange: dr |
  ^self extractForDateRange: dr . average 
] .

#----------

defineMethod:
[ | totalForDateRange: dr |
  ^self extractForDateRange: dr . total 
] .

#----------

defineMethod:
[ | maxForDateRange: dr |
  ^self extractForDateRange: dr . max 
] .

#----------

defineMethod:
[ | minForDateRange: dr |
  ^self extractForDateRange: dr . min 
] .

#----------

defineMethod:
[ | productForDateRange: dr |
  ^self extractForDateRange: dr . product 
] .

#----------

defineMethod:
[ | stdDevForDateRange: dr |
  ^self extractForDateRange: dr . stdDev
] .

#----------

defineMethod:
[ | medianForDateRange: dr |
  ^self extractForDateRange: dr . median
] .

#----------

defineMethod:
[ | modeForDateRange: dr |
  ^self extractForDateRange: dr . mode
] .

#----------

defineMethod:
[ | cGrow | 
  100 * ( 
          ( lastObservation / firstObservation toThe: (1 / (count - 1) )
          ) - 1
        )
] .

defineMethod:
[ | cGrowForDateRange: dr |
  ^self extractForDateRange: dr . cGrow
] .

#----------

defineMethod:
[ | gMeanForDateRange: dr |
  ^self extractForDateRange: dr . gMean 
] .

#----------

defineMethod: 
[ | lsGrow |

  !rankedTS <- ^self rankUp: [ ^date asIDate7 ] ;
  !sumX <-  rankedTS total: [ rank ] ;
  !sumXlogY <- rankedTS total: [ rank * asDouble log ] ;
  !sumLogY <- ^self total: [ asDouble log ] ;
  !sumX2 <- rankedTS total: [ rank * rank ] ;
  100 * ( 1 exp toThe: (
                         (sumXlogY - sumX * sumLogY / count ) /
                         (sumX2 - sumX * sumX / count)
                       ) . - 1
        )
] .

defineMethod: 
[ | lsGrowForDateRange: dr |
  ^self extractForDateRange: dr . lsGrow 
] ;

#------------------------------

Prototypical TimeSeries defineMethod: [ | extractUniqueForDateRange: dr |
  !offset <- dr increment ;
  ^self extractValuesForDateRange: dr within: offset
] ;

Prototypical TimeSeries defineMethod: [ | extractValuesForDateRange: dr within: offset |
  !ts <- ^global Prototypical TimeSeries new ;
  !default <- ^self asOf: earliestPossibleDate ;
  dr evaluate:
   [ ts put: default ;
     ^self effectiveDateAsOf: ^date . + offset > ^date
         ifTrue: [ ts put: value ]
   ];
  ts
] ;


#------------------------------

####################
#  display
####################

Prototypical TimeSeries
defineMethod:
[ | displayAll |
^self do: [ ^date print: 15 ; print; newLine print;];
].

#----------
#  displayAllChanges
#     only display the first point in a series of identical points
#     and exclude earliestPossibleDate
#----------
defineMethod:
[ | displayAllChanges | 
  !ts <- ^self ;
  ^self select: [ ^date isntDefault ] .
     select: [ ^self isntEquivalentTo: (^my ts lag: 1 days)  ] .
  do: [ ^date print: 12 ; printNL ] 
] .
;

#------------------------------

####################
#  "Vector" Methods
####################

Prototypical TimeSeries
defineMethod:
[ | + value |
  value isTimeSeries
   ifTrue: [ ^self send: [ ^self + (^my value asOf: ^date ) ] ] .
   elseIf: [ value isNumber ] 
     then: [ ^self send: [ ^self + ^my value ] ] .
     else: [ ^self toList + value ] 
] .
defineMethod:
[ | - value |
  value isTimeSeries
   ifTrue: [ ^self send: [ ^self - (^my value asOf: ^date ) ] ] .
   elseIf: [ value isNumber ] 
     then: [ ^self send: [ ^self - ^my value ] ] .
     else: [ ^self toList - value ] 
] .
defineMethod:
[ | * value |
  value isTimeSeries
   ifTrue: [ ^self send: [ ^self * (^my value asOf: ^date ) ] ] .
   elseIf: [ value isNumber ] 
     then: [ ^self send: [ ^self * ^my value ] ] .
     else: [ ^self toList * value ] 
] .
defineMethod:
[ | / value |
  value isTimeSeries
   ifTrue: [ ^self send: [ ^self / (^my value asOf: ^date ) ] ] .
   elseIf: [ value isNumber ] 
     then: [ ^self send: [ ^self / ^my value ] ] .
     else: [ ^self toList / value ] 
] .

####################
#  iterate: 
#    force sequential processing of t/s
####################

defineMethod:
[ | iterate: block |      #- run list operation serially , preserve ^date
  !dates <- ^self send: [ ^date ] . toList ;
  !counter <- 1 ;
  [ counter <= ^self count ] 
  whileTrue:
     [ !date <- dates at: counter ;
       ^self select: [ ^date = (^my date) ] . send: block ;
       :counter <- (counter + 1 ) asInteger ;
     ] ;
  ^self
] .

;


?g
