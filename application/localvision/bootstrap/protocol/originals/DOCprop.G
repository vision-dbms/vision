#--------------------
#  Includes:
#      FixedProperty
#         TimeSeriesProperty
#--------------------

FixedProperty getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: FixedProperty.
setFunctionTypeTo: "Conversion".
;

FixedProperty getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information is 
stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

FixedProperty getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: FixedProperty.
setFunctionTypeTo: "Schema" .
;

FixedProperty getMessage: "isFixedProperty".
setDescriptionTo: "Returns the value TRUE when sent to any fixed property.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

FixedProperty getMessage: "isProperty".
setDescriptionTo: "Returns the value TRUE when sent to any fixed property.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

FixedProperty getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'FixedProperty'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;

#----------------------

TimeSeriesProperty getMessage: "asSelf".
setDescriptionTo: "Returns the original object.".
setReturnObjectTypeTo: TimeSeriesProperty.
setFunctionTypeTo: "Conversion".
;

TimeSeriesProperty getMessage: "classDescriptor".
setDescriptionTo: 
"Reference to the object which describes the class.  Class level information is 
stored here.".
setReturnObjectTypeTo: Schema ClassDescriptor.
setFunctionTypeTo: "Schema".
;

TimeSeriesProperty getMessage: "defaultInstance" .
setDescriptionTo: 
"Returns the default instance of the recipient object's class" .
setReturnObjectTypeTo: TimeSeries.
setFunctionTypeTo: "Schema" .
;

TimeSeriesProperty getMessage: "isTimeSeriesProperty".
setDescriptionTo: 
"Returns the value TRUE when sent to any time series property.".
setReturnObjectTypeTo: TRUE.
setFunctionTypeTo: "Inquiry".
;

TimeSeriesProperty getMessage: "whatAmI".
setDescriptionTo: "Returns the string 'TimeSeriesProperty'.".
setReturnObjectTypeTo: String.
setFunctionTypeTo: "Schema".
;


