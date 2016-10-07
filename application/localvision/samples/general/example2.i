     Company defineMethod:
     [ | sectorIndustryReportFor: list |
     #-- list  is a list of company objects

     "***  Multiple Company Data Sheet  ***" center: 70 . printNL ;
     "Sector-Industry Breakdown" center: 70 . printNL ;
     "Top 2 Sectors By Sales" center: 70 . printNL ;
     newLine print ; 
     "Ticker" print: 10 ;
     "Name" print: 30 ;
     "% of Sec/Ind" print: -12 ; 
     "Sales" print: -10 ;
     "P-E" print: -8 ;
     newLine print ; 
     "-" fill: 70 . printNL ;
     
     #---  list is the list Supplied as a Parameter to this Method
     list groupedBy: [ industry sector ] .
     extendBy: 
        [ !sectorSales <- groupList total: [ sales ] ;
          !sectorPE <- groupList average: [ price / earningsPerShare ] ;
        ] .
     rankDown: [ sectorSales ] .
     select: [ rank <= 2 ] .
     do: [ "Sector " print ; code print: 10 ; name printNL ;
           newLine print ; 
           groupList groupedBy: [ industry ] .
           do: [ !industrySales <- groupList total: [ sales ] ;
                 !industryPE <- groupList average: [ price / earningsPerShare ] ;
                 !sectorSales <- ^my sectorSales ; 
                 "Industry " print ; code print: 10 ; name printNL ;
                 groupList 
                 do: [ ticker print: 10 ; 
                       name print: 30 ;
                       sales / ^my sectorSales * 100 print: 6.1 ;
                       sales / ^my industrySales * 100 print: 6.1 ;
                       sales printWithCommas: 10.0 ;
                       price / earningsPerShare print: 8 ;
                       newLine print ; 
                     ] ;

                 "Industry Totals: " print: 40 ; 
                 industrySales / sectorSales * 100 print: 6.1 ;
                 100.0 print: 6.1 ;
                 industrySales printWithCommas: 10.0 ;
                 industryPE printNL: 8 ; 
                 newLine print ; 
               ] ;
           "Sector " concat: code . concat: " Totals: " . print: 40 ;
           100.0 print: 6.1 ; 100.0 print: 6.1 ;
           sectorSales printWithCommas: 10.0 ;
           sectorPE print: 8 ; 
           newLine print ;  newLine print ; 
         ] ;
      ] ;
     
     #--  Run this New Method
     Company sectorIndustryReportFor: testList ;     



