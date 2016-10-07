
     List defineMethod:
     [ | quintile: block1 by: block2 |
     
     #---  Titles
        "Cross Tabular Quintile Distribution" center: 70 . printNL ;
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
          extendBy: [ !quintile2 <- quintile ] ;
     
     #---  Analyze
       quintileList groupedBy: [ quintile1 ] . 
          sortUp: [ ^self ] .
       do: [ !currentQuintile1 <- ^self ;
             currentQuintile1 print: -10 ;
             "  |  " print ; 
             5 sequence
             do: [ !q <- ^self ;
                   ^my groupList 
                      select: [ quintile2 = ^my q ] . count print: 9 ;
                 ] ;
             newLine print ; 
           ] ;
     ] ;
     
     ###  And Run It
     Company masterList
       quintile: [ sales ] by: [ price / earningsPerShare ] 


