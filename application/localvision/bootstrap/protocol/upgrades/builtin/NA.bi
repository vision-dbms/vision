#####################
####  Undefined  ####
#####################

####################
#  Primitives and Converses
####################

Prototypical Undefined

defineMethod:  [ | = x |
  x isUndefined
] .
defineMethod:  [ | != x |
  x isUndefined not
] .

defineMethod:
[ | localizeValue |
  ^self _inContextOf: GlobalWorkspace
] .

;
