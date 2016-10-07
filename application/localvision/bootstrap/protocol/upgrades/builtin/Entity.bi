######################
#### Entity  ####
######################

#-- reset the code: delete the current code, then set to new one and add alias
Entity defineMethod: [ | resetCodeTo: string | 
  ^self deleteAlias: code ;    #- current code
  ^self setCodeTo: string ;
  ^self
] ;

Entity defineMethod:  [ | setCodeTo: string |
string isntDefault ifTrue:
  [
  :code <- ^self getClusteredString: string ;
  ^self addAlias: string ; 
  ] ;
^self
] ;

#--------------------
#  Global Access
#--------------------
Entity defineMethod: [ | globalId | ^self id ] ;

Entity defineMethod: [ | asGlobal | 
  ^self globalLocateId: ^self globalId . else: ^self
] ;


#--------------------
#  Entity
#  From core/builtin/patch_124.vis 12/15/2008
#--------------------

#- changed master to activeList
Entity defineMethod: [ | globalLocateId: id |
!class <- ^self ;
id isEntity
  ifTrue: [ id ] .
  elseIf: [ id isString ]
    then:
     [ !localClass <- class whatAmI ;
       GlobalWorkspace VisionDB activeList select: [ home isntNA ] .
          sortUp: [ sortCode ] .
          select: [ ^my localClass locateInDictionaryOf: home . isntNA ] .
         send: [ ^my localClass evaluateIn: home . locateId: ^my id ] .
         select: [isntDefault ] . at: 1 .
     ]
] ;

