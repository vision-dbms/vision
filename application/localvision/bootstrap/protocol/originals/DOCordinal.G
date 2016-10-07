#--------------------
#        Ordinal
#           Character
#           Date
#           DateOffset
#              BusinessDays
#              Days
#              MonthBeginnings
#              MonthEnds
#              Months
#              QuarterBeginnings
#              QuarterEnds
#              Quarters
#              YearBeginnings
#              YearEnds
#              Years
#           Number
#              Double
#              Float
#              Integer
#           String
#--------------------

Ordinal getMessage: "<".
setSelectorTo: "< aNumber" .
setDescriptionTo: 
"Tests if recipient is smaller than supplied value.  Each subclass of Ordinal 
must redefine this message.  If a subclass does not implement <, then an 
attempt to send the message to a subclass results in an error message of the 
form: >>> '<' must be implemented for 'recipient' <<<.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Number.
;

Ordinal getMessage: "<=".
setSelectorTo: "<= aNumber" .
setDescriptionTo: 
"Tests if recipient is smaller than or equal to supplied value.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Number.
;

Ordinal getMessage: ">".
setSelectorTo: "> aNumber" .
setDescriptionTo: "Tests if recipient is larger than supplied value.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Number.
;

Ordinal getMessage: ">=".
setSelectorTo: ">= aNumber" .
setDescriptionTo: 
"Tests if recipient is larger than or equal to supplied value.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Number.
;

Ordinal getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: Ordinal.
setFunctionTypeTo: "Conversion".
;

Ordinal getMessage: "between:and:" .
setSelectorTo: "between: anOrdinal1 and: anOrdinal2" .
setDescriptionTo: "Tests if recipient Ordinal is between supplied Ordinal1 and
supplied Ordinal2" .
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: "Ordinal" .
setParameter: 2 typeTo: "Ordinal" .
;

Ordinal getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

Ordinal getMessage: "defaultInstance".
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: Ordinal.
setFunctionTypeTo: "Schema" .
;

Ordinal getMessage: "inRange:".
setSelectorTo: "Ordinal inRange: aList" .
setDescriptionTo: 
"Tests if recipient Ordinal is in range specified by aList, where aList
contains 2 elements - the start and end of the range (inclusive)." .
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison" .
setParameter: 1 typeTo: "List" .
;

Ordinal getMessage: "inSet:".
setSelectorTo: "inSet: aList" .
setDescriptionTo: "Tests if recipient Ordinal is included in supplied list." .
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison" .
setParameter: 1 typeTo: "List" .
;

Ordinal getMessage: "isOrdinal".
setDescriptionTo: "Returns a TRUE value when sent to any Ordinal.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

Ordinal getMessage: "new".
setDescriptionTo: "Undefines the 'new' message for Ordinal classes." .
setReturnObjectTypeTo: Ordinal.
setFunctionTypeTo: "Creation" .
;

Ordinal getMessage: "notBetween:and:".
setSelectorTo: "notBetween: anOrdinal1 and: anOrdinal2" .
setDescriptionTo: 
"Tests if recipient Ordinal is not between supplied Ordinal1
and supplied Ordinal2." .
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison" .
setParameter: 1 typeTo: "Ordinal" .
setParameter: 2 typeTo: "Ordinal" .
;

Ordinal getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'Ordinal'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

#--------------------

Character getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: Character.
setFunctionTypeTo: "Conversion".
;

Character getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

Character getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: Character.
setFunctionTypeTo: "Schema" .
;

Character getMessage: "isCharacter".
setDescriptionTo: "Returns the value TRUE when sent to any Character.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

Character getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'Character'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;


#--------------------

Date getMessage: "DayOfWeekDescriptor".
setDescriptionTo: "Returns default 'Date DayOfWeekDescriptor' object" .
setReturnObjectTypeTo: Date DayOfWeekDescriptor .
setFunctionTypeTo: "Constant".
; 

Date getMessage: "MonthDescriptor".
setDescriptionTo: "Returns default 'Date MonthDescriptor' object" .
setReturnObjectTypeTo: Date MonthDescriptor .
setFunctionTypeTo: "Constant".
; 


Date getMessage: "!=".
setSelectorTo: "!= aDate" .
setDescriptionTo: "Tests if recipient date is the same as the supplied date.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Date.
; 

Date getMessage: "+".
setSelectorTo: "+ aDateOffset" .
setDescriptionTo: "Adds the supplied date increment to the recipient date.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateOffset.
;

Date getMessage: "-".
setSelectorTo: "- aDateOffset" .
setDescriptionTo: 
"Subtracts the supplied date increment from the recipient date".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateOffset.
;

Date getMessage: "<".
setSelectorTo: "< aDate" .
setDescriptionTo: "Tests if recipient date comes before supplied date.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Date.
;

Date getMessage: "<=".
setSelectorTo: "<= aDate" .
setDescriptionTo: 
"Tests if recipient date is the same or earlier than supplied date.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Date.
;

Date getMessage: "=".
setSelectorTo: "= aDate" .
setDescriptionTo: "Tests if recipient date is the same as the supplied date.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Date.
;

Date getMessage: ">".
setSelectorTo: "> aDate" .
setDescriptionTo: "Tests if recipient date is later than supplied date.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Date.
;

Date getMessage: ">=".
setSelectorTo: ">= aDate" .
setDescriptionTo: 
"Tests if recipient date is the same or earlier than supplied date.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Date.
;

Date getMessage: "asBDay".
setDescriptionTo: 
"Converts the recipient date to a business day (Monday - Friday).  If the 
recipient date is a Monday through Friday date, that date is returned.  If the 
recipient date is a Saturday or Sunday date, the previous Friday's date is 
returned.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Conversion".
;

Date getMessage: "asBDayMonday".
setDescriptionTo: 
"Converts the recipient date to a business day (Monday - Friday).  If the 
recipient date is a Monday through Friday date, that date is returned.  If the 
recipient date is a Saturday or Sunday date, the following Monday's date is 
returned.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Conversion".
;

Date getMessage: "asCurrentYearMMDD".
setDescriptionTo: "Converts recipient date to same date in current year.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Conversion".
;

Date getMessage: "asDate".
setDescriptionTo: "Returns recipient stripped of any extensions.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Conversion".
;

Date getMessage: "asIDate7".
setDescriptionTo: 
"Converts recipient into an integer representing the number of days since 
January 1, 0001.  Useful for computing the number of days between two dates.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Conversion".
;

Date getMessage: "asInteger".
setDescriptionTo: "Converts a date into an integer in YYYYMMDD form.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Conversion".
;

Date getMessage: "asMonthEnd".
setDescriptionTo: 
"Returns the month-end date for the Date represented by the recipient.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Conversion".
;

Date getMessage: "asMostRecentBusinessMonthEnd".
setDescriptionTo: 
"Returns the last business day in the month on or before the recipient date.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Conversion".
;

Date getMessage: "asMostRecentMonthEnd".
setDescriptionTo: 
"Returns the last month-end date relative to the recipient date.  If 
recipient is not a month-end date, the previous month-end date is returned.  
If recipient is a month-end date, it is returned.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Conversion".
;

Date getMessage: "asQuarterEnd".
setDescriptionTo: 
"Returns the quarter-end date for the Date represented by the recipient.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Conversion".
;

Date getMessage: "asYearEnd".
setDescriptionTo: 
"Returns the year-end date for the Date represented by the recipient.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Conversion".
;

Date getMessage: "asYearBeginning".
setDescriptionTo: 
"Returns the year-beginning date for the Date represented by the recipient.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Conversion".
;

Date getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Conversion".
;

Date getMessage: "by:from:evaluate:".
setSelectorTo: "by: aDateOffset from: aDate evaluate: aBlock" .
setDescriptionTo: 
"Evaluates supplied Block for recipient Date by supplied increment from 
supplied Date".
setFunctionTypeTo: "Conversion".
setParameter: 1 typeTo: DateOffset.
setParameter: 2 typeTo: Date.
setParameter: 3 typeTo: Block.
;

Date getMessage: "by:to:evaluate:".
setSelectorTo: "by: aDateOffset to: aDate evaluate: aBlock" .
setDescriptionTo: 
"Evaluates supplied Block for recipient Date by supplied increment to 
supplied Date.".
setFunctionTypeTo: "Conversion".
setParameter: 1 typeTo: DateOffset.
setParameter: 2 typeTo: Date.
setParameter: 3 typeTo: Block.
;

Date getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

Date getMessage: "count30DayDaysTo:".
setSelectorTo: "count30DayDaysTo: aDate" .
setDescriptionTo: 
"Counts the number of days between original date and date supplied, assuming 
all months have 30 days.  Assumes original date is earlier than date 
supplied.  Used primarily in yield-to-maturity type applications.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Date.
;

Date getMessage: "count360DayYearsTo:".
setSelectorTo: "count360DayYearsTo: aDate" .
setDescriptionTo: 
"Counts the number of years between original date and date supplied, 
assuming all months have 30 days (all years have 360 days).  Assumes original 
date is earlier than date supplied.  Used primarily in yield-to-maturity type 
applications.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Date.
;

Date getMessage: "count6monthIntervalsTo:".
setSelectorTo: "count6monthIntervalsTo: aDate" .
setDescriptionTo: 
"Counts the number of 6 month intervals between two dates. Assumes original 
date is earlier than supplied date.  Used primarily in bond applications - 
interpreted as number of full coupon payments remaining. Assumes end point is 
at a six month interval.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Date.
;

Date getMessage: "countDaysTo:".
setSelectorTo: "countDaysTo: aDate" .
setDescriptionTo: 
"Counts the number of days between recipient date and date supplied.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Date.
;

Date getMessage: "countBDaysTo:".
setSelectorTo: "countBDaysTo: aDate" .
setDescriptionTo: 
"Counts the number of business days between recipient date and date supplied.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Date.
;

Date getMessage: "countFullMonthsTo:".
setSelectorTo: "countFullMonthsTo: aDate" .
setDescriptionTo: "Computes number of FULL months between two dates.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Date.
;

Date getMessage: "countMonthsTo:".
setSelectorTo: "countMonthsTo: aDate" .
setDescriptionTo: "Computes number of months between two dates.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Date.
;

Date getMessage: "countIntervalsOf:to:".
setSelectorTo: "countIntervalsOf: aDateOffset to: aDate" .
setDescriptionTo: 
"Counts the number of full intervals between recipient date and date supplied".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: DateOffset.
setParameter: 2 typeTo: Date.
;

Date getMessage: "countYearsTo:".
setSelectorTo: "countYearsTo: aDate" .
setDescriptionTo: 
"Computes the number of years (full and fractional) between two dates.  A full 
year is assumed to have 365.25 days.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Date.
;

Date getMessage: "day".
setDescriptionTo: "Returns the 'day' portion of recipient date.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Descriptive".
;

Date getMessage: "dayNumberOfWeek".
setDescriptionTo: 
"Returns a number (1 - 7) indicating recipient's day of week.  1 is Monday; 
7 is Sunday.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Inquiry".
;

Date getMessage: "dayOfWeek".
setDescriptionTo: "Returns day of week as a string (e.g. Monday).".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Inquiry".
;

Date getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Schema" .
;

Date getMessage: "evaluate:".
setSelectorTo: "evaluate: aBlock" .
setDescriptionTo: 
"Evaluates all statements in the block as of recipient date.  The recipient 
date becomes the default inside the block (changing the value of the 
magic word ^date).  The default date would otherwise be the current date 
(accessible by the magic word ^today).".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
;

