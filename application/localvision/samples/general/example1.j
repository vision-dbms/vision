

     Company defineMethod:
     [ | financialAnalysisForPeriods: number |
         !lastDate <- :sales effectiveDate ;     #-- most recent date
         !offset <- (number - 1 ) asInteger ;    #-- years to subtract
         !firstDate <- lastDate - 2 yearEnds ;   #-- default first date
         offset isInteger                        #-- recompute if input is valid 
            ifTrue: [ :firstDate <- lastDate - offset yearEnds ] ;
         !dr <- lastDate to: firstDate by: 1 yearEnds ;
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
     ] ;
     
     Named Company GM financialAnalysisForPeriods: 4 ;


