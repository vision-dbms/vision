DateRange getMessage: "asDateList".
setDescriptionTo: "Converts a date range into a list of dates.".
setReturnObjectTypeTo: List.
setFunctionTypeTo: "Conversion".
;

DateRange getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: DateRange.
setFunctionTypeTo: "Conversion".
;

DateRange getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information is 
stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

DateRange getMessage: "date1".
setDescriptionTo: "Returns the starting date of the date range.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Descriptive".
;

DateRange getMessage: "date2".
setDescriptionTo: "Returns the ending date of the date range.".
setReturnObjectTypeTo: Date.
setFunctionTypeTo: "Descriptive".
;

DateRange getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: DateRange.
setFunctionTypeTo: "Schema" .
;

DateRange getMessage: "evaluate:".
setSelectorTo: "evaluate: aBlock ".
setDescriptionTo: 
"Evaluates all statements in the block for each date in the Date Range.  Each 
Date in the Date Range becomes the default inside the block (changing the value 
of the 'magic word' ^date).".
setReturnObjectTypeTo: DateRange.
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
;

DateRange getMessage: "extract:for:".
setSelectorTo: "extract: block for: object".
setDescriptionTo: 
"This message evaluates the supplied block for the supplied object for each
date in the recipient date range and returns the result as a time series,
oen point for each date in the range." .
setReturnObjectTypeTo: TimeSeries .
setFunctionTypeTo: "Evaluation".
setParameter: 1 typeTo: Block.
setParameter: 2 typeTo: Object .
;

DateRange getMessage: "increment".
setDescriptionTo: "Returns the increment of the date range.".
setReturnObjectTypeTo: DateOffset.
setFunctionTypeTo: "Descriptive".
;

DateRange getMessage: "isDateRange".
setDescriptionTo: "Returns a TRUE value when sent to any DateRange.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

DateRange getMessage: "new" .
setDescriptionTo: "Disables the 'new' message." .
setReturnObjectTypeTo: DateRange.
setFunctionTypeTo: "Creation" .
;

DateRange getMessage: "print".
setDescriptionTo: 
"Redefines the standard print message to print out a description of the
date range.".
setReturnObjectTypeTo: DateRange.
setFunctionTypeTo: "Display".
;

DateRange getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'DateRange'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;
 
DateRange getMessage: "=".
setSelectorTo: "= aValue" .
setDescriptionTo: "Tests if recipient equals the supplied value.".
setReturnObjectTypeTo: Boolean.
setFunctionTypeTo: "Comparison".
;