Date getMessage: "extract:for:".
setSelectorTo: "extract: block for: object".
setDescriptionTo: 
"This message evaluates the supplied block for the supplied object and returns
the value as of the recipient date." .
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Object .
;
Date getMessage: "formatUsingLongName".
setDescriptionTo: "Returns date as string in the form March 31, 1991.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
;

Date getMessage: "formatUsingMMDD".
setDescriptionTo: "Returns date as string in the form 3/31.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
;

Date getMessage: "formatUsingMMDDYY".
setDescriptionTo: "Returns date as string in the form 3/31/91.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
;

Date getMessage: "formatUsingMMDDYYYY".
setDescriptionTo: "Returns date as string in the form 3/31/1991".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
;

Date getMessage: "formatUsingShortName".
setDescriptionTo: "Returns date as string in the form 31-Mar-1991.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
;

Date getMessage: "getDayOfWeekDescriptor".
setDescriptionTo: "Returns DayOfWeekDescriptor for current date.".
setReturnObjectTypeTo: Date DayOfWeekDescriptor.
setFunctionTypeTo: "Access".
;

Date getMessage: "getMonthDescriptor".
setDescriptionTo: "Returns MonthDescriptor for current date.".
setReturnObjectTypeTo: Date MonthDescriptor.
setFunctionTypeTo: "Access".
;

Date getMessage: "inputExpression".
setDescriptionTo: "Used by DDMS ToolKit to format recipient as input." .
setFunctionTypeTo: "Format".
setLevelTo: "Advanced" .
;

Date getMessage: "isDefault".
setDescriptionTo: "Returns TRUE if date is earliest possible date." .
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Inquiry".
;

Date getMessage: "isDate".
setDescriptionTo: "Returns a TRUE value when sent to any date.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

Date getMessage: "isInitialDate".
setDescriptionTo: 
"Returns TRUE if date is earliest possible date, otherwise returns FALSE.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Inquiry".
;

Date getMessage: "isLastBusinessDayOfMonth".
setDescriptionTo: 
"Determines if recipient date is the last business day of the month.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Inquiry".
;

Date getMessage: "isMonthEnd".
setDescriptionTo: 
"Determines whether recipient date is the last day of the month.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Inquiry".
;

Date getMessage: "isValid".
setDescriptionTo: 
"Returns TRUE if recipient date is greater than or equal to earliest possible 
date (January 1, 0001).".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Inquiry".
;

Date getMessage: "isntValid".
setDescriptionTo: 
"Returns TRUE if recipient date less than earliest possible date 
(January 1, 0001).".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Inquiry".
;

Date getMessage: "month".
setDescriptionTo: "Returns the 'month' portion of recipient date.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Descriptive".
;

Date getMessage: "quarterNumber".
setDescriptionTo: "Returns the quarter (1-4) of the recipient date.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Descriptive".
;

Date getMessage: "print".
setDescriptionTo: 
"Redefines the standard print message so that the value of the recipient date 
is printed.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Display".
;

Date getMessage: "print:".
setSelectorTo: "print: anInteger" .
setDescriptionTo: 
"Prints the recipient date using the supplied format.  If the width is less 
than 8 characters, the format is mm/yy padded with spaces at the right.  If 
the width is 8 characters, the format is mm/dd/yy.  If the width is 9 
characters, the format is mm/dd/yy_.  Otherwise, the format is mm/dd/yyyy 
padded with spaces at the right.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Display".
setParameter: 1 typeTo: Integer.
;

Date getMessage: "to:by:".
setSelectorTo: "to: aDate by: aDateOffset" .
setDescriptionTo: 
"Produces a Date Range from two Dates and a Date Increment supplied.  Dates 
can be in ascending or descending order.".
setReturnObjectTypeTo: DateRange.
setFunctionTypeTo: "Conversion".
setParameter: 1 typeTo: Date.
setParameter: 2 typeTo: DateOffset.
;

Date getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'Date'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

Date getMessage: "year".
setDescriptionTo: "Retuns the 'year' portion of recipient date.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Descriptive".
;

#--------------------

DateOffset getMessage: "*".
setSelectorTo: "* aValue" .
setDescriptionTo: "Multiplies the recipient offset by the value supplied.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Value.
;

DateOffset getMessage: "+".
setSelectorTo: "+ aValue" .
setDescriptionTo: "Adds the recipient offset to the value supplied.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Value.
;

DateOffset getMessage: "asDate".
setDescriptionTo: "Adds the recipient offset to the current date (^date).".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Conversion".
;

DateOffset getMessage: "asInteger".
setDescriptionTo: "Converts a DateOffset into an integer in YYYYMMDD form.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Conversion".
;

DateOffset getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: DateOffset.
setFunctionTypeTo: "Conversion".
;

DateOffset getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

DateOffset getMessage: "defaultInstance" .
setDescriptionTo: "Returns the default instance of the recipient object's class".
setReturnObjectTypeTo: DateOffset.
setFunctionTypeTo: "Schema" .
;

DateOffset getMessage: "evaluate:".
setSelectorTo: "evaluate: aBlock" .
setDescriptionTo: 
"Adds the recipient to the current ^date and evaluates the supplied block.
For example:
     -3 monthEnds evaluate: [ ^date printNL ] ;
" .
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
;

DateOffset getMessage: "evaluate:from:to:".
setSelectorTo: "evaluate: aBlock from: aDate to: aDate" .
setDescriptionTo: 
"Evaluates the supplied block for the date range formed by date1, date2, and 
the recipient DateOffset.".
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Date.
setParameter: 3 typeTo: Date.
;

DateOffset getMessage: "incrementName".
setDescriptionTo: 
"Returns the name portion of the recipient DateOffset as a String.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Descriptive".
;

DateOffset getMessage: "isDateOffset".
setDescriptionTo: "Returns a TRUE value when sent to any DateOffset.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

DateOffset getMessage: "print".
setDescriptionTo: 
"Redefines the standard print message so that the content of the actual date 
offset is printed.".
setReturnObjectTypeTo: DateOffset.
setFunctionTypeTo: "Display".
;

DateOffset getMessage: "subtractedFrom:".
setSelectorTo: "subtractedFrom: aDate" .
setDescriptionTo: 
"Subtracts the recipient DateOffset from the supplied Date.".
setReturnObjectTypeTo: Date.
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: Date.
;

DateOffset getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'DateOffset'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

#--------------------

BusinessDays getMessage: "asDateOffset".
setDescriptionTo: "Returns recipient.".
setReturnObjectTypeTo: DateOffset.
setFunctionTypeTo: "Conversion".
;

BusinessDays getMessage: "asInteger".
setDescriptionTo: "Converts a BusinessDays into an integer in YYYYMMDD form.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Conversion".
;

BusinessDays getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: BusinessDays.
setFunctionTypeTo: "Conversion".
;

BusinessDays getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

BusinessDays getMessage: "decrement:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

BusinessDays getMessage: "defaultInstance" .
setDescriptionTo: "Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: BusinessDays.
setFunctionTypeTo: "Schema" .
;

BusinessDays getMessage: "from:to:evaluate:".
setSelectorTo: "from: aDate to: aDate evaluate: aBlock" .
setDescriptionTo: 
"Evaluates supplied Block for the date range formed by Date1, Date2, and 
recipient BusinessDays" .
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Date.
setParameter: 2 typeTo: Date.
setParameter: 3 typeTo: Block.
;

BusinessDays getMessage: "increment:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

BusinessDays getMessage: "incrementName".
setDescriptionTo: 
"Returns the name portion of the recipient BusinessDays as a string.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Descriptive".
;

BusinessDays getMessage: "isBusinessDays".
setDescriptionTo: "Returns the value TRUE when sent to any BusinessDays.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

BusinessDays getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'BusinessDays'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema"
;

#--------------------

Days getMessage: "asDateOffset".
setDescriptionTo: "Returns recipient Days as DateOffset.".
setReturnObjectTypeTo: DateOffset.
setFunctionTypeTo: "Conversion".
;

Days getMessage: "asInteger".
setDescriptionTo: "Converts a Days into an integer in YYYYMMDD form.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Conversion".
;

Days getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: Days.
setFunctionTypeTo: "Conversion".
;

Days getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

Days getMessage: "decrement:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Days getMessage: "defaultInstance".
setDescriptionTo: 
"Returns the default instance of the recipient object's class".
setReturnObjectTypeTo: Days.
setFunctionTypeTo: "Schema".
;

Days getMessage: "from:to:evaluate:".
setSelectorTo: "from: aDate to: aDate evaluate: aBlock" .
setDescriptionTo: 
"Evaluates supplied Block for the date range formed by Date1, Date2, and the 
recipient Days" .
setReturnObjectTypeTo: Days.
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Date.
setParameter: 2 typeTo: Date.
setParameter: 3 typeTo: Block.
;

Days getMessage: "increment:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Days getMessage: "incrementName".
setDescriptionTo: 
"Returns the name portion of the recipient Days as a String.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Descriptive".
;

Days getMessage: "isDays".
setDescriptionTo: "Returns a TRUE value when sent to any Day.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

Days getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'Days'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

#--------------------

MonthBeginnings getMessage: "asDateOffset" .
setDescriptionTo: "Returns recipient MonthBeginnings as DateOffset" .
setReturnObjectTypeTo: DateOffset.
setFunctionTypeTo: "Conversion" .
;

MonthBeginnings getMessage: "asInteger".
setDescriptionTo: "Converts a MonthBeginnings into an integer in YYYYMMDD form.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Conversion".
;

MonthBeginnings getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: MonthBeginnings.
setFunctionTypeTo: "Conversion".
;

MonthBeginnings getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

MonthBeginnings getMessage: "decrement:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

MonthBeginnings getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: MonthBeginnings.
setFunctionTypeTo: "Schema" .
;

MonthBeginnings getMessage: "from:to:evaluate:" .
setSelectorTo: "from: aDate to: aDate evaluate: aBlock" .
setDescriptionTo: 
"Evaluates the supplied block for the date range formed by date1, date2, and 
the recipient MonthBeginnings.".
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Date.
setParameter: 2 typeTo: Date.
setParameter: 3 typeTo: Block.
;

MonthBeginnings getMessage: "increment:" .
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

MonthBeginnings getMessage: "incrementName" .
setDescriptionTo: 
"Returns the name portion of the recipient MonthBeginnings as a string" .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Descriptive" .
;

MonthBeginnings getMessage: "isMonthBeginnings".
setDescriptionTo: "Returns the value TRUE when sent to any monthBeginning.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

MonthBeginnings getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'MonthBeginnings'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;


#--------------------

MonthEnds getMessage: "asDateOffset" .
setDescriptionTo: "Returns the recipient MonthEnds as DateOffset" .
setReturnObjectTypeTo: DateOffset.
setFunctionTypeTo: "Conversion" .
;

MonthEnds getMessage: "asInteger".
setDescriptionTo: "Converts a MonthEnds into an integer in YYYYMMDD form.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Conversion".
;

MonthEnds getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: MonthEnds.
setFunctionTypeTo: "Conversion".
;

MonthEnds getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

MonthEnds getMessage: "decrement:" .
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

MonthEnds getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: MonthEnds.
setFunctionTypeTo: "Schema" .
;

MonthEnds getMessage: "from:to:evaluate:" .
setSelectorTo: "from: aDate to: aDate evaluate: aBlock" .
setDescriptionTo: 
"Evaluates the supplied block for the date range formed by date1, date2, and 
the recipient MonthEnds.".
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Date.
setParameter: 2 typeTo: Date.
setParameter: 3 typeTo: Block.
;

MonthEnds getMessage: "increment:" .
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

