###################
####  Boolean  ####
###################

####################
#  Primitives and Converses
####################

Prototypical Boolean

defineMethod:
[ | localizeValue |
  ^self _inContextOf: GlobalWorkspace
] .

;

################
####  TRUE  ####
################

TRUE
defineMethod:  [ | = x |
  x isTrue
] .
defineMethod:  [ | != x |
  x isTrue not
] ;

#################
####  FALSE  ####
#################

FALSE
defineMethod:  [ | = x |
  x isFalse
] .
defineMethod:  [ | != x |
  x isFalse not
] ;
