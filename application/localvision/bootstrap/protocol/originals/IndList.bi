"
#######################
####  IndexedList  ####
#######################
"
?g

####################
#  Print and Other Standard Messages
####################

Prototypical IndexedList
;

####################
#  Primitives and Converses
####################

Prototypical IndexedList
   define: 'toList'       toBe: 162 asPrimitive .  #IndexedListToSequencedList
   define: 'count'        toBe: 170 asPrimitive .  #CountListElements
   define: 'iterate:by:'  toBe: 179 asPrimitive .  #Iterate
   define: 'do:'          toBe: 180 asPrimitive .  #IterateInContext
   define: 'send:'        toBe: 181 asPrimitive .  #IterateInContextForValue
   define: 'extendBy:'    toBe: 182 asPrimitive .  #IterateInContextForEnvironment
   define: 'select:'      toBe: 183 asPrimitive .  #IterateInContextForSubset
   define: 'groupedBy:usingCollector:'
     toBe: 184 asPrimitive .                #IterateInContextForGrouping
    define: 'basicSortUp:'
      toBe: 185 asPrimitive .               #IterateInContextForAscendingSort
    define: 'basicSortDown:' 
      toBe: 186 asPrimitive .               #IterateInContextForDescendingSort

    define: 'basicTotal:'    
      toBe: 190 asPrimitive .           #IterateInContextForNumericTotal
    define: 'basicAverage:'
      toBe: 191 asPrimitive .           #IterateInContextForNumericAverage
    define: 'basicMin:'
      toBe: 192 asPrimitive .           #IterateInContextForNumericMinimum
    define: 'basicMax:'
      toBe: 193 asPrimitive .           #IterateInContextForNumericMaximum
    define: 'ncount:'
      toBe: 194 asPrimitive .           #IterateInContextForNumericCount
    define: 'basicRankUp:usingCollector:'
      toBe: 195 asPrimitive .           #IterateInContextForAscendingRank
    define: 'basicRankDown:usingCollector:'
      toBe: 196 asPrimitive .           #IterateInContextForDescendingRank
    define: 'basicProduct:'
      toBe: 197 asPrimitive .           #IterateInContextForNumericProduct
    define: 'basicRunningTotal:usingCollector:'
      toBe: 198 asPrimitive .           #IterateInContextForRunningTotal

    define: 'valueCell:'       toBe: 281 asPrimitive .    #LocateItem
    define: 'newValueCell:'    toBe: 282 asPrimitive .    #DefineItem
    define: 'delete:'          toBe: 283 asPrimitive .    #DeleteItem
#    define: 'newPrototype'     toBe: 284 asPrimitive .    #NewAList
#    define: 'clusterNew'       toBe: 285 asPrimitive .    #NewAListInStore
    define: 'deleteColumn:'    toBe: 286 asPrimitive .    #DeleteColumn
;

#----------

Prototypical IndexedList defineMethod: [ | extendIndex |
!origList <- ^self asSelf; 

#--  get all keys in cluster
!keyList <- ^self asSelf asPOP getNthPOP: 2 .   
     asObject instanceList send: [value] ;
!nilValue <- ^self valueCell: "newKeyWithNoValue";   # Setup an invalid hit

# Operate on the keys to get the hits on the original List.
!hitList <- keyList 
    extendBy: [!index <- ^self;
               !myValue <- ^my origList valueCell: index;
              ] .
    select: [myValue != ^my nilValue ] ;

!newList <- ^global IndexedList new;

# Using the list of valid keys populate the newList.
hitList 
do: [^my newList at: index put: 
        (^my origList at: index . extendBy: [ !index <- ^my index ] )
    ] ;

newList

];

?g