MonthEnds getMessage: "incrementName" .
setDescriptionTo: 
"Returns the name portion of the recipient MonthEnds as a string." .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Descriptive" .
;

MonthEnds getMessage: "isMonthEnds".
setDescriptionTo: "Returns the value TRUE when sent to any MonthEnd.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

MonthEnds getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'MonthEnds'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;


#--------------------

Months getMessage: "asDateOffset" .
setDescriptionTo: "Returns the recipient Months as a DateOffset" .
setReturnObjectTypeTo: DateOffset.
setFunctionTypeTo: "Conversion" .
;

Months getMessage: "asInteger".
setDescriptionTo: "Converts a Months into an integer in YYYYMMDD form.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Conversion".
;

Months getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: Months.
setFunctionTypeTo: "Conversion".
;

Months getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

Months getMessage: "decrement:" .
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Months getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: Months.
setFunctionTypeTo: "Schema" .
;

Months getMessage: "from:to:evaluate:" .
setSelectorTo: "from: aDate to: aDate evaluate: aBlock" .
setDescriptionTo: 
"Evaluates the supplied block for the date range formed by date1, date2, and 
the recipient Months.".
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Date.
setParameter: 2 typeTo: Date.
setParameter: 3 typeTo: Block.
;

Months getMessage: "increment:" .
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Months getMessage: "incrementName" .
setDescriptionTo: 
"Returns the name portion of the recipient Months as a String" .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Descriptive" .
;

Months getMessage: "isMonths".
setDescriptionTo: "Returns the value TRUE when sent to any Month.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

Months getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'Months'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;


#--------------------

QuarterBeginnings getMessage: "asDateOffset" .
setDescriptionTo: "Returns the recipient QuarterBeginnings as DateOffset" .
setReturnObjectTypeTo: DateOffset.
setFunctionTypeTo: "Conversion" .
;

QuarterBeginnings getMessage: "asInteger".
setDescriptionTo: "Converts a QuarterBeginnings into an integer in YYYYMMDD form.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Conversion".
;

QuarterBeginnings getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: QuarterBeginnings.
setFunctionTypeTo: "Conversion".
;

QuarterBeginnings getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

QuarterBeginnings getMessage: "decrement:" .
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

QuarterBeginnings getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: QuarterBeginnings.
setFunctionTypeTo: "Schema" .
;

QuarterBeginnings getMessage: "from:to:evaluate:" .
setSelectorTo: "from: aDate to: aDate evaluate: aBlock" .
setDescriptionTo: 
"Evaluates the supplied block for the date range formed by date1, date2, and 
the recipient QuarterBeginnings.".
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Date.
setParameter: 2 typeTo: Date.
setParameter: 3 typeTo: Block.
;

QuarterBeginnings getMessage: "increment:" .
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

QuarterBeginnings getMessage: "incrementName" .
setDescriptionTo: 
"Returns the name portion of the recipient QuarterBeginnings as a string" .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Descriptive" .
;

QuarterBeginnings getMessage: "isQuarterBeginnings".
setDescriptionTo: "Returns a TRUE value when sent to any QuarterBeginning.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

QuarterBeginnings getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'QuarterBeginnings'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;


#--------------------

QuarterEnds getMessage: "asDateOffset" .
setDescriptionTo: "Returns the recipient QuarterEnds as DateOffset" .
setReturnObjectTypeTo: DateOffset.
setFunctionTypeTo: "Conversion" .
;

QuarterEnds getMessage: "asInteger".
setDescriptionTo: "Converts a QuarterEnds into an integer in YYYYMMDD form.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Conversion".
;

QuarterEnds getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: QuarterEnds.
setFunctionTypeTo: "Conversion".
;

QuarterEnds getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

QuarterEnds getMessage: "decrement:" .
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

QuarterEnds getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: QuarterEnds.
setFunctionTypeTo: "Schema" .
;

QuarterEnds getMessage: "from:to:evaluate:" .
setSelectorTo: "from: aDate to: aDate: evaluate: aBlock" .
setDescriptionTo: 
"Evaluates the supplied block for the date range formed by date1, date2, and 
the recipient QuarterEnds.".
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Date.
setParameter: 2 typeTo: Date.
setParameter: 3 typeTo: Block.
;

QuarterEnds getMessage: "increment:" .
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

QuarterEnds getMessage: "incrementName" .
setDescriptionTo: 
"Returns the name portion of the recipient QuarterEnds as a string." .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Descriptive" .
;

QuarterEnds getMessage: "isQuarterEnds".
setDescriptionTo: "Returns a TRUE value when sent to any QuarterEnd.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

QuarterEnds getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'QuarterEnds'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;


#--------------------

Quarters getMessage: "asDateOffset" .
setDescriptionTo: "Returns the recipient Quarters as DateOffset." .
setReturnObjectTypeTo: DateOffset.
setFunctionTypeTo: "Conversion" .
;

Quarters getMessage: "asInteger".
setDescriptionTo: "Converts a Quarters into an integer in YYYYMMDD form.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Conversion".
;

Quarters getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: Quarters.
setFunctionTypeTo: "Conversion".
;

Quarters getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

Quarters getMessage: "decrement:" .
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Quarters getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: Quarters.
setFunctionTypeTo: "Schema" .
;

Quarters getMessage: "from:to:evaluate:" .
setSelectorTo: "from: aDate to: aDate evaluate: aBlock".
setDescriptionTo: 
"Evaluates the supplied block for the date range formed by date1, date2, and 
the recipient Quarters.".
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Date.
setParameter: 2 typeTo: Date.
setParameter: 3 typeTo: Block.
;

Quarters getMessage: "increment:" .
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Quarters getMessage: "incrementName" .
setDescriptionTo: 
"Returns the name portion of the recipient Quarters as a string." .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Descriptive" .
;

Quarters getMessage: "isQuarters".
setDescriptionTo: "Returns the value TRUE when sent to any Quarters.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

Quarters getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'Quarters'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;


#--------------------

YearBeginnings getMessage: "asDateOffset" .
setDescriptionTo: "Returns recipient YearBeginnings as DateOffset." .
setReturnObjectTypeTo: DateOffset.
setFunctionTypeTo: "Conversion" .
;

YearBeginnings getMessage: "asInteger".
setDescriptionTo: "Converts a YearBeginnings into an integer in YYYYMMDD form.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Conversion".
;

YearBeginnings getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: YearBeginnings.
setFunctionTypeTo: "Conversion".
;

YearBeginnings getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

YearBeginnings getMessage: "decrement:" .
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

YearBeginnings getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: YearBeginnings.
setFunctionTypeTo: "Schema" .
;

YearBeginnings getMessage: "from:to:evaluate:" .
setSelectorTo: "from: aDate to: aDate evaluate: aBlock" .
setDescriptionTo: 
"Evaluates the supplied block for the date range formed by date1, date2, and 
the recipient YearBeginnings.".
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Date.
setParameter: 2 typeTo: Date.
setParameter: 3 typeTo: Block.
;

YearBeginnings getMessage: "increment:" .
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

YearBeginnings getMessage: "incrementName" .
setDescriptionTo: 
"Returns the name portion of the recipient YearBeginnings as a string." .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Descriptive" .
;

YearBeginnings getMessage: "isYearBeginnings".
setDescriptionTo: "Returns a TRUE value when sent to any QuarterBeginning.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

YearBeginnings getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'YearBeginnings'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;


#--------------------

YearEnds getMessage: "asDateOffset" .
setDescriptionTo: "Returns the recipient YearEnds as a DateOffset." .
setReturnObjectTypeTo: DateOffset.
setFunctionTypeTo: "Conversion" .
;

YearEnds getMessage: "asInteger".
setDescriptionTo: "Converts a YearEnds into an integer in YYYYMMDD form.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Conversion".
;

YearEnds getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: YearEnds.
setFunctionTypeTo: "Conversion".
;

YearEnds getMessage: "classDescriptor".
setDescriptionTo: "Reference to the object which describes the class.  Class
level information is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

YearEnds getMessage: "decrement:" .
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

YearEnds getMessage: "defaultInstance" .
setDescriptionTo: "Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: YearEnds.
setFunctionTypeTo: "Schema" .
;

YearEnds getMessage: "from:to:evaluate:" .
setSelectorTo: "from: aDate to: aDate evaluate: aBlock" .
setDescriptionTo: 
"Evaluates the supplied block for the date range formed by date1, date2, and 
the recipient DateOffset.".
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Date.
setParameter: 2 typeTo: Date.
setParameter: 3 typeTo: Block.
;

YearEnds getMessage: "increment:" .
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

YearEnds getMessage: "incrementName" .
setDescriptionTo: 
"Returns the name portion of the recipient YearEnds as a string." .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Descriptive" .
;

YearEnds getMessage: "isYearEnds".
setDescriptionTo: "Returns a TRUE value when sent to any YearEnd.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

YearEnds getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'YearEnds'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;


#--------------------

Years getMessage: "asDateOffset" .
setDescriptionTo: "Returns the recipient Years as a DateOffset." .
setReturnObjectTypeTo: DateOffset.
setFunctionTypeTo: "Conversion" .
;

Years getMessage: "asInteger".
setDescriptionTo: "Converts a Years into an integer in YYYYMMDD form.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Conversion".
;

Years getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: Years.
setFunctionTypeTo: "Conversion".
;

Years getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

Years getMessage: "decrement:" .
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Years getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: Years.
setFunctionTypeTo: "Schema" .
;

Years getMessage: "from:to:evaluate:" .
setSelectorTo: "from: aDate to: aDate evaluate: aBlock" .
setDescriptionTo: 
"Evaluates the supplied block for the date range formed by date1, date2, and 
the recipient Years.".
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Date.
setParameter: 2 typeTo: Date.
setParameter: 3 typeTo: Block.
;

Years getMessage: "increment:" .
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Years getMessage: "incrementName" .
setDescriptionTo: 
"Returns the name portion of the recipient Years as a string." .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Descriptive" .
;

Years getMessage: "isYears".
setDescriptionTo: "Returns the value TRUE when sent to any quarter.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

Years getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'Years'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

#--------------------

Number getMessage: "!=".
setSelectorTo: "!= aValue" .
setDescriptionTo: "Tests if supplied value is not equal to recipient Number.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Value.
;

Number getMessage: ">".
setSelectorTo: "> aValue" .
setDescriptionTo: "Tests if recipient Number is greater than supplied value.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Value.
;

Number getMessage: ">=".
setSelectorTo: ">= aValue" .
setDescriptionTo: 
"Tests if recipient Number is greater than or equal to supplied value.  If 
invalid parameter is supplied, NA is returned.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Value.
;

Number getMessage: "absoluteValue".
setDescriptionTo: 
"Computes the absolute value of the Number.  Non-negative values return 
unchanged; negative values returned without the negative sign.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
;

Number getMessage: "arcCosine".
setDescriptionTo: "Basic mathematical function.".
setReturnObjectTypeTo: Undefined.
setFunctionTypeTo: "Computation".
;

Number getMessage: "arcSine".
setDescriptionTo: "Basic mathematical function.".
setReturnObjectTypeTo: Undefined.
setFunctionTypeTo: "Computation".
;

Number getMessage: "arcTangent".
setDescriptionTo: "Basic mathematical function.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Number getMessage: "asAPowerOf:".
setSelectorTo: "asAPowerOf: baseNumber" .
setDescriptionTo: 
"Raises the baseNumber to the recipient Number power 
(baseNumber ^ recipient).".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Number.
;

Number getMessage: "asDate".
setDescriptionTo: "Converts the recipient Number into a Date.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Conversion".
;

