---
title: "The Vision Story"

toc_sticky: false

sidebar:
  nav: "sb_TheVisionStory"
---

## In the Beginning

In the beginning, way back in the late 1970s, two budding young entrepreneurs, Mike Caruso and Leslie Newman, had some time to kill when a meeting in Washington D.C. was cancelled at the last moment.  Sitting in the local pub waiting for the next flight back to Boston, they started to chat about data and technology and other fun topics.  

Before the day was through, it was easy to imagine ways to drive the next generation of information analysis.  They knew then that they were destined to act on their vision.  

![First Vision Doodles](/original/doodles.jpg)

Before long, they were joined by a third partner, Rich Newman. They formed Innovative Systems Techniques (InSyTe).  The Vision journey was underway.

### The Technology Landscape

In the early 1980s, opportunities to unleash the power of computers to look at information in new ways abounded:

* Minicomputers were replacing mainframes for targeted applications
* PCs were new and VisiCalc, followed by 1-2-3 and Excel were rapidly embraced
* 2400 baud was normal; 9600 was super fast
* Relational databases were emerging as the new standard for collecting and storing transactional data
* Powerful, modular Unix workstation hardware targeted at engineering problems was getting faster and cheaper
* Object oriented concepts were in their infancy

### The Business Landscape

In the beginning, way back in the early 1980s, new business models formed to tap these technology-driven opportunities:

* Specialized niche players like Data Resources and FactSet leveraged mainframe technology to offer targeted data and services focused on data analysis instead of data collection
* Business schools like Wharton featured a Decision Science track and developed skill sets around interactive, functional programming languages such as APL
* Next generation products targeted the marriage of mainframe information analysis tools with data downloading to spreadsheets

## Vision Takes Root

We were at the dawn of a new era and accelerating trend in the power and availability of computing.  Our modest goal was to create a modern information analysis framework leveraging the untapped potential of engineering workstation platforms, targeting data and technology-savvy users around specific market-focused needs.

The Financial Services Industry was an obvious initial target for many reasons:

* Experience with the data and application domains
* Ever-increasing thirst for more sources and types of information
* Large quantities of data supplied in bulk, not in transactional format
* Open to new ideas and tools that enhanced productivity and led to faster, better decisions

We found some early advocates and got to work!

## A Simple Hard Problem

_Find the members and average weighted pe for the last 12 month-ends for the S&P 500 and group by deciles_

### Building-block Queries:

* _ibm pe_
* _for {date} ibm pe_
* _for {each date} ibm pe_

* _for {each member of the sp500} pe, weighted pe_
* _for {each date} for {each member of the sp500} pe , weighted pe_
* _sp500List decile weighted pe_
* _for {each date} sp500List decile weighted pe_

* _sp500List compare weighted pe to average for decile_
* _for {each date} sp500List compare weighted pe to average for decile_
* _for {each portfolio} for {each date} compare weighted pe to average for decile_

### Notable Hidden Facts

* The data item _pe_ is a simple ratio based on data measured and recorded at different frequencies and at different points in time
* Different companies post eps on different dates
* Both values are impacted by splits, implying adjustments to historical data
* Switching to a global index introduces currency challenges
* Membership in the SP500 changes over time

## Modeling a Simple Solution

### Asking for _ibm pe_

> _ibm = Security ibm_<br>
> _pe = price / eps_<br>
> _price = splitAndCurrencyAdjustedPriceFromThePriceDB_<br>
> _eps = splitAndCurrencyAdjustedEpsFromTheFundamentalDB_<br>

### Asking for _sp500List_

> _sp500List = list of SP500 holdings_<br>
> _holding = sp500List[0]_<br>
> _holding wtPe = weight * security pe_<br>
> _holding weight = .123_<br>
> _holding security = ibm_<br>

## Introducing the Object Model

###  Basic Terminology

<table>
<tr><td> <b>Classes</b> </td><td> organize information and rules for different types of Objects </td></tr>
<tr><td> <b>Functions/Methods</b> </td><td> encapsulate program logic for a Class of Objects </td></tr>
<tr><td> <b>Properties</b> </td><td> store values including references to other Objects </td></tr>
</table>

### Simple Sample Summary
<table>
<tr><th>Class</th><th> Operation</th><th> Rule</th></tr>
<tr><td>Security</td><td> pe</td><td> price / eps</td></tr>
<tr><td>Security</td><td> price </td><td>priceRecord adjustedPrice</td></tr>
<tr><td>Security </td><td> eps </td><td>fundamentalRecord adjustedEps</td></tr>
<tr><td>PriceRecord</td><td> adjustedPrice</td><td> unadjustedPrice * splitFactor * currencyFactor</td></tr>
<tr><td>PriceRecord</td><td> splitFactor </td><td>security splitData getFactorRelativeTo: priceDate</td></tr>
<tr><td>FundamentalRecord</td><td> adjustedEPS</td><td> unadjustedEps * splitFactor * currencyFactor</td></tr>
<tr><td>FundamentalRecord</td><td> splitFactor </td><td>security splitData getFactorRelativeTo: epsDate</td></tr>
<tr><td>Holding</td><td> wtdPe </td><td>weight * security pe</td></tr>
<tr><td>Holding </td><td>weight</td><td> 0.123</td></tr>
<tr><td>Holding </td><td>security</td><td> ibm</td></tr>
</table>

### Observations

* Complex rules are required to correctly interpret and use data
* These rules should be encapsulated in a reusable form so that they do not need to be replicated for each application
* Despite the complexity associated with accessing and using the data, simple queries should be simple to state
* Time is everywhere independent of the applications and entities it helps to model
* Comparable data is measured at different points in time including irregular, event-driven data
* Time series of different frequency need to be efficiently combined with minimal redundancy and maximal consistency
* Complex aggregates, not just numbers and strings, vary over time

