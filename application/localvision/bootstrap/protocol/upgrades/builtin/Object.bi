##################
####  Object  ####
##################

#=============
# Provide unary message to cast base types 
# in federated dbs into local context
#=============

Object defineMethod: [ | localizeValue |
  ^self GlobalWorkspace = ^self CoreWorkspace
    ifTrue: [^self] ifFalse: [GlobalWorkspace NA]
] ;

#--------------------
#  implement displayAll for Object
#  patch_125.vis
#--------------------

Object defineMethod: [ | displayAll | 
   ^self value printNL
] ;
