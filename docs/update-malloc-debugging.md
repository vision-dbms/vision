---
title: Debugging Update Malloc Errors
---

## Build a reproducer

First, to reproduce the error, we had to find the `limit datasize` from the job that malloc-ed and then run it in a session connected to the appropriate version of the database.

It is important to note the run against an old version of the database because the update had subsequently completed; building new holdings takes more memory than overwriting holdings already in the database. It will also take at least some memory to profile the database.

```
% limit datasize 4000000
% batchvision -v 5099805
V> 20091016 evaluate: [ IVR Holdings doitFor: 20091016 asDate; ];
V> ?G
** MALLOC
% limit datasize 5000000
% batchvision -v 5099805
V> 20091016 evaluate: [ IVR Holdings doitFor: 20091016 asDate; ];
V> ?G
V> "starting investigation" printNL;
```

## Run your reproducer in a "bigger" session 

Then you can see above that you simply bump the `limit datasize` in another session with your reproducer.  This results in a running session where you can look at the structures that your update has modified!

## Survey the big structures

This query quickly shows you how much memory the big containers are using:

```
^tmp define: 'bigContainerCount' toBe: 10;

!bigContainers <- AdminTools objectSpaceList
  extendBy: [ !bc; !os <- ^self; ] .
  iterate: [
    :bc <- os containerList
      select: [ (containerAddrType > 0) ] .
      sortDown: [ containerSize ] .
      first: ^tmp bigContainerCount ;
  ] .
  collectListElementsFrom: [ bc ] .
  reject: [ containerSize isNA ] . # optional but usually helpful
  sortDown: [ containerSize ] .
  first: ^tmp bigContainerCount;

bigContainers do: [
    displayContainerInfo;
  ] .
  total: [ containerSize ] . printWithCommasNL: 20;
```
_**TIP:** In large databases you may want to do this query only on the object spaces that you know are impacted by the load.  Iterating through each object spaces containerList can take a very long time and use memory in a session that won't be reclaimed_

Which looks like:
```
7      2073  1      Double   599384068  309834          32       3
10      300  1     Pointer   431030356  346407          32       3
7      2062  1     Pointer   322948304  309833          32       3
10    12817  1      Double   179353108  346411          32       3
7      2060  1    Property   139319916  309831          32      13
10    11997  1     Pointer   102939952  345912          32       3
10    12027  1        List   102914580  345918          32       3
10     2780  1     Pointer    98961276  345311          32       3
10     2794  1     Pointer    98961276  344594          32       3
10     2464  1     Pointer    62721356  346409          32       3
    2,138,534,192.00

V> AdminTools displayMemoryStats;
V> ?g
  Current allocation level:      3,353,062,696
  Session maximum:               3,874,077,748
  Heap limit:                   10,240,000,000
  Total size of mapped segments683,359,575,500
```

The top 10 segments account for 2.1GB of the 3.8GB needed to run this update.  But what are they?

