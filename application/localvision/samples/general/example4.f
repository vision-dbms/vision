     #---  Create New Class
     Object createSubclass: "Report" ;
     
     #---  Define Some Properties
     Report
        defineFixedProperty: 'itemList' .
        defineFixedProperty: 'criteria' .
        defineFixedProperty: 'sort' .
        defineFixedProperty: 'sortIsUp' .
     ;
     
     #--- Define a Report Instance
     !rep1 <- Report newAs: "Report1" ;
     rep1 :itemList <- ticker, name, sales, pe ;
     rep1 :criteria <- [ sales > 50000 ] ;
     rep1 :sort <- [ sales ] ;
     rep1 :sortIsUp <- FALSE ;
     
     #---  Define A Method to display the Report
     Report
     defineMethod:
     [ | displayFor: list | 
     
       itemList
        do: [ dataType isNumber 
                 ifTrue: [ header print: (width * -1) ] 
                ifFalse: [ header print: width ] ;
            ] ;
       newLine print ;
     
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
     ] ;
     
     #--- And run it for rep1 on testList
     rep1 displayFor: testList


