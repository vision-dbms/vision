     List defineMethod:
     [ | quintile: block1 by: block2 for: block3 |
     
     #---  Titles
        "Cross Tabular Quintile Distribution" center: 70 . printNL ;
        " " print: 30 ; "   For: " print ; block3 printNL ;
        newLine print ;
     
        block1 asString print: 10 ; "  |  " print ; 
        block2 asString concat: " Quintiles" . center: 45 . printNL ;
     
        "Quintiles" print: 10 ; "  |  " print ;
        5 sequence do: [ ^self print: 9 ] ;
        newLine print ; 
        "-" fill: 70 . printNL ;

     #---  Setup
       !quintileList <- ^self 
          quintileUp: block1 .
          extendBy: [ !quintile1 <- quintile ] .
          quintileUp: block2 .
          extendBy: [ !quintile2 <- quintile ;
                      !block3Value <- ^self send: ^my block3 ;
                    ] ;
     
     #---  Analyze
       quintileList groupedBy: [ quintile1 ] . 
          sortUp: [ ^self ] .
       do: [ !currentQuintile1 <- ^self ;
             #-- For each of the 5 quintiles, extract the sublist
             #-- send returns the last statement in block. 
             #-- quintileList is therefore a list of 5 lists
             !quintileList <- 5 sequence
                 send: [ !q <- ^self ;
                         ^my groupList select: [ quintile2 = ^my q ] 
                       ] ;

             #-- Now just use quintileList to display stats
             currentQuintile1 print: -3 ;
             "Count" print: 7  ; "  |  " print ; 
             quintileList do: [ ^self count print: 9 ] ;
             newLine print ; 
     
             "Average" print: -10  ; "  |  " print ; 
             quintileList do: [ ^self average: [ block3Value ] . print: 9 ] ;
             newLine print ; 
     
             "Std Dev" print: -10  ; "  |  " print ; 
             quintileList do: [ ^self stdDev: [ block3Value ] . print: 9 ] ;
             newLine print ; 
             newLine print ; 
           ] ;
     ] ;
     
     ###  And Run It
     Company masterList
       quintile: [ sales ] 
             by: [ price / earningsPerShare ] 
            for: [ netIncome] 



