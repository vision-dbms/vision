"
######################
####  Collection  ####
######################
"
?g

####################
#  Print and Other Standard Messages
####################
Prototypical Collection
   define: 'count' toBe: NA .
defineMethod: [ | print | 
  ^self whatAmI print ; " of " print ; ^self count asString print ;
  ^self
] .
;

####################
#  Primitives and Converses
####################

Prototypical Collection

defineMethod:
[ | new | 
  ^self newPrototype clusterNew
] .

#----------

defineMethod:	
[| groupedBy: aBlock |
^self groupedBy: aBlock usingCollector: [|:groupList| ^current]
] .

#----------

defineMethod:
[ | rankUp: valueBlock usingCollector: collectorBlock |
    ^self basicRankUp: [
	^self basicSend: ^my valueBlock. canonicalizedForSort
    ] usingCollector: collectorBlock
].

defineMethod:
[ | rankUp: aBlock |
^self rankUp: aBlock usingCollector: [|:rank| ^current]
] .

defineMethod:
[ | rankUp: aBlock by: bBlock |
  !list <- ^self extendBy: [ !rank;  ] ;
  list groupedBy: bBlock . 
  do: [ 
      groupList rankUp: ^my aBlock . 
          do: [ ^self super :rank <- rank ;];
      ];
 list
] .

defineMethod: 
[ | rankUp: aBlock by: bBlock by: cBlock |
  !list <- ^self extendBy: [ !rank;  ] ;
  list groupedBy: bBlock . 
  do: [ 
      groupList rankUp: ^my aBlock by: ^my cBlock .
           do: [ ^self super :rank <- rank ; ];
      ];
  list
] .

#----------

defineMethod:
[ | rankDown: aBlock |
^self rankDown: aBlock usingCollector: [|:rank| ^current]
] .

defineMethod:
[ | rankDown: valueBlock usingCollector: collectorBlock |
    ^self basicRankDown: [
	^self basicSend: ^my valueBlock. canonicalizedForSort
    ] usingCollector: collectorBlock
].

defineMethod: 
[ | rankDown: aBlock by: bBlock |
  !list <- ^self extendBy: [ !rank;  ] ;
  list groupedBy: bBlock . 
  do: [ 
      groupList rankDown: ^my aBlock . 
           do: [ ^self super :rank <- rank ;];
      ];
  list
] .

defineMethod: 
[ | rankDown: aBlock by: bBlock by: cBlock |
  !list <- ^self extendBy: [ !rank;  ] ;
  list groupedBy: bBlock . 
  do: [ 
      groupList rankDown: ^my aBlock by: ^my cBlock .
          do: [ ^self super :rank <- rank ; ];
      ];
  list
] .

#----------

defineMethod:
[ | runningTotal: aBlock |
^self runningTotal: aBlock usingCollector: [|:runningTotal| ^current]
] .

defineMethod:
[ | runningTotal: valueBlock usingCollector: collectorBlock |
    ^self basicRunningTotal: [
	^self basicSend: ^my valueBlock. canonicalizedForSort
    ] usingCollector: collectorBlock
].

defineMethod:
[ | runningAverage: aBlock |
  !list <- ^self runningTotal: aBlock . extendBy: [ !runningAverage ];
  list select: [ runningTotal isNumber ] . numberElements
    do: [ :runningAverage <- runningTotal / position ];
  list
] .

;

?g

#--------------------------------------------------

####################
#  Add Interesting Methods that apply to List, IndexedList, and TS
#    classes.  These were moved from subclasses to Collection as
#    of 10/31/93 as part of the batt release 4 reorganization.
#  Patches are in EXT___.5.7a
####################

#--------------------
#  Access
#--------------------

Prototypical Collection

defineMethod:
[ | collect: aBlock |
#-- extends recipient by !value
  ^self extendBy: [ !value <- ^self send: ^my aBlock ]
] .

#----------

defineMethod:
[ | anElement |
  ^self toList at: ^self count
] .

#----------

defineMethod:
[ | send: block with: parameter |
  ^self send: [ ^self send: ^my block with: ^my parameter ]
] .

#----------

defineMethod: [ | doReverse: block |
  ^self numberElements sortDown: [ position ] .
  do: [ ^self basicSend: ^my block ] ;
  ^self
]  .

#----------

defineMethod:
[ | collect: block with: parameter |
  ^self extendBy: [ !value <- ^self send: ^my block with: ^my parameter ]
] .

defineMethod:
[ | first: n | 
  ^self select: [ isntDefault ] . numberElements
     select: [position <= ^my n ]
] .

defineMethod:
[ | last: n | 
  !list <- ^self select: [ isntDefault ] . numberElements ;
  !count <- list count; 
  list select: [^my count - position < ^my n]
] .

defineMethod:
[ | nonDefaults | 
  ^self select: [ isntDefault ]
] .

defineMethod: [ | from: start to: end | 
  :start <- start asInteger else: 1 . min: ^self count ;
  :end <- end asInteger else: ^self count . max: 1 ; 
  ^self numberElements
      select: [ position between: ^my start and: ^my end ] .
      send: [ ^self super ] 
] .

defineMethod: [ | from: position |
  ^self from: position to: ^self count 
] .

defineMethod: [ | to: position | 
  ^self from: 1 to: position
] .

defineMethod: [ | reject: block |
    ^self select: [ ^self basicSend: ^my block . not ] 
] .
;

