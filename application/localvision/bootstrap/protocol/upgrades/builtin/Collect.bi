######################
####  Collection  ####
######################


# $File: //depot/vision/applications/core/builtin/mainline/Collect.bi $
# $DateTime: 2009/08/25 08:17:45 $
# $Revision: #12 $
# $Author: sglee $


#--------------------
#  Collection: groupedBy:in:else:
#    new method to capture all criteria not passed in second parameter
#--------------------
# from extendAll.700

Prototypical Collection

defineMethod: [| groupedBy: block in: list else: other | 
!ilist <- CoreWorkspace IndexedList new; 
list numberElements 
do: [ ^my ilist at: ^self asSelf 
               put: (^self asSelf 
                        extendBy: [!groupList <- CoreWorkspace List new; 
                                   !position <- ^my position; 
                                  ] 
                    ) ; 
    ] ;
!exList <- CoreWorkspace IndexedList new; 
^self groupedBy: block .
do: [^my ilist at: ^self asSelf . isntNA 
        ifTrue: [^my ilist at: ^self asSelf .:groupList <- groupList;]
       ifFalse: [^my exList at: ^self asSelf
                           put: (^self asSelf 
                                    extendBy: [!groupList <- ^my groupList; ]
	  		          );
                ] ;
    ] ;
:other <- other
   extendBy: [!groupList <- ^my exList collectListElementsFrom: [groupList] ];
ilist sortUp: [ position ] . , other
] ;

#--------------------
#  Collection: new statistic support
#--------------------
# from extendAll.700

Prototypical Collection

defineMethod: [ | countGoodOnes |
   ^self select: [ isntDefault ] . count
] .

defineMethod: [ | percentAvailable |
   (^self countGoodOnes / ^self count * 100) else: 0.0
] .

defineMethod: [ | countPositives |
   ^self select: [ ^self > 0 ] . count
] .

defineMethod: [ | countNegatives |
   ^self select: [ ^self < 0 ] . count
] .

defineMethod: [ | countZeros |
   ^self select: [ ^self = 0 ] . count
] .

defineMethod: [ | weightedAverage |
  ^self average: [ ^self ] withWeights: [ weight ]
] .

defineMethod: [ | weightedHarmonicMean |
  ^self harmonicMean: [ ^self ] withWeights: [ weight ]
] .

#defineMethod: [ | median: aBlock withWeights: bBlock |
#!list <- ^self 
#  extendBy: [!val1 <- ^self send: ^my aBlock; 
#             !val2 <- ^self send: ^my bBlock; 
#            ] .
#    select: [ val1 isntNA && val2 isntNA ] .
#    sortUp: [ val1 ] ;
#!total <- list total: [ val2 ] ;
#:list <- list runningTotal: [ val2 / ^my total ] .
#    select: [ runningTotal > .5 ] ;
#list count > 0 
#   ifTrue: [ list at: 1 . val1 ] 
#] .

defineMethod: [ | weightedMedian |
  ^self median: [ ^self ] withWeights: [ weight ]
] .

defineMethod: [ | total: aBlock adjustedBy: bBlock |
!list <- ^self 
  extendBy: [!val1 <- ^self send: ^my aBlock; 
             !val2 <- ^self send: ^my bBlock; 
             !val <- val1 isNumber && val2 isNumber 
                ifTrue: [val1 * val2]; 
            ].
     select: [val isntNA]; 
list total: [ val ] 
] .

defineMethod: [ | adjustedTotal |
  ^self total: [ ^self ] adjustedBy: [ adjustFactor ] 
] .

defineMethod: [ | totalNum: aBlock toTotalDenom: bBlock |
!list <- ^self 
  extendBy: [!val1 <- ^self send: ^my aBlock; 
             !val2 <- ^self send: ^my bBlock; 
            ].
     select: [val1 isntNA && val2 isntNA ]; 
(list total: [ val1 ] ) / (list total: [ val2 ] )
] .