Number getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Conversion".
;

Number getMessage: "base0CellPositionOf:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Number getMessage: "base1CellPositionOf:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Number getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

Number getMessage: "cosine".
setDescriptionTo: "Basic mathematical function.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Number getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Schema" .
;

Number getMessage: "exp".
setDescriptionTo: "Raises the mathematical constant e (~ 2.7183) to the
recipient Number power (i.e., e ^ recipient).".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Number getMessage: "hyperbolicCosine".
setDescriptionTo: "Basic mathematical function.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Number getMessage: "hyperbolicSine".
setDescriptionTo: "Basic mathematical function.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Number getMessage: "hyperbolicTangent".
setDescriptionTo: "Basic mathematical function.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Number getMessage: "inputExpression".
setDescriptionTo: "Used by DDMS ToolKit to format recipient as input." .
setFunctionTypeTo: "Format".
setLevelTo: "Advanced" .
;

Number getMessage: "isNumber".
setDescriptionTo: "Returns a TRUE value when sent to any Number.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

Number getMessage: "log".
setDescriptionTo: 
"Computes the natural log of the recipient Number.  Recipient must be 
positive; the log is undefined for numbers less than or equal to 0.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Number getMessage: "log:".
setSelectorTo: "log: aNumber " .
setDescriptionTo: 
"Computes the log of the recipient number using supplied base.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Number .
;

Number getMessage: "max:".
setSelectorTo: "max: aNumber" .
setDescriptionTo: 
"Computes the larger of the supplied value and the recipient Number. If an 
invalid parameter is supplied, NA is returned.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Number.
;

Number getMessage: "min:".
setSelectorTo: "min: aNumber" .
setDescriptionTo: 
"Computes the smaller of the supplied value and the recipient Number. If an 
invalid parameter is supplied, NA is returned.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Number.
;

Number getMessage: "negated".
setDescriptionTo: "Multiplies the recipient Number by -1.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Number getMessage: "pctChangeTo:".
setSelectorTo: "pctChangeTo: aNumber" .
setDescriptionTo: 
"Computes the percentage change between the recipient and supplied Number.  
Uses the formula: (recipient / supplied Value - 1 ) * 100. If supplied 
parameter is 0, NA is returned.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Number.
;

Number getMessage: "print:".
setSelectorTo: "print: aNumber" .
setDescriptionTo: 
"Prints the recipient Number using the supplied format.  If no decimal is 
provided in the format, the default value of 2 decimal places is used.  When 
integers are printed, the decimal specification will be ignored.  If the 
specification width is not large enough, **** will be displayed instead of 
the value.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Display".
setParameter: 1 typeTo: Number.
;

Number getMessage: "printWithCommas:".
setSelectorTo: "printWithCommas: aNumber" .
setDescriptionTo: 
"Prints the Number using the supplied format, inserting commas where 
appropriate.  If no decimal is provided in the format, the default value of 2 
decimal places is used.  When integers are printed, the decimal specification 
will be ignored.  If the specification width is not large enough, **** will be 
displayed instead of the value.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Display".
setParameter: 1 typeTo: Number.
;

Number getMessage: "printWithLeadingZeros:".
setSelectorTo: "printWithLeadingZeros: width" .
setDescriptionTo: 
"Displays the recipient with leading 0s to fill out the supplied width
if width is positive.  If width is negative, no padding is done.
" .
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Display".
setParameter: 1 typeTo: Number.
;

Number getMessage: "printWithLeadingZerosNL:".
setSelectorTo: "printWithLeadingZerosNL: width" .
setDescriptionTo: 
"Displays the recipient with leading 0s to fill out the supplied width
followed by a new line if width is positive.  If width is negative, no 
padding is done.
" .
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Display".
setParameter: 1 typeTo: Number.
;

Number getMessage: "round".
setDescriptionTo: "Rounds the Number to the nearest integer.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Computation".
;

Number getMessage: "round:".
setSelectorTo: "round: anInteger" .
setDescriptionTo: 
"Rounds the Number to the specified number of decimal places. Supplied integer 
must be non-negative.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Integer.
;

Number getMessage: "sequence".
setDescriptionTo: 
"Returns a list of integers from 1 through recipient Number.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Conversion".
;

Number getMessage: "sequence0".
setDescriptionTo: 
"Returns a list of integets from 0 through recipient minus 1.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Conversion".
;

Number getMessage: "sign".
setDescriptionTo: 
"Returns the value -1, 0, or 1, depending on the sign of the recipient." .
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Computation".
;

Number getMessage: "sine".
setDescriptionTo: "Basic mathematical function.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Number getMessage: "sprint:".
setSelectorTo: "sprint: aNumber" .
setDescriptionTo: 
"Returns the recipient number as a string formatted to the number of 
characters specified.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
setParameter: 1 typeTo: Number.
;

Number getMessage: "sprintWithCommas:".
setSelectorTo: "sprintWithCommas: aNumber" .
setDescriptionTo: 
"Returns the recipient number as a string formatted to the number of 
characters specified, inserting commas as appropriate.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
setParameter: 1 typeTo: Number.
;

Number getMessage: "sqrt".
setDescriptionTo: "Returns the square root of the recipient Number.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Number getMessage: "tangent".
setDescriptionTo: "Basic mathematical function.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Number getMessage: "toThe:".
setSelectorTo: "toThe: aNumber" .
setDescriptionTo: "Raises the recipient Number to the specified power.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Number.
;

Number getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'Number'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

Number getMessage: "within:percentOf:".
setSelectorTo: "within: x percentOf: y" .
setDescriptionTo: 
"Computes whether the recipient number is within x percent of the value y.  
This message builds a closed interval around y and tests to see if x falls in 
that range.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Number.
setParameter: 2 typeTo: Number .
;

#--------------------

Double getMessage: "*".
setSelectorTo: "* aValue" .
setDescriptionTo: 
"Multiplies recipient by the supplied value.  If invalid parameter is given, 
it returns NA.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Number .
;

Double getMessage: "+".
setSelectorTo: "+ aValue" .
setDescriptionTo: 
"Adds supplied value to the recipient.  If an invalid parameter is given, it 
returns NA.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Number.
;

Double getMessage: "-".
setSelectorTo: "- aValue".
setDescriptionTo: 
"Subtracts supplied value from the recipient.  If an invalid parameter is 
given, it returns NA.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Number.
;

Double getMessage: "/".
setSelectorTo: "/ aValue" .
setDescriptionTo: 
"Divides recipient by the supplied value.  If invalid parameter is given, it 
returns NA.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Number.
;

Double getMessage: "<".
setSelectorTo: "< aValue" .
setDescriptionTo: "Tests if recipient is less than supplied value.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Number.
;

Double getMessage: "<=".
setSelectorTo: "<= aValue" .
setDescriptionTo: 
"Tests if recipient is less than or equal to supplied value.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Number.
;

Double getMessage: "=".
setSelectorTo: "= aValue" .
setDescriptionTo: "Tests if recipient equals the supplied value.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: Number.
;

Double getMessage: "arcCosine".
setDescriptionTo: "Basic mathematical function.".
setReturnObjectTypeTo: Undefined.
setFunctionTypeTo: "Computation".
;

Double getMessage: "arcSine".
setDescriptionTo: "Basic mathematical function.".
setReturnObjectTypeTo: Undefined.
setFunctionTypeTo: "Computation".
;

Double getMessage: "arcTangent".
setDescriptionTo: "Basic mathematical function.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Double getMessage: "asAPowerOf:".
setSelectorTo: "asAPowerOf: aBaseNumber" .
setDescriptionTo: 
"Raises supplied number to the recipient power (baseNumber ^recipient).".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Number.
;

Double getMessage: "asDouble".
setDescriptionTo: "Returns recipient stripped of any extensions.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Conversion".
;

Double getMessage: "asFloat".
setDescriptionTo: "Converts recipient into a Number of class Float.".
setReturnObjectTypeTo: Float.
setFunctionTypeTo: "Conversion".
;

Double getMessage: "asInteger".
setDescriptionTo: "Converts recipient into a Number of class Integer.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Conversion".
;

Double getMessage: "asNumber".
setDescriptionTo: 
"Returns recipient stripped of any extensions.  Same message as 'asDouble'.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Conversion".
;

Double getMessage: "asPointerTo:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Double getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Conversion".
;

Double getMessage: "canonicalizedForSort".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
;

Double getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

Double getMessage: "cosine".
setDescriptionTo: "Basic mathematical function.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Double getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Schema" .
;

Double getMessage: "dividedInto:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Double getMessage: "exp".
setDescriptionTo: 
"Raises the mathematical constant e (~2.7183) to the recipient power 
(i.e., e ^ recipient).".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Double getMessage: "hyperbolicCosine".
setDescriptionTo: "Basic mathematical function.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Double getMessage: "hyperbolicSine".
setDescriptionTo: "Basic mathematical function.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Double getMessage: "hyperbolicTangent".
setDescriptionTo: "Basic mathematical function".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Double getMessage: "isDouble".
setDescriptionTo: "Returns a TRUE value when sent to any Double.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

Double getMessage: "log".
setDescriptionTo: 
"Computes the natural log of the recipient.  Recipient must be positive; the 
log is undefined for numbers less than or equal to 0.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Double getMessage: "orderUsingDictionary:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Double getMessage: "print".
setDescriptionTo: 
"Redefines the standard print message so that the value of the number is 
printed.  The default print format for doubles is 9 character positions with 
two decimal places.  The format is expanded if more space is needed.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Display".
;

Double getMessage: "printOf:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Double getMessage: "printWithCommasOf:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Double getMessage: "sine".
setDescriptionTo: "Basic mathematical function.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Double getMessage: "sprintOf:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Double getMessage: "sprintWithCommasOf:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Double getMessage: "sqrt".
setDescriptionTo: 
"Returns the square root of the recipient.  Recipient must be a 
non-negative Double.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Double getMessage: "subtractedFrom:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Double getMessage: "tangent".
setDescriptionTo: "Basic mathematical function.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Double getMessage: "toThe:".
setSelectorTo: "toThe: aValue" .
setDescriptionTo: "Raises the recipient to the specified power.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Number.
;

Double getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'Double'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

#--------------------

Float getMessage: "*".
setSelectorTo: "* aValue" .
setDescriptionTo: 
"Multiplies recipient by the supplied value.  If an invalid paramter is given, 
it returns NA.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Float getMessage: "+".
setSelectorTo: "+ aValue" .
setDescriptionTo: 
"Adds supplied value to the recipient.  If an invalid parameter is given, it 
returns NA.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Float getMessage: "-".
setSelectorTo: "- aValue" .
setDescriptionTo: 
"Subtracts supplied value from the recipient.  If an invalid parameter is given, 
it returns NA.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Float getMessage: "/".
setSelectorTo: "/ aValue" .
setDescriptionTo: 
"Divides recipient by the supplied value.  If an invalid parameter is given, 
it returns NA.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Float getMessage: "<".
setSelectorTo: "< aValue" .
setDescriptionTo: 
"Tests if recipient is less that supplied value.  If an invalid parameter is 
given, it returns NA.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
;

Float getMessage: "<=".
setSelectorTo: "<= aValue" .
setDescriptionTo: 
"Tests if recipient is less than or equal to supplied value.  If an invalid 
parameter is given, it returns NA.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
;

Float getMessage: "=".
setSelectorTo: "= aValue" .
setDescriptionTo: 
"Tests if recipient equals the supplied value.  If an invalid parameter is 
given, it returns NA.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
;

