####################
#  DataItem support
####################

# $File: //depot/vision/applications/core/builtin/mainline/DataItem.bi $
# $DateTime: 2007/05/21 19:59:58 $
# $Revision: #4 $
# $Author: dohara $

#--------------------
#  New variation on DataItem code
#--------------------

# at the statSet list, 
#added shortName= attribute.  
#This allows the report the option of displaying shortName of "WAvg" instead of the entire code "WeightedAverage" 
#This is the style we use for PortfolioHistory and PortfolioCOmpare

DataItem defineMethod: [ | describeYourself | 
!atts <- "id", code;
isStat || [ statSet isntDefault ] ifTrue: [ atts , "isStat", "Yes" ] ;
isGroup ifTrue: [ atts, "isGroup", "Yes" ] ;
FormatTools openTag: "DataItem" withAttributes: atts. printNL ;
!format <- FormatTools getNextIndentLevel ;
format buildTag: "NAME" 
   withAttributes: NA andContent: ^self name . printNL ;
format buildTag: "SHORT_NAME" 
   withAttributes: NA andContent: ^self shortName . printNL ;
format buildTag: "SORT_CODE"
   withAttributes: NA andContent: ^self sortCode . printNL ;
format buildTag: "BASE_CLASS"
   withAttributes: NA andContent: ^self baseClass whatAmI . printNL ;
format buildTag: "ACCESS_PATH"
   withAttributes: NA andContent: ^self accessPath . printNL ;
format buildTag: "WIDTH"
   withAttributes: NA andContent: ^self defaultWidth . printNL ;
format buildTag: "DECIMALS"
   withAttributes: NA andContent: ^self defaultDecimals . printNL ;
statSet isntDefault
ifTrue:
  [ !atts <- "count", (statSet isList ifTrue: [ statSet count ] ifFalse: [1]);
    format startCollection: "STATS" withAttributes: atts . printNL ;
   !nextLevel <- format getNextIndentLevel ;
   statSet
   do: [ ^my nextLevel buildTag: "STAT" 
             withAttributes: "id", code, "shortName",shortName andContent: NA . printNL ;
       ] ;      
  format endCollection: "STATS" . printNL ;
  ] ;
FormatTools closeTag: "DataItem" . printNL ;
] ;

DataItem defineMethod: [ | getOrCreateItemsFor: ids from: class | 
!list <- 
  ids isCollection 
  ifTrue: 
    [ ids send: [^my locateId: ^self . else: [ asString ] ] 
    ] .
  elseIf: [ids isUniverse && [ids entityType isDataItem] ]
    then: [ids list sortUp: [sortCode] ] .
  elseIf: [ids isString]
  then: [ !universe <- CoreWorkspace Universe locateId: ids; 
          universe isntDefault 
              ifTrue: [universe list sortUp: [sortCode] ].
                else: [ids breakOn: "," .
                         send: [ ^my locateId: ^self . else: [ ^self ] ] .
                         select: [ isntDefault ] 
                      ] 
        ] ; 
:list <- 
  list isntNA 
  ifTrue: 
    [ list 
        select: [isntDefault] .
      extendBy: 
        [ !fullAccessPath <- asString ;
          !id <- asString ; !label <- asString ;
          isDataItem 
             ifTrue: [ :fullAccessPath <- ^self getAccessFrom: ^my class ;
                       :id <- code ; :label <- name ;
                     ] ;
          !block <- fullAccessPath isntNA 
             ifTrue: [fullAccessPath asBlock]; 
        ] .
      select: [block isBlock].
    ] .
  elseIf: [ids isDefault]
    then: [^self getValidStatsFor: class] ;
list
] ;

PropertySetup updateFromString: "property | classId | dataType | 
cap | DataItem | Double | 
" ;