#--------------------
#  sorting and ranking
#--------------------

Prototypical Collection

defineMethod: [ | sortUp: sortBlock |
  !dictionary <- ^self basicSpecialized;
  ^self basicSortUp:
     [
	^self basicSend: ^my sortBlock. orderUsingDictionary: ^my dictionary
     ]
] .

defineMethod: [ | sortDown: sortBlock |
  !dictionary <- ^self basicSpecialized;
  ^self basicSortDown:
     [
	^self basicSend: ^my sortBlock. orderUsingDictionary: ^my dictionary
     ]
] .

defineMethod:
[ | sortUpBy: b1 then: b2 |
  ^self sortUp: b2 . sortUp: b1
] .

defineMethod:
[ | sortUpBy: b1 then: b2 then: b3 |
  ^self sortUp: b3 . sortUp: b2 . sortUp: b1
] .

defineMethod:
[ | sortUpBy: b1 then: b2 then: b3 then: b4 |
  ^self sortUp: b4 . sortUp: b3 . sortUp: b2 . sortUp: b1
] .

defineMethod:
[ | sortDownBy: b1 then: b2 |
  ^self sortDown: b2 . sortDown: b1
] .

defineMethod:
[ | sortDownBy: b1 then: b2 then: b3 |
  ^self sortDown: b3 . sortDown: b2 . sortDown: b1
] .

defineMethod:
[ | sortDownBy: b1 then: b2 then: b3 then: b4 |
  ^self sortDown: b4 . sortDown: b3 . sortDown: b2 . sortDown: b1
] .

;

#--------------------
#  statistical
#--------------------

Prototypical Collection

defineMethod: 
[ | total: block | 
  !list <- ^self send: [ ^self basicSend: ^my block ] . select: [ isNumber ] ;
  list count > 0 
      ifTrue: [ list basicTotal: [ ^self asSelf ] ] 
] .

defineMethod: [ | total | ^self total: [ ^self asSelf ] ] .

#----------

defineMethod: 
[ | average: block | 
  !list <- ^self send: [ ^self basicSend: ^my block ] . select: [ isNumber ] ;
  list count > 0 
      ifTrue: [ list basicAverage: [ ^self asSelf ] ] 
] .

defineMethod: [ | average | ^self average: [ ^self asSelf ] ] .

#----------

defineMethod: 
[ | min: block | 
  !list <- ^self send: [ ^self basicSend: ^my block ] . select: [ isNumber ] ;
  list count > 0 
      ifTrue: [ list basicMin: [ ^self asSelf ] ] 
] .

defineMethod: [ | min | ^self min: [ ^self asSelf ] ] .

#----------

defineMethod: 
[ | max: block | 
  !list <- ^self send: [ ^self basicSend: ^my block ] . select: [ isNumber ] ;
  list count > 0 
      ifTrue: [ list basicMax: [ ^self asSelf ] ] 
] .

defineMethod: [ | max | ^self max: [ ^self asSelf ] ] .

#----------

defineMethod: 
[ | product: block | 
  !list <- ^self send: [ ^self basicSend: ^my block ] . select: [ isNumber ] ;
  list count > 0 
      ifTrue: [ list basicProduct: [ ^self asSelf ] ]
] .

defineMethod: [ | product | ^self product: [ ^self asSelf ] ] .

#----------

defineMethod: 
[ | stdDev: block | 
  !list <- ^self send: [ ^self basicSend: ^my block ] . select: [ isNumber ] ;
  !mean <- list average ;
  (list total: [ (^self - ^my mean ) toThe: 2 ] . / list count ) sqrt
] .

defineMethod: [ | stdDev | ^self stdDev: [ ^self asSelf ] ] .

#----------

defineMethod:
[ | mode: aBlock |

  !data <- ^self groupedBy:  aBlock ;
  !maxValue <- data max: [ groupList count ];
  data select: [ groupList count = ^my maxValue ] .
        average: [ ^self asNumber ] 

] .

defineMethod: [ | mode | ^self mode: [ ^self asSelf ] ] .

#----------

defineMethod: [ | median: aBlock| 
   !list <- ^self collect: aBlock .
      select: [value isntNA ]. toList 
      sortUp: [value] ; 
   !mid <- ((list count + 1) / 2) asInteger; 
   !midValue <- list at: mid . value ;
  list count mod: 2 .= 1 || [ midValue isNumber not ] 
      ifTrue: [ midValue ] 
     ifFalse: [(list at: mid .value ) / 2 + 
               (list at: (mid + 1) .value) / 2
              ]
] .

defineMethod: [ | median | ^self median: [ ^self asSelf ] ] .

#----------

defineMethod:
[ | gMean: block |
  !list <- ^self send: [ ^self basicSend: ^my block ] . select: [ isNumber ] ;
  list product toThe: ( 1 / list count )
] .

defineMethod: [ | gMean | ^self gMean: [ ^self asSelf ] ] .

;

#----------
#  regress:
#
#  Returns an extended object which responds to the beta, alpha,
#    pearson, rsq, and stdErr messages that are computed using a standard
#    linear regression line.  The recipient object is the collection
#    of dependent data points and the argument is a collection of
#    independent data points; collections should contain numeric
#    data values and should contain the same number of data points,
#    otherwise NA values are returned.
#
#  For example:   
#     (2,3,9,1,8,7,5) regress: (6,5,11,7,5,4,4) .
#     do: [ beta print ; alpha print ; 
#           pearson print ; rsq print ; stdErr printNL ;
#         ] ;
#  
#----------

