Account defineMethod: 
    [ | firstReport | 

"Account: " concat: code . center:80 . printNL;  # center account 
newLine print ;

"Code" print:10 ;
"Name" print:35 ;
"Shares" print:12 ;
"Total Mkt Val" print:15 ;
"% of Port" printNL ;
newLine print ;

holdings
sortDown:[percentOfPort] .  # sort lowest to smallest
do:
   [
    security code print:10 ;
    security name print:35 ;
    shares print:10 ;
    totalMarketValue printWithCommas:15 ;
    percentOfPort printNL ;
   ];
];
