"
##############################
####  DateRange Protocol  ####
##############################
"
?g

####################
#  Print and Other Standard Messages
####################

Prototypical DateRange
defineMethod:
[ | print |
  date1 print ;
  " to " print; date2 print;
  " by"  print; increment print;
  ^self
] .

;

####################
#  Additional Protocol
####################

Prototypical DateRange

   define: 'isObjectSpace' toBe: FALSE .

defineMethod: [ | new | ^self ] .

defineMethod:
[ | evaluate: aBlock |
  increment from: date1 to: date2 evaluate: aBlock;
  ^self
] .

defineMethod: 
[ | asDateList |
  !dateList <- ^global Prototypical List new;
  ^self evaluate:
     [
     dateList, ^date
     ];
   dateList

] .

defineMethod:
[ | = dr2 | 
  dr2 isDateRange
   ifTrue: [ (date1 = dr2 date1) && (date2 = dr2 date2) &&
             (increment = dr2 increment)
           ]
   ifFalse: [ FALSE ] 
] .

defineMethod:
[ | iterate: block |      #- run list operation serially , preserve ^date
  !dates <- ^self asDateList ;
  !counter <- 1 ;
  [ counter <= dates count ] 
  whileTrue:
     [ !date <- dates at: counter ;
       date evaluate: block ;
       :counter <- (counter + 1 ) asInteger ;
     ] ;
  ^self
] .
;

####################
#  Define an object and a  date range method that evaluates a block 
#    for a supplied object for each date in the range and returns the 
#    results as a time series, one point for each date in the range.
#  If supplied date range is a single date, the value is returned as
#    of the date.
####################

Prototypical DateRange
defineMethod:
[ | extract: block for: object | 
  !ts <- ^global TimeSeries new ;
  ^self evaluate: [ ts put: (object send: block) ] ;
  ts
] ;


?g