( [`AdminTools displayMemoryStats` missing?](#admintools-displaymemorystats) )

## Which business objects do the containers belong to?

This query will give you tips as to what properties the biggest containers belong to:

```
bigContainers first: 4 . iterate: [ 
  "=" fill: 60 . printNL; 
  "Container: " print;
  ^self displayContainerInfo;
  newLine print;
  ^self displayLocalVisionReferents;
  newLine print;
];
```

```
============================================================
Container: 7      2073  1      Double   599384068  309834          32       3

ConstituentData [  7:   2089] '_shares'
TimeSeries ID: [  7:   2063]
Number of TimeSeries in Store:   7619556
Number of TimePoints in Store:      5276
TimeSeries LStore profile:
  LStore ID: [  7:   2061]
  Number of Lists in Store:   7619556
  LStore Content profile:
    Vector ID: [  7:   2060]
    Number of elements:                           80737066
    Number of usegments:                                 2
    Number needing alignment:                            0
    Number of transitions:                        11609984


============================================================
Container: 10      300  1     Pointer   431030356  346407          32       3

Account [ 10:    955] 'holdingsSeries'
TimeSeries ID: [ 10:    303]
Number of TimeSeries in Store:     54352
Number of TimePoints in Store:      5319
TimeSeries LStore profile:
  LStore ID: [ 10:    302]
  Number of Lists in Store:     54352
  LStore Content profile:
    Vector ID: [ 10:     25]
    Number of elements:                          107757579
    Number of usegments:                                 5
    Number needing alignment:                            0
    Number of transitions:                          345859


============================================================
Container: 7      2062  1     Pointer   322948304  309833          32       3

ConstituentData [  7:   2089] '_shares'
TimeSeries ID: [  7:   2063]
Number of TimeSeries in Store:   7619556
Number of TimePoints in Store:      5276
TimeSeries LStore profile:
  LStore ID: [  7:   2061]
  Number of Lists in Store:   7619556
  LStore Content profile:
    Vector ID: [  7:   2060]
    Number of elements:                           80737066
    Number of usegments:                                 2
    Number needing alignment:                            0
    Number of transitions:                        11609984


============================================================
Container: 10    12817  1      Double   179353108  346411          32       3

Account [ 10:    955] '_totalMarketValue'
TimeSeries ID: [ 10:    650]
Number of TimeSeries in Store:     54352
Number of TimePoints in Store:      5318
Size of Cartesian Space:       289043936
Number of MapTransitions:          69671
TimeSeries LStore profile:
  LStore ID: [ 10:    648]
  Number of Lists in Store:     54352
  LStore Content profile:
    Vector ID: [ 10:    647]
    Number of elements:                          107703574
    Number of usegments:                                20
    Number needing alignment:                           18
    Number of transitions:                          154030
```
The `_totalMarketValue`, `_shares` and  `holdingsSeries` taking up most of the memory from a holdings load is pretty much what one would expect.  But an update was unexpectedly using too much memory these techniques would be the exact techniques would use to begin to figure out why.

# Memory Usage Nuances

## Container Table Traversal

Container Table traversal uses up memory that can't be reclaimed by a session.  This query demonstrates this phenomenon effectively:

```
"Before:" printNL;
AdminTools displayMemoryStats;

AdminTools objectSpaceList iterate: [ containerList ];

newLine print;
"After Traversal:" printNL;
AdminTools displayMemoryStats;

Utility collectSessionGarbage;

newLine print;
"After collectSessionGarbage:" printNL;
AdminTools displayMemoryStats;
```

You can see that the 427MB used by the traversal can't be reclaimed by a `collectSessionGarbage`:
```
  Current allocation level:         80,892,443
  Session maximum:                  80,906,379
  Heap limit:                   10,240,000,000
  Total size of mapped segments    340,008,248

After Traversal:
  Current allocation level:        427,029,603
  Session maximum:                 471,514,023
  Heap limit:                   10,240,000,000
  Total size of mapped segments356,799,206,488

After collectSessionGarbage:
  Current allocation level:        427,029,603
  Session maximum:                 471,514,023
  Heap limit:                   10,240,000,000
  Total size of mapped segments356,825,094,992
```

## Entity Creation memory spikes

Often there are many large properties off of key Entities such as `Security` or `Account`.  Creating a single entity here can often take much more memory than one might expect:

```
"Before:" printNL;
AdminTools displayMemoryStats;

Security createInstance

newLine print;
"After:" printNL;
AdminTools displayMemoryStats;
```

Creating a single security instance takes 500MB of memory
```
Before:
  Current allocation level:          2,641,712
  Session maximum:                   2,659,968
  Heap limit:                    2,684,354,560
  Total size of mapped segments     18,833,024

After:
  Current allocation level:        482,271,800
  Session maximum:                 482,273,068
  Heap limit:                    2,684,354,560
  Total size of mapped segments    784,757,728
```

## AdminTools displayMemoryStats

In case you don't have this method in your database:

```
AdminTools respondsTo: 'displayMemoryStats' . ifFalse: [ 
  AdminTools define:'sessionAllocationHighWaterMark' toBePrimitive: 600 withControlValue: 1;
  AdminTools define:'sessionAllocationResourceLimit' toBePrimitive: 600 withControlValue: 2;
  AdminTools define:'sessionMappedAddressSpaceLevel' toBePrimitive: 600 withControlValue: 3;

  AdminTools defineMethod: [|displayMemoryStats|
    "  Current allocation level:    " print;
    totalNetworkAllocation printWithCommasNL: 15.000000;
    "  Session maximum:             " print;
    sessionAllocationHighWaterMark printWithCommasNL: 15.000000;
    "  Heap limit:                  " print;
    sessionAllocationResourceLimit printWithCommasNL: 15.000000;
    "  Total size of mapped segments" print;
    sessionMappedAddressSpaceLevel printWithCommasNL: 15.000000;
  ];
];
```
