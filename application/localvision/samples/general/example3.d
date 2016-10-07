     #---  Create variables for the 2 blocks and the list 
     !block1 <- [ netIncome ] ;
     !block2 <- [ earningsPerShare ] ;
     !list <- testList ;
     
     #---  Use these variables to build quintileList
     !quintileList <- list 
        quintileUp: block1 .
        extendBy: [ !quintile1 <- quintile ] .
        quintileUp: block2 .
        extendBy: [ !quintile2 <- quintile ] ;
     
     quintileList groupedBy: [ quintile1 ] . 
        sortUp: [ ^self ] .
     do: [ ^my block1 print ;          #-- print the block as a label
           " Quintile " print ; 
           ^self print: 3 ;
           newLine print ; 
           !block2 <- ^my block2 ;     #-- copy value for next level 
           5 sequence 
           do: [ !q <- ^self ; 
                 ^my block2 print ;    #-- print the block as a label
                 " Quintile " print ; 
                 q print: 3 ;
                 " Includes: " print ;
                 ^my groupList
                    select: [ quintile2 = ^my q ] . count print: 5 ;
                 " Elements" printNL ; 
               ] ;
           newLine print ; 
         ] ;



