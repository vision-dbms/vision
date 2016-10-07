##################################################
#  This Vision script can be executed to generate an unformatted
#  document describing all the messages in your current Vision database.
#  The file "classes.doc" will contain all messages by class, sorted 
#  alphabetically within class type .  The file "xref.doc" will contain
#  a cross reference of all messages defined for any class in the database.
##################################################

"xref.sample" asFileContents evaluate ; 
"classes.doc" substituteOutputOf: [ 
Schema ClassDescriptor instanceList
sortUpBy: [ !id <- type code take: 1 ;
            id = "C" ifTrue: [ :id <- "B" ] ;
            id = "O" ifTrue: [ :id <- "Y" ] ;
            id = "I" ifTrue: [ :id <- "Z" ] ;
            id
          ] 
    then: [ name ] .
do: [ fullReport;
      newPage print ; 
    ] .
] ;
?g
"xref.doc" substituteOutputOf: [ 
        Schema ClassDescriptor
        do: [ 
            !title <- "***  Message Cross Reference - All Messages  ***" ;
            !list <- MessageImplementationDescriptor activeList
               select: [ usageLevel isInternal not ] ;
            ^self alphaXRefFor: list withTitle: title ;
           ^self functionXRefFor: list withTitle: title ;
           ] ;
] ;

?g
