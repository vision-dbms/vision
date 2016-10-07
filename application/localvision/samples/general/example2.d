     "***  Multiple Company Data Sheet  ***" center: 70 . printNL ;
     newLine print ; 
     "Ticker" print: 10 ;
     "Name" print: 30 ;
     "Industry" print: 10 ; 
     "Sales" print: -10 ;
     "P-E" print: -8 ;
     newLine print ; 
     "-" fill: 70 . printNL ;
     
     testList groupedBy: [ industry ] .
     do: [ code print: 10 ;
           name print: 30 ; 
           " " print: 10 ;                     
           groupList total: [ sales ] . print: 10 ;    #-- total sales 
           groupList                                   #-- average pe
               average: [ price / earningsPerShare ] . print: 8 ;
           newLine print ; 
           groupList
           do: [ ticker print: 10 ; 
                 name print: 30 ;
                 industry code print: 10 ;
                 sales print: 10 ;
                 price / earningsPerShare print: 8 ;
                 newLine print ; 
               ] ;
           newLine print ; 
         ] ;
     


