---
title: "The Vision Debugger"

sidebar:
  nav: "sb_Troubleshooting"
---

Ever wish that you could look inside an executing Vision program and
figure out what is going on, without resorting to throwing tons of
<i>print</i> statements throughout your code.  Something like an
interactive Vision debugger perhaps?  Apparently, a group of
industrious beings had this same idea back in the late 90s (that's
1990s for the kids among you).  Put aside for higher priority pursuits
like federation, VCA, pools, and bridges, this module was all but
forgotten.  A recent walk through the wilds of Vision open-source
code, led us to this awesome discovery - the makings of a solid Vision
debugger have been embedded into the Vision engine since release 6.2.
Nothing is audited, tested, or packaged so there's tons of work to do
to make this code releasable, but, what is there can be used to
illustrate basic stories and generate cerebral discussions about the
possibilities.

## The Basic Concepts

The most basic action to take is to set breakpoints that will stop
the code in specific places so that you have a chance to poke around
at the actual state of affairs while a program is executing.  You set
breakpoints by flagging one or more methods at which you wish to
stop.  During execution, the engine will stop when it encounters any
of the methods that have breakpoints set.   While stopped, you can
use vanilla Vision code to look at properties, print information, and
even reset some values before continuing.

In debug mode, when Vision encounters a breakpoint, you have
programmatic access to a handle or controller object known as a
<b>Suspension</b>.  The simplest instruction you can give a <b>Suspension</b> object
is <i>resume</i> - this instructs Vision to continue executing the current
code through to completion or the next breakpoint, whichever comes
first.  You can also tell the <b>Suspension</b> to <i>abort</i>, stopping the code
execution at this point.  The <b>Suspension</b> also has access to the state
representing the running code.  This state is accessed via the
<i>suspendee</i> message which returns a pointer to the place in the code
where the execution has stopped.  In the basic case, this will be at
the entry point of the method in which the breakpoint was set.

The <i>suspendee</i> object provides access to a good deal of information.
You can look at where you are.  You can access the calling method,
<i>caller</i>, the penultimate calling method, <i>caller caller</i>, the
antepenultimate calling method, <i>caller caller caller</i> and the stack
of callers from the top, <i>callers</i>.  Each <i>suspendee</i> calling level
represents a debugger <b>Task</b> object and provides a number of different
display methods.

Within a <i>suspendee</i> or any of its <i>callers</i>, you have access to the
details of the Vision code that is in play via the <i>current</i>, <i>self</i>,
and <i>my</i> messages.  For example, <i>suspendee current</i> expression
will give you access to the local state of the method's block at the
time of the suspension, including the number of instances in the task
(_suspendee current instanceList)_, any of the local variable values
defined within the method, and any of the values assocated with
objects accessible within the method.  In addition to looking at these
values, you can change any values within the suspendee block before
proceeding.

Once you are done poking around inside the <i>suspendee</i>, you can resume
the suspension or walk through it a step at a time.   There are
messages at <b>Suspension</b> to step through the method in a number of ways.
More on this later.

## A Simple Example

To start, set a breakpoint in one or more methods.  Here we are
telling the engine to suspend execution whenever the <i>initialize</i>
method in the class <b>Company</b> is encountered.

```
Input:
Company setBreakInMethod: 'initialize' . printNL ;

Output:
TRUE
```

Now run your Vision code under a debugger process:

```
Input:
Debug run:
  [ 
      Company createInstance: "C1" ;
  ] ;

Output:
in debug for block
...

+++ 7   Suspension      1      W Entry     C1        | |initialize|
                                                     | ^super initialize; 
                                                     | ^
                                                     | :securityList <- defaultInstance securityList; 
                                                     | ^self


```

The Vision engine suspends execution when it encounters the
<i>initialize</i> method.  At this point, you can view the stack of
caller methods that led to this suspension:

```
Input:
Debug getSuspension  suspendee displayCallers ;

Output:
9   BlockTask       1      R Running   C1            | |initializeLocalAttributes|
                                                     | ^super initializeLocalAttributes; 
                                                     | ^self initialize; 
                                                     | ______^
                                                     | ^self

10  BlockTask       1      R Running   C1            | ^self initializeKeys:
                                                     |   ^my keys; 
                                                     | ^self initializeDefaults; 
                                                     | ^self initializeLocalAttributes; 
                                                     | ______^
                                                     | ^self initializeNames; 

11  PrimitiveTask   1      R Running   C1            | <SendForExecution> ... value

12  BlockTask       1      R Running   Default       | | 
                                                     | createInstance: keys| 
                                                     | ^self new 
                                                     | do: [
                                                     | ^
                                                     |     ^self initializeKeys: 
                                                     |       ^my keys;
                                                     |     ^self initializeDefaults; 
                                                     |     ^self initializeLocalAttributes; 
                                                     |     ^self initializeNames; 
                                                     |    ]

13  BlockTask       1      R Running   CoreWorkspace | Company createInstance: "C1"; 
                                                     | ________^

14  PrimitiveTask   1      R Running   [             | <EvaluateForValue> ^current value

2   TopTask         1      R Running                 | <---Top---> ... value

```
This display output is similar to the output we see when we turn on
the verbose selector-not-found option.  The difference is that we are
actually suspended within the Vision program and are free to poke
around our current state.  From the <i>suspendee</i>,
we can access values associated with the point at which we have
stopped or any of the execution layers in our calling stack.
You have access to the contexts normally associated with
magic words from within the state of the suspendee or any of its
callers, by using the messages <i>self</i>, <i>current</i>, and
<i>my</i>.

