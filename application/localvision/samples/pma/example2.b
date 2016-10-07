# Add some headers 
"Code" print:10;
"Name" print:35;
"Shares" print:12;
"Total Mkt Val" print:15;
"% of Port" printNL;
newLine print;

Named Account \102 holdings
sortDown:[percentOfPort] .  # sorted in descending order
do:
   [
    security code print:10;
    security name print:35;
    shares print:10;
    totalMarketValue printWithCommas:15;
    percentOfPort printNL;
   ];
