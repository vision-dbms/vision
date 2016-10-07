##################
###  Integer  ###
##################

#--------------------
#  Integer
#  From core/builtin/patch_124.vis 12/15/2008
#--------------------

Integer defineMethod: [ | canonicalized |
  ^self asInteger asPointerTo: GlobalWorkspace Integer
];


