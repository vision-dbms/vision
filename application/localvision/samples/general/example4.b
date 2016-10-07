     !list <- testList ;
     !blockList <- 
         [ ticker ] , [ name ] , [ sales ] , [ price / earningsPerShare ] ;

     blockList     #--  display each block as a heading
        do: [ asUndelimitedString print: 12 ] ;
     newLine print ; 
     
     list
     do: [ !element <- ^self ;
           ^my blockList
              do: [ ^my element send: ^self . print: 12 ] ;
            newLine print ;
         ] ;