Float getMessage: "asDouble".
setDescriptionTo: "Converts recipient into a Number of class Double".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Conversion".
;

Float getMessage: "asFloat".
setDescriptionTo: "Returns recipient stripped of any extensions.".
setReturnObjectTypeTo: Float.
setFunctionTypeTo: "Conversion".
;

Float getMessage: "asInteger".
setDescriptionTo: "Converts recipient into a Number of class Integer.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Conversion".
;

Float getMessage: "asNumber".
setDescriptionTo: "Returns recipient stripped of any extensions.".
setReturnObjectTypeTo: Float.
setFunctionTypeTo: "Conversion".
;

Float getMessage: "asPointerTo:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Float getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: Float.
setFunctionTypeTo: "Conversion".
;

Float getMessage: "canonicalizedForSort".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
;

Float getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

Float getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: Float.
setFunctionTypeTo: "Schema" .
;

Float getMessage: "dividedInto:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Float getMessage: "isFloat".
setDescriptionTo: "Returns a TRUE value when sent to any Float.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

Float getMessage: "orderUsingDictionary:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Float getMessage: "print".
setDescriptionTo: 
"Redefines the standard print message so that the value of the number is 
printed.  The default format for floats is 9 character positions with two 
decimal places.  The format is expanded if more space is needed.".
setReturnObjectTypeTo: Float.
setFunctionTypeTo: "Display".
;

Float getMessage: "printOf:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Float getMessage: "printWithCommasOf:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Float getMessage: "sprintOf:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Float getMessage: "sprintWithCommasOf:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Float getMessage: "subtractedFrom:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Float getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'Float'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

#--------------------

Integer getMessage: "*".
setSelectorTo: "* aValue" .
setDescriptionTo: 
"Multiplies recipient by the supplied value.  If an invalid parameter is 
given, it returns NA.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Integer getMessage: "+".
setSelectorTo: "+ aValue" .
setDescriptionTo: "Adds supplied value to the recipient.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Integer getMessage: "-".
setSelectorTo: "- aValue" .
setDescriptionTo: "Subtracts supplied value from the recipient.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Integer getMessage: "/".
setSelectorTo: "/ aValue" .
setDescriptionTo: "Divides recipient by the supplied value.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Computation".
;

Integer getMessage: "<".
setSelectorTo: "< aValue" .
setDescriptionTo: "Tests if recipient is less than supplied value.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
;

Integer getMessage: "<=".
setSelectorTo: "<= aValue" .
setDescriptionTo: 
"Tests if recipient is less than or equal to supplied value.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
;

Integer getMessage: "=".
setSelectorTo: "= aValue" .
setDescriptionTo: "Tests if recipient equals the supplied value.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
;

Integer getMessage: "_integerRefTo:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Integer getMessage: "_primitiveRefTo:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Integer getMessage: "adjustedMod:".
setSelectorTo: "adjustedMod: aNumber" .
setLevelTo: "Reserved" .
setDescriptionTo: 
"Returns the recipient modulo supplied number as a value from 1 to
aNumber (instead of mod: which returns a number from 0 to aNumber - 1)." .
setReturnObjectTypeTo: Number .
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Number.
;

Integer getMessage: "asDate".
setDescriptionTo: 
"Converts an Integer into a Date.  Assumes recipient is in one of the 
following forms: YYYYMMDD, YYMMDD, YYMM, YY.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "asDateFromMMDD".
setDescriptionTo: 
"Converts an integer in month-day form to a date.  Invalid month-day 
combination returns an NA.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "asDateFromMMDDYY".
setDescriptionTo: 
"Converts an integer in month-day-year form to a date. Invalid month-day-year 
combination returns an NA.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "asDateFromMMDDYYYY".
setDescriptionTo: 
"Converts an integer in month-day-year form to a date. Invalid month-day-year 
combination returns an NA.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "asDateFromMMYY".
setDescriptionTo: 
"Converts an integer in month-year format to a date. Invalid month-year 
combination returns an NA.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "asDateFromYYMM".
setDescriptionTo: 
"Converts an integer in year-month format to a date. Invalid year-month
combination returns an NA.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "asDateFromYYMMDD".
setDescriptionTo: 
"Converts an integer in year-month-day format to a date. Invalid year-month-day 
combination returns an NA.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "asDateFromYYYYMM".
setDescriptionTo: 
"Converts integer to a month-end date where year is represented by the
full four digits.  For example:
     200012 asDateFromYYYYMM
returns the date 12/31/2000." .
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "asDouble".
setDescriptionTo: "Converts recipient into a Number of class Double.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "asFloat".
setDescriptionTo: "Converts recipient into a Number of class Float.".
setReturnObjectTypeTo: Float.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "asInteger".
setDescriptionTo: "Returns recipient stripped of any extensions.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "asMonthEnd".
setDescriptionTo: 
"Returns month-end date in current year for month represented by recipient.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "asMonthEndInYear:".
setSelectorTo: "asMonthEndInYear: aYear" .
setDescriptionTo: 
"Returns month-end date in supplied year for month represented by recipient.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Conversion".
setParameter: 1 typeTo: Integer .
;

Integer getMessage: "asNumber".
setDescriptionTo: "Returns recipient stripped of any extensions.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "asPointerTo:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Integer getMessage: "asPrimitive".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "asReferenceTo:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Integer getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "asString".
setDescriptionTo: "Converts recipient integer into a string.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "base0CellPositionOf:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Integer getMessage: "base1CellPositionOf:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Integer getMessage: "basicAsDate".
setDescriptionTo: 
"Converts an Integer of form (YYYYMMDD) into a Date. Assumes Integer is in the 
form YYYYMMDD (e.g., 19920922 for September 22, 1992).  To construct a date 
from three numbers, year, month, and day the following approach can be used: 
(year * 10000 + month * 100 + day) asInteger asDate.  See the 'asDate' message 
for a more general implementation.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "businessDays".
setDescriptionTo: 
"Converts the recipient into a BusinessDays DateOffset. Used in conjunction 
with DateRange formation and date arithmetic.".
setReturnObjectTypeTo: BusinessDays.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "canonicalizedForSort".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
;

Integer getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

Integer getMessage: "days".
setDescriptionTo: 
"Converts the recipient into a Days DateOffset.  Used in conjunction with 
DateRange formation and date arithmetic.".
setReturnObjectTypeTo: Days.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Schema" .
;

Integer getMessage: "dividedInto:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Integer getMessage: "dropFromString:".
setSelectorTo: "dropFromString: aString" .
setDescriptionTo: 
"Drops the recipient number of characters from supplied string.  A positive 
integer drops characters starting at the beginning of the string.  A negative 
number drops characters starting at the end of the string.  If you drop more 
characters than the string contains, a string with 0 characters is returned.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
setParameter: 1 typeTo: String.
;

Integer getMessage: "evaluate:".
setSelectorTo: "evaluate: aBlock" .
setDescriptionTo: 
"Converts the recipient to a Date and evaluates the supplied block as of this 
date.  If recipient does not convert into a valid date, the block is not 
evaluated.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
;

Integer getMessage: "fillFromString:".
setSelectorTo: "fillFromString: aString" .
setDescriptionTo: 
"Repeats the characters in the supplied string until a string of the length 
specified by the recipient is formed.  A negative number repeats the pattern 
from right to left.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
setParameter: 1 typeTo: String.
;

Integer getMessage: "instancesAdjacentTo:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: NA.
;

Integer getMessage: "isInteger".
setDescriptionTo: "Returns a TRUE value when sent to any Integer.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

Integer getMessage: "mod:".
setSelectorTo: "mod: aValue" .
setDescriptionTo: 
"Computes the recipient Integer modulo of the supplied value.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Computation".
setParameter: 1 typeTo: Number.
;

Integer getMessage: "monthBeginnings".
setDescriptionTo: 
"Converts the recipient into a MonthBeginnings DateOffset. Used in conjunction 
with DateRange formation and date arithmetic.".
setReturnObjectTypeTo: DateOffset.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "monthEnds".
setDescriptionTo: 
"Converts the recipient into a MonthEnds DateOffset.  Used in conjunction with 
DateRange formation and date arithmetic.".
setReturnObjectTypeTo: DateOffset.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "months".
setDescriptionTo: 
"Converts the recipient into a Months DateOffset.  Used in conjunction with 
DateRange formation and date arithmetic.".
setReturnObjectTypeTo: DateOffset.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "numberOfDigits".
setDescriptionTo: "Computes the number of digits in the recipient.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Inquiry".
;

Integer getMessage: "orderUsingDictionary:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Integer getMessage: "prefixWith:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Integer getMessage: "print".
setDescriptionTo: 
"Redefines the standard print message so that the value of the recipient 
Integer is printed.  The default print format for integers is 9 character 
positions with no decimal places.  The format is expanded if more space is 
needed.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Display".
;

Integer getMessage: "printOf:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Integer getMessage: "printWithCommasOf:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Integer getMessage: "quarterBeginnings".
setDescriptionTo: 
"Converts the recipient into a QuarterBeginnings DateOffset.  Used in 
conjunction with DateRange formation and date arithmetic.".
setReturnObjectTypeTo: DateOffset.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "quarterEnds".
setDescriptionTo: 
"Converts the recipient into a QuarterEnds DateOffset. Used in conjunction 
with DateRange formation and date arithmetic.".
setReturnObjectTypeTo: DateOffset.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "quarters".
setDescriptionTo: 
"Converts the recipient into a Quarterrs DateOffset.  Used in conjunction with 
DateRange formation and date arithmetic.".
setReturnObjectTypeTo: DateOffset.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "sequence".
setDescriptionTo: 
"Returns a list of integers from 1 through recipient Integer.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "sequence0".
setDescriptionTo: 
"Returns a list of integers from 0 through recipient Integer minus 1.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "sequenceTo:".
setSelectorTo: "sequenceTo: endNumber" .
setDescriptionTo: 
"Returns a list of integers from recipient value to the supplied end number.
If the supplied paramter is not a number, 0 will be used as the end-point." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "sprintOf:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Integer getMessage: "sprintWithCommasOf:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setParameter: 1 typeTo: NA.
;

Integer getMessage: "subtractedFrom:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

Integer getMessage: "takeFromString:".
setSelectorTo: "takeFromString: aString" .
setDescriptionTo: 
"Keeps the recipient number of characters from supplied string.  A positive 
number keeps characters starting at the beginning of the string.  A negative 
number keeps characters starting at the end of the string.  If you keep more 
characters than the string contains, blanks will be added to the beginning or 
end of the string.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
setParameter: 1 typeTo: String.
;

Integer getMessage: "to:by:".
setSelectorTo: "to: aDate by: aDateOffset" .
setDescriptionTo: 
"Converts the recipient into a date, then constructs a date range from this 
date, the supplied date, and the the supplied increment.".
setReturnObjectTypeTo: DateRange.
setFunctionTypeTo: "Conversion".
setParameter: 1 typeTo: Date.
setParameter: 2 typeTo: DateOffset.
;

Integer getMessage:  "whatAmI".
setDescriptionTo: "Returns the string 'Integer'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

Integer getMessage: "yearBeginnings".
setDescriptionTo: 
"Converts the recipient into a YearBeginnings DateOffset. Used in conjunction 
with DateRange formation and date arithmetic.".
setReturnObjectTypeTo: DateOffset.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "yearEnds".
setDescriptionTo: 
"Converts the recipient into a YearEnds DateOffset. Used in conjunction with 
DateRange formation and date arithmetic.".
setReturnObjectTypeTo: DateOffset.
setFunctionTypeTo: "Conversion".
;

