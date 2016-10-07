##################################################
#  INIT schema records for existing classes
#  Assumes OS3
##################################################

"
####################
#  Schema: Object
####################
" print ; 

Object define: 'classDescriptor' toBe: Schema ClassDescriptor ;
Object classDescriptor 
 do: [ :class <- ^global Object ;
       :code <- class whatAmI ; 
       :defaultAccessPath <- ^global ;
       :name <- code ; 
       :parent <- ^self ; 
       :number <- 1 ; 
       :messageType <- ^global Named MessageType Constant ;
       :usageLevel <- ^global Named UsageLevel Basic ;
       :childList <- ^global IndexedList new ;
       :inheritancePath <- ^global IndexedList new ; 
       inheritancePath at: ^self put: ^self ;
      :defaultPropertyList <- ^global IndexedList new ;
      :genericClassDescriptor <- ^self ; 
       ^self 
         setClassTypeTo: "Core" .
         setAutoCreateNamingDictionaryOff 
     ] ;


####################
#  BuiltIn Classes
####################

Schema addClass: Boolean .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: FALSE .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: TRUE .
  setClassTypeTo: "BuiltIn" ;

Schema ClassDescriptor createInstance: DateRange, "DateRange", ^global .
  setClassTypeTo: "BuiltIn" ;
Schema initializeClass: DateRange ;

Schema addClass: TheDefaultFixedProperty .
  setClassTypeTo: "BuiltIn" .
  setLevelTo: "Internal" .
  do: [ :messageType <- ^global Named MessageType FixedProperty] ;
Schema addClass: TheDefaultProperty .
  setClassTypeTo: "BuiltIn" .
  setLevelTo: "Internal" .
  do: [ :messageType <- ^global Named MessageType TimeSeriesProperty] ;

Schema addClass: Function .
  setLevelTo: "Advanced" .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: ComputedFunction .
  setLevelTo: "Advanced" .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: Block .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: Method .
  setLevelTo: "Internal" .
  setClassTypeTo: "BuiltIn" .
  do: [ :messageType <- ^global Named MessageType Method ] ;
Schema addClass: Primitive .
  setClassTypeTo: "BuiltIn" .
  setLevelTo: "Internal" .
  do: [ :messageType <- ^global Named MessageType Primitive ] ;
Schema addClass: EnumeratedFunction .
  setLevelTo: "Advanced" .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: Collection .
  setLevelTo: "Advanced" .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: IndexedCollection .
  setLevelTo: "Advanced" .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: IndexedList .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: TimeSeries .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: SequencedCollection .
  setLevelTo: "Advanced" .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: List asList .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: Value .
  setLevelTo: "Advanced" .
  setClassTypeTo: "BuiltIn" ;

Schema addClass: Ordinal .
  setLevelTo: "Advanced" .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: Character .
  setLevelTo: "Reserved" .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: Date .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: DateOffset .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: BusinessDays .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: Days .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: MonthBeginnings .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: MonthEnds .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: Months .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: QuarterBeginnings .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: QuarterEnds .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: Quarters .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: YearBeginnings .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: YearEnds .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: Years .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: Number .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: Double .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: Float .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: Integer .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: String .
  setClassTypeTo: "BuiltIn" ;

Schema addClass: Selector .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: Undefined .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: NoValue .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: Utility .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: Utility UnixSeconds at: Utility .
  setClassTypeTo: "BuiltIn" ;
Schema addClass: Utility SessionAttribute at: Utility .
  setClassTypeTo: "BuiltIn" ;

"
####################
#  Schema: Object Spaces
####################
" print ; 

#--Schema addClass: super super .
#--   setClassTypeTo: "ObjectSpace" ;
Schema addClass: super .
   setClassTypeTo: "ObjectSpace" ;
Schema addClass: ^global .
   setClassTypeTo: "ObjectSpace" ;
