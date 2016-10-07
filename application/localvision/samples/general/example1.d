

     Company defineMethod:                          #--  this line is new
     [ | financialAnalysis |                        #--  name the method
         !dr <- 90 to: 88 by: 1 yearEnds ;
         name center: 45 . printNL ;
         "Financial Summary" center: 45 . printNL ;
         "Prepared: " print ;
         ^today formatUsingShortName print ;
         "     Latest Price: " print ; 
         price printNL ;
         newLine print ; 
     
         " " print: 15 ; 
         dr evaluate: [ ^date year print: 10 ; ] ;
         newLine print ; 
         " " print: 15 ;
         dr evaluate: [ " " print ; "-" fill: 9 . print ] ;
         newLine print ; 

         "Sales" print: 15 ;
         dr evaluate: [ sales print: 10 ] ;
         newLine print ;
     
         "EPS" print: 15 ;
         dr evaluate: [ earningsPerShare print: 10 ] ;
         newLine print ;
     
         "PE" print: 15 ;
         dr evaluate: [ price / earningsPerShare print: 10 ] ;
         newLine print ;
     ] ;   #-- this line ends the method
     
     #---  Run the method
     Named Company GM financialAnalysis  ;



