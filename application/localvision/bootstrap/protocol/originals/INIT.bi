##################################################
#  This Script Defines a Standard Set of Messages for All
#  BuiltIn Classes.  It is part of all networks bootstrapped
#  as of 9/92 and should be selectively installed in old networks
#  as needed prior to installation of the Schema management module.
# 
#  Run in OS2
##################################################

"
####################
###  Installation of Standard Messages at All BuiltIn Classes
####################
" printNL ;

#------------------------------

####################
#  Object Space
####################
^current 
  define: 'BuiltInWorkspace' toBe: ^current .
  define: 'number' toBe: 2 .
  define: 'whatAmI' toBe: "BuiltInWorkspace" .
  define: 'isObjectSpace' toBe: TRUE ;
Prototypical Object define: 'isObjectSpace' toBe: FALSE ;
 
###################
#  Object
##################
BuiltInWorkspace define: 'Object' toBe: Prototypical Object newPrototype ;
Object 
  define: 'whatAmI' toBe: "Object" .
  define: 'isObject' toBe: TRUE .
  defineAsMessage: 'asSelf' ;


####################
#  Boolean
####################
BuiltInWorkspace define: 'Boolean' toBe: Prototypical Boolean ;
Boolean
  define: 'whatAmI' toBe: "Boolean" .
  define: 'isBoolean' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
TRUE 
  define: 'whatAmI' toBe: "TRUE" .
  define: 'isTRUE' toBe: TRUE ;
FALSE
  define: 'whatAmI' toBe: "FALSE" .
  define: 'isFALSE' toBe: TRUE ;
Object
  define: 'isBoolean' toBe: FALSE .
  define: 'isFALSE' toBe: FALSE .
  define: 'isTRUE' toBe: FALSE .
;

####################
#  DateRange
####################
BuiltInWorkspace define: 'DateRange' toBe: Prototypical DateRange ;
DateRange
  define: 'whatAmI' toBe: "DateRange" .
  define: 'isDateRange' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isDateRange' toBe: FALSE ;

####################
#  FixedProperty
####################
!FixedProperty <- TheDefaultFixedProperty ;
FixedProperty
  define: 'whatAmI' toBe: "FixedProperty" .
  define: 'isFixedProperty' toBe: TRUE .
  define: 'isProperty' toBe: TRUE ;

!TimeSeriesProperty <- TheDefaultProperty ;
TimeSeriesProperty
  define: 'whatAmI' toBe: "TimeSeriesProperty" .
  define: 'isTimeSeriesProperty' toBe: TRUE ;

Object 
  define: 'isProperty' toBe: FALSE .
  define: 'isFixedProperty' toBe: FALSE .
  define: 'isTimeSeriesProperty' toBe: FALSE .
;

####################
# Function
####################
BuiltInWorkspace define: 'Function' toBe: Prototypical Function ;
Function
  define: 'whatAmI' toBe: "Function" .
  define: 'isFunction' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isFunction' toBe: FALSE ;

BuiltInWorkspace define: 'ComputedFunction' toBe: Prototypical ComputedFunction ;
ComputedFunction
  define: 'whatAmI' toBe: "ComputedFunction" .
  define: 'isComputedFunction' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isComputedFunction' toBe: FALSE ;

!Block <- [ ] ;
Block
  define: 'whatAmI' toBe: "Block" .
  define: 'isBlock' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isBlock' toBe: FALSE ;

!Method <- Prototypical Method ;
Method
  define: 'whatAmI' toBe: "Method" .
  define: 'isMethod' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isMethod' toBe: FALSE ;

!Primitive <- Prototypical Primitive ;
Primitive
  define: 'whatAmI' toBe: "Primitive" .
  define: 'isPrimitive' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isPrimitive' toBe: FALSE ;

BuiltInWorkspace define: 'EnumeratedFunction' toBe: Prototypical EnumeratedFunction ;
EnumeratedFunction
  define: 'whatAmI' toBe: "EnumeratedFunction" .
  define: 'isEnumeratedFunction' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isEnumeratedFunction' toBe: FALSE ;

