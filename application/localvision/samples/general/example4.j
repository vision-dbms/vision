

     !salesPrior <- ReportItem new
        do: [ :header <- "Prev Sales" ;
              :dataType <- ^global Number ;
              :width <- 12 ;
              :accessBlock <- [ :sales lag: 1 yearEnds ] ;
            ] ;
     
     rep1 
       setItemListTo: name, sales, salesPrior .
       displayFor: Company masterList ;
     

