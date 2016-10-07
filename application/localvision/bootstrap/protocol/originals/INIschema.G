##################################################
#   VISION Core Classes: Schema
#      creates the core classes needed to support the Schema ToolKit
#  Run In New Ospace (no -U option)
##################################################

"
####################
#  Create Schema Object Space
####################
" print ; 

Environment MaintenanceTools
   setupSpaceFor: "Schema" 
   as: "DBA" 
   description: "Schema Management Tools" ;


"
####################
#  INItialize schema classes
####################
" print ; 

ToolKit specializeAs: "Schema" ;
CoreWorkspace define: "Schema" toBe: Schema ;


Schema specializeAs: "Descriptor" at: Schema ;
Schema Descriptor specializeAs: "ClassDescriptor" at: Schema ;
Schema Descriptor specializeAs: "MessageDescriptor" at: Schema ; 
Schema Descriptor specializeAs: "MessageImplementationDescriptor" at: Schema ;

SystemDictionary newAs: "MessageDictionary" ;


