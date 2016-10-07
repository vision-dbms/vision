Named Account \102 holdings
groupedBy:[security company industry sector] .  # group into sectors
sortUp:[name] . # sort by sector name
do:
   [
    code print:10 ;      # sector code
    name printNL:35 ;    # sector name
    groupList           # for each holding in sector
    do:
       [
        security code print:10 ;
        security name print:30 ;
        shares print:10 ;
        totalMarketValue printWithCommas:15 ;
        percentOfPort printNL ;
       ];
        newLine print ;
   ];
