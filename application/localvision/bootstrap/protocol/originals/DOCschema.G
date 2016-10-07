

ToolKit getMessage: "isToolKit" .
   setFunctionTypeTo: "Inquiry" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: TRUE .
   setDescriptionTo: "Returns the value TRUE when recipient is any
ToolKit object." .
;

ToolKit getMessage: "description" .
   setFunctionTypeTo: "Descriptive" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: "Description of the recipient." .
;

ToolKit getMessage: "setDescriptionTo:" .
   setSelectorTo: "setDescriptionTo: text" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: ToolKit .
   setParameter: 1 typeTo: String .
   setDescriptionTo: "This method is used to update the description property." .
;

ToolKit getMessage: "_briefDescription" .
   setFunctionTypeTo: "Descriptive" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: "Property storing brief Description of the recipient." .
;

ToolKit getMessage: "briefDescription" .
   setFunctionTypeTo: "Descriptive" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: "Returns value of _briefDescription if defined,
the first line of description otherwise." .
;

ToolKit getMessage: "setBriefDescriptionTo:" .
   setSelectorTo: "setDescriptionTo: text" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: ToolKit .
   setParameter: 1 typeTo: String .
   setDescriptionTo: 
"This method is used to update the _briefDescription property." .
;

#======================================================================

Schema getMessage: "CD" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema ClassDescriptor classDescriptor .
   setDescriptionTo: "This constant returns the default instance of the 
Schema ClassDescriptor class." .
;

Schema getMessage: "ClassDescriptor" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema ClassDescriptor classDescriptor .
   setDescriptionTo: "This constant returns the default instance of the 
Schema ClassDescriptor class." .
;

Schema getMessage: "Descriptor" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema Descriptor .
   setDescriptionTo: "This constant returns the default instance of the 
Schema Descriptor class." .
;

Schema getMessage: "MD" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema MessageDescriptor .
   setDescriptionTo: "This constant returns the default instance of the 
Schema MessageDescriptor class." .
;

Schema getMessage: "MID" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema MessageImplementationDescriptor .
   setDescriptionTo: "This constant returns the default instance of the 
Schema MessageImplementationDescriptor class." .
;

Schema getMessage: "MessageDescriptor" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema MessageDescriptor .
   setDescriptionTo: "This constant returns the default instance of the 
Schema MessageDescriptor class." .
;

Schema getMessage: "MessageDictionary" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: SystemDictionary .
   setDescriptionTo: "This constant returns the internal dictionary used 
to cross-reference schema entries." .
;

Schema getMessage: "MessageImplementationDescriptor" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema MessageImplementationDescriptor .
   setDescriptionTo: "This constant returns the default instance of the 
Schema MessageImplementationDescriptor class." .
;

Schema getMessage: "addClass:" .
   setSelectorTo: "addClass: class" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema ClassDescriptor classDescriptor .
   setParameter: 1 typeTo: Object .
   setDescriptionTo: "This method creates a class descriptor instance for
the supplied class if it does not currently have one and its super class
does have a class descriptor." .
;

Schema getMessage: "addClass:as:at:" .
   setSelectorTo: "addClass: class as: name at: object" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setParameter: 1 typeTo: Object .
   setParameter: 2 typeTo: String .
   setParameter: 3 typeTo: Object .
   setDescriptionTo: "This method creates a new class descriptor instance