Prototypical Collection
defineMethod:
[ | regress: list2 | 
  !regressData <- ^global Object 
     extendBy: [ !beta ; !alpha ; !pearson ; !rsq ; !stdErr  ] ;
  !seriesY <- ^self select: [ isNumber ] . toList ;
  !seriesX <- list2 select: [ isNumber ] . toList ;
  seriesY count = seriesX count &&
  seriesY count = ^self count &&
#-->>> seriesX count = seriesX count
  seriesX count = list2 count
  ifTrue:
    [
    !num <- seriesY count ; 
    !aveX <- seriesX average ; 
    !sumX <- seriesX total ;
    !aveY <- seriesY average ;
    !sumY <- seriesY total ;
    !sumXY <- num sequence 
      total: [ (^my seriesY at: ^self ) * (^my seriesX at: ^self) ] ;
    !sumXX <- seriesX total: [ ^self * ^self ] ;
    !sumYY <- seriesY total: [ ^self * ^self ] ;
    !beta <- (num * sumXY - sumX * sumY) / (num * sumXX - sumX * sumX) ;
    regressData :beta <- beta ;
    !alpha <- aveY - beta * aveX ;
    regressData :alpha <- alpha ;
    !tmp1 <- num * sumXY - sumX * sumY ;
    !tmp2 <- num * sumXX - sumX * sumX ;
    !tmp3 <- num * sumYY - sumY * sumY ;
    !pearson <- tmp1 / (tmp2 * tmp3) sqrt ;
    regressData :pearson <- pearson ;
    regressData :rsq <- pearson * pearson ;
    !tmp4 <- 1 / (num * (num -2) ) ;
    regressData :stdErr <- ( tmp4 * (tmp3 - tmp1 * tmp1 /tmp2) ) sqrt ;
    ] ;
  regressData
] ;

Prototypical Collection defineMethod: [ | largest: block cutoff: n |
  n isNumber not || [ n <= 1 ] 
   ifTrue: [ ^self sortDown: block . at: 1 ] 
  ifFalse: [ ^self sortDown: block . first: n ] 
] ;

Prototypical Collection defineMethod: [ | largest: block |
  ^self largest: block cutoff: NA
] ;

Prototypical Collection defineMethod: [ | smallest: block cutoff: n |
  n isNumber not || [ n <= 1 ] 
   ifTrue: [ ^self sortUp: block . at: 1 ] 
  ifFalse: [ ^self sortUp: block . first: n ] 
] ;

Prototypical Collection defineMethod: [ | smallest: block |
  ^self smallest: block cutoff: NA
] ;

#----------
#  Other statistical messages
#     these messages return a number
#----------

Prototypical Collection 
defineMethod:
[ | correlate: blockX with: blockY | 
  !data <- ^self 
    extendBy: [ !x <- ^self send: ^my blockX ; 
                !y <- ^self send: ^my blockY ;
              ] . 
    select: [ x isNumber && y isNumber ] ;
  ((data average: [ x * y ] ) - (data average: [x] ) * (data average: [y] )) / 
     (data stdDev: [x] ) / (data stdDev: [y] )
] .

defineMethod:
[ | rankCorrelate: blockX with: blockY | 
  ^self 
     rankUp: blockX . extendBy: [!rankX <- rank] .
     rankUp: blockY . extendBy: [!rankY <- rank] .
     correlate: [ rankX ] with: [ rankY ]
] .

#----------

defineMethod: 
[ | harmonicMean: aBlock |
  !list <- ^self send: aBlock . send: [ 1 / ^self ] ;
  1 / list average
] .

defineMethod:
[ | harmonicMean |
  ^self harmonicMean: [ ^self asSelf ]
] .

defineMethod: 
[ | harmonicMean: aBlock withWeights: bBlock |
  !list <- ^self 
     extendBy: [ !val1 <- 1/ (^self send: ^my aBlock) ;
                 !val2 <- ^self send: ^my bBlock ;
                 !val <- val1 * val2 ;
               ] .
       select: [ val isntNA ] ;
    (list total: [ val2 ] ) /  (list total: [ val ] )
] .

#----------

defineMethod:
[ | compound: aBlock |
  (^self send: aBlock . select: [ isNumber ] .
     product: [ 1 + ^self / 100 ] . - 1 ) * 100 
] .

defineMethod:
[ | compound |
  ^self compound: [ ^self asSelf ]
] .

;

#----------
#  normalize:   returns original list extended by norm 
#----------

Prototypical Collection

defineMethod:
[ | normalize: block |
  !data <- ^self collect: block ;
  !mean <- data average: [ value ] ;
  !std <- data stdDev: [ value ] ;
  data extendBy: 
     [ !norm <- ( value - ^my mean) / ^my std ;
     ]
] .

defineMethod:
[ | normalize | 
  ^self normalize: [ ^self asSelf ] 
] .

defineMethod:
[ | normalize: aBlock by: bBlock | 
  !list <- ^self extendBy: [!norm ]; 
  list groupedBy: bBlock .
  do: [
      groupList normalize: ^my aBlock .
         do: [^self super :norm <- norm;]; 
      ]; 
  list
] .

