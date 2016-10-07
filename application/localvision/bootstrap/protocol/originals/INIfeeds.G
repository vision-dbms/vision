##################################################
#  DataFeed processing : core initializations
##################################################

####################
#  class creation
####################
DataFeed defineMethod: [ | initializeGlobalSubclassProperties |
  ^super initializeGlobalSubclassProperties ;
  !newClass <- ^self ;
  newClass define: 'storeXRef' toBe: newClass asList ;
  newClass define: 'currentStore' toBe: newClass ;
  newClass establishResidenceInSpaceOf: ^global Environment DBA DataFeed ;
  ^self createNewStoreAt: ^global Environment DBA DataFeed ;
  ^self
] ;

DataFeed defineMethod: [ | createNewStore | 
  ^self createNewStoreAt: ^global 
] ;

DataFeed defineMethod: [ | createNewStoreAt: object | 
  !newStore <- ^self newPrototype clusterNew 
  do: [:defaultFlag <- TRUE ;
      ] ;
  newStore establishResidenceInSpaceOf: object ;
  object define: ^self classDescriptor code toBe: newStore; 
  ^self storeXRef , newStore; 
  ^self define: 'currentStore' toBe: newStore ; 
  newStore
] ;

DataFeed defineMethod: [ | cleanupClassStructures | 
  ^super cleanupClassStructures ;
  ^self storeXRef 
  do: [ ^self rcleanStore 
          ifTrue: [ ^self whatAmI print ; 
                    " -- cleaning store " print ; ^self displayPOP ;
                  ] ;
      ] ;
] ;

#--------------------
#  Feed Processing Classes
#--------------------
DataFeed createSubclass: "GlobalsFeed" at: CoreWorkspace ;

DataFeed createSubclass: "MasterFeed" at: CoreWorkspace ;
DataFeed createSubclass: "EntityExtenderFeed" at: CoreWorkspace ;
DataFeed createSubclass: "TransactionFeed" at: CoreWorkspace ;
DataFeed createSubclass: "MembershipFeed" at: CoreWorkspace ;
DataFeed createSubclass: "AliasFeed" at: CoreWorkspace ;
DataFeed createSubclass: "XRefFeed" at: CoreWorkspace ;
DataFeed createSubclass: "RangeGroupFeed" at: CoreWorkspace ;

GlobalsFeed classDescriptor setDescriptionTo:
  "The GlobalsFeed is used to set processing options immediately prior 
to an running an upload." ;
MasterFeed classDescriptor setDescriptionTo: 
   "Master feeds are used to create and refresh Entity instances." ;
EntityExtenderFeed classDescriptor setDescriptionTo:
  "Entity Extender feeds are used to update additional properties for
an Entity that may be directly defined at the entity or linked to the
entity via a property returning a DataRecord.  Data may be tracked for
a fixed point in time or over time." ;
TransactionFeed classDescriptor setDescriptionTo:
  "Transaction feeds are used to create and update LinkRecord subclasses
and related cross-referencing information." ;
MembershipFeed classDescriptor setDescriptionTo:
  "Membership feeds are used to update one-to-many relationships between
two Entity classes." ;
AliasFeed classDescriptor setDescriptionTo:
  "Alias feeds are used to set aliases for Entity lookup." ;
XRefFeed classDescriptor setDescriptionTo:
  "XRef feeds are used to define alternative lookup dictionaries for an
Entity class." ;
RangeGroupFeed classDescriptor setDescriptionTo:
  "RangeGroup feeds are used to define lower and upper boundaries for
a RangeClassification subclass." ;

#--------------------
#  Schema Definition Feeds
#--------------------
DataFeed createSubclass: "SchemaFeeds" at: CoreWorkspace ;

SchemaFeeds createSubclass: "ClassSetup" at: CoreWorkspace ;
SchemaFeeds createSubclass: "PropertySetup" at: CoreWorkspace ;
SchemaFeeds createSubclass: "MessageSetup" at: CoreWorkspace ;

SchemaFeeds createSubclass: "DataFeedSetup" at: CoreWorkspace ;
DataFeedSetup createSubclass: "MasterFeedSetup" at: CoreWorkspace ;
DataFeedSetup createSubclass: "EntityExtenderFeedSetup" at: CoreWorkspace ;
DataFeedSetup createSubclass: "TransactionFeedSetup" at: CoreWorkspace ;
DataFeedSetup createSubclass: "MembershipFeedSetup" at: CoreWorkspace ;
DataFeedSetup createSubclass: "AliasFeedSetup" at: CoreWorkspace ;
DataFeedSetup createSubclass: "XRefFeedSetup" at: CoreWorkspace ;
DataFeedSetup createSubclass: "RangeGroupFeedSetup" at: CoreWorkspace ;

^global Schema createMessagesForClass: ^global ;
^global Schema createMessagesForClass: CoreWorkspace ;

SchemaFeeds classDescriptor setDescriptionTo:
  "Schema feeds are used to define structures in your Vision database." ;
ClassSetup classDescriptor setDescriptionTo:
  "The ClassSetup feed is used to create core classes in your Vision database." ;
PropertySetup classDescriptor setDescriptionTo:
  "The PropertySetup feed is used to define and describe messages for the
classes in your Vision database." ;
MessageSetup classDescriptor setDescriptionTo:
  "The MessageSetup feed is used to define description information
for existings messages in your Vision database." ;

DataFeedSetup classDescriptor setDescriptionTo:
  "DataFeedSetup feeds are use to create and set options for DataFeed 
classes." ;
MasterFeedSetup classDescriptor setDescriptionTo:
  "The MasterFeedSetup feed is used to create and set options for MasterFeeds.";
EntityExtenderFeedSetup classDescriptor setDescriptionTo:
  "The EntityExtenderFeedSetup feed is used to create and set options for
EntityExtenderFeeds." ;
TransactionFeedSetup classDescriptor setDescriptionTo:
  "The TransactionFeedSetup feed is used to create and set options for
TransactionFeeds." ;
MembershipFeedSetup classDescriptor setDescriptionTo:
  "The MembershipFeedSetup feed is used to create and set options for
MembershipFeeds." ;
AliasFeedSetup classDescriptor setDescriptionTo:
  "The AliasFeedSetup is used to create and set options for AliasFeeds." ;
XRefFeedSetup classDescriptor setDescriptionTo:
  "The XRefFeedSetup is used to create and set options for XRefFeeds." ;
RangeGroupFeedSetup classDescriptor setDescriptionTo:
  "The RangeGroupFeedSetup is used to create and set options for
RangeGroupFeeds." ;

#--------------------
#  Core Feeds
#--------------------
MasterFeed createSubclass: "CurrencyMaster" at: CoreWorkspace ;
MasterFeed createSubclass: "UniverseMaster" at: CoreWorkspace ;

MembershipFeed createSubclass: "UniverseMembers" at: CoreWorkspace ;

CurrencyMaster classDescriptor setDescriptionTo:
  "The CurrencyMaster feed is used to create and refresh instances of
the class Currency." ;

UniverseMaster classDescriptor setDescriptionTo:
  "The UniverseMaster feed is used to create and refresh instances of
the class Universe." ;

UniverseMembers classDescriptor setDescriptionTo:
  "The UniverseMembers feed is used to name andndefine lists of related 
entities over time." ;
