######################
####  TimeSeries  ####
######################

####################
#  Primitives and Converses
####################

Prototypical TimeSeries

defineMethod:
[ | localizeValue |
  !newTS <- GlobalWorkspace TimeSeries new ;
  ^self do: [^my newTS put: (^self localizeValue)] ;
  newTS
] .

defineMethod:
[ | onDate: aDate |
  !date <- ^self convertToConformantKey: aDate ;
  date isntNA && [ ^self effectiveDateAsOf: date . = date ]
    ifTrue: [ ^self asOf: date ]
   ifFalse: [ ^self firstObservation defaultInstance ]
] .

;