defineMethod:
[ | normalize: aBlock by: bBlock by: cBlock | 
  !list <- ^self extendBy: [!norm;]; 
  list groupedBy: bBlock .
  do: [
      groupList normalize: ^my aBlock by: ^my cBlock .
         do: [^self super :norm <- norm;]; 
      ]; 
  list
] .

;

;

#----------
#  weighted averages - returns number
#----------

Prototypical Collection
defineMethod: 
[ | average: aBlock withWeights: bBlock |
  !list <- ^self 
  extendBy: [ !val1 <- ^self send: ^my aBlock ;
              !val2 <- ^self send: ^my bBlock ;
              !val <- val1 isNumber && val2 isNumber
                 ifTrue: [ val1 * val2 ] ;
            ] .
     select: [ val isntNA ] ;
  (list total: [ val ] ) /  (list total: [ val2 ] )
] .

;

####################
#  Extract data over a date range for a collection using supplied block
#    and date range; returns a time series of lists of the data element
#    with the t/s points corresonding to the dates in the date range;
#    The getTotals: method computes the total for the list for each date
####################

Prototypical Collection
defineMethod: [ | extractTSOf: block overDateRange: drange |
!ts <- ^global TimeSeries new ;
drange evaluate: [ ts put: (^self send: block) ] ;
ts
] ;
  
Prototypical Collection
defineMethod: [ | getTotals: block overDateRange: drange |
  ^self extractTSOf: block overDateRange: drange .
      send: [ ^self total ] 
] ;

#--------------------
#  Grouping
#--------------------

Prototypical Collection

defineMethod:
[ | groupPrintUsing: aBlock |
  ^self groupedBy: aBlock .
  do: [ ^self print: 20 ; groupList count printNL ]
] .

#----------

defineMethod:
[ | groupedBy: aBlock usingCutoffs: cutoffPoints |
  !alist <- (cutoffPoints count + 1) sequence0 ;    #- all possible numbers
  !cutoffList <- cutoffPoints sortUp: [ ^self ] ;   #- ordered cutoffs
  !ilist <- ^global IndexedList new;                #- working list
  ilist at: NA
       put: ( NA
              extendBy: 
              [!groupList <- ^global List new ;
               !categoryNumber ;
               !lowerBoundary ;
               !upperBoundary ;
               !label <-  "     Not Available  " ;
              ] 
            ); 

#--- Initialize iList to appear to be a groupedBy list of empty groupLists
#--- Also define boundary and labels information
  alist 
  do: [ !cutoffList <- ^my cutoffList ;
        ^my ilist
           at: ^self asSelf 
          put: ( ^self asSelf 
                   extendBy: 
                   [!groupList <- ^global List new ;
                    !categoryNumber <- ^self ; 
                    !lowerBoundary <- ^my cutoffList at: categoryNumber ;
                    !upperBoundary <- ^my cutoffList at: categoryNumber + 1 ;
                    !label <- 
                       [ lowerBoundary isntNA && upperBoundary isntNA
                         ifTrue: 
                           [ lowerBoundary print: 8 ; " - " print ;
                             upperBoundary print: -8 ; " " print ; 
                             TRUE
                           ] .
                          elseIf: lowerBoundary isNA
                          then: 
                            [ "  <= " print: -11 ; 
                               upperBoundary print: -8 ; " " print ; 
                               TRUE 
                            ] .
                          else:
                            [ "  > " print: -11 ; 
                              lowerBoundary print: -8 ; " " print ; 
                            ] ;
                       ] divertOutput ;
                   ] 
               ); 
     ]; 

#--- Group the recipient list according to what will be its categoryNumber.
!values <- ^self collect: aBlock ;
ilist at: NA . :groupList <- (values select: [ value isNA ] ) ;

values select: [ value isntNA ] .
   groupedBy: 
      [ ^my cutoffPoints 
         total: 
           [ ^my value > asNumber         #-- How many cutoff points
                ifTrue: [1]		  #	   is it greater than?
               ifFalse: [0]		  #	   That's its 
         ] .				  #	   categoryNumber!
         asInteger
      ].
   do: [
          ^my ilist at: ^self asSelf . :groupList <- groupList ;
       ]; 

  ilist toList

] .

#----------
#  Clustered String Grouped By Message
#----------

defineMethod:
[ | groupedByString: strBlock |

  ## Extend list by 'value' (evaluation of strBlock)
  !exList <- ^self collect: strBlock;

  ## Create local dictionary and generate unique strings
  !dict <- ^global Dictionary basicSpecialized ;
  exList do: [ ^my dict at: value put: value ];

  ## Return extended list grouped by unique string
  exList groupedBy: [ ^my dict at: value ]

] .

#----------

####################
#  mgroupedBy: block
#    block returns a list of instances
#  For example, if PortfolioGroup portfolios returns a list of
#   portfolios, PortfolioGroup instanceList mgroupedBy: [ portfolios ] 
#   would return a list of portfolios where each element responds to
#   groupList with the list of PortfolioGroups containing it
####################

defineMethod:
[ | mgroupedBy: block | 
  !list <- ^global List new ; 
  ^self do: [ !currentElement <- ^self ;
              !groups <- currentElement basicSend: ^my block ;
              !list <- ^my list ;
              groups 
              do: [ !group <- ^self ;
                    ^my list , 
                    (^my currentElement extendBy: [ !group <- ^my group ])
                  ] ;
            ] ;
   list groupedBy: [ group ]
] .

