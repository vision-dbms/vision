#######################
####  IndexedList  ####
#######################

# $File: //depot/vision/applications/core/builtin/mainline/IndList.bi $
# $DateTime: 2005/12/05 15:20:32 $
# $Revision: #3 $
# $Author: dohara $

####################
#  Primitives and Converses
####################

Prototypical IndexedList

defineMethod: [ | at: aKey |
   aKey isInteger ifTrue: [ :aKey <- aKey asPointerTo: CoreWorkspace Integer ] ;
   ^self valueCell: aKey .
        send: [ isntNA ifTrue: [value] ]
] .

defineMethod: [ | at: aKey put: aValue |
   aKey isInteger ifTrue: [ :aKey <- aKey asPointerTo: CoreWorkspace Integer ] ;
  ^self newValueCell: aKey .<- aValue
] .

;
