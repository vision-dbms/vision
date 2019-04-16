---
title: "VdbScan Error Message Lookup"

sidebar:
  nav: "sb_Troubleshooting"
---

Scan for your error messages here.  Try not to clutter this up with things other than the error message.

## [Cartesian Space Exceeded](VdbScan-cartesian-space-exceeded.html)

### From `VdbScan.vis`:

```
--> Running out of Cartesian Space For:
[ 97:    842]  Time Series                  176,909,428
   Rows:                                      1,128,715
   Columns:                                       1,901
   Elements:                                 37,058,058
   Indexing Type: Product Mapped
   Cartesian Space Size:                  2,145,687,215
   Percent Full:                                      2%
   -- If converted to Partition Mapped: 
      Map Size:                             148,232,272
   Map Size:                                 19,618,564
```

### Other symptomatic error messages

```
***** 21F81870.85AF4426.00000000 Tue Oct  6 04:14:38 2015 vadmin [10148 10148 1067 1067]
>>>  Error Log Entry[1]  <<<
*	Positional Token Representation Type Handler [1418]
*	An Internal Inconsistency
*	_AppendAdjustment: Instruction Out of Bounds
*		Origin:2146621926, Shift:2040846, ElementCount:2146621926
```

```
***** 21F80E78.85AF4426.00000000 Tue Oct  6 06:45:13 2015 vadmin [10148 10148 1067 1067]
>>>  Error Log Entry[1]  <<<
*	Link Representation Type Handler [669]
*	An Internal Inconsistency
*	rtLINK_Append: Reference Out of Order: O=-2147483177, RL=2147482216
```

More information about Cartesian Space Exceeded problems: [Cartesian Space Exceeded](VdbScan-cartesian-space-exceeded.html)

## [Requested Alteration Count Exceeds Maximum](VdbScan-alteration-count-exceeds-maximum.html)

```
--> Notable NDF.ERRORS contents:
----------------------------------------------------------------
***** 21885C63.37A48C50.00000000 Tue Jun 16 06:24:14 2015 ssga [12000 12000 700 700]
>>>  Error Log Entry[1]  <<<
*       Positional Token Representation Type Handler [1667]
*       A Usage Error
*       Requested alteration count 72835664 exceeds maximum 16777215 [32:2078]
```

More information: [Requested Alteration Count Exceeds Maximum](VdbScan-alteration-count-exceeds-maximum.html)

## NDF Too Big
[comment]: # (## [NDF Too Big](VdbScan-ndf-too-big.html))

### From `VdbScan.vis`:

```
--> NDF is getting dangerously large.  Size:     822,750,236
```

[comment]: # (More Information: [NDF Too Big](VdbScan-ndf-too-big.html))

## Huge Containers Found
[comment]: # (## [Huge Containers Found](VdbScan-container-too-large.html))

### From `VdbScan.vis`:

```
--> Huge Containers Found:
     1048707516  [  8:    131]
     1046948108  [  8:    294]
     1046896168  [  8:    681]
     1040010468  [  7:   5654]
```

[comment]: # (More Information: [Huge Containers Found](VdbScan-container-too-large.html))

## Container Table Out of Space
[comment]: # ([Container Table Out of Space](VdbScan-container-table-out-of-space.html))

### From `VdbScan.vis`:

```
--> Container Table Running out of Space:
 3,569,572 ObjectSpace   3: CoreWorkspace
```

[comment]: # (More Information: [Container Table Out of Space](VdbScan-container-table-out-of-space.html))

## "malloc Landmines" in read queries - [Clean has FAILED for spaces](VdbScan-clean-has-failed.html)

### From `VdbScan.vis`:

```
--> Clean has FAILED for spaces:
  spaces cleaned:        81 - complete: FALSE
275932456 Valid     83/10011  06/15/16:11:07:37 Cleanup/RunDaily:  Clean Space 83
275931808 Valid     84/9983   06/15/16:11:07:35 Cleanup/RunDaily:  Clean Space 84
275931160 Valid     61/124669 06/15/16:11:07:03 Cleanup/RunDaily:  Clean Space 61
275929812 Valid     85/2267   06/15/16:11:07:03 Cleanup/RunDaily:  Clean Space 85
275929220 Valid     81/9722   06/15/16:11:05:43 Cleanup/RunDaily:  Clean Space 81
275928600 Valid     82/9625   06/15/16:11:05:43 Cleanup/RunDaily:  Clean Space 82
```

Additional Information: [Clean has FAILED for spaces](VdbScan-clean-has-failed.html)

# Other Error Messages

## No More Room In Container Table

This error message looks like: 

```
>>>  Error Trapped By Read-Eval-Print  <<<
*       The Object Memory Manager [851]
*       A Memory Manager (M) Error
*       No More Room In Container Table (/home/ssga/localvision/network/NDF:0)
```