#----------

####################
#  groupedBy: block1 and: block2
#    returns a list of all block1/block2 combinations
####################

defineMethod:
[ | groupedBy: b1 and: b2 |
  !list <- ^global List new ;
  ^self groupedBy: b1 .
  do: [ !list <- ^my list ;
        !firstGroupID <- [ ^self print ] divertOutput ;
        !groups <- groupList groupedBy: ^my b2 ;
        groups
        do: [ !newID <- [ ^my firstGroupID print ;
                          "." print ;
                          ^self print ;
                        ] divertOutput ;
              ^my list , 
              (newID extendBy: [ !groupList <- ^my groupList ] );
            ] ;
       ] ;
   list
] .

#----------

defineMethod:
[ | groupedByCriteria: groupBlocks |
  !results <- ^global List new ;
  !keys <- ^global List new ;
  !blockList <- groupBlocks isCollection
      ifTrue: [groupBlocks] ifFalse: [groupBlocks asList] ;
  ^self
    collectGroupsForCriteria: blockList atLevel: 1 in: results withKeys: keys
] .

defineMethod:
[ | collectGroupsForCriteria: blocks atLevel: level in: list withKeys: keys |
  !groupList <- ^self ;
  level > blocks count 
  ifTrue:
     [ list,
       ( ^global Object 
          extendBy: [!keyList   <- ^my keys ;
                     !groupList <- ^my groupList;
                    ] 
       )
     ] 
  ifFalse: 
     [
     ^self groupedBy: (blocks at: level).
     do: [
         groupList
           collectGroupsForCriteria: ^my blocks
                            atLevel: (^my level + 1) asInteger
                                 in: ^my list
                           withKeys: (^my keys append: ^self asSelf)
        ]
    ];
  list
].

#----------

#--- returns a group for any value in recipient or list
defineMethod:
[ | groupedBy: block union: list | 
  !ilist <- ^global IndexedList new ; 
  list do: [ ^my ilist 
               at: ^self asSelf 
              put: (^self asSelf 
                      extendBy: [ !groupList <- ^global List new ]
                   ) ;
           ] ;
  ^self groupedBy: block . 
  do: [ ^my ilist 
               at: ^self asSelf
              put: (^self asSelf 
                      extendBy: [ !groupList <- ^my groupList ] 
                   ) ;
      ] ;
  ilist toList
] .

#----------

#--- returns a group for every value in list
defineMethod: 
[ | groupedBy: block in: list | 
  !ilist <- ^global IndexedList new ; 
  list numberElements
  do: [ ^my ilist 
           at: ^self asSelf 
          put: (^self asSelf 
                  extendBy: [ !groupList <- ^global List new ;
                              !position <- ^my position ;
                            ]
               ) ;
           ] ;
  ^self groupedBy: block . 
  do: [ ^my ilist at: ^self asSelf . isntNA
        ifTrue:
          [ ^my ilist at: ^self asSelf . :groupList <- groupList ;
          ] ;
      ] ;
  ilist sortUp: [ position ] 
] .

#----------

#--- returns a group for any value in recipient and list
defineMethod:
[ | groupedBy: block intersect: list | 
  ^self groupedBy: block in: list .
      select: [ groupList count > 0 ] 
] .

#----------

defineMethod: 
[ | histogram: aBlock title: title |

!gl <- ^self groupedBy: aBlock;
!scaleFactor <- gl max: [ groupList count ] . / 50;

gl do: [
       !numberOfXs <- (groupList count / ^my scaleFactor) asInteger;
       "|" printNL: -9 ;
       ^self print: 8;
       "|" print;
       "X" fill: numberOfXs . print;
       groupList count printNL: 10;
       ];

"        |" printNL;
" " print: 8;
"-" fill: 50 . printNL;
newLine print;
" " print: 8;
title centerNL: 50;

] .

;


#--------------------
#  Multi-List
#  NOTE: these message return a List independent of class of
#      recipient Collection
#--------------------

Prototypical Collection

defineMethod: 
[ | collectListElementsFrom: aBlock|

#----------
# cumulative union - based on aBlock returning a list for each
#                    element in ^self
#----------
!resultList <- ^global Prototypical List new; 
^self numberElements
 do: [ !resultList <- ^my resultList; 
	^self super basicSend: ^my aBlock . numberElements
            do: [ :position <- ^my position * 10000 + position  ] .
	do: [^my resultList , ^self]]; 
resultList sortUp: [ position ] . send: [ ^self super ] 
] .

#----------

defineMethod:
[ | copyListElements|
  ^self send: [ ^self ]
] .

#----------

defineMethod:
[ | append: aList |

#--  creates a new list consisting of elements in ^self followed by aList
#--  note that returned object is a standard List independent of initial class
  !initialList <- ^self toList numberElements ;
  !appendList <- aList isCollection
       ifTrue: [ aList toList ] ifFalse: [ aList asList ] . numberElements
   do: [ :position <- (position + ^my initialList count) asInteger ] ;
  appendList do: [^my initialList , ^self] ;
  initialList sortUp: [ position ] . send: [ ^self super ] 

] .     # append:

#----------

defineMethod: 
[ | union: aList |
  ^self union: aList using: [ ^self ] . standardize
] .        # union:

defineMethod: 
[ | union: aList using: aBlock |
  ^self append: aList . groupedBy: aBlock 
] .        # union: using:

