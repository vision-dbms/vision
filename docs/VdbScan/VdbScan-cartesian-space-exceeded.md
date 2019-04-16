---
title: "VdbScan Cartesian Space Exceeded"

sidebar:
  nav: "sb_Troubleshooting"
---

## Symptoms

These errors [can look like this](VdbScan-error-message-lookup.html#cartesian-space-exceeded).

## Fix

A Product Mapped index running out of cartesian space needs to be converting the index type to Partition Mapped; ideally before it actually runs out of space.  The `AdminTools displayWorrisomeIndexes` report will indicate the POP (or POPs) that you need to convert:

```
--> Running out of Cartesian Space For:
[  8:    557]  Time Series                  312,472,020
   Rows:                                        535,002
   Columns:                                       3,198
   Elements:                                 58,526,198
   Indexing Type: Product Mapped
   Cartesian Space Size:                  1,710,936,396
   Percent Full:                                      3%
   -- If converted to Partition Mapped:
      Map Size:                             234,104,832
   Map Size:                                  5,346,076
-------------------------------------------------------
```

The limit for cartesian space size on a Product Mapped index is 2,147,483,648 (2 ** 31).  The cartesian space above is 2,145,687,215; this is 0.08% from the limit; any small increase in the cartesian space limit will be exceeded.  This property will need to be converted.  The following example code will change the mapping type and verify that the resultant structure is realized correctly:

```
[
  !pop <- 8 asObjectSpace packPOP: 557;
  AdminTools displayMemoryStats;

  newLine print;
  "Exaimine starting point: " printNL;
  pop analyzeIndexedListMap: FALSE;

  newLine print;
  "Doing conversion..." printNL;
  pop asObject __setIndexingTypeToPartitionMapped;

  "verifying resultant structures..." printNL;
  pop asObject rcleanStoreAndDictionary;

  newLine print;
  "Examine results: " printNL;
  pop analyzeIndexedListMap: FALSE;

] value;
```

And it's output:
```
  Current allocation level:        127,900,108
  Session maximum:                 146,642,326
  Heap limit:                    2,684,354,560
  Total size of mapped segments    788,147,316

Exaimine starting point:
[  8:    557]  Time Series                  333,515,460
   Rows:                                        555,431
   Columns:                                       3,401
   Elements:                                 62,549,285
   Indexing Type: Product Mapped
   Cartesian Space Size:                  1,889,020,831
   Percent Full:                                      3%
   -- If converted to Partition Mapped:
      Map Size:                             250,197,180
   Map Size:                                  5,627,468
-------------------------------------------------------

Doing conversion...
verifying resultant structures...

Examine results:
[  8:    557]  Time Series                  573,985,880
   Rows:                                        555,431
   Columns:                                       3,401
   Elements:                                 62,549,285
   Indexing Type: Partition Mapped
   -- If converted to Product Mapped:
      Cartesian Space Size:               1,889,020,831
      Percent Full:                                   3%
      Map Size:                                      NA
   Map Size:                                250,197,180
-------------------------------------------------------

  Current allocation level:        480,220,650
  Session maximum:                 493,553,405
  Heap limit:                    2,684,354,560
  Total size of mapped segments  6,803,146,876
```

## Caveats

### Missing code

Getting a Selector Not Found for `__setIndexingTypeToPartitionMapped`, `AdminTools displayMemoryStats` or `AdminTools displayWorrisomeIndexes`?  If you have `VdbScan.vis` that contains all of the code you need.

### Memory Usage

Depending on the underlying data, Partition Mapped index maps can be larger than Product Mapped index maps; therefore requiring more memory to modify.  The `AdminTools displayWorrisomeIndexes` report will give you an estimate of the differences in size for the conversion.

In one example above, `[ 77:    842]`,  the conversion will increase the map size from 19MB to 148MB.  Additional work might need to be done to account for the extra memory used in updating this structure; especially if many maps are converted at the same time.

In the `[  8:    557]` example above the conversion increases the map size from 5MB to 250MB.  This could definitely cause `malloc` errors in updates.

Sometimes, however, converting to partition mapped actually shrinks the map size.  Again this depends on the underlying data.

### Inaccessible Structures

The cartesian space can be exceeded without aligning the underlying map; in this scenario the structure (`IndexedList` or `TimeSeries`) that includes the map will be rendered unreadable and unwriteable.  At this point, you have two unpleasant options for recovering this data:

1. Roll back the database to before the point where the cartesian space was exceeded, convert the mapping type to partition mapped and catch the database up to where it was before the rollback.
1. Use `batchvision -v` to access a readable version of the map, dump the data to a store outside the database, delete the unusable product mapped property, recreate the property partition mappped and reload the data from the dumped data.
