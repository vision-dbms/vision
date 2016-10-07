     Report
     
     #---  Add asOfDate and title as Report attributes
         defineFixedProperty: 'asOfDate' .
     defineMethod:
     [ | setAsOfDateTo: date | 
       :asOfDate <- date asDate ; 
       ^self 
     ] .
         defineFixedProperty: 'title' .
     defineMethod:
     [ | setTitleTo: string | 
       :title <- string asString ;
       ^self
     ] .

     #---  Modify to use title and asOfDate; display other criteria
     defineMethod:
     [ | displayFor: list | 
     
       !totalWidth <- itemList total: [ width ] ;
       title center: totalWidth . printNL ;
       !evaluationDate <- asOfDate ;    #-- use asOfDate if defined
       evaluationDate isNA ifTrue: [ :evaluationDate <- ^date ] ;
       "As Of: " print ;
       evaluationDate printNL ;
       "    Criteria: " print ; criteria printNL ;
       "    Sort " print ;
       sortIsUp ifTrue: [ "Up: " print ] ifFalse: [ "Down: " print ] ;
       sort printNL ; 
       newLine print ; 
     
       itemList
        do: [ dataType isNumber 
                 ifTrue: [ header print: (width * -1) ] 
                ifFalse: [ header print: width ] ;
            ] ;
       newLine print ;
     
       evaluationDate evaluate: [       #-- evaluate report using correct date
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
       ] ;     #--- end of evaluate:
     
     ] ;       #--- end of method
     
     #--- Set Title and rerun it as of 1989
     rep1 
       setTitleTo: "First Report" .
       setAsOfDateTo: 89 .
       displayFor: Company masterList 


