"
################
####  List  ####
################
"
?g

####################
#  Print and Other Standard Messages
####################
Prototypical List
;

####################
#  Primitives and Converses
####################

Prototypical List
#  define: 'newPrototype'     toBe: 160 asPrimitive .   #NewList
  define: ','                toBe: 161 asPrimitive .   #AppendObjectToList
#  define: 'clusterNew'       toBe: 163 asPrimitive .   #NewListInStore
  define: 'count'            toBe: 170 asPrimitive .   #CountListElements
  define: 'iterate:by:'      toBe: 179 asPrimitive .   #Iterate
  define: 'do:'              toBe: 180 asPrimitive .   #IterateInContext
  define: 'send:'            toBe: 181 asPrimitive .   #IterateInContextForValue
  define: 'extend:'          toBe: 182 asPrimitive .   #IterateInContextForEnvironment
  define: 'extendBy:'        toBe: 182 asPrimitive .   #IterateInContextForEnvironment
  define: 'select:'          toBe: 183 asPrimitive .   #IterateInContextForSubset
  define: 'groupedBy:usingCollector:'
    toBe: 184 asPrimitive .       #IterateInContextForGrouping
  define: 'basicSortUp:'     toBe: 185 asPrimitive .   #IterateInContextForAscendingSort
  define: 'basicSortDown:'   toBe: 186 asPrimitive .   #IterateInContextForDescendingSort

  define: 'basicTotal:'           toBe: 190 asPrimitive .   #IterateInContextForNumericTotal
  define: 'basicAverage:'         toBe: 191 asPrimitive .   #IterateInContextForNumericAverage
  define: 'basicMin:'             toBe: 192 asPrimitive .   #IterateInContextForNumericMinimum
  define: 'basicMax:'             toBe: 193 asPrimitive .   #IterateInContextForNumericMaximum
  define: 'ncount:'          toBe: 194 asPrimitive .   #IterateInContextForNumericCount
  define: 'basicRankUp:usingCollector:'
    toBe: 195 asPrimitive .                            #IterateInContextForAscendingRank
  define: 'basicRankDown:usingCollector:'
    toBe: 196 asPrimitive .                            #IterateInContextForDescendingRank
  define: 'basicProduct:'         toBe: 197 asPrimitive .   #IterateInContextForNumericProduct
  define: 'basicRunningTotal:usingCollector:'
    toBe: 198 asPrimitive .                            #IterateInContextForRunningTotal

  define: 'base1CellAtPosition:'  toBe: 172 asPrimitive .
  define: 'base0CellAtPosition:'  toBe: 171 asPrimitive .
;


####################
#  toList message returns ^self at List - it performs a conversion
#     for all other Collection classes
####################

Prototypical List 
defineMethod:
[ | toList | 
  ^self
] .
;

####################
#  positional access
####################

Prototypical List 

defineMethod:
[ | at: position |
    !cell <- ^self base1CellAtPosition: position ;
    cell isntNA ifTrue: [ cell value ] 
].

#----------

defineMethod:
[ | at0: position|
    !cell <- ^self base0CellAtPosition: position ;
    cell isntNA ifTrue: [ cell value ] 
].

#----------

defineMethod:
[ | uniformAt: position |
  !cell <- ^self at: position ;
  cell isntNA ifTrue: [ cell ] ifFalse: [ ^self at: 1 ]
] .

#----------

defineMethod:
[ | uniformAt0: position |
  !cell <- ^self at0: position ;
  cell isntNA ifTrue: [ cell ] ifFalse: [ ^self at0: 0 ]
] .

#----------

defineMethod:
[ | at: position put: value |
  !cell <- ^self base1CellAtPosition: position; 
  cell isntNA ifTrue: [cell <- value];
  value
] .
;

#########################
# Formatting
#########################

Prototypical List

defineMethod:
[ | display: aBlockList |
^self do:
      [
      !element <- ^self;
      ^my aBlockList do: [ ^my element send: ^self . print:12 ];
      ^global newLine print;
      ]
] .

#----------

defineMethod: 
[ | displayAcross: aBlock |

# row label
    "                           " print;
# main data
    ^self do: [ ^self send: ^my aBlock . print:12 ];
    ^global newLine print;
] .
;

#########################
# Multiple List Manipulation
#########################

Prototypical List

#----------

defineMethod: 
[ | appendListElementsFrom: aList |

#####
#  appends aList to ^self
#  NOTE: ORIGINAL LIST IS CHANGED - ORDER NOT PRESERVED
#####
   !initialList <- ^self;
   aList do: [ ^my initialList, ^self ;];
   ^self
] .

;

#--------------------
#  "Vector" Methods
#
#  List versions do a position + - * / if both values are numbers
#  If argument is a scalar, the same value is applied to each element in
#    the original list
#
#  Note: these methods assume that the list(s) and scalar values are numeric;
#        no parameter checking is performed
#
#--------------------

Prototypical List
defineMethod:
[ | + value |
  value isCollection
  ifTrue:
    [ !list <- value toList ;
      ^self numberElements 
          send: [ ^self + (^my list at: position) ] 
    ]
  ifFalse: [ ^self send: [ ^self + ^my value ] ] 
] .

defineMethod:
[ | - value |
  value isCollection
  ifTrue:
    [ !list <- value toList ;
      ^self numberElements 
          send: [ ^self - (^my list at: position) ] 
    ]
  ifFalse: [ ^self send: [ ^self - ^my value ] ] 
] .

defineMethod:
[ | * value |
  value isCollection
  ifTrue:
    [ !list <- value toList ;
      ^self numberElements 
          send: [ ^self * (^my list at: position) ] 
    ]
  ifFalse: [ ^self send: [ ^self * ^my value ] ] 
] .

defineMethod:
[ | / value |
  value isCollection
  ifTrue:
    [ !list <- value toList ;
      ^self numberElements 
          send: [ ^self / (^my list at: position) ] 
    ]
  ifFalse: [ ^self send: [ ^self / ^my value ] ] 
] .
;


#------------------------------

####################
#   Intra-List computations
####################

Prototypical List

defineMethod:
[ | createLinks |
  !l <- ^self extendBy: [ !nextElement; !previousElement ] ;
  l count sequence
  do: [ !element <- ^my l at: asNumber ;
        !prior <- ^my l at: (asNumber - 1 ) asInteger ; 
        !next <- ^my l at: (asNumber + 1) asInteger ; 
        prior isntNA ifTrue: [ element :previousElement <- prior ] ;
        next isntNA ifTrue: [ element :nextElement <- next ]
      ] ;
   l
] .
;


#------------------------------

##################################################
#  execution control
##################################################
#--------------------
#  List execute: message in: class
#    this message creates and executes the message:
#        class messageUsing: list
# e.g.,   mylist execute: "report1" in: Company
#         Company report1Using: mylist
#   The version at Object turns the single object into a list of 1 and
#      executes the list version of the method
# This message is used by the UIMS Application classes
#--------------------

Prototypical List

defineMethod:
[ | execute: message in: class |
  !list <- ^self ;
  "^self :" concat: message . concat: "Using: 0 " .
      evaluateIn: class . value: list
] ;

?g

