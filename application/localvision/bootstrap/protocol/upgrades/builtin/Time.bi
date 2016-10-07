################
####  Time  ####
################

# 02/14/06 - doh - Fixed bug with date=date and first date has an extension.

# $File: //depot/vision/applications/core/builtin/mainline/Time.bi $
# $DateTime: 2006/02/14 14:47:41 $
# $Revision: #3 $
# $Author: dohara $

####################
#  Primitives and Converses
####################

Time

defineMethod:
[ | localizeValue |
  ^self _inContextOf: GlobalWorkspace
] .

;
#-----------------------------
#  High Frequency Timeseries
#-----------------------------
# from EXTdate.6.0, DEFhighResTime.6.0, DEFlowResTime.6.0

#--------------------
#  Date lookup resolution in t/s
#--------------------

Date defineMethod: [ | convertToMyKindOfObject: aCandidate |
  !myObject <- aCandidate asDate ;
  myObject isDate
  ifTrue: [ myObject __asPointerTo: ^self asDate ]
  ifFalse: [ CoreWorkspace NA ]
];

LowResolutionTime defineMethod: [ | convertToMyKindOfObject: aCandidate |
  !myObject <- aCandidate asLowResolutionTime ;
  myObject isLowResolutionTime
  ifTrue: [ myObject __asPointerTo: ^self asLowResolutionTime ]
  ifFalse: [ CoreWorkspace NA ]
];

HighResolutionTime defineMethod: [ | convertToMyKindOfObject: aCandidate |
  !myObject <- aCandidate asHighResolutionTime ;
  myObject isHighResolutionTime
  ifTrue: [ myObject __asPointerTo: ^self asHighResolutionTime ]
  ifFalse: [ CoreWorkspace NA ]
];
