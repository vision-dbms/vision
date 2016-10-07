##################################################
#  INSTance creation and setup for Schema-support classes
##################################################

SystemTable specializeAs: "MessageType" ;
SystemTable specializeAs: "ClassType" ;
SystemTable specializeAs: "FunctionType" ;
SystemTable specializeAs: "UsageLevel" ;

MessageType newAs: "Constant" ;
MessageType newAs: "FixedProperty" ;
Named MessageType FixedProperty :shortName <- "Fixed" ;
MessageType newAs: "TimeSeriesProperty" ;
Named MessageType TimeSeriesProperty :shortName <- "TimeSeries" ;
MessageType newAs: "Method" ;
MessageType newAs: "Primitive" ;

ClassType newAs: "BuiltIn" ;
ClassType newAs: "Core" ;
ClassType newAs: "User" ;
ClassType newAs: "Incorporator" ;
ClassType newAs: "ObjectSpace" ;

UsageLevel newAs: "Basic" ;
UsageLevel newAs: "Internal" ;
UsageLevel newAs: "Advanced" ;
UsageLevel newAs: "Reserved" ;