#----------

defineMethod:
[ | intersect: aList |
  ^self intersect: aList using: [ ^self ] . standardize
] .        # intersect: 

defineMethod:
[ | intersect: aList using: aBlock |
  ^self unique union: aList unique using: aBlock .
         select: [ groupList count = 2 ] .
] .        # intersect: using:

#----------

defineMethod: 
[ | exclude: aList |
  ^self unique union: aList unique using: [ ^self ] .
        select: [ groupList count != 2] . standardize
          intersect: ^self
] .        # exclude:

defineMethod:
[ | exclude: aList using: aBlock |
  ^self unique union: aList unique using: aBlock .
       select: [ groupList count != 2 ] .   # element not in both lists
          send:  [ groupList at: 1 ] .   # single element in list
       intersect: ^self using: aBlock .     # only in ^self
          send:  [ groupList at: 1 ] .   # single element in list
] .       # exclude: using:

#----------

#--------------------
#  difference:
#     returns a list of 2 elements:
#        first element contains elements in list 1 that are in list 2
#        second element contains elements in list 2 that are not in list 1
#--------------------

defineMethod:
[ | difference: list2 | 
  !xref1 <- ^global IndexedList new ;
  !xref2 <- ^global IndexedList new ;
  ^self do: [ ^my xref1 at: ^self asSelf put: ^self asSelf ] ;
  list2 do: [ ^my xref2 at: ^self asSelf put: ^self asSelf ] ;
  !notIn2 <- ^self select: [ ^my xref2 at: ^self asSelf . isNA ] ;
  !notIn1 <- list2 select: [ ^my xref1 at: ^self asSelf . isNA ] ;
  notIn2 asList , notIn1
] .

#----------

defineMethod:
[ | unique |
  ^self groupedBy: [ ^self ] . standardize
] .

defineMethod:
[ | standardize |
#--  strips off 1 layer of extension
#--    first super return the list element; second super strips 1 layer
  ^self send: [ super super ] 
] .        # standardize

#----------

defineMethod:
[ | includesElement: element |
  ^self select: [ ^self asSelf = ^my element asSelf ] . count > 0
] .

defineMethod:
[ | excludesElement: element |
  ^self select: [ ^self asSelf = ^my element asSelf ] . count = 0
] .

defineMethod:
[ | isEquivalentTo: object | 
  object isCollection && [ ^self count = object count ]
  ifTrue:
    [ !list1 <- ^self sortUp: [ asSelf ] . numberElements ;
      !list2 <- object sortUp: [ asSelf ] ;
      list1 select: [ ^my list2 at: position . asSelf != asSelf ] .
         count = 0 
    ] 
  ifFalse: [ FALSE ] 
] .

;


#--------------------
#  Intra_List computations
#--------------------

Prototypical Collection

#----------

defineMethod:
[ | numberElements | 
  ^self runningTotal: [ 1 ] .
  send: [ ^self super 
             basicExtend: [ !position <- ^my runningTotal asInteger ] 
        ]
] .

defineMethod:
[ | numberElementsInReverse |
  !cnt <- ^self count ;
  ^self numberElements
     do: [ :position <- (^my cnt - position + 1) asInteger ] 
] .

defineMethod:
[ | linkElements | 
  !linkedList <- ^self numberElements
     extendBy: [ !prior; !next ] ;
  !tmpList <- linkedList toList ;
  linkedList
  do: [ :prior <- ^my tmpList at: position -1 ;
        :next <- ^my tmpList at: position + 1 ;
      ]
] .

defineMethod:
[ | tileUp: aValueBlock usingCollector: aCollector tiles: aNumber |

###  Number of elements per bucket based on non-NA values
!size <- ^self select: [ ^self send: ^my aValueBlock . isntNA ] .
                    count / aNumber;

^self rankUp: aValueBlock
      usingCollector: [ |:rank|
                        ^self send: ^my aCollector
                              with: ( 1 + (rank - 1) / ^my size) asInteger
                      ]
] .

defineMethod:
[ | tileUp: aValueBlock using: variableName tiles: aNumber |
  !collector <- "| :" concat: variableName asString . concat: " | ^current" ;
  ^self tileUp: aValueBlock usingCollector: collector asBlock tiles: aNumber
] .

defineMethod: 
[ | tileUp: aBlock tiles: aNumber |
  ^self tileUp: aBlock usingCollector: [| :tile| ^current] tiles: aNumber
] .

#----------

defineMethod: 
[ | decileUp: aBlock |
  ^self tileUp: aBlock usingCollector: [ |:decile| ^current ] tiles: 10
] .

defineMethod:
[ | decileUp: aBlock using: variableName | 
  ^self tileUp: aBlock using: variableName tiles: 10 
] .

defineMethod: 
[ | quintileUp: aBlock |
  ^self tileUp: aBlock usingCollector: [ |:quintile| ^current ] tiles: 5
] .

defineMethod:
[ | quintileUp: aBlock using: variableName | 
  ^self tileUp: aBlock using: variableName tiles: 5
] .

defineMethod:
[ | percentileUp: aBlock | 
  ^self tileUp: aBlock usingCollector: [| :percentile| ^current] tiles: 100
] .

defineMethod:
[ | percentileUp: aBlock using: variableName | 
  ^self tileUp: aBlock using: variableName tiles: 100
] .

