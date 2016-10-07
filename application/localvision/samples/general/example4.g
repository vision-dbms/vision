     #---  Define Cover Methods to Set the Report Parameters
     Report
     defineMethod:
     [ | setItemListTo: blocks | 
       :itemList <- blocks ; 
       ^self           # return current Report object
     ] .
     defineMethod:
     [ | setCriteriaTo: block | 
       block isBlock
         ifTrue: [ :criteria <- block ] 
        ifFalse: [ ">>>  Cannot Set Criteria.  <<<" printNL ] ;
       ^self           # return current Report object
     ] .
     defineMethod:
     [ | setSortUpTo: block | 
       block isBlock
         ifTrue: [ :sort <- block ;
                   :sortIsUp <- TRUE ;
                 ] 
        ifFalse: [ ">>>  Cannot Set Sort.  <<<" printNL ] ;
       ^self           # return current Report object
     ] .
     defineMethod:
     [ | setSortDownTo: block | 
       block isBlock
         ifTrue: [ :sort <- block ;
                   :sortIsUp <- FALSE ;
                 ] 
        ifFalse: [ ">>>  Cannot Set Sort.  <<<" printNL ] ;
       ^self           # return current Report object
     ] .
     ;
     
     #---  Set Some Values and Run Report for Company masterList
     rep1 
       setItemListTo: name, pe, sales .
       setCriteriaTo: [ sales > 25000 && price / earningsPerShare < 10 ] .
       setSortDownTo: [ sales ] .
       displayFor: Company masterList ;



