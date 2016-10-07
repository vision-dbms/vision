     #---  Create New Class
     Object createSubclass: "ReportItem" ;
     
     #---  Define Some Properties
     ReportItem
        defineFixedProperty: 'header' .
        defineFixedProperty: 'dataType' .
        defineFixedProperty: 'width' .
        defineFixedProperty: 'accessBlock' .
     ;

     #---  Create Some Instances
     !ticker <- ReportItem new
        do: [ :header <- "Ticker" ;
              :dataType <- ^global String ;
              :width <- 10 ;
              :accessBlock <- [ ticker ] ;
            ] ;
     
     !name <- ReportItem new
        do: [ :header <- "Name" ;
              :dataType <- ^global String ;
              :width <- 25 ;
              :accessBlock <- [ name ] ;
            ] ;
     
     !sales <- ReportItem new
        do: [ :header <- "Sales" ;
              :dataType <- ^global Number ;
              :width <- 12 ;
              :accessBlock <- [ sales ] ;
            ] ;
     
     !pe <- ReportItem new
        do: [ :header <- "PE Ratio" ;
              :dataType <- ^global Number ;
              :width <- 10 ;
              :accessBlock <- [ price / earningsPerShare ] ;
            ] ;