defineMethod: [ | sumNumToSumDen |
  ^self totalNum: [ numerator ] toTotalDenom: [ denominator ] 
] .

;

####################
#  New and streamlined statistic calculation  (should be core change)
####################
Collection defineMethod: [ | harmonicMean: aBlock | 
  !list <- ^self send: aBlock . select: [ isntNA ] .
        send: [ 1 / ^self ] ;
  1 / list average
] ;

Collection defineMethod: [| harmonicMean: aBlock withWeights: bBlock | 
  !list <- ^self 
    extendBy: 
      [ !val2 <- ^self send: ^my bBlock; 
        !val <-  val2 / (^self send: ^my aBlock) ;
      ] . select: [ val isNumber ] ;
  (list basicTotal: [val2]) / (list basicTotal: [val])
] ;


Collection defineMethod: [ | weightedAverageNoNegs | 
  ^self select: [ ^self >= 0 ] .
       average: [^self] withWeights: [weight]
] ;

Collection defineMethod: [ | averageNoNegs | 
  ^self select: [ ^self >= 0 ] .
       average: [^self] 
] ;

Collection defineMethod: [ | selectWithinStdDevOf: n | 
  :n <- n asInteger else: 2 ;
  !mean <- ^self average ;
  !std2 <- ^self stdDev * n ;
  !lower <- mean - std2 ;
  !upper <- mean + std2 ;
  ^self select: [ ^self between: ^my lower and: ^my upper ]
] ;

Collection defineMethod: [ | weightedAverage2Std | 
  ^self selectWithinStdDevOf: 2 . 
       average: [^self] withWeights: [weight]
] ;

Collection defineMethod: [ | weightedHAverage2Std | 
  ^self selectWithinStdDevOf: 2 . 
       harmonicMean: [^self] withWeights: [weight]
] ;

Collection defineMethod: [ | weightedAverage2StdNoNegs | 
  ^self select: [ ^self >= 0 ] .
      selectWithinStdDevOf: 2 . 
       average: [^self] withWeights: [weight]
] ;

Collection defineMethod: [ | average2Std | 
  ^self selectWithinStdDevOf: 2 . 
       average: [^self] 
] ;

Collection defineMethod: [ | average2StdNoNegs | 
  ^self select: [ ^self >= 0 ] .
      selectWithinStdDevOf: 2 . 
       average: [^self] 
] ;

Collection defineMethod: [ | pctPositives |
  ^self select: [ ^self > 0 ] . count / ^self count * 100
] ;

Collection defineMethod: [ | pctNegatives |
  ^self select: [ ^self < 0 ] . count / ^self count * 100
] ;

Collection defineMethod: [ | pctZeros |
  ^self select: [ ^self = 0 ] . count / ^self count * 100
] ;

Collection defineMethod: [ | weightedAverageInterQuartile |
  ^self select: [ ^self between: lower and: upper ] . weightedAverage
] ;

Collection defineMethod: [ | interquartileRange |
!tiles <- ^self select: [ isntNA ] .
    tileUp: [ ^self ] using: "quartile" tiles: 4 ;
!max1 <- tiles select: [ quartile = 1 ] . max ;
!min4 <- tiles select: [ quartile = 4 ] . min ;
!factor <- (min4 - max1) * 4 ;
(max1 - factor), (min4 + factor)
] ;

###
# >> 4/3/07 tlh from core/builtins/patch_119.vis
###



#--------------------
#  add rank up/down low/high/mean variations  (from OA)
#--------------------

Collection defineMethod: [ | rankUpLow: aBlock |
  !list <- ^self collect: aBlock . 
      extendBy: [ !rank ] ;
  list select: [ value isntNA ] . groupedBy: [ value ] .
    sortUp: [ ^self ] .
    runningTotal: [ groupList count ] .
    linkElements
  do: [ !adjustedRank <- 
          (prior isntNA 
            ifTrue: [ prior runningTotal ] else: [ 0 ] ) asInteger increment ;
        groupList do: [ :rank <- ^my adjustedRank ] ;
      ] ;
  list
] ;
      