Integer getMessage: "years".
setDescriptionTo: 
"Converts the recipient into a Years DateOffset. Used in conjunction with 
DateRange formation and date arithmetic.".
setReturnObjectTypeTo: DateOffset.
setFunctionTypeTo: "Conversion".
;

#--------------------

String getMessage: "!=".
setSelectorTo: "!= aString" .
setDescriptionTo: 
"Tests if recipient string is the same as the supplied string.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
;

String getMessage: "<".
setSelectorTo: "< aString" .
setDescriptionTo: 
"Tests if recipient string comes before supplied string alphabetically.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: String.
;

String getMessage: "<=".
setSelectorTo: "<= aString" .
setDescriptionTo: 
"Tests if recipient string comes before or is equal to supplied string.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: String.
;

String getMessage: "=".
setSelectorTo: "= aString" .
setDescriptionTo: 
"Tests if supplied string contains the identical sequence as the recipient 
String.  Two strings are NOT identical if one has extra blanks at the beginning 
or end.  Upper and lower case letters are NOT 
identical.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: String.
;

String getMessage: ">".
setSelectorTo: "> aString" .
setDescriptionTo: 
"Tests if recipient String comes after supplied String alphabetically.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: String.
;

String getMessage: ">=".
setSelectorTo: ">= aString" .
setDescriptionTo: 
"Tests if recipient String is the same or comes after supplied String 
alphabetically.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
setParameter: 1 typeTo: String.
;
 
String getMessage: "UpperCaseDictionary".
setDescriptionTo: "Lower to upper case conversion chart.".
setReturnObjectTypeTo: Dictionary.
setFunctionTypeTo: "Constant".
;

String getMessage: "LowerCaseDictionary".
setDescriptionTo: "Upper to lower case conversion chart.".
setReturnObjectTypeTo: Dictionary.
setFunctionTypeTo: "Constant".
;

String getMessage: "additionUpdateDumpOf:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

String getMessage: "appendOutputOf:".
setSelectorTo: "appendOutputOf: aBlock" .
setDescriptionTo: 
"Appends all print statement that are invoked directly or indirectly
within the supplied block to the Unix file; the recipient string represents 
the Unix filename. " .
setFunctionTypeTo: "Utility".
setParameter: 1 typeTo: Block.
;

String getMessage: "as:".
setSelectorTo: "as: type" .
setDescriptionTo: 
"Converts the recipient String to an instance of the class indicated by
the parameter, if possible.  If the type is a string that is not the
default String, the supplied type will be evaluated to determine its class. 
The implementation of 'convertFrom:' defined for that class will be used
to convert the string to the correct class.  The version at Object looks
the string up in the class' default naming dictionary if it exists.  
If the recipient String contains the ',' character, this method returns
a list of objects of the supplied type.  Any String that cannot be
converted to the supplied class returns NA." .
setFunctionTypeTo: "Conversion".
;

String getMessage: "asBlock".
setDescriptionTo: 
  "Converts recipient into a Block that contains the recipient." .
setReturnObjectTypeTo: Block .
setFunctionTypeTo: "Conversion".
;

String getMessage: "asBlockIn:".
setDescriptionTo: "Converts recipient into a Block that contains the recipient.
The block is created in the execution environment of the suplied object." .
setReturnObjectTypeTo: Block .
setFunctionTypeTo: "Conversion".
;

String getMessage: "asCurrency".
setDescriptionTo: "Converts recipient into a Currency.".
setReturnObjectTypeTo: Currency.
setFunctionTypeTo: "Conversion".
;

String getMessage: "asInteger".
setDescriptionTo: "Converts recipient into a Number of class Integer.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Conversion".
;

String getMessage: "asFileContents".
setDescriptionTo: 
"Converts the file name specified by the recipient string into a String
containing the file's contents.  If the file is not found, an empty string
is returned." .
setReturnObjectTypeTo: String .
setFunctionTypeTo: "Conversion".
;

String getMessage: "asLines".
setDescriptionTo: 
"Returns a list of strings based on carriage returns in recipient string.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Conversion".
;

String getMessage: "asNumber".
setDescriptionTo: 
"Converts recipient string into a number.  Strings containing numbers with no 
decimal point are converted to integers.  Strings containing non-numeric 
characters are converted to NA.".
setReturnObjectTypeTo: Number.
setFunctionTypeTo: "Conversion".
;

String getMessage: "asQuotedString".
setDescriptionTo: 
"Returns the recipient string enclosed in double quotes." .
setReturnObjectTypeTo: String .
setFunctionTypeTo: "Conversion".
;

String getMessage: "asRecords".
setDescriptionTo: 
"Converts the file specified in the recipient string into
a list of strings, with each element representing a record in the
original file.  Empty lines are excluded from the returned list." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Conversion".
;

String getMessage: "asCSVRecords".
setDescriptionTo: 
"Converts a comma-separated-value format file to a list of strings,
with each element responding to the message 'fields', which returns
a list of the comma-separted values.  Embedded commas in the original
file are preserved.  For example, if the file 'sample.dat' contains:
     key1, description1, value1
     key2, \"description 2 with , character\", value2
then:
     \"sample.dat\" asCSVRecords
     do: [ \"Field 1: \" print ; fields at: 1 . printNL ;
           \"Field 2: \" print ; fields at: 2 . printNL ;
           \"Field 3: \" print ; fields at: 3 . printNL ;
         ] ;

displays:

     Field 1: key1
     Field 2: description1
     Field 3: value1
     Field 1: key2
     Field 2: description 2 with , character
     Field 3: value2
" .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Conversion".
;

String getMessage: "asCells".
setDescriptionTo: 
"Converts the recipient string into a list of strings extended
by the 'cells' message which returns a list of strings generated
using the tab or Vision delimiter. Lines containing Vision Formatter
commands (i.e., @@@) are removed as are any cells that contain an 
escape character." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Conversion".
;

String getMessage: "asCellsOn:".
setSelectorTo: "asCellsOn: delimiter" .
setDescriptionTo: 
"Converts the recipient string into a list of strings extended
by the 'cells' message which returns a list of strings generated
using supplied delimiter. Lines containing Vision Formatter
commands (i.e., @@@) are removed as are any cells that contain an 
escape character." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Conversion".
setParameter: 1 typeTo: String .
;

String getMessage: "asDate".
setDescriptionTo: 
"Converts the recipient String into a Date object.  The recipient
can contain a value in one of the following forms: 
      \"yy\", \"yymm\", \"yymmdd\", \"yyyymmdd\"
      \"m/d/y\"     where m is 1-12; d is 1-31; y is yy or yyyy
      \"m-d-y\"     where m is 1-12; d is 1-31; y is yy or yyyy
      \"+ # offset\", where # is integer and offset is a DateOffset
      \"- # offset\", where # is integer and offset is a DateOffset
      \"today\"
      \"yesterday\"

The 'offset' formats add/subtract the offset from the current value of '^date'.
For example:
     \"961231\" asDate      --> 12/31/96
     \"1/3/95\" asDate      --> 1/3/95
     \"+ 3 days\"           --> current date + 3 days
     \"today\"              --> today's date
" .
setReturnObjectTypeTo: Date .
setFunctionTypeTo: "Conversion".
;

String getMessage: "asSelector".
setDescriptionTo: 
"Returns the recipient as a selector object.  The version at Selector returns
recipient." .
setReturnObjectTypeTo: Selector .
setFunctionTypeTo: "Conversion".
;

String getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Conversion".
;

String getMessage: "asString".
setDescriptionTo: "Returns recipient string stripped of any extensions.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Conversion".
;

String getMessage: "asUpperLowerWords".
setDescriptionTo: 
"Returns a list of strings based on upper case letter word boundaries.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Format".
;

String getMessage: "asWords".
setDescriptionTo: 
"Returns a list of strings representing the individual words in the recipient
string where a word is terminated by a blank, tab or new line character." .
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Conversion" .
;

String getMessage: "at:".
setSelectorTo: "at: n " .
setDescriptionTo: 
"Returns the character at the specified position.  For example:

     \"abcdefghij\" at: 3 .

returns the single character string \"c\"." .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
setParameter: 1 typeTo: Integer .
;

String getMessage: "at:put:".
setSelectorTo: "at: n put: object" .
setDescriptionTo: 
"Inserts the supplied string at the supplied character position.  For 
example:

     \"abcdefghij\" at: 3 put: \"xyz\"

returns the string \"abxyzcdefghij\"." .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
setParameter: 1 typeTo: Integer .
setParameter: 2 typeTo: String .
;

String getMessage: "breakOn:" .
setSelectorTo: "breakOn: aCharacterSet" .
setDescriptionTo: 
"Breaks the recipient string into a list using the characters in the
string parameter as break points.  The break point characters are not
included in the resultant list elements.  For example, the expression:
     myString breakOn: newLine
can be used to return a list of strings corresponding to the 'records'
in the recipient string." .
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Conversion".
setParameter: 1 typeTo: String .
;

String getMessage: "breakString:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: String .
;

String getMessage: "tokenizeOn:" .
setSelectorTo: "tokenizeOn: aCharacterSet" .
setDescriptionTo: 
"Breaks the recipient string into a list using the characters in the
string parameter as break points.  The break point characters ARE 
included in the resultant list elements.  The elements in the returned
list alternate between the substrings between matches and the matched
character." .
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Conversion".
setParameter: 1 typeTo: String .
;

String getMessage: "breakTokenize:".
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
;

String getMessage: "breakOnString:" .
setSelectorTo: "breakOnString: string" .
setDescriptionTo: 
"Breaks the recipient string into a list using exact matches to the supplied
string parameter as break points.  The break point characters are NOT
included in the resultant list elements.  " .
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Conversion".
setParameter: 1 typeTo: String .
;

String getMessage: "stringBreak:".
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
;

String getMessage: "tokenizeOnString:" .
setSelectorTo: "tokenizeOnString: string" .
setDescriptionTo: 
"Breaks the recipient string into a list using exact matches to the
string parameter as break points.  The break point strings ARE 
included in the resultant list elements.  The elements in the returned
list alternate between the substrings between matches and the matched
string." .
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Conversion".
setParameter: 1 typeTo: String .
;

String getMessage: "stringTokenize:".
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
;

String getMessage: "breakOnPattern:" .
setSelectorTo: "breakOnPattern: pattern" .
setDescriptionTo: 
"Breaks the recipient string into a list using regular expression 
matches to the supplied
string parameter as break points.  The break point characters are NOT
included in the resultant list elements.  " .
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Conversion".
setParameter: 1 typeTo: String .
;

String getMessage: "patternBreak:".
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
;

String getMessage: "tokenizeOnPattern:" .
setSelectorTo: "tokenizeOnPattern: pattern" .
setDescriptionTo: 
"Breaks the recipient string into a list using regular expression
matches to the
string parameter as break points.  The break point strings ARE 
included in the resultant list elements.  The elements in the returned
list alternate between the substrings between matches and the matched
string." .
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Conversion".
setParameter: 1 typeTo: String .
;

String getMessage: "patternTokenize:".
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
;

String getMessage: "canonicalizedForSort".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
;

String getMessage: "capitalize".
setDescriptionTo: "Capitalizes the first letter of the string.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
;

String getMessage: "cbreakOn:" .
setSelectorTo: "cbreakOn: aCharacterSet" .
setDescriptionTo: 
"Breaks the recipient string into a list using the characters NOT in the
string parameter as break points.  The break point characters are not
included in the resultant list elements.  This message is the converse
of 'breakOn:'. " .
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Conversion".
setParameter: 1 typeTo: String .
;

