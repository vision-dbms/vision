---
---

When a query that shouldn't be using up a lot of memory but is check the network maintenance logs.  Especially if it is a read only query.  Especially if it seems impossible to recover that memory from within the session.

## Troubleshooting

Failing to recover memory within a session looks like:

```
# run query here
AdminTools displayMemoryStats;
"COLLECTING GARBAGE" printNL;
Utility collectSessionGarbage;
AdminTools displayMemoryStats;
```

with output:

```
  Current allocation level:      3,639,834,928
  Session maximum:               3,648,481,697
  Heap limit:                    3,708,354,560
  Total size of mapped segments  8,828,141,976
COLLECTING GARBAGE
  Current allocation level:      3,644,079,204
  Session maximum:               3,648,493,713
  Heap limit:                    3,708,354,560
  Total size of mapped segments  8,828,141,976
```

The current allocation **WENT UP!**

## Fixing

Here you need to find the underlying structure and force clean to complete on it.

### Brute Force `cleanSpace`

If an ObjectSpace cleanSpace is running out of memory you can try:

* cleaning parts of the space using cleanStore
* forcing parts of the space to be cleaned by accessing those stores then commiting a bit at a time
* increasing the datasize limit and running the cleanSpace

We often use the limit datasize option for the sake of expediency. First you bump the `limit datasize` by a signficant factor (5x usually) then run the following:

```
GlobalWorkspace AdminTools setAllocationThresholdTo: 100000000;
10 asObjectSpace cleanSpace;
```

Which completed successfully. After forcing a `cleanSpace` to complete a full database clean and GC should be performed to make sure there are no other database problems lurking.