Collection defineMethod: [ | rankDownLow: aBlock |
  !list <- ^self collect: aBlock . 
      extendBy: [ !rank ] ;
  list select: [ value isntNA ] . groupedBy: [ value ] . 
    sortDown: [ ^self ] .
    runningTotal: [ groupList count ] .
    linkElements
  do: [ !adjustedRank <- 
          (prior isntNA 
            ifTrue: [ prior runningTotal ] else: [ 0 ] ) asInteger increment ;
        groupList do: [ :rank <- ^my adjustedRank ] ;
      ] ;
  list
] ;
      
#----------

Collection defineMethod: [ | rankUpHigh: aBlock |
  !list <- ^self collect: aBlock . 
      extendBy: [ !rank ] ;
  list select: [ value isntNA ] . groupedBy: [ value ] .
    sortUp: [ ^self ] .
    runningTotal: [ groupList count ] .
    linkElements
  do: [ !adjustedRank <- runningTotal asInteger ;
        groupList do: [ :rank <- ^my adjustedRank ] ;
      ] ;
  list
] ;
      
Collection defineMethod: [ | rankDownHigh: aBlock |
  !list <- ^self collect: aBlock . 
      extendBy: [ !rank ] ;
  list select: [ value isntNA ] . groupedBy: [ value ] . 
    sortDown: [ ^self ] .
    runningTotal: [ groupList count ] .
    linkElements
  do: [ !adjustedRank <- runningTotal asInteger ;
        groupList do: [ :rank <- ^my adjustedRank ] ;
      ] ;
  list
] ;
    
#----------

Collection defineMethod: [ | rankUpMean:  aBlock |
  !list <- ^self collect: aBlock . 
      extendBy: [ !rank ] ;
  list select: [ value isntNA ] . groupedBy: [ value ] .
    sortUp: [ ^self ] .
    runningTotal: [ groupList count ] .
    linkElements
  do: [ !adjustedRank <- runningTotal ;
        groupList count > 1
        ifTrue:
          [ :adjustedRank <-  prior isntNA
                ifTrue: [ prior runningTotal + runningTotal ] 
               ifFalse: [ runningTotal ] . / 2 + .5 ;
          ] ;
        adjustedRank asInteger = adjustedRank
             ifTrue: [ :adjustedRank <- adjustedRank asInteger ] ;
        groupList do: [ :rank <- ^my adjustedRank ] ;
      ] ;
  list
] ;
      
Collection defineMethod: [ | rankDownMean:  aBlock |
  !list <- ^self collect: aBlock . 
      extendBy: [ !rank ] ;
  list select: [ value isntNA ] . groupedBy: [ value ] .
    sortDown: [ ^self ] .
    runningTotal: [ groupList count ] .
    linkElements
  do: [ !adjustedRank <- runningTotal ;
        groupList count > 1
        ifTrue:
          [ :adjustedRank <-  prior isntNA
                ifTrue: [ prior runningTotal + runningTotal ] 
               ifFalse: [ runningTotal ] . / 2 + .5 ;
          ] ;
        adjustedRank asInteger = adjustedRank
             ifTrue: [ :adjustedRank <- adjustedRank asInteger ] ;
        groupList do: [ :rank <- ^my adjustedRank ] ;
      ] ;
  list
] ;
      
#--------------------
#  Collection
#     rankCorrelate: with:
#  Use the rankUpMean: method to preserve values for ties thereby
#    producing stable results for the correlation; otherwise, the
#    rank correlation could produce different values for the same
#    data sets in different orders
#
#--------------------

Collection defineMethod: [| rankCorrelate: blockX with: blockY| 
^self 
       rankUpMean: blockX . extendBy: [ !rankX <- rank ] .
       rankUpMean: blockY . extendBy: [ !rankY <- rank ] .
    correlate: [rankX] with: [rankY]
] ;