String getMessage: "cbreakString:".
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
;

String getMessage: "ctokenizeOn:" .
setSelectorTo: "ctokenizeOn: aCharacterSet" .
setDescriptionTo: 
"Breaks the recipient string into a list using the characters NOT in the
string parameter as break points.  The break point characters ARE
included in the resultant list elements.  This message is the converse
of 'tokenizeOn:'. " .
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Conversion".
setParameter: 1 typeTo: String .
;

String getMessage: "cbreakTokenize:".
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
;

String getMessage: "center:".
setSelectorTo: "center: anInteger" .
setDescriptionTo: 
"Centers the recipient string in specified width. If string 
contains more characters than supplied width, it is truncated to fit and no 
centering is performed.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
setParameter: 1 typeTo: Integer.
;

String getMessage: "centerNL:".
setSelectorTo: "centerNL: anInteger" .
setDescriptionTo: 
"Centers the recipient String using the supplied width and then 
starts a new line.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
setParameter: 1 typeTo: Integer.
;

String getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information 
is stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

String getMessage: "concat:".
setSelectorTo: "concat: aString" .
setDescriptionTo: 
"Appends supplied string to the end of the recipient string.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
setParameter: 1 typeTo: String.
;

String getMessage: "contains:".
setSelectorTo: "contains: aString" .
setDescriptionTo: 
"Tests if the supplied string is included in the recipient string.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: String.
;

String getMessage: "containsSubstring:".
setSelectorTo: "containsSubstring: substring" .
setDescriptionTo: 
"This message returns TRUE if substring is found in the recipient.  Unlike
the 'contains:' message, this form matches each character literally, so
no wildcard characters are recognized.  For example,

     \"abc\" contains: \"^a\" 

returns TRUE, but

     \"abc\" containsSubstring: \"^a\" 

returns FALSE." .
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: String.
;

String getMessage: "convertToCaseInsensitiveRegEx".
setDescriptionTo: 
"This message covnerts the recipient into a string that can be used
for case insensitive string matches with messages such as \"contains:\".
For example:
     \"app\" convertToCaseInsensitiveRegEx
returns the string \"[Aa][Pp][Pp]\".  The expression:
     \"Apple\" contains: \"app\"
returns FALSE.  The expression:
     \"Apple\" contains: \"app\" convertToCaseInsensitiveRegEx
returns TRUE.
" .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Conversion".
;

String getMessage: "fileExists".
setDescriptionTo: 
"Returns TRUE if recipient refers to an existing file, FALSE otherwise." .
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Inquiry".
;

String getMessage: "findSubstringOrigin:".
setSelectorTo: "findSubstringOrigin: substring" .
setDescriptionTo: 
"Returns the zero-based position of the first occurrence of the
sub-string in the recipient.  If the string is not found,
the length of the recipient is returned.  This version matches
the characters in the sub-string explicitly.  " .
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: String.
;

String getMessage: "findPatternOrigin:".
setSelectorTo: "findPatternOrigin: pattern" .
setDescriptionTo: 
"Returns the zero-based position of the first occurrence of the
pattern in the recipient.  If the string is not found,
the length of the recipient is returned.  This version views the
pattern as a regular expression (i.e., wildcards are interpretted)." .
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: String.
;

String getMessage: "findSubstringExtent:".
setSelectorTo: "findSubstringExtent: substring" .
setDescriptionTo: 
"Returns the length of the sub-string if it is present in the recipient.
If the string is not prsent, zero is returned.  This version matches the 
characters in the sub-string explicitly.  " .
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: String.
;

String getMessage: "findPatternExtent:".
setSelectorTo: "findPatternExtent: pattern" .
setDescriptionTo: 
"Returns the length of the pattern if it is present in the recipient.  If
it is not present, zero is returned.  This version views the pattern as a
regular expression (i.e., wildcards are interpretted)." .
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: String.
;

String getMessage: "findSubstringSuffix:".
setSelectorTo: "findSubstringSuffix: substring" .
setDescriptionTo: 
"Returns the zero-based position of the first character after the first
occurrence of the sub-string in the recipient.  If the string is not 
found, the length of the recipient is returned.  This version matches
the characters in the sub-string explicitly.  " .
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: String.
;

String getMessage: "findPatternSuffix:".
setSelectorTo: "findPatternSuffix: pattern" .
setDescriptionTo: 
"Returns the zero-based position of the first character after the first
occurrence of the pattern in the recipient.  If the string is not found,
the length of the recipient is returned.  This version views the
pattern as a regular expression (i.e., wildcards are interpretted)." .
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: String.
;

String getMessage: "convertToNumber".
setDescriptionTo: 
"Removes any non-numeric characters (0-9 . - +) from the start and end of 
the string and any commas from within the remaining string.  If the 
resultant string can be converted by a number it is, otherwise NA is
returned." .
setReturnObjectTypeTo: Number .
setFunctionTypeTo: "Conversion".
;

String getMessage: "count".
setDescriptionTo: "Counts the number of characters in recipient string.
Returns a non-negative integer.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Inquiry".
;

String getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema" .
;

String getMessage: "deleteFromDictionaryOf:".
setSelectorTo: "deleteFromDictionaryOf: anObject" .
setDescriptionTo: 
"Converts the recipient string to a selector and removes the protocol 
implemented by it from the dictionary associated with the supplied object.".
# setReturnObjectTypeTo: .
setFunctionTypeTo: "Messages".
setParameter: 1 typeTo: Object.
;

String getMessage: "drop:".
setSelectorTo: "drop: aNumber" .
setDescriptionTo: 
"Drops the specified number of characters form the beginning or end of 
recipient string.  A positive integer drops characters starting at the 
beginning of the string.  A negative number drops characters starting at the 
end of the string.  If you drop more characters that the string contains, a 
string with 0 characters is returned.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
setParameter: 1 typeTo: Number.
;

String getMessage: "evaluate".
setDescriptionTo: "Executes string as VISION expression at ^global.".
# setReturnObjectTypeTo: .
setFunctionTypeTo: "Evaluation".
;

String getMessage: "evaluateIn:".
setSelectorTo: "evaluateIn: anObject" .
setDescriptionTo: 
"Executes string as VISION expression in supplied environment.".
# setReturnObjectTypeTo: .
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Object.
;

String getMessage: "evaluateWithCompilationErrorHandler:in:".  
setLevelTo: "Internal" .
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
setParameter: 2 typeTo: NA.
;

String getMessage: "fill:".
setSelectorTo: "fill: aNumber" .
setDescriptionTo: 
"Repeats the characters in the original string until a string of the specified 
length is formed.  A negative number repeats the pattern from right to left.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
setParameter: 1 typeTo: Number.
;

String getMessage: "filterOutputOf:".
setSelectorTo: "filterOutputOf: aBlock" .
setDescriptionTo: 
"Supplies the printed output associated with the execution of the
supplied block as input to the recipient string as a Unix Command.

For example:  

     \"format\" filterOutputOf: [ Named Company IBM displayReport ] 

will redirect the report through the format program as standard input
(i.e., format < report).  By default, the results of executing this
expression will be printed on your screen." .
# setReturnObjectTypeTo: .
setFunctionTypeTo: "Utility".
setParameter: 1 typeTo: Block.
;

String getMessage: "fixed".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
;

String getMessage: "formatForPRN".
setDescriptionTo: 
"reformats the recipient string for use as PRN-style input
Strings are printed in quotes, numbers are printed as is, and field
are separated by a space.  This message assumes that the recipient
string is delimited by the tab or Vision delimiter." .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
;

String getMessage: "formatForExcel".
setDescriptionTo: 
"reformats the recipient string for use as tab delimited
output.  This message assumes that the recipient string is delimited
by the tab or Vision delimiter." .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
;

String getMessage: "from:to:".
setSelectorTo: "from: pos1 to: pos2" .
setDescriptionTo: 
"Returns a substring of the recipient object from the starting position
to the ending position.  The supplied parameters should be positive
integers.  If pos1 is greater than pos2, the resultant string is reversed." .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
setParameter: 1 typeTo: Integer .
setParameter: 2 typeTo: Integer .
;

String getMessage: "from:for:".
setSelectorTo: "from: pos1 for: count" .
setDescriptionTo: 
"Returns a substring of the recipient object from the supplied position
for count characters.  If count is positive, supplied position is the 
first position of the returned string.  If count is negative, supplied
position is the last position of the returned string." .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
setParameter: 1 typeTo: Integer .
setParameter: 2 typeTo: Integer .
;

String getMessage: "from:".
setSelectorTo: "from: pos" .
setDescriptionTo: 
"Returns a substring of recipient starting from position indicated by
parameter.  If 'pos' is numeric, the substring starting with this
character position to the end of the string is returned.  If 'pos'
is a character, the substring starting from the first occurrence of
this character to the end of the string is returned." .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
setParameter: 1 typeTo: Ordinal .
;

String getMessage: "to:".
setSelectorTo: "to: pos" .
setDescriptionTo: 
"Returns a substring of recipient from start of recipient to position
indicated by parameter.  If 'pos' is numeric, the substring ending with
this character position is returned.  If 'pos' is a character, the
substring starting from the beginning of the recipient through to the
first occurrence of this character is returned." .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
setParameter: 1 typeTo: Ordinal .
;

String getMessage: "howManyOf:".
setSelectorTo: "howManyOf: charset" .
setDescriptionTo: 
"Returns the number of occurrences of any of the characters in the supplied
character set in the recipient String.  For example:
     \"1/15/96\" howManyOf: \"/\" . printNL ;
" .
setReturnObjectTypeTo: Integer .
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: String .
;

String getMessage: "isBlank".
setDescriptionTo: "Returns TRUE if string is all blanks.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Inquiry".
;

String getMessage: "isBlankFor:".
setSelectorTo: "isBlankFor: aNumber" .
setDescriptionTo: "Returns TRUE if string is blank for first n characters.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: Number.
;

String getMessage: "isDefault".
setDescriptionTo: "Returns TRUE if string is empty.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Inquiry".
;

String getMessage: "isString".
setDescriptionTo: "Returns a TRUE value when sent to any String.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

String getMessage: "locateInDictionaryOf:".
setSelectorTo: "locateInDictionaryOf: anObject" .
setDescriptionTo: 
"Converts the recipient string into a selector and returns the intensional 
form of the value assessed in the dictionary of the supplied object.  Send
the 'value' message to evaluate the object returned.".
setLevelTo: "Advanced" .
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: Object.
;

String getMessage: "locateOrAddInDictionaryOf:".
setSelectorTo: "locateOrAddInDictionaryOf: anObject" .
setDescriptionTo: 
"Converts the recipient string into a selector and looks up this selector in 
the supplied object's dictionary, returning the intensional form of the value 
if present.  Otherwise the selector is added to the dictionary and the 
intensional form is returned.  Send 'value' message to intensional form to 
evaluate it.  " .
setLevelTo: "Advanced" .
setFunctionTypeTo: "Access".
setParameter: 1 typeTo: Object.
;

String getMessage: "lowercase".
setDescriptionTo: "Converts the first letter of a string to lowercase.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
;

String getMessage: "matchesString:".
setLevelTo: "Internal" .
setSelectorTo: "matchesString: aString" .
setDescriptionTo: 
"Tests whether supplied string is identical to recipient string.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: String.
;

String getMessage: "notifyAsComment".
setLevelTo: "Reserved" .
setDescriptionTo: "Obsolete.".
setFunctionTypeTo: "Format".
;

String getMessage: "notifyAsError".
setLevelTo: "Reserved" .
setDescriptionTo: "Obsolete.".
setFunctionTypeTo: "Format".
;

