     "***  Multiple Company Data Sheet  ***" center: 70 . printNL ;
     newLine print ; 
     "Ticker" print: 10 ;
     "Name" print: 30 ;
     "% of Ind" print: -10 ; 
     "Sales" print: -10 ;
     "P-E" print: -8 ;
     newLine print ; 
     "-" fill: 70 . printNL ;
     
     testList groupedBy: [ industry ] .
     do: [ !industrySales <- groupList total: [ sales ] ;
           !industryPE <- groupList average: [ price / earningsPerShare ] ;
           code print: 10 ;
           name print: 30 ; 
           newLine print ; 
           groupList
           do: [ ticker print: 10 ; 
                 name print: 30 ;
                 sales / ^my industrySales * 100 print: 10 ;
                 sales printWithCommas: 10.0 ;
                 price / earningsPerShare print: 8 ;
                 newLine print ; 
               ] ;

           "--> Industry Totals: " print: 40 ;        #-- General label 
           100.00 print: 10 ;                         #-- Industry %
           industrySales printWithCommas: 10.0 ;      #-- Industry Sales 
           industryPE print: 8 ;                      #-- Industry PE
           newLine print ; 
           newLine print ; 
    ] ;



