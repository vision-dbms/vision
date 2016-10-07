
     90 evaluate: 
       [ Named Company send: [ GM, IBM ] .
         do: [ 
       #--  ^self or ^current needed here to avoid syntax error
               ^self financialAnalysisForPeriods: 2 ;   
               newLine print ;
               newLine print ;
             ] 
        ] ;