String getMessage: "notifyAsWarning".
setLevelTo: "Reserved" .
setDescriptionTo: "Obsolete.".
setFunctionTypeTo: "Format".
;

String getMessage: "orderUsingDictionary:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

String getMessage: "pad:".
setSelectorTo: "pad: aNumber" .
setDescriptionTo: 
"Pads the string with blanks if number of characters in the string
is less than supplied parameter.  If 'n' is negative, string is
padded on the left; otherwise, it is padded on the right." .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
setParameter: 1 typeTo: Number.
;

String getMessage: "prefixNotSpannedBy:".
setSelectorTo: "prefixNotSpannedBy: aString" .
setDescriptionTo:
"This message returns the number of characters at the beginning of the 
recipient string that do not match any character in the supplied string.".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: String .
;

String getMessage: "prefixNotSpannedIn:".
setSelectorTo: "prefixNotSpannedIn: aString" .
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
# setReturnObjectTypeTo: .
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: String.
;

String getMessage: "prefixSpannedBy:".
setSelectorTo: "prefixSpannedBy: aString" .
setDescriptionTo:
"This message returns the number of characters at the beginning of the 
recipient string that match any character in the supplied string".
setReturnObjectTypeTo: Integer.
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: String .
;

String getMessage: "prefixSpannedIn:".
setSelectorTo: "prefixSpannedIn: aString" .
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
# setReturnObjectTypeTo: .
setFunctionTypeTo: "Inquiry".
setParameter: 1 typeTo: String.
;

String getMessage: "prefixWith:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

String getMessage: "print".
setDescriptionTo: 
"Redefines the standard print message so that the content of the string is 
printed.  All characters are printed; no truncation or padding occurs.  
Strings can contain the carraige return character.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Display".
;

String getMessage: "print:".
setSelectorTo: "print: anInteger" .
setDescriptionTo: 
"Prints the string using the supplied width.  Strings containing fewer 
characters than the supplied width are padded with the appropriate number 
of blanks on the right (i.e., left-justified).  If the supplied width is 
negative, the string will be padded with the blanks on the left (right-
justified).  If the string contains more characters than the width supplied, 
the rightmost characters will be removed.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Display".
setParameter: 1 typeTo: Integer.
;

String getMessage: "printPRN".
setDescriptionTo: 
"Prints the recipient string using 'prn' format.  The recipient is printed 
surrounded by quotes.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Display".
;

String getMessage: "printPRN:".
setSelectorTo: "printPRN: anInteger" .
setDescriptionTo: 
"Prints the recipient object in 'prn' format using the supplied width.  The 
output string is surrounded by quotes.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Display".
setParameter: 1 typeTo: Integer.
;

String getMessage: "replacementUpdateDumpOf:".
setLevelTo: "Internal" .
setDescriptionTo: "For Internal Use Only.".
setFunctionTypeTo: "Primitive".
setParameter: 1 typeTo: NA.
;

String getMessage: "reverse".
setDescriptionTo: 
"Returns the recipient string with each character in reverse order.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
	;

String getMessage: "sprint:".
setSelectorTo: "sprint: anInteger" .
setDescriptionTo: 
"Returns the recipient string in a new string formatted to the number of 
characters specified in supplied width".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
setParameter: 1 typeTo: Integer.
;

String getMessage: "startingPositionsOf:".
setSelectorTo: "startingPositionsOf: subString" .
setDescriptionTo: 
"Returns the list of position numbers at which the supplied substring
starts in the recipient string." .
setReturnObjectTypeTo: List .
setReturnObjectElementTypeTo: Integer . 
setFunctionTypeTo: "Format".
;

String getMessage: "startingPositionOf:".
setSelectorTo: "startingPositionOf: subString" .
setDescriptionTo: 
"Returns the position number at which the first occurence of the supplied
substring starts in the recipient string." .
setReturnObjectTypeTo: Integer .
setFunctionTypeTo: "Format".
;

String getMessage: "stripBoundingBlanks".
setDescriptionTo: 
"Returns the string stripped of blanks on either side.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
;

String getMessage: "stripChar:" .
setSelectorTo: "stripChar: char" .
setDescriptionTo: 
"Returns the recipient string with the supplied character removed
for every occurrence" .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
setParameter: 1 typeTo: String .
;

String getMessage: "stripCommas".
setDescriptionTo: "Returns the string stripped of any commas.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
;

String getMessage: "stripLeadingBlanks".
setDescriptionTo: "Returns the string stripped of any leading blanks.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
;

String getMessage: "stripLeading:".
setDescriptionTo: "Returns the string stripped of characters in the
supplied parameter.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
;

String getMessage: "stripTrailingBlanks".
setDescriptionTo: 
"Returns the string stripped of any blanks that follow the last character.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
;

String getMessage: "substituteOutputOf:".
setSelectorTo: "substituteOutputOf: aBlock" .
setDescriptionTo: 
"Creates a Unix file containing all print statement that are invoked directly
or indirectly within the supplied block; the recipient string represents the
Unix filename " .
# setReturnObjectTypeTo: .
setFunctionTypeTo: "Utility".
setParameter: 1 typeTo: Block.
;

String getMessage: "take:".
setSelectorTo: "take: anInteger" .
setDescriptionTo: 
"Keeps the specified number of characters from the beginning or end of the 
string.  A positive number keeps characters starting at the beginning of the 
string.  A negative number keeps characters starting at the end of the string.  
If you keep more characters than the string contains, blanks will be added to 
the beginning or end of the string.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
setParameter: 1 typeTo: Integer.
;

String getMessage: "toDouble".
setDescriptionTo: 
"Converts recipient string to a double precision number if possible.  If 
recipient does not contian a valid number, NA is returned.".
setReturnObjectTypeTo: Double.
setFunctionTypeTo: "Conversion".
;

String getMessage: "toList".
setDescriptionTo: 
"Converts recipient string to a list of characters.  The resultant list has
one element for each character in the recipient string." .
setReturnObjectTypeTo: List .
setFunctionTypeTo: "Conversion".
;

String getMessage: "toUpper".
setDescriptionTo: 
"Converts the characters in the recipient string to upper case letters and
returns a single string." .
setReturnObjectTypeTo: String .
setFunctionTypeTo: "Conversion".
;

String getMessage: "toLower".
setDescriptionTo: 
"Converts the characters in the recipient string to lower case letters and
returns a single string." .
setReturnObjectTypeTo: String .
setFunctionTypeTo: "Conversion".
;

String getMessage: "toggleCase".
setDescriptionTo: 
"Reverses the case of all lower and upper case letters in recipient." .
setReturnObjectTypeTo: String .
setFunctionTypeTo: "Conversion".
;

String getMessage: "translate:to:".
setSelectorTo: "translate: charSet to: string" .
setDescriptionTo: 
"Replaces all occurrences of any character in the string supplied as the
first parameter with the string supplied a the second parameter.  This
differs from the 'translateString:to:' message which replaces all 
occurrences of the first string with the second string." .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
setParameter: 1 typeTo: String .
setParameter: 2 typeTo: String .
;
String getMessage: "translateString:to:".
setSelectorTo: "translateString: fromString to: toString" .
setDescriptionTo: 
"Replaces all occurrences of the 'fromString' with 'toString'.  Note
that this differs from the 'translate:to:' message which replaces any
character in the first parameter with the string supplied as the second
parameter.  Note that this version returns NA when the translation 
encounters overlapping replacments." .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
setParameter: 1 typeTo: String .
setParameter: 2 typeTo: String .
;
String getMessage: "translatePattern:to:".
setSelectorTo: "translatePattern: pattern to: toString" .
setDescriptionTo: 
"Replaces all occurrences of the 'pattern' (which is interpretted as
a regular expression) with 'toString'.  " .
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Format".
setParameter: 1 typeTo: String .
setParameter: 2 typeTo: String .
;
String getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'String'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

String getMessage: "newAssociativeCluster" .
   setDescriptionTo: "Creates a new associative string cluster." .
   setReturnObjectTypeTo: String .
   setFunctionTypeTo: "Creation" .
;
String getMessage: "isAnAssociativeCluster" .
   setDescriptionTo: "Is recipient an associative string cluster?" .
   setReturnObjectTypeTo: Boolean .
   setFunctionTypeTo: "Inquiry" .
;

String getMessage: "insertIntoCluster:" .
   setDescriptionTo: "Inserts recpient string into supplied cluster." .
   setReturnObjectTypeTo: String .
   setFunctionTypeTo: "Update" .
   setParameter: 1 typeTo: String .
;
String getMessage: "locateInCluster:" .
   setDescriptionTo: "Accesses recpient string in supplied cluster.
If not found, NA is returned." .
   setReturnObjectTypeTo: String .
   setFunctionTypeTo: "Access" .
   setParameter: 1 typeTo: String .
;
String getMessage: "deleteFromCluster:" .
   setDescriptionTo: "Deletes recpient string from supplied cluster.
If not found, FALSE is returned." .
   setReturnObjectTypeTo: Boolean .
   setFunctionTypeTo: "Update" .
   setParameter: 1 typeTo: String .
;
String getMessage: "clusterInsert:" .
   setDescriptionTo: "Inserts supplied string into recipient cluster." .
   setReturnObjectTypeTo: String .
   setFunctionTypeTo: "Update" .
   setParameter: 1 typeTo: String .
;
String getMessage: "clusterLocate:" .
   setDescriptionTo: "Accesses supplied string in recipient cluster.
If not found, NA is returned." .
   setReturnObjectTypeTo: String .
   setFunctionTypeTo: "Access" .
   setParameter: 1 typeTo: String .
;
String getMessage: "clusterDelete:" .
   setDescriptionTo: "Deletes supplied string from recipient cluster.
If not found, FALSE is returned." .
   setReturnObjectTypeTo: Boolean .
   setFunctionTypeTo: "Update" .
   setParameter: 1 typeTo: String .
;

String getMessage: "copyString".
setDescriptionTo: 
"Returns a copy of the recipient string (in a new cluster)." .
setReturnObjectTypeTo: String .
setFunctionTypeTo: "Conversion".
;
String getMessage: "asUpdateBlock".
setDescriptionTo: 
"Converts the recipient into a block that returns the intensional form
of the recipient (i.e., with a : prepended to it)." .
setReturnObjectTypeTo: Block .
setFunctionTypeTo: "Conversion".
;

String getMessage: "asClass".
setDescriptionTo: 
"Converts the recipient into the default instance of a class by the recpient's
name, if possible." .
setReturnObjectTypeTo: Object .
setFunctionTypeTo: "Conversion".
;

String getMessage: "asProgramWith:".
setDescriptionTo: 
"Runs the recipient as an external program.  Output generated by running
the supplied block is passed to the external program as standard input." .
setReturnObjectTypeTo: String .
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block .
;

String getMessage: "asProgram".
setDescriptionTo: 
"Runs the recipient as an external program.  " .
setReturnObjectTypeTo: String .
setFunctionTypeTo: "Evaluation".
;

String getMessage: "convertToCaseInsensitiveRegEx".
setDescriptionTo: 
"Converts the recipient into a string that represents a regular expression
that can be used to perform case insensitive matches." .
setReturnObjectTypeTo: String .
setFunctionTypeTo: "Conversion".
;

String getMessage: "validateAsExpressionIn:".
setDescriptionTo: 
"validates that the recipient can be run as an expression without
'selector not founds' if sent to an instance of the supplied parameter's
class" .
setReturnObjectTypeTo: Boolean .
setFunctionTypeTo: "Inquiry".
;

