     !list <- testList ;
     !itemList <- ticker, name, sales, pe ;
     !criteria <- [ sales > 50000 ] ;         #--  criteria to use
     !sort <- [ sales ] ;                     #--  sort rule
     !sortIsUp <- FALSE ;                     #--  sort direction flag
     
     itemList
      do: [ dataType isNumber 
               ifTrue: [ header print: (width * -1) ] 
              ifFalse: [ header print: width ] ;
          ] ;
     newLine print ;
     
     #---  Apply selection criteria and sort the result
     !universe <- list select: criteria ;
     sortIsUp 
        ifTrue: [ :universe <- universe sortUp: sort ] 
       ifFalse: [ :universe <- universe sortDown:  sort ] ;
     
     universe
     do: [ !element <- ^self ; 
           ^my itemList
           do: [ ^my element send: accessBlock . print: width ] ;
           newLine print ; 
         ] ;


