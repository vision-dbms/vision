################
####  List  ####
################
# $File: //depot/vision/applications/core/builtin/mainline/List.bi $
# $DateTime: 2008/12/18 09:07:00 $
# $Revision: #4 $
# $Author: sglee $


####################
#  Primitives and Converses
####################

Prototypical List

defineMethod:
[ | localizeValue |
  !newList <- GlobalWorkspace List new ;
  ^self do: [^my newList, (^self localizeValue)] ;
  newList
] .

;

#--------------------
#  List indexBy: block
#--------------------
#4/3/07 tlh core/builtin/patch_121.vis
#
List defineMethod: [ | indexBy: block |
  !ilist <- CoreWorkspace IndexedList new ;
  ^self collect: block .
      do: [ ^my ilist at: value put: ^self super ] ;
  ilist
] ;

#
#  implement displayAll for List 
#  patch_125.vis
#
List defineMethod: [ | displayAll | 
   ^self numberElements do: [ position print: -6 ; printNL ] ;
] ;

#
# Add new method to support iterate with send message
#  patch_125.vis
#
List defineMethod: [ | iterateSend: block by: n |
  ^self extendBy: [ !result ] .
     iterate: [ :result <- ^self send: ^my block ] by: n .
     send: [ result ] 
] ;


