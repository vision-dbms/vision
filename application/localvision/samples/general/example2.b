     "***  Multiple Company Data Sheet  ***" center: 70 . printNL ;
     newLine print ; 
     
     "Ticker" print: 10 ;
     "Name" print: 30 ;
     "Industry" print: 10 ; 
     "Sales" print: -10 ;
     "P-E" print: -8 ;
     newLine print ; 
     "-" fill: 70 . printNL ;
     
     testList
     do: [ 
         ticker print: 10 ; 
         name print: 30 ;
         industry code print: 10 ;
         sales print: 10 ;
         price / earningsPerShare print: 8 ;
         newLine print ; 
         ] ;