#----------

defineMethod:
[ | tileDown: aValueBlock usingCollector: aCollector tiles: aNumber |

###  Number of elements per bucket based on non-NA values
!size <- ^self select: [ ^self send: ^my aValueBlock . isntNA ] .
                                                       count / aNumber;
^self rankDown: aValueBlock
      usingCollector: [ |:rank|
                        ^self send: ^my aCollector
                              with: ( 1 + (rank - 1) / ^my size) asInteger
                      ]
] .

defineMethod:
[ | tileDown: aValueBlock using: variableName tiles: aNumber |

!collector <- "| :" concat: variableName asString . concat: " | ^current" ;
^self tileDown: aValueBlock usingCollector: collector asBlock tiles: aNumber
] .

defineMethod:
[ | tileDown: aBlock tiles: aNumber | 
  ^self tileDown: aBlock usingCollector: [| :tile| ^current] tiles: aNumber
] .

#----------

defineMethod:
[ | percentileDown: aBlock | 
  ^self tileDown: aBlock usingCollector: [| :percentile | ^current ] tiles: 100
] .

defineMethod:
[ | percentileDown: aBlock using: variableName | 
  ^self tileDown: aBlock using: variableName tiles: 100
] .

defineMethod: 
[ |decileDown: aBlock |
^self tileDown: aBlock usingCollector: [ |:decile| ^current ] tiles: 10
]  .

defineMethod:
[ | decileDown: aBlock using: variableName | 
  ^self tileDown: aBlock using: variableName tiles: 10
] .

defineMethod: 
[ |quintileDown: aBlock |
  ^self tileDown: aBlock usingCollector: [ |:quintile| ^current ] tiles: 5
] .

defineMethod:
[ | quintileDown: aBlock using: variableName | 
  ^self tileDown: aBlock using: variableName tiles: 5
] .

#----------

defineMethod:
[ | weightedQuintile: aBlock |

###  compute running totals for attribute
    !rtList <- 
       ^self sortUp: aBlock .  runningTotal: aBlock ;
###  find max running total ( = total) and divide by number of buckets
    !targetSize  <- rtList max: [ runningTotal ] . / 5 ;
###  return integer from 1 - 5 as quintile (NA for NA's)
    rtList extendBy:
       [
       !quintile <- ( (runningTotal - 1 ) / ^my targetSize + 1 ) asInteger
       ]
] .

defineMethod:
[ | weightedDecile: aBlock |

###  compute running totals for attribute
    !rtList <- 
       ^self sortUp: aBlock .  runningTotal: aBlock ;
###  find max running total ( = total) and divide by number of buckets
    !targetSize  <- rtList max: [ runningTotal ] . / 10;
###  return integer from 1 - 10 as decile (NA for NA's)
    rtList extendBy:
       [
       !decile <- ( (runningTotal - 1 ) / ^my targetSize + 1 ) asInteger
       ]
] .

#----------

defineMethod: 
[ | percentileDown: aBlock by: bBlock |
  !list <- ^self extendBy: [ !percentile ] ;
  list groupedBy: bBlock . 
   do: [ 
       groupList percentileDown: ^my aBlock . 
          do: [ ^self super :percentile <- percentile ;];
       ];
  list
] .

defineMethod: 
[ | percentileUp: aBlock by: bBlock |
  !list <- ^self extendBy: [ !percentile;  ] ;
  list groupedBy: bBlock . 
  do: [ 
      groupList percentileUp: ^my aBlock . 
         do: [ ^self super :percentile <- percentile ;];
      ];
  list
] .

defineMethod: 
[ | percentileDown: aBlock by: bBlock by: cBlock |
  !list <- ^self extendBy: [ !percentile;  ] ;
  list groupedBy: bBlock . 
  do: [ 
       groupList percentileDown: ^my aBlock by: ^my cBlock .
          do: [ ^self super :percentile <- percentile ; ];
      ];
  list
] .

defineMethod: 
[ | percentileUp: aBlock by: bBlock by: cBlock |
  !list <- ^self extendBy: [ !percentile;  ] ;
  list groupedBy: bBlock . 
  do: [ 
      groupList percentileUp: ^my aBlock by: ^my cBlock .
          do: [ ^self super :percentile <- percentile ; ];
      ];
  list
] .

defineMethod: 
[ | quintileDown: aBlock by: bBlock |
  !list <- ^self extendBy: [ !quintile;  ] ;
  list groupedBy: bBlock . 
  do: [ 
      groupList quintileDown: ^my aBlock . 
           do: [ ^self super :quintile <- quintile ;];
      ];
  list
] .

defineMethod: 
[ | quintileUp: aBlock by: bBlock |
  !list <- ^self extendBy: [ !quintile;  ] ;
  list groupedBy: bBlock . 
  do: [ 
      groupList quintileUp: ^my aBlock . 
          do: [ ^self super :quintile <- quintile ;];
      ];
  list
] .

defineMethod: 
[ | quintileDown: aBlock by: bBlock by: cBlock |
  !list <- ^self extendBy: [ !quintile;  ] ;
  list groupedBy: bBlock . 
  do: [ 
      groupList quintileDown: ^my aBlock by: ^my cBlock .
          do: [ ^self super :quintile <- quintile ; ];
      ];
  list
] .