supplying a name to use as the class descriptor's code
(if different from the class' 'whatAmI' value) and the object representing
the default access path (if different from ^global)." .
;

Schema getMessage: "addClass:at:" .
   setSelectorTo: "addClass: class at: object" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setParameter: 1 typeTo: Object .
   setParameter: 2 typeTo: Object .
   setDescriptionTo: "This method creates a new class descriptor instance,
supplying the object representing the default access path (if different
from ^global)." .
;

Schema getMessage: "asSelf" .
   setFunctionTypeTo: "Conversion" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema .
   setDescriptionTo: "Returns the default Schema object, stripped of
any extensions." .
;

Schema getMessage: "audit" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: NoValue .
   setDescriptionTo: "This method is run by the 'Schema processAllMessages' 
message, summarizing the status of the schema classes after the processing 
has run." .
;

Schema getMessage: "classDescriptor" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema ClassDescriptor classDescriptor .
   setDescriptionTo: "Returns the class descriptor for the Schema class." .
;

Schema getMessage: "cleanupForClasses:" .
   setSelectorTo: "cleanupForClasses: cdList" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: NoValue .
   setParameter: 1 typeTo: List .
   setDescriptionTo: "This method runs as part of the 'Schema
processAllMessages' process and is not invoked directly." .
;

Schema getMessage: "createNewMessagesForClasses:" .
   setSelectorTo: "createNewMessagesForClasses: cdList" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: NoValue .
   setParameter: 1 typeTo: List .
   setDescriptionTo: "This method runs as part of the 'Schema
processAllMessages' process and is not invoked directly." .
;

Schema getMessage: "defaultInstance" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema .
   setDescriptionTo: "Returns the default Schema instance." .
;

Schema getMessage: "deleteMessage:in:" .
   setSelectorTo: "deleteMessage: name in: cd" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Object .
   setParameter: 1 typeTo: String .
   setParameter: 2 typeTo: Schema CD classDescriptor .
   setDescriptionTo: "This method runs as part of the 'deleteMessage:' 
method and is not invoked directly." .
;

Schema getMessage: "displayStatus" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: NoValue .
   setDescriptionTo: "This message displays a description of the current
Schema class status." .
;

Schema getMessage: "getClassesNeedingUpdate" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setReturnObjectContainerTo: List .
   setDescriptionTo: "This method runs as part of the 'Schema
processAllMessages' process and is not invoked directly." .
;

Schema getMessage: "initializeClass:" .
   setSelectorTo: "initializeClass: class" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: NoValue .
   setParameter: 1 typeTo: Object .
   setDescriptionTo: "This method runs as part of the 'addClass:' process
and is not invoked directly." .
;

Schema getMessage: "initializeForPosting" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: NoValue .
   setDescriptionTo: "This method runs as part of the 'Schema
processAllMessages' process and is not invoked directly." .
;

Schema getMessage: "isSchema" .
   setFunctionTypeTo: "Inquiry" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: TRUE .
   setDescriptionTo: "Returns the value TRUE when recipient is any
Schema object." .
;

Schema getMessage: "lastPosting" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Inquiry" .
   setReturnObjectTypeTo: Utility UnixSeconds .
   setDescriptionTo: "This property contains the time stamp of the last 
Schema update." .
;

Schema getMessage: "processAllMessages" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: NoValue .
   setDescriptionTo: "This method is used to post all new and updated
messages in the schema
database.  It is periodically run as part of standard update procedures
and can be executed temporarily during a session in which you have defined new
messages." .
;

Schema getMessage: "refreshExistingSelectorsForClasses:" .
   setSelectorTo: "refreshExistingSelectorsForClasses: cdList" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: NoValue .
   setParameter: 1 typeTo: List .
   setDescriptionTo: "This method runs as part of the 'Schema
processAllMessages' process and is not invoked directly." .
;

Schema getMessage: "setDefaultValueOf:at:to:" .
   setSelectorTo: "setDefaultValueOf: name at: cd to: default" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Object .
   setParameter: 1 typeTo: String .
   setParameter: 2 typeTo: Schema CD classDescriptor .
   setParameter: 3 typeTo: Object .
   setDescriptionTo: "This method runs as part of 
the 'defineFixedProperty:withDefault:'  method and is not invoked directly." .
;

Schema getMessage: "setSiteNameTo:" .
   setSelectorTo: "setSiteNameTo: name" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema .
   setParameter: 1 typeTo: String .
   setDescriptionTo: "Defines a constant representing the name of the
Vision database." .
;

Schema getMessage: "siteDescription" .
   setFunctionTypeTo: "Descriptive" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Object .
   setDescriptionTo: "Returns a description of this Vision database." .
;

Schema getMessage: "siteName" .
   setFunctionTypeTo: "Descriptive" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: "Returns the name of this Vision database" .
;

Schema getMessage: "updateMethod:at:with:" .
   setSelectorTo: "updateMethod: name at: cd with: source" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: NoValue .
   setParameter: 1 typeTo: String .
   setParameter: 2 typeTo: Schema CD classDescriptor .
   setParameter: 3 typeTo: String .
   setDescriptionTo: "This method runs as part of
the  'defineMethod:' method when you provide your method definition as
a text string." .
;

Schema getMessage: "whatAmI" .
   setFunctionTypeTo: "Descriptive" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: "Returns the name of the recipient's class." .
;

Schema Descriptor getMessage: "asSelf" .
   setFunctionTypeTo: "Conversion" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema Descriptor .
   setDescriptionTo: "Returns the default Schema Descriptor object, stripped of
any extensions." .
;

Schema Descriptor getMessage: "classDescriptor" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema ClassDescriptor classDescriptor .
   setDescriptionTo: "Returns the class descriptor for the Schema Descriptor
class." .
;

Schema Descriptor getMessage: "defaultInstance" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema Descriptor .
   setDescriptionTo: "Returns the default Schema Descriptor instance." .
;


Schema Descriptor getMessage: "isDescriptor" .
   setFunctionTypeTo: "Inquiry" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: TRUE .
   setDescriptionTo: "Returns the value TRUE when sent to any 
Schema Descriptor." .
;

Schema Descriptor getMessage: "whatAmI" .
   setFunctionTypeTo: "Descriptive" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: "Returns the string \"Schema Descriptor\". " .
;

Schema ClassDescriptor getMessage: "_htmlDocName" .
   setFunctionTypeTo: "Descriptive" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: "Returns the name of the HTML document that describes
this class in detail on insyte's www.insytenet.com web site." .
;

Schema ClassDescriptor getMessage: "_imageName" .
   setFunctionTypeTo: "Descriptive" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: "Returns the name of the image file associated
with this class." .
;

Schema ClassDescriptor getMessage: "asClassDescriptor" .
   setFunctionTypeTo: "Conversion" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Object .
   setDescriptionTo: "Returns the recipient stripped of extensions." .
;

Schema ClassDescriptor getMessage: "asSelf" .
   setFunctionTypeTo: "Conversion" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema ClassDescriptor classDescriptor .
   setDescriptionTo: "Returns the recipient, stripped of extensions." .
;

Schema ClassDescriptor getMessage: "assignDefaultType" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setDescriptionTo: "Updates the 'type' property in the recipient with a
default ClassType based on the underlying class.  This method runs as a
part of new instance initializations." .
;

Schema ClassDescriptor getMessage: "autoCreateNamingDictionary" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Boolean .
   setDescriptionTo: "Indicates whether the 'createSubclass' message should
automatically create a new naming dictionary for the class.  This property
is set using the 'setAutoCreateNamingDictionaryOn' 
and 'setAutoCreateNamingDictionaryOff' messages." .
;

Schema ClassDescriptor getMessage: "buildInheritance" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: NoValue .
   setDescriptionTo: "This method is invoked by the 'initializeLocalAttributes'
message and is used to create the super and subclass maps for the recipient
class descriptor.  It should not be used directly." .
;

Schema ClassDescriptor getMessage: "childList" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setReturnObjectContainerTo: IndexedList .
   setParameter: 1 typeTo: Schema CD classDescriptor .
   setDescriptionTo: "This property contains an indexed list of 
class descriptors that are direct subclasses of the recipient's class." .
;

Schema ClassDescriptor getMessage: "class" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Object .
   setDescriptionTo: 
"This property returns the default instance of the actual class associated
with this class descriptor." .
;

Schema ClassDescriptor getMessage: "classDescriptor" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema ClassDescriptor classDescriptor .
   setDescriptionTo: 
"This constant returns the class descriptor for the Schema ClassDescriptor
class." .
;

Schema ClassDescriptor getMessage: "clearDefaultValueUsing:" .
   setSelectorTo: "clearDefaultValueUsing: mid" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: NoValue .
   setParameter: 1 typeTo: Object .
   setDescriptionTo: 
"This method is used by the 'clearDefaultValue' message defined at
MessageImplementationDescriptor and should not be invoked directly." .
;

Schema ClassDescriptor getMessage: "clearMessageUpdateFlag" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: NoValue .
   setDescriptionTo: 
"This method is used to clear the flag that indicates that messages have
been updated for this class.  It is not usually invoked directly." .
;

Schema ClassDescriptor getMessage: "clusterStrings" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: NoValue .
   setDescriptionTo: "This method is used as part of schema maintenance and
is not invoked directly." .
;

Schema ClassDescriptor getMessage: "createNamingDictionaryAt:" .
   setSelectorTo: "createNamingDictionaryAt: dictionary" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setParameter: 1 typeTo: Dictionary .
   setDescriptionTo: 
"This method is used to create a naming dictionary for the class if one
does not already exist.  The parameter should itself be a dictionary
that is used to name references to other dictionaries (e.g., Named).
This method creates a new instance of this dictionary, assigns it to
the 'namingDictionary' property at the class descriptor, and defines
the class descriptor's code at the supplied dictionary to return this
new dictionary.  Current instances in the class are then added to
the dictionary. For example, to create a dictionary for MyClass at Named 
use:

    MyClass classDescriptor createNamingDictionaryAt: Named ;

If MyClass had an instance with code \"my1\", the expression:

       Named MyClass my1

would access this instance.  All new instances would automatically get
cross-referenced in this naming dictionary." .
;

Schema ClassDescriptor getMessage: "defaultAccessPath" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Object .
   setDescriptionTo: 
"This property contains a reference to the class at which this class
was defined.  For example, the expression:

     Schema ClassDescriptor classDescriptor defaultAccessPath

returns the class Schema." .
;

Schema ClassDescriptor getMessage: "defaultInstance" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema ClassDescriptor classDescriptor .
   setDescriptionTo: "Returns the default Schema ClassDescriptor" .
;

Schema ClassDescriptor getMessage: "defaultPropertyList" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MID .
   setReturnObjectContainerTo: IndexedList .
   setParameter: 1 typeTo: Schema MD .
   setDescriptionTo: 
"This property contains a list, indexed by MessageDescriptor, of
MessageImplementationDescriptors that provide default values.  This
list is used to initialize new instances of the class.  The list is
maintained by the messages defined at MessageImplementationDescriptor
and should not be changed directly." .
;

Schema ClassDescriptor getMessage: "flagForMessageUpdate" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: NoValue .
   setDescriptionTo: 
"This method is used to set the flag that indicates that messages have
been updated for this class.  It is not usually invoked directly." .
;

Schema ClassDescriptor getMessage: "genericClassDescriptor" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Object .
   setDescriptionTo: "" .
;

Schema ClassDescriptor getMessage: "getAllChildren" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setReturnObjectContainerTo: List .
   setDescriptionTo: 
"This message returns a list of class descriptors that refer to sub
classes of the recipient." .
;

Schema ClassDescriptor getMessage: "getInheritancePath" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setReturnObjectContainerTo: List .
   setDescriptionTo: 
"This message returns a list of class descriptors representing the
super class hierarchy of the recipient.  The list is sorted with the
class descriptor for Object first and the class descriptor for the
recipient class last." .
;

Schema ClassDescriptor getMessage: "getMessageImplementationFor:" .
   setSelectorTo: "getMessageImplementationFor: message" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MID .
   setParameter: 1 typeTo: String .
   setDescriptionTo: 
"This method is invoked by the \"getMessage:\" message and is not used
directly." .
;

Schema ClassDescriptor getMessage: "getMessagesReturningClass" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema MID .
   setReturnObjectContainerTo: List .
   setDescriptionTo: 
"Returns the list of MessageImplementationDescriptors that return an
instance of the recipient's class or one of its subclasses.  " .
;

Schema ClassDescriptor getMessage: "getMessagesReturningSuperClass" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema MID .
   setReturnObjectContainerTo: List .
   setDescriptionTo: 
"Returns the list of MessageImplementationDescriptors that return an
instance of one of the recipient's super classes.  This is useful if
you want to find the messages that potentially return an instance of
this class.  For example, a message that returns an arbitrary Entity, could
potentially return a Currency, Security, or Country object." .
;

Schema ClassDescriptor getMessage: "htmlDocName" .
   setFunctionTypeTo: "Descriptive" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: "Returns the name of the HTML document that describes
this class in detail on insyte's www.insytenet.com web site; if not document
is specifically defined, the document name associated with the first super
class in the recipient's hierarchy that has a document is returned." .
;

Schema ClassDescriptor getMessage: "imageName" .
   setFunctionTypeTo: "Descriptive" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: "Returns the name of the image file associated with
this class formatted as an image tag if defined.  The class name is
returned otherwise." .
;

Schema ClassDescriptor getMessage: "includeInDataModel" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Boolean .
   setDescriptionTo: "Returns TRUE if the recipient's class is an
application class, FALSE otherwise." .
;

Schema ClassDescriptor getMessage: "inheritancePath" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setReturnObjectContainerTo: IndexedList .
   setParameter: 1 typeTo: Schema CD classDescriptor .
   setDescriptionTo: 
"This property returns a list, indexed by class descriptor of the
class descriptors in the recipient's super class hierarchy including
the recipient." .
;

Schema ClassDescriptor getMessage: "inheritsFrom:" .
   setSelectorTo: "inheritsFrom: classDescriptor" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Boolean .
   setParameter: 1 typeTo: Schema CD classDescriptor .
   setDescriptionTo: 
"This method returns TRUE if the class associated with the recipient object
inherits from the class associated with the class descriptor provided,
FALSE otherwise.  This is similar to the version at Object except the
class descriptors of the two classes are used for the inquiry." .
;

Schema ClassDescriptor getMessage: "initializeDefaultInstance" .
   setFunctionTypeTo: "Creation" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setDescriptionTo: 
"This method is run as part of the 'initializeLocalAttributes' step of
new class descriptor creation.  If the class has a naming dictionary,
it adds a reference to the newly created Class as \"Default\".  If the
new class shares the parent's naming dictionary, the name added to
the dictionary concatenates the class name to the string \"Default\".
" .
;

Schema ClassDescriptor getMessage: "initializeDefaultPropertyList" .
   setFunctionTypeTo: "Creation" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setDescriptionTo: 
"This method is run as part of the 'initializeLocalAttributes' step of
new class descriptor creation.  It copies the defaultPropertyList from
its parent class descriptor." .
;

Schema ClassDescriptor getMessage: "initializeKeys:" .
   setSelectorTo: "initializeKeys: keys" .
   setFunctionTypeTo: "Creation" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setParameter: 1 typeTo: List .
   setDescriptionTo: 
"This method is run as part of the 'createInstance:' process.  Three
keys are supplied: the default instance of the class, the class name,
and the default access path.  These values are used to set the values
of the 'class', 'code', and 'defaultAccessPath' properties in the new
class descriptor." .
;

Schema ClassDescriptor getMessage: "initializeLocalAttributes" .
   setFunctionTypeTo: "Creation" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setDescriptionTo: 
"This method is run as part of the 'createInstance:' process.  It sets
the values for a number of properties, builds the inheritance map,
initializes the default propertyList, creates a naming dictionary if
appropriate, and initializes the default instance." . ;

Schema ClassDescriptor getMessage: "initializeNamingDictionary" .
   setFunctionTypeTo: "Creation" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setDescriptionTo: 
"This method looks at the 'autoCreateNamingDictionary' flag and
creates a new naming dictionary if it is set to TRUE.  It
creates the dictionary in the same place as its parent's naming
dictionary if one exists, otherwise it creates it at ^global Named." .
;

Schema ClassDescriptor getMessage: "initializeStringCluster" .
   setFunctionTypeTo: "Creation" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setDescriptionTo: 
"This method creates the initial string cluster for the underlying class." .
;

Schema ClassDescriptor getMessage: "isApplicationClass" .
   setFunctionTypeTo: "Inquiry" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Boolean .
   setDescriptionTo: "Returns TRUE if the recipient's type is 
Named ClassType Application." .
;

Schema ClassDescriptor getMessage: "isBuiltInClass" .
   setFunctionTypeTo: "Inquiry" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Boolean .
   setDescriptionTo: 
"Returns TRUE if the recipient's type is Named ClassType BuiltIn." .
;

Schema ClassDescriptor getMessage: "isClassDescriptor" .
   setFunctionTypeTo: "Inquiry" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: TRUE .
   setDescriptionTo: "Returns the value TRUE." .
;

Schema ClassDescriptor getMessage: "isInInheritancePathOf:" .
   setSelectorTo: "isInInheritancePathOf: classDescriptor" .
   setFunctionTypeTo: "Inquiry" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Boolean .
   setParameter: 1 typeTo: Schema CD classDescriptor .
   setDescriptionTo: 
"This method returns TRUE if the recipient inherits from the supplied
parameter or the supplied parameter inherits from the recipient." .
;

Schema ClassDescriptor getMessage: "isIncorporatorClass" .
   setFunctionTypeTo: "Inquiry" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Boolean .
   setDescriptionTo: 
"Returns TRUE if the recipient's class inherits from IncorporatorPrototype." .
;

Schema ClassDescriptor getMessage: "isObjectDescriptor" .
   setFunctionTypeTo: "Inquiry" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Boolean .
   setDescriptionTo: 
"This method returns TRUE if the recipient is Object's classDescriptor." .
;

Schema ClassDescriptor getMessage: "isObjectSpaceClass" .
   setFunctionTypeTo: "Inquiry" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Boolean .
   setDescriptionTo: 
"Returns TRUE if the recipient's type is Named ClassType ObjectSpace." .
;

Schema ClassDescriptor getMessage: "isSubClassOf:" .
   setSelectorTo: "isSubClassOf: classDescriptor" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Boolean .
   setParameter: 1 typeTo: Schema CD classDescriptor .
   setDescriptionTo: "Returns TRUE if the recipient is a sub class of
the parameter." .
;

Schema ClassDescriptor getMessage: "isSuperClassOf:" .
   setSelectorTo: "isSuperClassOf: classDescriptor" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Boolean .
   setParameter: 1 typeTo: Schema CD classDescriptor .
   setDescriptionTo: "Returns TRUE if the recipient is a super class of
the parameter." .
;

Schema ClassDescriptor getMessage: "messageType" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: MessageType .
   setDescriptionTo: "Used to determine the underlying type of a message,
this message is not normally used directly." .
;

Schema ClassDescriptor getMessage: "messagesHaveBeenUpdated" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Boolean .
   setDescriptionTo: 
"This property flags classes whose message dictionary has changed since
messages were last posted." .
;

Schema ClassDescriptor getMessage: "name" .
   setFunctionTypeTo: "Descriptive" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: "Returns descriptive name for recipient.
This is normally the 'whatAmI' for the underlying class." .
;

Schema ClassDescriptor getMessage: "namingDictionary" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Dictionary .
   setDescriptionTo: 
"This property returns the naming dictionary for the class." .
;

Schema ClassDescriptor getMessage: "number" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Integer .
   setDescriptionTo: 
"This property contains the instance number for this class descriptor." .
;

Schema ClassDescriptor getMessage: "objectDescriptor" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema ClassDescriptor classDescriptor .
   setDescriptionTo: 
"This constant returns the classDescriptor for the class Object." .
;

Schema ClassDescriptor getMessage: "parent" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setDescriptionTo: 
"This property returns the class descriptor associated with the super
class of the recipient's underlying class." .
;

Schema ClassDescriptor getMessage: "primaryStringCluster" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: 
"This property returns the primary string cluster defined for the
recipient's underlying class." .
;

Schema ClassDescriptor getMessage: "print" .
   setFunctionTypeTo: "Display" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setDescriptionTo: "Prints the recipient object's code." .
;

Schema ClassDescriptor getMessage: "print:" .
   setSelectorTo: "print: format" .
   setFunctionTypeTo: "Display" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setParameter: 1 typeTo: Integer .
   setDescriptionTo: "Prints the recipient object's code." . 
;

Schema ClassDescriptor getMessage: "sampleUsage" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: "Describes sample syntax for accessing instances
of the recipient's underlying class." .
;

Schema ClassDescriptor getMessage: "setAutoCreateNamingDictionaryOff" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setDescriptionTo: 
"This method is used to indicate that new subclasses should not
automatically create new naming dictionaries." .
;

Schema ClassDescriptor getMessage: "setAutoCreateNamingDictionaryOn" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setDescriptionTo: 
"This method is used to indicate that new subclasses should
automatically create new naming dictionaries." .
;

Schema ClassDescriptor getMessage: "setClassTypeTo:" .
   setSelectorTo: "setClassTypeTo: typeCode" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setParameter: 1 typeTo: String .
   setDescriptionTo: 
"This method is used to change the value of 'type' from its initial value
(e.g., Named ClassType Application) to a different type.  Available types are:
BuiltIn, Application, DataFeed, ToolKit, Dictionary, ObjectSpace, Other,
and Unused.  The initial value of type is set during instance creation
using the method 'assignDefaultType'. " .
;

Schema ClassDescriptor getMessage: "setGenericClassTo:" .
   setSelectorTo: "setGenericClassTo: descriptor" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setParameter: 1 typeTo: Schema CD classDescriptor . 
   setDescriptionTo: "" .
;

Schema ClassDescriptor getMessage: "setHtmlDocNameTo:" .
   setSelectorTo: "setHtmlDocNameTo: string" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setParameter: 1 typeTo: String .
   setDescriptionTo: "Redefines the name of the html document describing
the recipient's underlying class." .
;

Schema ClassDescriptor getMessage: "setNamingDictionaryTo:" .
   setSelectorTo: "setNamingDictionaryTo: dictionary" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo:  Schema CD classDescriptor .
   setParameter: 1 typeTo: Dictionary .
   setDescriptionTo: 
"This method is used to set the naming dictionary for the recipient to
an existing dictionary." .
;

Schema ClassDescriptor getMessage: "setSampleUsageTo:" .
   setSelectorTo: "setSampleUsageTo: text" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setParameter: 1 typeTo: String .
   setDescriptionTo: "Updates the property 'sampleUsage'. " .
;

Schema ClassDescriptor getMessage: "showChildren" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: NoValue .
   setDescriptionTo: 
"This method recursively displays each subclass of the recipient class." .
;

Schema ClassDescriptor getMessage: "showChildrenAtLevel:" .
   setSelectorTo: "showChildrenAtLevel: level" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: NoValue .
   setParameter: 1 typeTo: Integer .
   setDescriptionTo: "This method is used to implement the recursive display
of child classes in the 'showChildren' method." .
;

Schema ClassDescriptor getMessage: "showInheritance" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: NoValue .
   setDescriptionTo: 
"This method displays the inheritance hierarchy for the recipient." .
;

Schema ClassDescriptor getMessage: "showParents" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: NoValue .
   setDescriptionTo: 
"This method displays the super class hierarchy for the recipient." .
;

Schema ClassDescriptor getMessage: "trackDefaultValueUsing:" .
   setSelectorTo: "trackDefaultValueUsing: mid" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: NoValue .
   setParameter: 1 typeTo: Schema MID .
   setDescriptionTo: 
"This method is used to manage the default values for the class and should
not be accessed directly." .
;

Schema ClassDescriptor getMessage: "type" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: ClassType .
   setDescriptionTo: 
"This property returns an instance of the class ClassType.  Available types 
are: BuiltIn, Application, DataFeed, ToolKit, Dictionary, ObjectSpace, Other,
and Unused.  The initial value of type is set during instance creation
using the method 'assignDefaultType'. " .
;

Schema ClassDescriptor getMessage: "whatAmI" .
   setFunctionTypeTo: "Descriptive" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: "Returns the string \"Schema ClassDescriptor\". " .
;

Schema MessageDescriptor getMessage: "MessageDictionary" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: SystemDictionary .
   setDescriptionTo: 
"Returns an internal dictionary used to cross reference all message names." .
;

Schema MessageDescriptor getMessage: "asSelf" .
   setFunctionTypeTo: "Conversion" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema MD .
   setDescriptionTo: "Returns the recipient, stripped of extensions." .
;

Schema MessageDescriptor getMessage: "classDescriptor" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema ClassDescriptor classDescriptor .
   setDescriptionTo: 
"This constant returns the class descriptor for the Schema MD class." .
;

Schema MessageDescriptor getMessage: "clusterStrings" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: NoValue .
   setDescriptionTo: 
"This method is used as part of the schema maintenance step and
is not invoked directly." .
;

Schema MessageDescriptor getMessage: "defaultInstance" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema MessageDescriptor .
   setDescriptionTo: "Returns the default Schema MessageDescriptor instance." .
;

Schema MessageDescriptor getMessage: "functionType" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: FunctionType .
   setDescriptionTo: 
"This property returns an instance of the class FunctionType.  You can
create and set function types with the 'setFunctionTypeTo:' message." .
;

Schema MessageDescriptor getMessage: "getMessage:in:" .
   setSelectorTo: "getMessage: string in: cd" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MID .
   setParameter: 1 typeTo: String .
   setParameter: 2 typeTo: Schema CD classDescriptor .
   setDescriptionTo: 
"This method returns the MessageImplementationDescriptor for the supplied
message string and classDescriptor combination." .
;

Schema MessageDescriptor getMessage: "getMessageFor:" .
   setSelectorTo: "getMessageFor: string" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MD .
   setParameter: 1 typeTo: String .
   setDescriptionTo: 
"This method returns the MessageDescriptor for the supplied string." .
;

Schema MessageDescriptor getMessage: "getOrCreate:in:" .
   setSelectorTo: "getOrCreate: name in: cd" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MD .
   setParameter: 1 typeTo: String .
   setParameter: 2 typeTo: Schema CD classDescriptor .
   setDescriptionTo: 
"This method returns the MessageImplementationDescriptor for the message
string and classDescriptor provided, creating new instances of
Schema MessageDescriptor and Schema MessageImplementation as needed." .
;

Schema MessageDescriptor getMessage: "implementationList" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema MID .
   setReturnObjectContainerTo: IndexedList .
   setParameter: 1 typeTo: Schema CD classDescriptor .
   setDescriptionTo: 
"This property contains an indexed list of message implementation
descriptors defined for the message associated with the recipient.
The list is indexed by classDescriptor." .
;

Schema MessageDescriptor getMessage: "initializeKeys:" .
   setSelectorTo: "initializeKeys: key" .
   setFunctionTypeTo: "Creation" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MD .
   setParameter: 1 typeTo: List .
   setDescriptionTo: 
"This method is run as part of the 'createInstance:' process.  A single
string representing the message is supplied as the key." .
;

Schema MessageDescriptor getMessage: "initializeLocalAttributes" .
   setFunctionTypeTo: "Creation" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MD .
   setDescriptionTo: 
"This method is run as part of the 'createInstance:' process." .
;

Schema MessageDescriptor getMessage: "internalSelector" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: 
"This property is used to store an internal representation of the selector
used to define the message." .
;

Schema MessageDescriptor getMessage: "isMessageDescriptor" .
   setFunctionTypeTo: "Inquiry" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: TRUE .
   setDescriptionTo: "Returns the value TRUE." .
;

Schema MessageDescriptor getMessage: "message" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: "This method returns the recipient's message name." .
;

Schema MessageDescriptor getMessage: "setDescriptionTo:" .
   setSelectorTo: "setDescriptionTo: text" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MD .
   setParameter: 1 typeTo: String .
   setDescriptionTo: "Sets the description for this message descriptor and
sets the description for any implementations of this message that 
currently have no description." .
;

Schema MessageDescriptor getMessage: "setFunctionTypeTo:" .
   setSelectorTo: "setFunctionTypeTo: type" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MD .
   setParameter: 1 typeTo: String .
   setDescriptionTo: 
"This method is used to set or change the value of the 'functionType'
property.  The parameter can be an instance of the class FunctionType
or a string.  If a string is supplied, and a function type instance by
that name does not exist, a new instance of FunctionType is created.
" .
;

Schema MessageDescriptor getMessage: "whatAmI" .
   setFunctionTypeTo: "Descriptive" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: "Returns the string \"Schema MessageDescriptor\". " .
;

Schema MessageImplementationDescriptor getMessage: "accessBlock" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Block .
   setDescriptionTo: 
"This property contains a block that could be sent to an object to execute
this message." .
;

Schema MessageImplementationDescriptor getMessage: "asSelf" .
   setFunctionTypeTo: "Conversion" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema MID .
   setDescriptionTo: "Returns the recipient, stripped of extensions." .
;

Schema MessageImplementationDescriptor getMessage: "class" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Object .
   setDescriptionTo: 
"Returns the default instance of the underlying class associated with
the recipient." .
;

Schema MessageImplementationDescriptor getMessage: "classDescriptor" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema ClassDescriptor classDescriptor .
   setDescriptionTo: 
"This constant returns the class descriptor for the class Schema
MessageImplementationDescriptor." .
;

Schema MessageImplementationDescriptor getMessage: "cleanupDeletedObject" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MID .
   setDescriptionTo: 
"This method is executed by the 'flagForDeletionWithReason:' message and
should not be accessed directly. " .
;

Schema MessageImplementationDescriptor getMessage: "clearDefaultValue" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MID .
   setDescriptionTo: 
"This message is used to clear the default value associated with the
recipient.  Default values are used to initialize new instances of
the class during instance creation." .
;

Schema MessageImplementationDescriptor getMessage: "clusterStrings" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MID .
   setDescriptionTo: 
"This method is used as part of the schema maintenance step and
is not invoked directly." .
;

Schema MessageImplementationDescriptor getMessage: "createdInLastPosting" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: TRUE .
   setDescriptionTo: 
"This property is used to flag messages that were created during the
most recent execution of the 'Schema processAllMessages' processing." .
;

Schema MessageImplementationDescriptor getMessage: "defaultInstance" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema MessageImplementationDescriptor .
   setDescriptionTo: "Returns the default Schema 
MessageImplementationDescriptor." .
;

Schema MessageImplementationDescriptor getMessage: "defaultValue" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Object .
   setDescriptionTo: 
"This property is used to store the default value associated with this
message.  The value is set and cleared using the 'setDefaultValueTo:'
and 'clearDefaultValue' messages." .
;

Schema MessageImplementationDescriptor getMessage: "describe" .
   setFunctionTypeTo: "Display" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: NoValue .
   setDescriptionTo: 
"This method is used to display descriptive information about the
recipient in a multi-line format." .
;

Schema MessageImplementationDescriptor getMessage: "displayInfo" .
   setFunctionTypeTo: "Display" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema MID  .
   setDescriptionTo: 
"Displays the message, type, and class associated with the recipient" .
;

Schema MessageImplementationDescriptor getMessage: "flagAsKeyField" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MID .
   setDescriptionTo: 
"Flags the message to indicate it could serve as the 'key' for the class.
Although, Vision does not require or use directly, this is sometimes 
useful for classes that are presented as part of a data model." .
;

Schema MessageImplementationDescriptor getMessage: "flagAsPartialKeyField" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MID .
   setDescriptionTo:
"Flags the message to indicate it could serve as one of 
the 'keys' for the class.
Although, Vision does not require or use directly, this is sometimes 
useful for classes that are presented as part of a data model." .
;

Schema MessageImplementationDescriptor getMessage: "formatMessageDescription" .
   setFunctionTypeTo: "Display" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: 
"Formats the description to adjust for various conventions used by 
different document browsers." .
;

Schema MessageImplementationDescriptor getMessage: "formatMessageImplementation" .
   setFunctionTypeTo: "Display" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: 
"Formats the implementation of the message as a single string that
could be used to re-define the message." .
;

Schema MessageImplementationDescriptor getMessage: "fullSelector" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: "Returns the name of the message with appropriate
parameters names filled in for binary and keyword messages." .
;

Schema MessageImplementationDescriptor getMessage: "functionType" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: FunctionType .
   setDescriptionTo: 
"This property returns an instance of the class FunctionType.  You can
create and set function types with the 'setFunctionTypeTo:' message." .
;

Schema MessageImplementationDescriptor getMessage: "getInternalMessage" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: 
"This method returns the internal form of the underlying message and
is not used directly." .
;

Schema MessageImplementationDescriptor getMessage: "getSelector" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: 
"This method returns the underlying message and is used to initialize
the 'fullSelector' property." .
;

Schema MessageImplementationDescriptor getMessage: "hasDefaultValue" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Boolean .
   setDescriptionTo: 
"This property contains the value TRUE if the recipient has a default
value, NA otherwise.  Its value is controlled by the 'setDefaultValueTo:'
and 'clearDefaultValue' messages." .
;

Schema MessageImplementationDescriptor getMessage: "implementationSource" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: 
"This property tracks the actual implementation of a message over time.
Each time the message definition changes, this time series is updated
to reflect the new value.  This property can be accessed directly to
display current and prior values.  Updates to this property are managed
by the 'Schema processAllMessages' process." .
;

Schema MessageImplementationDescriptor getMessage: "includeInDataModel" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Boolean .
   setDescriptionTo: "This method returns TRUE if the functionType 
property is set to the Data instance." .
;

Schema MessageImplementationDescriptor getMessage: "initializeForPosting" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: NoValue .
   setDescriptionTo: 
"This method runs as part of the 'Schema processAllMessages' process." .
;

Schema MessageImplementationDescriptor getMessage: "initializeKeys:" .
   setSelectorTo: "initializeKeys: keys" .
   setFunctionTypeTo: "Creation" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MID .
   setParameter: 1 typeTo: List .
   setDescriptionTo: 
"This method is run as part of the 'createInstance:' process.  The
first key provided represents the message descriptor and the second key
is the class descriptor." .
;

Schema MessageImplementationDescriptor getMessage: "initializeLocalAttributes" .
   setFunctionTypeTo: "Creation" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MID .
   setDescriptionTo: 
"This method is run as part of the 'createInstance:' process." .
;

Schema MessageImplementationDescriptor getMessage: "internalMethod" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: 
"This property contains the internal form of the underlying message and
is not used directly." .
;

Schema MessageImplementationDescriptor getMessage: "isMessageImplementationDescriptor" .
   setFunctionTypeTo: "Inquiry" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: TRUE .
   setDescriptionTo: "Returns the value TRUE." .
;

Schema MessageImplementationDescriptor getMessage: "isTimeVarying" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Boolean .
   setDescriptionTo: "This method returns TRUE if the 'type' property refers
to a TimeSeriesProperty or a TimeVaryingMethod.  Note that to a flag a 
method as time varying, you need to explicitly set its type.  Otherwise,
the type is determined automatically by the method that creates and
manages these descriptors." .
;

Schema MessageImplementationDescriptor getMessage: "keyType" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: KeyType .
   setDescriptionTo: "Returns the KeyType (Full or Partial) if appropriate,
NA otherwise." .
;

Schema MessageImplementationDescriptor getMessage: "message" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: 
"This method returns the string associated with the message.  The
actual message is stored in the 'code' property." .
;

Schema MessageImplementationDescriptor getMessage: "messageClassDescriptor" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setDescriptionTo: 
"This property returns the class descriptor for the class associated
with this recipient." .
;

Schema MessageImplementationDescriptor getMessage: "messageDescriptor" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema MD .
   setDescriptionTo: 
"This property returns the message descriptor for the message associated
with this recipient." .
;

Schema MessageImplementationDescriptor getMessage: "parameterList" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setReturnObjectContainerTo: IndexedList .
   setParameter: 1  typeTo: Integer .
   setDescriptionTo: 
"Returns an indexed list of class descriptors describing the parameters
associated with this message if this is a keyword or binary and message.
The first parameter is used to store the index type if this message
returns its value as an IndexedList." .
;

Schema MessageImplementationDescriptor getMessage: "refreshValues" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: NoValue .
   setDescriptionTo: 
"This method is executed as part of the 'Schema processAllMessages'
process." .
;

Schema MessageImplementationDescriptor getMessage: "refreshedInLastPosting" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Boolean .
   setDescriptionTo: 
"This property is used to flag messages that were refreshed during the
most recent execution of the 'Schema processAllMessages' processing." .
;

Schema MessageImplementationDescriptor getMessage: "returnObjectContainer" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setDescriptionTo: 
"This property contains the class descriptor for the type of container
returned by this message.  Objects can be returned as a single scalar
value, in which case the return object container is Object.  Some
messages return collections of values, in which case the return object
container will be List, IndexedList, or TimeSeries.  A message that
returns a TimeSeries should not be confused with a time series property
or a method that varies over time.  Time varying messages will produce
different results if they are evaluated as of different dates but do
not actually return a TimeSeries object.  A
message that returns a TimeSeries container is actually returning
a TimeSeries object which responds to all the TimeSeries messages." .
;

Schema MessageImplementationDescriptor getMessage: "returnObjectType" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema CD classDescriptor .
   setDescriptionTo: 
"This property contains the class descriptor for the type of object
returned by this message.  By default, the class descriptor for Object
is returned.  You can change this value using the 'setReturnObjectTypeTo:'
message.  If the message returns a collection of objects, this property
is set to the type of object included in the collection.  The 
property 'returnObjectContainer' would indicate that the result is returned 
as a collection." .
;

Schema MessageImplementationDescriptor getMessage: "sampleUsage" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: "Sample usage for this message." .
;

Schema MessageImplementationDescriptor getMessage: "seeAlsoList" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Schema MID .
   setReturnObjectContainerTo: List .
   setDescriptionTo: "Returns a list of related messages." .
;

Schema MessageImplementationDescriptor getMessage: "setDefaultValueTo:" .
   setSelectorTo: "setDefaultValueTo: val" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MID .
   setParameter: 1 typeTo: Object .
   setDescriptionTo: 
"This method is used to set the default value for messages that are
fixed properties.  This default value will be used to initialize any
new instances created for the underlying class.  This message also
initializes the returnObjectType property to be the class descriptor
of the default value." .
;

Schema MessageImplementationDescriptor getMessage: "setDescriptionTo:" .
   setSelectorTo: "setDescriptionTo: text" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MID .
   setParameter: 1 typeTo: String .
   setDescriptionTo: 
"This method is used to update the description property.  If the recipient's
message descriptor description is NA, it will be set to the same value." .
;

Schema MessageImplementationDescriptor getMessage: "setFunctionTypeTo:" .
   setSelectorTo: "setFunctionTypeTo: type" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MID .
   setParameter: 1 typeTo: String .
   setDescriptionTo: 
"This method is used to set or change the value of the 'functionType'
property.  The parameter can be an instance of the class FunctionType
or a string.  If a string is supplied, and a function type instance by
that name does not exist, a new instance of FunctionType is created.
If the message descriptor associated with this recipient does not have
a function type, it is set to the value provided here." .
;

Schema MessageImplementationDescriptor
 getMessage: "setImplementationTextTo:forMethod:" .
   setSelectorTo: "setImplementationTextTo: text forMethod: method" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MID .
   setParameter: 1 typeTo: String .
   setParameter: 2 typeTo: Method .
   setDescriptionTo: 
"This method is executed as part of the 'Schema processAllMessages'
process." .
;

Schema MessageImplementationDescriptor getMessage: "setImplementationTo:" .
   setSelectorTo: "setImplementationTo: text" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MID .
   setParameter: 1 typeTo: String .
   setDescriptionTo: 
"This method is executed as part of the 'Schema processAllMessages' process." .
;

Schema MessageImplementationDescriptor getMessage: "setLevelTo:" .
   setSelectorTo: "setLevelTo: string" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MID .
   setParameter: 1 typeTo: String .
   setDescriptionTo: 
"This method is used to set or change the value of the 'usageLevel'
property.  The parameter can be an instance of the class UsageLevel
or a string.  " .
;

Schema MessageImplementationDescriptor getMessage: "setParameter:typeTo:" .
   setSelectorTo: "setParameter: number typeTo: parameterType" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "Schema" .
   setReturnObjectTypeTo: Schema MID .
   setParameter: 1 typeTo: Integer .
   setParameter: 2 typeTo: Object .
   setDescriptionTo: 
"Indicates the type of value expected for a parameter of a binary or
keyword message.  The first input corresponds to the parameter number;
the second input corresponds to the type which will be converted and
stored as a class descriptor.  Parameter 1 can also be used to store
the type of index for messages that return IndexedList containers." .
;

Schema MessageImplementationDescriptor
 getMessage: "setReturnObjectContainerTo:" .
   setSelectorTo: "setReturnObjectContainerTo: cd" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MID .
   setParameter: 1 typeTo: Object .
   setDescriptionTo: 
"This method sets the value of the returnObjectContainer property and should be
one of Object, List, TimeSeries, or IndexedList." .
;

Schema MessageImplementationDescriptor getMessage: "setReturnObjectTypeTo:" .
   setSelectorTo: "setReturnObjectTypeTo: cd" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MID .
   setParameter: 1 typeTo: Object .
   setDescriptionTo:
"This method is used to set the value of the 'returnObjectType' property." .
;

Schema MessageImplementationDescriptor getMessage: "setSampleUsageTo:" .
   setSelectorTo: "setSampleUsageTo: text" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MID .
   setParameter: 1 typeTo: String .
   setDescriptionTo: "Updates the property 'sampleUsage'. " .
;

Schema MessageImplementationDescriptor getMessage: "setSeeAlsoTo:" .
   setSelectorTo: "setSeeAlsoTo: midList" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MID .
   setParameter: 1 typeTo: Schema MID .
   setDescriptionTo: "Updates the property 'seeAlsoList'. The parameter
should be a list of message implementation descriptors." .
;

Schema MessageImplementationDescriptor getMessage: "setSelectorTo:" .
   setSelectorTo: "setSelectorTo: string" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MID .
   setParameter: 1 typeTo: String .
   setDescriptionTo: 
"This method is used to update the 'fullSelector' property." .
;

Schema MessageImplementationDescriptor getMessage: "setTypeTo:" .
   setSelectorTo: "setTypeTo: newType" .
   setFunctionTypeTo: "Update" .
   setLevelTo: "Schema" .
   setReturnObjectTypeTo: Schema MID .
   setParameter: 1 typeTo: Object .
   setDescriptionTo: 
"This method is used to set the 'type' property to the message type of
the current implementation.  The message type is an instance of the
class MessageType.  This method is executed as part of the 'Schema
processAllMessages' process and should not be run directly.
" .
;

Schema MessageImplementationDescriptor getMessage: "setupAccessBlocks" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Schema MID .
   setDescriptionTo: 
"This method is used to set the values for 'accessBlock' and 'updateBlock'.
It should not be accessed directly." .
;

Schema MessageImplementationDescriptor getMessage: "updateBlock" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "DBA" .
   setReturnObjectTypeTo: Block .
   setDescriptionTo: 
"This property contains a block that could be sent to an object to update
the property associated with this message." .
;

Schema MessageImplementationDescriptor getMessage: "type" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: MessageType .
   setDescriptionTo: 
"This property returns the value if the recipient's message type:
Constant, FixedProperty, TimeSeriesProperty, Method, TimeVaryingMethod,
or Primitive." .
;

Schema MessageImplementationDescriptor getMessage: "updateDate" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: Date .
   setDescriptionTo: 
"This property returns the date this message implementation was last
changed." .
;

Schema MessageImplementationDescriptor getMessage: "usageLevel" .
   setFunctionTypeTo: "Schema" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: UsageLevel .
   setDescriptionTo: 
"This property returns the usage level of the recipient." .
;

Schema MessageImplementationDescriptor getMessage: "whatAmI" .
   setFunctionTypeTo: "Descriptive" .
   setLevelTo: "Basic" .
   setReturnObjectTypeTo: String .
   setDescriptionTo: "Returns the string \"Schema 
MessageImplementationDescriptor\" . " .
;

