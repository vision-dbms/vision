     !list <- testList ;
     !itemList <- ticker, name, sales, pe ;     #-- list of ReportItems 

     itemList
      do: [ dataType isNumber    #-- right justify if type is Number
               ifTrue: [ header print: (width * -1) ] 
              ifFalse: [ header print: width ] ;
          ] ;
     newLine print ;

     list
     do: [ !element <- ^self ; 
           ^my itemList     #--  send item's accessBlock to each element 
           do: [ ^my element send: accessBlock . print: width ] ;
           newLine print ; 
         ] ;



