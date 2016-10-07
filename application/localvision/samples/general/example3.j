     #--  Define write: at Object to do standard print:
     Object
     defineMethod:
     [ | write: format | 
       ^self print: format
     ] ;
     
     #--  Define write: at Number to print with commas in format
     Number
     defineMethod:
     [ | write: format | 
       ^self printWithCommas: format
     ] ;
     
     #--  Define write: at NA to print with blanks
     NA
     defineMethod:
     [ | write: format | 
       " " print: format
     ] ;
     
     #-- Fix method to use write: with average and stdDev
     List defineMethod:
     [ | quintile: block1 by: block2 for: block3 |
     
        "Cross Tabular Quintile Distribution" center: 70 . printNL ;
        " " print: 30 ; "   For: " print ; block3 printNL ;
        newLine print ;
     
        block1 asString print: 10 ; "  |  " print ; 
        block2 asString concat: " Quintiles" . center: 45 . printNL ;
     
        "Quintiles" print: 10 ; "  |  " print ;
        5 sequence do: [ ^self print: 9 ] ;
        newLine print ; 
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
             newLine print ; 
     
             "Average" print: -10  ; "  |  " print ; 
             quintileList do: [ ^self average: [ block3Value ] . write: 9 ] ;
             newLine print ; 
     
             "Std Dev" print: -10  ; "  |  " print ; 
             quintileList do: [ ^self stdDev: [ block3Value ] . write: 9 ] ;
             newLine print ; 
             newLine print ; 
           ] ;
     ] ;
     
     ###  And Run It
     Company masterList
       quintile: [ sales ] by: [ price / earningsPerShare ] for: [ netIncome] 