Environment MaintenanceTools spaceList select: [ ^self hasSchema not ] .
do: [ "Adding Object Space: " print ; whatAmI printNL ;
      ^global Schema addClass: ^self at: ^self type .
           setClassTypeTo: "ObjectSpace" ;
    ] ;


"
####################
#  Schema: Core - general
####################
" print ; 

Schema addClass: Catalog .
  setLevelTo: "Reserved" .
  setClassTypeTo: "Core" ;
Schema addClass: EntityCatalog .
  setLevelTo: "Reserved" .
  setClassTypeTo: "Core" ;
Schema addClass: ApplicationCatalog .
  setLevelTo: "Reserved" .
  setClassTypeTo: "Core" ;

Schema addClass: Chain .
  setLevelTo: "Reserved" .
  setClassTypeTo: "Core" ;
Schema addClass: Chain Link at: Chain .
  setLevelTo: "Reserved" .
  setClassTypeTo: "Core" ;

Schema addClass: Descriptor .
  setClassTypeTo: "Core" ;
Schema addClass: Date DayOfWeekDescriptor at: Date .
  setClassTypeTo: "Core" ;
Schema addClass: Date MonthDescriptor at: Date .
  setClassTypeTo: "Core" ;

Schema addClass: Dictionary .
  setClassTypeTo: "Core" ;
Schema ClassDescriptor createInstance: Environment, "Environment", ^global .
  setClassTypeTo: "Core" ;
Schema initializeClass: Environment ;
Schema ClassDescriptor createInstance: Named, "Named", ^global .
  setClassTypeTo: "Core" ;
Schema initializeClass: Named ;
Schema ClassDescriptor
  createInstance: SystemDictionary, "SystemDictionary", ^global .
  setClassTypeTo: "Core" ;
Schema initializeClass: SystemDictionary ;

Schema addClass: Entity .
  setClassTypeTo: "Core" .
  setAutoCreateNamingDictionaryOn ;
Schema addClass: SystemTable .
  setClassTypeTo: "Core" ;

Schema addClass: IncorporatorPrototype .
  setClassTypeTo: "Core" ;
Schema addClass: PrivateRecord .
  setClassTypeTo: "Core" ;
Schema addClass: Specification .
  setClassTypeTo: "Core" ;

Schema addClass: ToolKit .
  setClassTypeTo: "Core" ;
Schema addClass: Environment MaintenanceTools at: Environment .
  setClassTypeTo: "Core" ;

Schema addClass: PrinterTools .
  setClassTypeTo: "Core" ;
Schema addClass: PrinterTools Descriptor at: PrinterTools .
  setClassTypeTo: "Core" ;

Schema addClass: UserEnvironment .
  setClassTypeTo: "Core" ;

"
####################
#  Schema: Core - schema classes
####################
" print ; 

Schema addClass: Schema .
  setClassTypeTo: "Core" ;

Schema addClass: Schema Descriptor at: Schema .
  setClassTypeTo: "Core" ;
Schema addClass: Schema ClassDescriptor at: Schema .
  setClassTypeTo: "Core" ;
Schema addClass: Schema MessageDescriptor at: Schema .
  setClassTypeTo: "Core" ;
Schema addClass: Schema MessageImplementationDescriptor at: Schema .
  setClassTypeTo: "Core" ;
Schema MessageImplementationDescriptor :messageClassDescriptor <-
    Schema MessageImplementationDescriptor classDescriptor ; 

Schema addClass: MessageType .
  setClassTypeTo: "Core" .
  setNamingDictionaryTo: Named MessageType ;
Schema addClass: ClassType .
  setClassTypeTo: "Core" .
  setNamingDictionaryTo: Named ClassType ;
Schema addClass: FunctionType .
  setClassTypeTo: "Core" .
  setNamingDictionaryTo: Named FunctionType ;
Schema addClass: UsageLevel .
  setClassTypeTo: "Core" .
  setNamingDictionaryTo: Named UsageLevel ;


Schema processAllMessages ;
