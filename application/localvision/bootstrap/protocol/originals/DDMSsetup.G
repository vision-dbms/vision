##################################################
#   Script To Setup A new Top Level Object Space
#     to house the DDMS tools
#
#  Must be run without the -U option to create a new space
##################################################

Environment MaintenanceTools
   setupSpaceFor: "DDMS" 
   as: "DBA" 
   description: "Data Dictionary Management System" ;

CoreWorkspace define: "DDMS" toBe: ^global ;

