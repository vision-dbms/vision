
     quintileList groupedBy: [ quintile1 ] . 
        sortUp: [ ^self ] .
     do: [ "netIncome Quintile " print ; 
           ^self print: 3 ;
           newLine print ; 

           5 sequence            #---  For each number from 1 to 5
           do: [ !q <- ^self ; 
                 "   earningsPerShare Quintile " print ; 
                 q print: 3 ;
                 " Includes: " print ;
                 ^my groupList   #-- select companies in this quintile 
                    select: [ quintile2 = ^my q ] . count print: 5 ;
                 " Elements" printNL ; 
               ] ;
           newLine print ; 
         ] ;