BuiltInWorkspace define: 'Collection' toBe: Prototypical Collection ;
Collection
  define: 'whatAmI' toBe: "Collection" .
  define: 'isCollection' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isCollection' toBe: FALSE ;

BuiltInWorkspace define: 'IndexedCollection' toBe: Prototypical IndexedCollection ;
IndexedCollection
  define: 'whatAmI' toBe: "IndexedCollection" .
  define: 'isIndexedCollection' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isIndexedCollection' toBe: FALSE ;

BuiltInWorkspace define: 'IndexedList' toBe: Prototypical IndexedList ;
IndexedList
  define: 'whatAmI' toBe: "IndexedList" .
  define: 'isIndexedList' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isIndexedList' toBe: FALSE ;

!TimeSeries <- Prototypical TimeSeries ;
TimeSeries
  define: 'whatAmI' toBe: "TimeSeries" .
  define: 'isTimeSeries' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isTimeSeries' toBe: FALSE ;

BuiltInWorkspace define: 'SequencedCollection' toBe: Prototypical SequencedCollection ;
SequencedCollection
  define: 'whatAmI' toBe: "SequencedCollection" .
  define: 'isSequencedCollection' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isSequencedCollection' toBe: FALSE ;

BuiltInWorkspace define: 'List' toBe: Prototypical List ;
List
  define: 'whatAmI' toBe: "List" .
  define: 'isList' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isList' toBe: FALSE ;

!Value <- Prototypical Value ;
Value
  define: 'whatAmI' toBe: "Value" .
  define: 'isValue' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isValue' toBe: FALSE ;


####################
#  Ordinal
####################
BuiltInWorkspace define: 'Ordinal' toBe: Prototypical Ordinal ;
Ordinal
  define: 'whatAmI' toBe: "Ordinal" .
  define: 'isOrdinal' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isOrdinal' toBe: FALSE ;

BuiltInWorkspace define: 'Character' toBe: Prototypical Character ;
Character
  define: 'whatAmI' toBe: "Character" .
  define: 'isCharacter' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isCharacter' toBe: FALSE ;

BuiltInWorkspace define: 'Date' toBe: Prototypical Date ;
Date
  define: 'whatAmI' toBe: "Date" .
  define: 'isDate' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isDate' toBe: FALSE ;

BuiltInWorkspace define: 'DateOffset' toBe: Prototypical DateOffset ;
DateOffset
  define: 'whatAmI' toBe: "DateOffset" .
  define: 'isDateOffset' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isDateOffset' toBe: FALSE ;

BuiltInWorkspace define: 'BusinessDays' toBe: Prototypical BusinessDays ;
BusinessDays
  define: 'whatAmI' toBe: "BusinessDays" .
  define: 'isBusinessDays' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isBusinessDays' toBe: FALSE ;

BuiltInWorkspace define: 'Days' toBe: Prototypical Days ;
Days
  define: 'whatAmI' toBe: "Days" .
  define: 'isDays' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isDays' toBe: FALSE ;

BuiltInWorkspace define: 'MonthBeginnings' toBe: Prototypical MonthBeginnings ;
MonthBeginnings
  define: 'whatAmI' toBe: "MonthBeginnings" .
  define: 'isMonthBeginnings' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isMonthBeginnings' toBe: FALSE ;

BuiltInWorkspace define: 'MonthEnds' toBe: Prototypical MonthEnds ;
MonthEnds
  define: 'whatAmI' toBe: "MonthEnds" .
  define: 'isMonthEnds' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isMonthEnds' toBe: FALSE ;

BuiltInWorkspace define: 'Months' toBe: Prototypical Months ;
Months
  define: 'whatAmI' toBe: "Months" .
  define: 'isMonths' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isMonths' toBe: FALSE ;

