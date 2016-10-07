Security masterList 
select:[ price > 50 ] .
groupedBy: [ company industry sector ] .
sortUp: [ name ] .
do: [  
      name printNL;  # This is the Sector name
      newLine print;
     
      groupList     
      do:
         [
          code print:10;
          name print:30;
          price printNL;
         ];
    newLine print;
    ];
