##################
####  Number  ####
##################

####################
#  Primitives and Converses
####################

Prototypical Number

defineMethod:
[ | localizeValue |
  ^self _inContextOf: GlobalWorkspace
] .

;

#--------------------
#  Number
#  From core/builtin/patch_124.vis 12/15/2008
#--------------------

Number defineMethod: [ | canonicalizedForSort |
  ^self asDouble asPointerTo: GlobalWorkspace Double
];

Number defineMethod: [ | orderUsingDictionary: dictionary |
    ^self asDouble asPointerTo: GlobalWorkspace Double
] ;


