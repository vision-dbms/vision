##############################
#  ToolKit Class:  AdminTools 
#    Supports Network Statistic Messages and Cleanup
##############################

ToolKit createSubclass: "AdminTools" ;
AdminTools classDescriptor setClassTypeTo: "Core" ;
Dictionary createSubclass: "Named" at: AdminTools ;
AdminTools Named classDescriptor setClassTypeTo: "Core" ;

####################
#  New Classes at AdminTools: ObjectSpace and _POP
####################
AdminTools createSubclass: "ObjectSpace" at: AdminTools ;
AdminTools ObjectSpace classDescriptor setClassTypeTo: "Core" ;
AdminTools createSubclass: "_POP" at: AdminTools ;
AdminTools _POP classDescriptor setClassTypeTo: "Core" ;

################
#  New Class at AdminTools: ClusterType
################
AdminTools createSubclass: "ClusterType" at: AdminTools ;
AdminTools ClusterType classDescriptor setClassTypeTo: "Core" ;
AdminTools Named createInstance: "ClusterType" ;
AdminTools ClusterType classDescriptor
     setNamingDictionaryTo: AdminTools Named ClusterType ;

