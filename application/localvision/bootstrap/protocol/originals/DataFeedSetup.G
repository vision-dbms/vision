##################################################
#   Script To Setup A new Top Level Object Space
#     to house the DataFeed tools
#
#  Must be run without the -U option to create a new space
##################################################

Environment MaintenanceTools
   setupSpaceFor: "DataFeed" 
   as: "DBA" 
   description: "Data Feed Management Tools" ;


