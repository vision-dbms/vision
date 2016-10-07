     90 evaluate:              #--  run the program asof 1990
        [
        !top3 <- Company masterList           #-- start with all companies
            rankDown: [ :sales pctChangeLag: 1 yearEnds ] .  #-- 1990 to 1989
            select: [ rank <= 3 ] ;           #-- select rank = 1-3
     
        "***  Financial Summary Report For Top 3 Companies  ***" printNL ;
        "       (Using 1990-1989 Percent Change in Sales)" printNL ;
     
        top3                                  #-- for each top3 company
        do: [ newLine print ; 
              newLine print ; 
              financialAnalysis ;             #-- run the method
            ] ;
        ] ;


