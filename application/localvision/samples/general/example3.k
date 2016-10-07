     List defineMethod:
     [ | quintile: block1 by: block2 for: block3 |
     
        "Cross Tabular Quintile Distribution" center: 70 . printNL ;
        " " print: 30 ; "   For: " print ; block3 printNL ;
        newLine print ;
     
        block1 asString print: 10 ; "  |  " print ; 
        block2 asString concat: " Quintiles" . center: 45 . printNL ;
     
        "Quintiles" print: 10 ; "  |  " print ;
        5 sequence do: [ ^self print: 9 ] ;
        "Totals" printNL: -9 ;                    #--  Add New Heading 
        "-" fill: 70 . printNL ;
     
       !quintileList <- ^self 
          quintileUp: block1 .
          extendBy: [ !quintile1 <- quintile ] .
          quintileUp: block2 .
          extendBy: [ !quintile2 <- quintile ;
                      !block3Value <- ^self send: ^my block3 ;
                    ] ;
     
       quintileList groupedBy: [ quintile1 ] . 
          sortUp: [ ^self ] .
       do: [ !currentQuintile1 <- ^self ;
             !quintileList <- 5 sequence
                 send: [ !q <- ^self ;
                         ^my groupList select: [ quintile2 = ^my q ] 
                       ] ;
             currentQuintile1 print: -3 ;
             "Count" print: 7  ; "  |  " print ; 
             quintileList do: [ ^self count print: 9 ] ;
             groupList count printNL ;            #--   Total count

             "Average" print: -10  ; "  |  " print ; 
             quintileList do: [ ^self average: [ block3Value ] . write: 9 ] ;
             groupList average: [ block3Value ] . printNL ;   #--- Ave 
     
             "Std Dev" print: -10  ; "  |  " print ; 
             quintileList do: [ ^self stdDev: [ block3Value ] . write: 9 ] ;
             groupList stdDev: [ block3Value ] . printNL ;     #-- StdDev
             newLine print ; 
           ] ;
     ] ;
     
     ###  And Run It
     Company masterList
       quintile: [ sales ] by: [ price / earningsPerShare ] for: [ netIncome] 


