##################################################
#   Script To Setup A new Top Level Object Space
#     to house the DataRecord classes and instances by default
#
#  Must be run without the -U option to create a new space
##################################################

Environment MaintenanceTools
   setupSpaceFor: "DataRecordClasses" 
   as: "DBA" 
   description: "Default Storage for DataRecord Instances" ;