defineMethod: 
[ | quintileUp: aBlock by: bBlock by: cBlock |
  !list <- ^self extendBy: [ !quintile;  ] ;
  list groupedBy: bBlock . 
  do: [ 
      groupList quintileUp: ^my aBlock by: ^my cBlock .
          do: [ ^self super :quintile <- quintile ; ];
      ];
  list
] .

#----------

defineMethod: 
[ | decileDown: aBlock by: bBlock |
  !list <- ^self extendBy: [ !decile;  ] ;
  list groupedBy: bBlock . 
  do: [ 
      groupList decileDown: ^my aBlock . 
           do: [ ^self super :decile <- decile ;];
      ];
  list
] .

defineMethod: 
[ | decileUp: aBlock by: bBlock |
  !list <- ^self extendBy: [ !decile;  ] ;
  list groupedBy: bBlock . 
  do: [ 
      groupList decileUp: ^my aBlock . 
          do: [ ^self super :decile <- decile ;];
      ];
  list
] .

defineMethod: 
[ | decileDown: aBlock by: bBlock by: cBlock |
  !list <- ^self extendBy: [ !decile;  ] ;
  list groupedBy: bBlock . 
  do: [ 
      groupList decileDown: ^my aBlock by: ^my cBlock .
          do: [ ^self super :decile <- decile ; ];
      ];
  list
] .

defineMethod: 
[ | decileUp: aBlock by: bBlock by: cBlock |
  !list <- ^self extendBy: [ !decile;  ] ;
  list groupedBy: bBlock . 
  do: [ 
      groupList decileUp: ^my aBlock by: ^my cBlock .
          do: [ ^self super :decile <- decile ; ];
      ];
  list
] .

defineMethod: [ | mavg: block observations: obs |

!multiple <- obs isCollection ; 
:obs <- multiple
   ifTrue: [ obs send: [ asNumber asInteger ] . 
               select: [ isInteger ] . select: [ ^self > 0 ] 
           ]
  ifFalse: [ obs > 0 ifTrue: [ obs asInteger ] else: 1 ] ;

#--  execute the supplied block (stored in value)
#--  and compute the running total
!data <- ^self collect: block ;
:data <- multiple 
   ifTrue: [ data extendBy: [ !mavgXRef <- ^global IndexedList new ] ]
  ifFalse: [ data extendBy: [ !mavg ] ] .
   runningTotal: [ value ] ;

!list <- data select: [ value isntNA ] . toList ;     #-- used for position access

#-- mavg is defined as the difference in runningTotals for
#-- the current element and element lag obs ; when the element
#-- is less than the number of observations to use, it is NA
#--    (or the average of the runningTotal to that point) ;
#--  NA's in the collection are skipped; 
#--  if multiple, store each moving average in mavgXRef

multiple
ifTrue:
  [ data select: [ value isntNA ] . numberElements
    do: [ !list <- ^my list ;
          ^my obs
          do: [ !prior <- ^my list at: (^my position - asSelf ) ;
                !base <- prior isntNA
                   ifTrue: [ prior runningTotal ] . else: [ 0 ] ;
                !mavg <- ^my position >= asSelf
                  ifTrue: [ (^my runningTotal - base) / asSelf ] ;
                ^my mavgXRef at: asSelf put: mavg ;
              ] ;
 
        ] ;
  ]
ifFalse:
  [ data numberElements  
    do: [ !prior <- ^my list at: (position - ^my obs );
          !base <- prior isntNA
             ifTrue: [ prior runningTotal ] . else: [ 0 ] ;
          :mavg <- position >= ^my obs
             ifTrue: [ (runningTotal - base) / ^my obs ] 
            ifFalse: [ NA ] ;      #- or [ runningTotal / position ] ;
        ] ;
  ] ;

data send: [ ^self super ]   #-- strip off runningTotal extension

] .
;

#--------------------
# Other Misc
#--------------------

Prototypical Collection

defineMethod: [ | isDefault | ^self count = 0 ] .

defineMethod:
[ | all: booleanBlock | 
  ^self select: booleanBlock . count = ^self count
] .

defineMethod:
[ | any: booleanBlock | 
  ^self select: booleanBlock . count > 0 
] .

defineMethod:
[ | iterate: block |      #- run list operation serially , in list order
  !counter <- 1 ;
  !list <- ^self toList ;
  [ counter <= ^self count ] 
  whileTrue:
     [ list at: counter . basicSend: block ;
       :counter <- (counter + 1 ) asInteger ;
     ] ;
  ^self
] .

;

#--------------------
#  display
#--------------------
Prototypical Collection 
defineMethod: [ | displayAcross: item withLabel: label andFormat: format |
!block <- item isBlock 
    ifTrue: [ item ] ifFalse: [ item asString asBlock ] ;
:format <- format else: 15.2 ;
label else: "" . print ; 
^self do: [ ^self basicSend: ^my block . printWithCommas: ^my format ] ;
newLine print ;
] ;

Prototypical Collection
 defineMethod: [ | display: items withFormat: formatList |
!blockList <- items numberElements
    extendBy: [ !block <- ^self isBlock 
                   ifTrue: [ ^self ] ifFalse: [ asString asBlock ] ;
                !format <- ^my formatList at: position . else: 15.2 ;
              ] ;
^self
 do: [ !element <- ^self ; 
       ^my blockList
         do: [ ^my element basicSend: asSelf . printWithCommas: format ] ;
        newLine print ;
     ] ;
] ;