## Where Were We Heading?

### Sample Vision Code

```
#--  dateRange represents the last 12 month-end dates
#--  portfolioList represents a list of the 20 largest portfolios 

!dateRange <- ^today - 1 monthEnds to: today - 12 monthEnds by: 1 monthEnds ;
!portfolioList <- Portfolio masterList sortDown: [ totalMarketValue ] . first: 20 ;

portfolioList do: [ 
dateRange evaluate:    
   [ account holdings decileUp: [ wtdPe ] .
             groupedBy: [ decile ] .
      do: [ !average <- groupList average: [ wtdPe ] ;
            " Decile: " print ; print ; average printNL ;
            groupList
            do: [
                  security name print ;
                  wtdPe print; (wtdPe - ^my average) print ;
                  security pe printNL
                ] ;
         ] ;
  ] ;  #- end of dateRange
] ;    #- end of portfolioList
```
###  Fun Fact 1: Encapsulated Use of _wtdPe_

The variable _wtdPe_ references the computed weighted-average pe, encapsulating all the underlying rules for access including split and currency adjustments:

* _holdings decileUp: [ wtdPe ]_
* _groupList average: [ wtdpe ]_
* _groupList do: [ wtdPe print ]_
* _groupList do: [ (wtdPe - ^my average) print ]_

###  Fun Fact 2: Drilling Down into the Grouped Lists

Note that after grouping into decile buckets, the actual sub lists that form each group are still available:

* _groupList do: [ ] ;_

and that data computed for the group such as _average_ can be used within the individual element's calculations:

* _!average <- groupList average: [ wtdPe ] ; groupList do: [ (wtdPe - ^my average) print ]_


### Fun Fact 3: Use of date range

The body of the code can be executed for each date in your date range, by wrapping the code in an evaluation block:

* _dateRange evaluate:   [ ] ;_

### Fun Fact 4: Use of portfolio list

The body of the code can be executed for a list of portfolios for each date in your date range, by wrapping the code in a do block:

* _portfolioList do: [ ] ;_


## Objects, Databases, and Scale

We want objects for their ability to provide the rules needed to correctly interpret and use our data.  Scaling and performance require that we also treat this as a data base problem.  Our first Vision systems managed ~800MB of data using machines with 8MB of memory; current systems are at least 3-4 orders of magnitude larger.

What options do we have for integrating the pieces?  Where do objects fit in?

* Traditional thinking implements objects using an externally compiled or interpreted object programming language, reserving the structuring and management of stored data to a DBMS. 
* With that division of labor, object programs are the consumers of data supplied from the database.
* Those programs are responsible for determining what data needs to be copied from the database to materialize the necessary objects.

So what is wrong with that model?  It may work for applications that consume relatively small
amounts of data, but ...

* ... it leaves the hard problem of determining how to bridge the gap between the object model and its data base backend as an exercise for the ‘programmer’.
* ... it is impossible to overstate the complexity of doing this for even the simplest looking applications.
* ... with the business logic that gives our data meaning external to the database, we fail to leverage many of the capabilities of the database engine beyond that of raw data storage.

### The Challenge

This is a data base scaled problem. Grafting a traditionally implemented programming language onto or into a database system isn’t going to work well at scale.  While it’s easy to implement a programming language, we don’t just need a programming language. We need a database optimized general purpose language for complex analytic queries. To achieve that goal, we need to get a better sense of how databases and programming language differ and how they can converge.

Databases usually ...

* ... start with a high level statement of ’what’ is wanted
* ... use algebraic and other mathematical structures to formulate ‘how’ to do things
* ... have and use awareness of the size and organization (ordering, partitioning, row vs column) of the data they manage

Programming languages usually...

* ... start with a statement of ‘how’ to do something
* ... stay close to the iterative, individual-at-a-time, scalar computational model of most modern hardware
* ... have no a priori knowledge of the data they will be processing or how their programs will be used
* ... do not have precise mathematical definitions

This sounds like a big problem, but there are tools and techniques in the world of computer science and mathematics that provide the needed insights.  Focused on transformations and relationships, those tools provide a framework for representing and manipulating large quantities of data in general ways as well a natural home for cross-cutting and implicit concerns like time.

## Vision Under the Hood

The Vision technology offers a relationship centric information model based on a category theory formulation of sets and functions (set theory).  Objects are abstract entities with no internal state. They are not records.  All information about an object is found in the properties and methods (functions) that relate it to other objects.  We can explore relationships a single point at a time or in parallel, over time.

* Vision is a computationally complete object-oriented, data base programming language.
* Vision is built upon a solid mathematical foundation and parallel execution model.
* Vision uses object-oriented encapsulation to properly hide rule details from their users.
* Vision addresses traditional db concerns of data access order and placement, compression, and many other optimization techniques.
* Vision provides a first class natural home for Time.

## Vision Through the Years

Vision has been in production since 1986 and continues to be a significant component of mission-critical environments today.

Vision is still a great fit for our modern world.  Object models are "in" and languages such as PYthon and JavaScript are mainstream.  The thirst for data continues - we call it _Big Data_ today.  SQL is omnipresent, but so is NoSQL and the quest for efficient ways to model and process data-intensive applications is greater than ever.  The role of the Data Scientist continues to expand, increasing the demand for new ways to organize and analyze data.

<table>
<tr><td>Sept 1986</td><td>First Vision system goes live</td></tr>
<tr><td>Sept 1987</td><td>Oldest Vision system still active today</td></tr>
<tr><td>Aug 2000</td><td>InSyTe acquired by FactSet</td></tr>
<tr><td>Nov 2016</td><td>Vision Open-sourced</td></tr>
</table>
