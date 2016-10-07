
Account defineMethod: 
[ | firstReport: sortString | # method with one parameter

"Account: " concat: code . center:80 . printNL ;  # center account 
"As Of: " concat: holdingsDate . center:80 . printNL ;  # center holdings date
newLine print;


"Code" print:10 ;
"Name" print:37 ;
"Shares" print:12 ;
"Total Mkt Val" print:15 ;
"% of Port" print:10 ;

newLine print ;

!sortBlock <- sortString asBlock ;
holdings
sortUp:sortBlock .  # sortBy user supplied rule
do:
   [
    security code print:10 ;
    security name print:35 ;
    shares printWithCommas:10 ;
    totalMarketValue printWithCommas:15 ;
    percentOfPort printNL:10 ;
   ];
];
