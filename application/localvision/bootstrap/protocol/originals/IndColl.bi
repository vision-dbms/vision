"
#############################
####  IndexedCollection  ####
#############################
"
?g

####################
#  Print and Other Standard Messages
####################
Prototypical IndexedCollection
;

####################
#  Primitives and Converses
####################
Prototypical IndexedCollection

defineMethod:
[ | at: aKey |
  ^self valueCell: aKey .
	send: [ isntNA ifTrue: [ value ] ]
] .

defineMethod:
[ | at: aKey put: aValue |
  ^self newValueCell: aKey. <- aValue
] .

;

?g
