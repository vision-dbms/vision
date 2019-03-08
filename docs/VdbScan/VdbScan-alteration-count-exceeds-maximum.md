---
---

# Requested Alteration Count Exceeds Maximum

## Severity: from coaching to **ROLLBACK**

If a structure with this error wasn't committed to the database there is nothing to fix.  Allow the client to ignore this error but not subsequent occurrences.  If this happens with any frequency (even monthly) figure out which transaction is causing this error and have the client add a `rcleanStore` to that transaction.  You don't want a structure with this error getting committed to the database.

The way you know if a structure with this error has been committed to the database is by examining the `garbageCollect.log`.  If this error appears in that log it means the structure has been committed:

```
Cleaning Object Space    32          Tables

      Current Allocation:     81,336,943.00
       Current Threshold:     2,000,000,000
>>>  Error Trapped By Read-Eval-Print  <<<
*       Positional Token Representation Type Handler [1667]
*       A Usage Error
*       Requested alteration count 72835664 exceeds maximum 16777215 [32:2078]

*****    P      1 CleanStore
*****    B      1
^self asObject cleanStore
ifTrue: [^my :cleanCount <- (^my cleanCount + 1) asInteger];
CoreWorkspace AdminTools updateNetworkIfSpaceIsLow;
```

At this point the only proven recovery method is a rollback to a version prior to that container being committed :(

## What it looks like:

```
--> Notable NDF.ERRORS contents:
----------------------------------------------------------------
***** 21885C63.37A48C50.00000000 Tue Jun 16 06:24:14 2015 ssga [12000 12000 700 700]
>>>  Error Log Entry[1]  <<<
*       Positional Token Representation Type Handler [1667]
*       A Usage Error
*       Requested alteration count 72835664 exceeds maximum 16777215 [32:2078]
```

## Resolution

If the structure has been committed you need to do a rollback.

Then you have two choices on how to proceed:
* Break up the transaction and do `rcleanStore` in between parts of the transaction
* change the mapping type of the underlying map to Partition mapped so it doesn't have this limit
   * Changing the mapping type is covered here: [[TomorrowsEmergencyErrorMessageLookup]]