Let's look at what information is
available about the current instance being initialized:
```
Input:
Debug getSuspension suspendee current do: [ whatAmI print: 20 ; code printNL ] ;

Output:
Company             C1
```
We can see that the current class we are operating within is
<b>Company</b> and that our current instance has a value for
<i>code</i> of "C1".

If we look at the result of the
<i>displayCallers</i> stack above, we can see that the suspension was called
from within the <i>initializeLocalAttributes</i> method.  This level
was in turn called from within an execution that knows about the value
of <i>^my keys</i>.  We can look at this value from the
suspension using:

```
Input:
Debug getSuspension suspendee caller caller my keys printNL ;
Output:
C1
```

Let's continue looking at the information
available about the current instance being initialized:
```
Input:
Debug getSuspension suspendee current do: [ code print: 20 ; name printNL ] ;

Output:
C1                        NA 
```
We can see that the code property has already been initialized to the
string "C1", but the name property is still NA.

The <i>stepInto</i>
message allows us to walk through the code, one step at a time:

	Input:
	Debug getSuspension stepInto
	
	Output:
	Debug ModelObject Subscription[22] to Debug ModelObject BlockTask[8] Unit Child Entry
	Input:
	Debug getSuspension suspendee displayInfo ;
	
	Output:
	27  BlockTask       1      R Entry     C1    | |initialize|
	                                             | !entity <- ^self; 
	                                             | ^
	                                             | ^self do: [
	                                             |     name isNA ifTrue: [:name <- code]; 
	                                             |     shortName isNA ifTrue: [:shortName <- code]; 
	                                             |     sortCode isNA ifTrue: [:sortCode <- code]; 
	                                             |     ^self addAlias: code; 
	                                             |  ...
	
	
	Input:
	Debug getSuspension suspendee current do: [ code print: 20 ; name printNL ] ;
	
	Output:
	C1                        NA 
	
The <i>nextExpression</i> message executes the current expression and
skips to the next one.  Let's look at the next expression:

	Input:
	Debug getSuspension nextExpression ;
	
	Output:
	Input:
	Debug getSuspension suspendee displayInfo ;
	
	Output:
	27  BlockTask       1      R Running   C1    | |initialize|
	                                             | !entity <- ^self; 
	                                             | ^self do: [
	                                             | ^
	                                             |     name isNA ifTrue: [:name <- code]; 
	                                             |     shortName isNA ifTrue: [:shortName <- code]; 
	                                             |     sortCode isNA ifTrue: [:sortCode <- code]; 
	                                             |     ^self addAlias: code; 
	                                             |    ]; 
	                                             |  ...
	
	
We are now at the start of the <i>do:</i> block.  Going to the next
expression executes the expressions in the block and proceeds to the next
statement.

	Input:
	Debug getSuspension nextExpression ;
	
	Output:
	Input:
	Debug getSuspension suspendee displayInfo ;
	
	Output:
	27  BlockTask       1      R Running   C1    |  ...
	                                             |     name isNA ifTrue: [:name <- code]; 
	                                             |     shortName isNA ifTrue: [:shortName <- code]; 
	                                             |     sortCode isNA ifTrue: [:sortCode <- code]; 
	                                             |     ^self addAlias: code; 
	                                             |    ]; 
	                                             | ^self initializationList 
	                                             | ^
	                                             | do: [^my entity send: ^self .<- (
	                                             |     ^my entity defaultInstance send: ^self .value); 
	                                             |    ]; 
	                                             | ^self initializeDataBaseLinks; 
	                                             | ^self

Let's look at the data after the <i>do:</i> block has completed:	
	
	Input:
	Debug getSuspension suspendee current do: [ code print: 20 ; name printNL ] ;
	
	Output:
	C1                  C1

Once we have executed the 'do:' block, the <i>name</i> property is set to
match the value of code.  We are still suspended, but now are waiting
at the point right after the <i>name</i> property has been set.  Before we
finish executing the method, let's intervene and change the value of
name to something else:

