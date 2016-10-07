Object classDescriptor
  setDescriptionTo:
"The class Object is the fundamental building block from which other
classes are derived.  Everything in the system is an object.  The
class Object defines protocol that every object created in the system
can respond to.  These are typically messages that support reasonable
default behavior which can be modified and extended as appropriate by
more specialized classes.  " ; 

Boolean classDescriptor 
  setDescriptionTo:  
"The class Boolean provides protocol for logical values.  Logical
values are represented by the two subclasses of Boolean - TRUE and
FALSE.  Most of the protocol for booleans is actually implemented at
the classes TRUE and FALSE.  The actual implementations will differ
between the two classes, but the functional goal of a message will be
the same.  " ;

TRUE classDescriptor 
  setDescriptionTo:  
"The class TRUE provides protocol for logical values that are TRUE."
;

FALSE classDescriptor 
  setDescriptionTo:  
"The class FALSE provides protocol for logical values that are FALSE."
;

DateRange classDescriptor 
  setDescriptionTo:  
"A DateRange is defined by specifying a starting date, an ending
date, and an increment.  Starting and ending dates can be any valid
absolute or relative date (e.g., 88; 8812; ^today; ^today - 1
monthEnds; 880315 - 5 days).  The increment is defined as an valid
date offset sent to an integer (e.g., 3 days; 1 monthEnds).  

In general, to form a date range, type:

     !dr <- date1 to: date2 by: increment ;

For example:

     !dr <- 8801 to: 8812 by: 1 monthEnds ;  
                         # defines month-end dates in 1988

     !dr <- ^today to: ^today - 1 years by: 5 businessDays ;
                         # defines weekly dates back one year
" ;

Function classDescriptor
  setDescriptionTo:
"The class Function is an abstract class that is used to organize the
classes in the hierarchy that can be evaluated.  " ;

ComputedFunction classDescriptor
  setDescriptionTo:
"The class ComputedFunction is an abstract class that is used to
organize the Function classes in the hierarchy that are computed
rather than enumerated.  " ;

Block classDescriptor
  setDescriptionTo:
"A Block is a set of one or more expressions that are viewed together
as a unit.  Each expression in a block is known as a statement.
Statements are separated by the character ; .  The block itself is
surrounded by square brackets.  The general form for a block is:  

     [ statement1 ;
       statement2 ;
       statement3 ;
       . . .
     ] ;" ;

EnumeratedFunction classDescriptor
   setDescriptionTo:
"The class EnumeratedFunction is an abstract class that is used to
organize the Function classes in the hierarchy that are enumerated
rather than computed.  " ;

Collection classDescriptor
  setDescriptionTo:
"The class Collection is an abstract class that is used to organize
multi-valued EnumeratedFunction classes in the hierarchy.  " ;

IndexedCollection classDescriptor
  setDescriptionTo:
"The class IndexedCollection is an abstract class that is used to
organize Collections that are indexed.  " ;

IndexedList classDescriptor
  setDescriptionTo:
"An IndexedList contains a set of elements that can be viewed as a
unit or referenced individually by index.  " ;

TimeSeries classDescriptor
  setDescriptionTo:
"A TimeSeries is a specialization of IndexedCollection whose index is
a date.  " ;

SequencedCollection classDescriptor
  setDescriptionTo:
"The class SequencedCollection is an abstract class that is used to
organize Collections that are not indexed.  " ;

List classDescriptor
  setDescriptionTo:
"Instances of the class List represent a group of objects which can
be viewed together as a unit.  The elements of the list are not
indexed directly.  Lists are created by sending the , anElement
message to an existing List.  The expression:  

     1, 2, 3, 4, 5

returns a List containing five elements." ;

Value classDescriptor
  setDescriptionTo:
"The instances of the class Value are single-valued enumerated
functions.  Value cells are accessed by requesting the intensional
form of a fixed property.  For example:  

     !x  ;                # define a fixed property at ^current
     :x whatAmI           # intensional form is a Value" ;

Ordinal classDescriptor
  setDescriptionTo: 
"The class Ordinal is an abstract class that is used to organize the
classes in the hierarchy whose instances can be compared along a
linear dimension.  Subclasses of the class Ordinal include Date,
DateOffset, Number, and String.  " ; 

Date classDescriptor
  setDescriptionTo:
"The instances of the class Date represent a specific day in the
calendar.  Each date encompasses a day, month, and year.  Dates are
formed in one of two ways.  Explicit dates can be derived by sending
the asDate message to integers in one of the forms illustrated below:

     Integer Form            Example             Returns Date
     -----------------------------------------------------------------
     YYYYMMDD                19850923 asDate      9/23/1985
       YYMMDD                  861015 asDate     10/15/1986
         YYMM                    8803 asDate      3/31/1988
           YY                      87 asDate     12/31/1987

Dates can also be formed relative to another date by adding or
subtracting a date offset to or from the date.  For example:  

     88 asDate - 5 days          ==>   12/26/1988
     87 asDate + 1 monthEnds     ==>    1/31/1988" ; 

DateOffset classDescriptor
  setDescriptionTo:
"The class DateOffset is an abstract class that supports the protocol
for the different classes of date increment.  DateOffset are used
primarily in the formation of date ranges and for date arithmetic as
illustrated below:  

     !dr <- 88 to: 85 by: 1 years
     !lastYearEnd <- ^today - 1 yearEnds;

Most of the protocol for is actually implemented at the
specializations of DateOffset.  The actual implementations will
differ among the classes, but the functional goal of a message will
be the same.  " ;


Number classDescriptor
  setDescriptionTo:
"The class Number is an abstract class that is used to organize the
numeric classes in the VISION hierarchy.  Each different kind of
numerical value is represented by one of three specializations of the
class Number:  Double, Float, or Integer.  Most numbers are either
Integer (no decimal value) or Double (decimal value).  The Float
class is used for efficiency in storing numbers in large databases
that are known not to require the extra precision and space
associated with double precision numbers.  " ;

Double classDescriptor
  setDescriptionTo:
"The class Double represents all double precision values.  " ;

Float classDescriptor
  setDescriptionTo:
"The class Float represents all Float values.  " ;

Integer classDescriptor
  setDescriptionTo:
"The class Integer represents all Integer values.  " ;

String classDescriptor
  setDescriptionTo:
"The instances of class String are objects that represent sequences
of characters.  The representation of a string is a sequence of
characters delimited by double quotes.  Unlike selectors, strings
need not be unique.  Any character may be included in a string.  If a
double quote is to be included, it must be preceded by the escape
character \." ;

Selector classDescriptor
  setDescriptionTo: 
"The instances of class Selector are objects that represent strings
used for names in the system.  The representation of a selector is a
sequence of alphanumeric characters surrounded by single quotes.
Each selector is unique; there can never be two selectors with
identical character sequences defined for the same class.  
For example, selectors are used to define a property at a class:

     myClass defineFixedProperty: 'aSelector'" ;

NA classDescriptor
  setDescriptionTo:
"This class represents values that are not available (NA)" ;

Utility classDescriptor
  setDescriptionTo:
"The Utility class is used to organize various messages that perform
utility functions."  ; 

Utility UnixSeconds classDescriptor 
  setDescriptionTo:
"The Utility UnixSeconds class provides a means for accessing and
manipulating timestamp information."  ; 

Catalog  classDescriptor
  setDescriptionTo:
"The Catalog class is used to create collections of objects that can
be named and clustered.  This class is used by some of the optional
ToolKits.  " ;

Chain classDescriptor
  setDescriptionTo:
"The Chain class implements the protocol needed to manage 'linked'
lists of structures.  This class is used by some of the optional
ToolKits."  ; 

Descriptor classDescriptor
  setDescriptionTo:
"The Descriptor subclasses are created to define and organize objects
with a fairly fixed set of instances that describe a particular type
of behavior.  Two subclasses are provided as part of the core
network:  'Date DayOfWeekDescriptor' and 'Date MonthDescriptor'.
There are seven instances of the class 'Date DayOfWeekDescriptor'
corresponding to the days of the week.  There are twelve instances of
the class 'Date MonthDescriptor' corresponding to the months of the
year.  " ; 

Dictionary classDescriptor
  setDescriptionTo:
"The Dictionary subclasses are used to name related objects in a
homogeneous way.  A Dictionary is similar to an IndexedList that is
indexed by a String value.  " ; 

Entity classDescriptor
  setDescriptionTo:
"The Entity subclasses are defined to describe real-world entities
such as companies, industries, countries, and products.  Instances of
the entity subclasses are often named and can be referenced directly
or used as part of a list.  Tables are often created as subclasses of
Entity.  "; 

IncorporatorPrototype classDescriptor
  setDescriptionTo:
"The IncorporatorPrototype class is used to organize classes that are
used strictly to incorporate external data."  ; 

ToolKit classDescriptor
  setDescriptionTo:
"The ToolKit classes of this class are used to organize related
collections of classes and messages."  ; 