#--------------------
#  Tiling methods - add low,high, and mean variations to standard tiling
# removed these with patch_122.vis, the methods were incomplete and would 
# give an SNF if used.
#--------------------

#patch_122.vis removed {decile,quintile,percentile}{up,down}{high,low,mean}
#              and removed tile{up,down}{low,high,mean}:usingCollector:tiles

###
# >> 4/3/07 tlh from core/builtins/patch_120.vis
###
#--------------------
#  statistical
#--------------------

Prototypical Collection

# stdDevSample assumes that the collection is a sample of the population.
# If it represents the entire population, then compute the standard deviation using stdDev.
# For large sample sizes, stdDevSample and stdDev return approximately equal values. 
# stdDevSample calculates the standard deviation using the "n-1" method.
# stdDev calculates the standard deviation using the "n" method. 

defineMethod:
[ | stdDevSample: block |
  !list <- ^self send: [ ^self basicSend: ^my block ] . select: [ isNumber ] ;
  !mean <- list average ;
  (list total: [ (^self - ^my mean ) toThe: 2 ] . / list count decrement ) sqrt
] .

defineMethod: [ | stdDevSample | ^self stdDevSample: [ ^self asSelf ] ] .

#----------

#- Fix a bug when list of lists is presented - modify to use basicSend to eliminate snf's
defineMethod: [ | median: aBlock |
   !list <- ^self extendBy: [ !value <- ^self basicSend: ^my aBlock ] .
      select: [value isntNA ] . toList
      sortUp: [value] ;
   !mid <- ((list count + 1) / 2) asInteger ;
   !midValue <- list at: mid . value ;
  list count mod: 2 . = 1 || [ midValue isNumber not ]
      ifTrue: [ midValue ]
     ifFalse: [ midValue / 2 + (list at: (mid + 1) .value) / 2 ]
] .

#----------

#- introduced in patch.core.613d
defineMethod: [ | median: aBlock withWeights: bBlock |
!list <- ^self
  extendBy: [ !val1 <- ^self basicSend: ^my aBlock ;
              !val2 <- ^self basicSend: ^my bBlock ;
            ] .
    select: [ val1 isntNA && val2 isntNA ] .
    sortUp: [ val1 ] ;
!total <- list total: [ val2 ] ;
:list <- list runningTotal: [ val2 / ^my total ] .
    select: [ runningTotal > 0.5 ] ;
list count > 0
   ifTrue: [ list at: 1 .val1 ]
] .

;

###
# >> 4/3/07 tlh core/builtin/patch_121.vis
###
#--------------------
#  Collection groupedByIList: block
#--------------------

Collection defineMethod: [ | groupedByIList: block |
^self groupedBy: block . indexBy: [ ^self super ] 
] ;

#--------------------
#  Collection groupedBy: block withIndex: iblock
#--------------------
Collection defineMethod: [ | groupedBy: block withIndex: iblock |
^self groupedBy: block .
do: [ 
      :groupList <- groupList indexBy: ^my iblock
    ] 
] ;

#--------------------
#  Collection flattenTreeBy: block
#  Collection flattenTreeBy: block leafCondition: leafBlock
#--------------------

Collection defineMethod: [|flattenTreeBy: block |
  !list <- ^self;
  list collectListElementsFrom: [
    !nextList <- ^self send: ^my block;
    nextList count = 0 ifTrue: [
      ^self
    ]
    ifFalse: [ nextList flattenTreeBy: ^my block ]
  ]
];

#----------

Collection defineMethod: [|flattenTreeBy: block leafCondition: leafBlock |
  !list <- ^self;
  list collectListElementsFrom: [
    ^self basicSend: ^my leafBlock . 
        ifTrue: [ ^self ] 
        else: 
          [ !nextList <- ^self send: ^my block;
            nextList flattenTreeBy: ^my block leafCondition: ^my leafBlock
          ]
     ]
];