```

Input:
Debug getSuspension suspendee current setNameTo: "THIS IS MY REAL NAME" ;
Debug getSuspension suspendee current do: [ code print: 20 ; name printNL ] ;

Output:
C1                  THIS IS MY REAL NAME

Input:
Debug getSuspension suspendee displayInfo ;

Output:
27  BlockTask       1      R Running   C1            |  ...
                                                     |     name isNA ifTrue: [:name <- code]; 
                                                     |     shortName isNA ifTrue: [:shortName <- code]; 
                                                     |     sortCode isNA ifTrue: [:sortCode <- code]; 
                                                     |     ^self addAlias: code; 
                                                     |    ]; 
                                                     | ^self initializationList 
                                                     | ^
                                                     | do: [^my entity send: ^self .<- (
                                                     |     ^my entity defaultInstance send: ^self .value); 
                                                     |    ]; 
                                                     | ^self initializeDataBaseLinks; 
                                                     | ^self


```
Now that we are done poking around inside the execution, let the
program finish to completion:

```

Input:
Debug getSuspension resume ;

Output:

```
Check that there are no more suspensions in the queue:

```
Input:
Debug getSuspension 
Output:
List of 0

```

The debugger is no longer active.  The name change that we made while
execution was suspended is still in effect:
```
Input:
Company masterList do: [ code print: 20 ; name printNL ] ;
Output:
C1                  THIS IS MY REAL NAME
```

## Selector Not Founds Work Too

The previous example illustrates how to suspend the Vision engine at
specific breakpoints that you set.  If you run your code under the
control of the <b>Debug</b> class, the engine will automically suspend
when it encounters a <i>Selector Not Found</i> error as well. 

For example, define a new method with a typical typo in it and run it in debugger mode:
```
Input:
Integer defineMethod: [ | printit |
"this is the number " conact: asSelf . printNL ;
] ;

Output:

Input:
Debug run: 
[ 3 printit ]  ;

Output:
in debug for block

Input:
Debug getSuspension suspendee displayCallers ;

Output:
65  BlockTask       1      R Running           3     | |printit|
                                                     | "this is the number "
                                                     |    conact: asSelf .printNL; 
                                                     | ___^

66  BlockTask       1      R Running   CoreWorkspace | 3 printit
                                                     | __^

67  PrimitiveTask   1      R Running   [             | <EvaluateForValue> ^current value

57  TopTask         1      R Running                 | <---Top---> ... value


```

That was almost too easy.  The culprit misspelled <i>conact:</i> is
identified and all we need to do is fix the code and we'll be set.

```
Input:
Integer defineMethod: [ | printit |
"this is the number " concat: asSelf . printNL ;
] ;

Output:
```
The debugger in this case told us basically what the verbose 'snf'
option tells us.  Now that the method has been fixed, let's run it
from within a list that includes different subclasses:

```
Input:
Debug run:
[ 3 sequence,10.1,5,20.2 do: [ printit ] ;
] ;

Output:
in debug for block

Input:
Debug getSuspension displayCallers ;

Output:
91  SNFTask         2      R Running       10.10     | Selector 'printit'
                                                     |     not found at Double

92  BlockTask       2      R Running       10.10     | printit
                                                     | ^

93  PrimitiveTask   2      R Running       10.10     | <SendForValue> ... value

94  UtilityTask     6      R Running                 | <---Utility---> ... basicSend:

95  ListEnumerationC1      R Running   List of 6     | <IterateInContext> ... basicSend:

96  BlockTask       1      R Running   CoreWorkspace | 3 sequence , 10.100000 , 5 , 20.200000 do: [printit]; 
                                                     | _______________________________________^

97  PrimitiveTask   1      R Running   [             | <EvaluateForValue> ^current value

85  TopTask         1      R Running                 | <---Top---> ... value
```

We have suspended execution because of the selector <i>printit</i>.
In this case though, we have a little more information to review.
Notice that the original list had 6 elements in it - 4 integers and 2
doubles.  The <i>SNFTask</i> indicates the selector error only applies
to 2 of the instances in our list because the specific selector is not
defined for the <b>Double</b> class. We can confirm which instances
are problematic by running:


```
Input:
Debug getSuspension suspendee self instanceList  do: [ printNL ] ;
Output:
    10.10
    20.20
```

These examples are just the tip of the proverbial debug iceberg.  The
idea that we can stop and explore the state of a Vision execution
interactively is intriguing and opens up many new avenues to explore
and ponder.

## Getting the Debugger

To load this code in your own Vision session, clone
[this repository](https://github.com/vision-dbms/vision-debugger),
start a Vision session in a directory containing the _scripts_ directory,
and run the Vision expression:

```
"scripts/621.setup-tests" asFileContents evaluate
```