BuiltInWorkspace define: 'QuarterBeginnings' toBe: Prototypical QuarterBeginnings ;
QuarterBeginnings
  define: 'whatAmI' toBe: "QuarterBeginnings" .
  define: 'isQuarterBeginnings' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isQuarterBeginnings' toBe: FALSE ;

BuiltInWorkspace define: 'QuarterEnds' toBe: Prototypical QuarterEnds ;
QuarterEnds
  define: 'whatAmI' toBe: "QuarterEnds" .
  define: 'isQuarterEnds' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isQuarterEnds' toBe: FALSE ;

BuiltInWorkspace define: 'Quarters' toBe: Prototypical Quarters ;
Quarters
  define: 'whatAmI' toBe: "Quarters" .
  define: 'isQuarters' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isQuarters' toBe: FALSE ;

BuiltInWorkspace define: 'YearBeginnings' toBe: Prototypical YearBeginnings ;
YearBeginnings
  define: 'whatAmI' toBe: "YearBeginnings" .
  define: 'isYearBeginnings' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isYearBeginnings' toBe: FALSE ;

BuiltInWorkspace define: 'YearEnds' toBe: Prototypical YearEnds ;
YearEnds
  define: 'whatAmI' toBe: "YearEnds" .
  define: 'isYearEnds' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isYearEnds' toBe: FALSE ;

BuiltInWorkspace define: 'Years' toBe: Prototypical Years ;
Years
  define: 'whatAmI' toBe: "Years" .
  define: 'isYears' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isYears' toBe: FALSE ;

BuiltInWorkspace define: 'Number' toBe: Prototypical Number ;
Number
  define: 'whatAmI' toBe: "Number" .
  define: 'isNumber' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isNumber' toBe: FALSE ;

BuiltInWorkspace define: 'Double' toBe: Prototypical Double ;
Double
  define: 'whatAmI' toBe: "Double" .
  define: 'isDouble' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isDouble' toBe: FALSE ;

BuiltInWorkspace define: 'Float' toBe: Prototypical Float ;
Float
  define: 'whatAmI' toBe: "Float" .
  define: 'isFloat' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isFloat' toBe: FALSE ;

BuiltInWorkspace define: 'Integer' toBe: Prototypical Integer ;
Integer
  define: 'whatAmI' toBe: "Integer" .
  define: 'isInteger' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isInteger' toBe: FALSE ;

BuiltInWorkspace define: 'String' toBe: Prototypical String ;
String
  define: 'whatAmI' toBe: "String" .
  define: 'isString' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isString' toBe: FALSE ;

#####################
#  Selector
#####################
BuiltInWorkspace define: 'Selector' toBe: Prototypical Selector ;
Selector
  define: 'whatAmI' toBe: "Selector" .
  define: 'isSelector' toBe: TRUE .
 defineAsMessage: 'asSelf' ;
Object define: 'isSelector' toBe: FALSE ;

####################
#  Undefined
####################
BuiltInWorkspace define: 'Undefined' toBe: NA ; 
Undefined
  define: 'whatAmI' toBe: "Undefined" .
  define: 'isUndefined' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isUndefined' toBe: FALSE ;

BuiltInWorkspace define: 'NoValue' toBe: [ ; ] ;
NoValue
  define: 'whatAmI' toBe: "NoValue" .
  define: 'isNoValue' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isNoValue' toBe: FALSE ;

####################
#  Utility   
####################

Utility
  define: 'whatAmI' toBe: "Utility" .
  define: 'isUtility' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isUtility' toBe: FALSE ;

Utility UnixSeconds
  define: 'whatAmI' toBe: "Utility UnixSeconds" .
  define: 'isUnixSeconds' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isUnixSeconds' toBe: FALSE ;

Utility SessionAttribute
  define: 'whatAmI' toBe: "Utility SessionAttribute" .
  define: 'isSessionAttribute' toBe: TRUE .
  defineAsMessage: 'asSelf' ;
Object define: 'isSessionAttribute' toBe: FALSE ;