#--------------------
#  Collection 
#   . New method 'regressWithX1: list2 andX2: list3'
#   . New method 'secdr'
#   . New method 'rate'
#  From core/builtin/patch_123.vis 12/12/2008
#--------------------


#----------
#  regressWithX1: list andX2: list
#
#  Returns an extended object which responds to the beta, alpha,
#  pearson, rsq, and stdErr messages that are computed using a standard
#  linear regression.  The recipient object is the collection
#  of dependent data points and the arguments are collections of
#  independent data points; collections should contain numeric
#  data values and should contain the same number of data points,
#  otherwise NA values are returned.
#
#----------

Prototypical Collection
defineMethod: [ | regressWithX1: list2 andX2: list3 |
  !regressData <- CoreWorkspace Object
     extendBy: [ !beta1 ; !beta2 ; !alpha ; !pearson ; !rsq ; !stdErr ] ;
  !seriesY <- ^self select: [ isNumber ] . toList ;
  !seriesX1 <- list2 select: [ isNumber ] . toList ;
  !seriesX2 <- list3 select: [ isNumber ] . toList ;
  seriesY count = seriesX1 count &&
  seriesY count = ^self count &&
  seriesX1 count = list2 count &&
  seriesX2 count = list3 count
  ifTrue:
  [
    !num <- seriesY count ;
    !aveY <- seriesY average ;
    !aveX1 <- seriesX1 average ;
    !aveX2 <- seriesX2 average ;
    # variance
    !varY <- seriesY total: [ (^self - ^my aveY) * ^self ] ;
    !varX1 <- seriesX1 total: [ (^self - ^my aveX1) * ^self ] ;
    !varX2 <- seriesX2 total: [ (^self - ^my aveX2) * ^self ] ;
    # covariance
    !covX1X2 <- num sequence
      total: [ ((^my seriesX1 at: ^self) - ^my aveX1) * (^my seriesX2 at: ^self) ] ;
    !covYX1 <- num sequence
      total: [ ((^my seriesY at: ^self) - ^my aveY) * (^my seriesX1 at: ^self) ] ;
    !covYX2 <- num sequence
      total: [ ((^my seriesY at: ^self) - ^my aveY) * (^my seriesX2 at: ^self) ] ;
    #- coefficients
    !beta1 <- (varX2 * covYX1 - covX1X2 * covYX2) / ( varX2 * varX1 - (covX1X2 toThe: 2)) ;
    !beta2 <-  (varX1 * covYX2 - covX1X2 * covYX1) / ( varX2 * varX1 - (covX1X2 toThe: 2)) ;
    !alpha <- aveY - beta1 * aveX1 - beta2 * aveX2 ;
    #- fitted values
    !fittedY <- num sequence
      send: [ ^my alpha + ^my beta1 * (^my seriesX1 at: ^self) + ^my beta2 * (^my seriesX2 at: ^self) ] ;
    !aveYfit <- fittedY average ;
    !varYfit <- fittedY total: [ (^self - ^my aveYfit) * ^self ] ;
    !covYYfit <- num sequence
      total: [ ((^my seriesY at: ^self) - ^my aveY) * (^my fittedY at: ^self) ] ;
    # statistics
    !ssTotal <- seriesY total: [ (^self - ^my aveY) toThe: 2 ] ;
    !ssRegression <- fittedY total: [ (^self - ^my aveY) toThe: 2 ] ;
    !ssResidual <- num sequence
      total: [ ((^my seriesY at: ^self) - (^my fittedY at: ^self)) toThe: 2 ] ;
    !pearson <- covYYfit / (varY * varYfit) sqrt ;
    !stdErr <- ( ssResidual / (num - 3) ) sqrt ;
    # return results
    regressData :beta1 <- beta1 ;
    regressData :beta2 <- beta2 ;
    regressData :alpha <- alpha ;
    regressData :pearson <- pearson ;
    regressData :rsq <- pearson * pearson ;
    regressData :stdErr <- stdErr ;
  ] ;
  regressData
] ;

