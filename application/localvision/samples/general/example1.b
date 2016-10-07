

     Named Company GM
     do: [ 
         !dr <- 90 to: 88 by: 1 yearEnds ;
         name center: 45 . printNL ;                      #-- center the name
         "Financial Summary" center: 45 . printNL ;       #-- center report title
         newLine print ; 
     
         " " print: 15 ; 
         dr evaluate: [ ^date year print: 10 ; ] ;
         newLine print ; 
         " " print: 15 ;
         dr evaluate: [ " " print ; "-" fill: 9 . print ] ;  #-- underline
         newLine print ; 
     
         "Sales" print: 15 ;
         dr evaluate: [ sales print: 10 ] ;
         newLine print ;
     
         "EPS" print: 15 ;
         dr evaluate: [ earningsPerShare print: 10 ] ;
         newLine print ;
     
         "Price" print: 15 ;
         dr evaluate: [ price print: 10 ] ;
         newLine print ;
         ] ;



