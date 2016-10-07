     "***  Multiple Company Data Sheet  ***" center: 70 . printNL ;
     newLine print ; 
     "Ticker" print: 10 ;
     "Name" print: 30 ;
     "% of Ind" print: -10 ;    #-- change this heading
     "Sales" print: -10 ;
     "P-E" print: -8 ;
     newLine print ; 
     "-" fill: 70 . printNL ;
     
     testList groupedBy: [ industry ] .
     do: [ 
     #-- save the summary stats as local variables
         !industrySales <- groupList total: [ sales ] ;
         !industryPE <- groupList average: [ price / earningsPerShare ] ;

         code print: 10 ;
         name print: 30 ; 
         " " print: 10 ; 
         industrySales print: 10 ;    #-- print the stored value
         industryPE print: 8 ;        #-- print the stored value
         newLine print ; 
         groupList
         do: [ ticker print: 10 ; 
               name print: 30 ;
         #-- compute the percent contribution
               sales / ^my industrySales * 100 print: 10 ;
               sales print: 10 ;
               price / earningsPerShare print: 8 ;
               newLine print ; 
             ] ;
         newLine print ; 
         ] ;



