"Code" print:10 ;
"Name" print:37 ;
"Shares" print:12 ;
"Total Mkt Val" print:15 ;
"% of Port" print:10 ;
"Sector" printNL:-10 ;

newLine print;

Named Account \102 holdings
sortDown:[percentOfPort] .
do:
   [
    security code print:10 ;
    security name print:35 ;
    shares printWithCommas:10 ;
    totalMarketValue printWithCommas:15 ;
    percentOfPort print:10 ;
    security company industry sector name printNL:-35 ;  #Navigate to sector
   ];