#----------
#  secdr
#----------

Prototypical Collection
defineMethod: [ | secdr |

!vals <- ^self send: [ log ] ;
!n <- ^self count ;
!x1 <- n sequence ;
!x2 <- x1 send: [ ^self toThe: 2 ] ;
!regress <- vals regressWithX1: x1 andX2: x2 ;
(100 * (2 * regress beta2) exp - 100)
] ;

Prototypical Collection
defineMethod: [ | rate |
!vals <- ^self send: [ log ] ;
100 * (vals regress: vals count sequence . beta exp) -100
] ;



#--------------------
#  Collection 
#   . New method 'rangeTileUp: block tiles: tileBy'
#  From core/builtin/patch_124.vis 12/15/2008
#--------------------

Prototypical Collection

defineMethod: [ | rangeTileUp: block tiles: tileBy |

!univ <- ^self
  extendBy: [ !factor <- ^self send: ^my block ;
              !tile ;
            ] ;
!min <- univ min: [factor] ;
!max <- univ max: [factor] ;
!incr <- (max - min) / tileBy ;

univ select: [ factor isntDefault ] .
     do: [
          !bucket <- (factor - ^my min) / ^my incr else: 0 ;
          bucket > (bucket asInteger)
       ##- If value has a decimal, put it in bucket above it
       ##-  (i.e. a value of 2.3 represents an item in 3rd bucket)
             ifTrue: [:tile <- (bucket + 1) asInteger ]
            ifFalse: [:tile <- bucket asInteger ] ;
            bucket = 0
              ifTrue: [:tile <- 1 ; ] ;  ##- Put into first bucket if value < 1
            bucket > ^my tileBy
              ifTrue: [:tile <- ^my tileBy ; ] ;   ##- Put into top bucket if value greater than total # of tiles
           ] ;

univ

] .
;


#--------------------
#  Collection groupedHierarchyBy: blocks
#  patch_125.vis
#--------------------

Collection defineMethod: [ | groupedHierarchyBy: blocks |
blocks isCollection ifFalse: [ :blocks <- blocks asList ] ;
!criteria <- blocks count sequence
    send: [ [ groups at: ^my ] ] . numberElements ;
!sort <- blocks count sequence
    send: [ [ keyList at: ^my . else: "" . asString ] ] ;
!data <- ^self
  extendBy: [ !groups <- ^my blocks send: [ ^my send: ^self ] ] ;
!groups <- criteria
   collectListElementsFrom:
       [ !keyCount <- ^my blocks count ;
         !criteria <- ^my criteria first: position ;
         ^my data groupedByCriteria: criteria .
            extendBy: [ !keys <- keyList ;
                        ^my keyCount decrement: keys count .
                           sequence do: [ ^my keys , "" ] ;
                      ] 
       ] ;
groups , (groups at: 1 . asSelf clusterNew 
            extendBy: [ !keys <- ^my blocks count sequence send: [ "" ] ;
                        !groupList <- ^my data ;
                        !keyList <- keys ;
                      ]
         ) ;
groups
   sortUpByCriteria: sort .
] ;


#--------------------
#  Collection collectSortedListelementsFrom: aBlock
#  patch_130.vis
#--------------------

Collection defineMethod: [ | collectSortedListElementsFrom: aBlock |
  ^self send: [
      !subList <- ^self send: ^my aBlock;
      subList isCollection ifTrue: [ subList ] else: [ subList asList ]
    ] .
    runningTotal: [ ^self count ] .
    do: [ :runningTotal <- runningTotal - ^self count ] .
    collectListElementsFrom: [
      # Calculate position in final list.
      ^self numberElements do: [ :position <- ^my runningTotal + position; ]
    ] .
    sortUp: [ position ] .
    send: [ ^self super ]
];




