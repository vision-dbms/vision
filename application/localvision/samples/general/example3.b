     #---  group by netIncome quintiles (quintile1)
     quintileList groupedBy: [ quintile1 ] . 
        sortUp: [ ^self ] .
     do: [                  #--  for each net income quintile
          "netIncome Quintile " print ; 
           ^self print: 3 ; #-- print the quintile1 number
           newLine print ; 
           #---  group the current quintile1 companies by quintile2
           groupList groupedBy: [ quintile2 ] .
             sortUp: [ ^self ] .
           do: [ "   earningsPerShare Quintile " print ; 
                 ^self print: 3 ;             #-- print quintile2 number 
                 " Includes: " print ; 
                 groupList count print: 5 ;   #-- print quintile2 count 
                 " Elements" printNL ; 
               ] ;
           newLine print ; 
         ] ;



