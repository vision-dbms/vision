---
title: "*nix Build instructions"

layout: single

sidebar:
  nav: "sb_GettingVision"
---

In this example we'll be building `release-8.1` on Linux from a fresh clone of the repository at `https://github.com/vision-dbms/vision.git`.  

1. Start Up `tcsh`
1. Get the source (here we'll use directory `vision-open-source`)
   * `git clone https://github.com/vision-dbms/vision.git vision-open-source`
   * `cd vision-open-source`
1. Switch to the branch you want to build
   * `git checkout release-8.1`
1. Add the build scripts to your path
   * `cd software`
   * ``setenv PATH `pwd`/builder/:$PATH``
1. Perform the build
   * `cd src/master/src/`
   * `setenv VERSION "8.1.0" && buildRelease $VERSION` in this case or
   * `visionBuilder` or
   * `visionBuilder debug`
1. Build the binary directory
   * `pushBinaries ../ ../../../../software/builds/$VERSION/Linux_x86_64`
1. Add those binaries to your path
   * `cd ../../../../software/builds/$VERSION/Linux_x86_64/bin`
   * ``setenv PATH `pwd`:$PATH``

# Tests

## IVR

This presupposes you have a NDFPathName pointing to a valid vision network and the PATH setup 

1. Setup your environment and the directory
   * `cd vision-open-source/software/testtools/ivr`
   * `mkdir workCache`
   * `setenv UserOSI 3`
   * `setenv VisionAdm 1`
1. Run the test suite
   * `batchvision < testkit/scripts/buildBaseline && batchvision < testkit/scripts/checkProposed`
   
Should look something like:

```
/home/osvadmin/vision-open-source/software/testtools/ivr [89]% batchvision < testkit/scripts/buildBaseline
V> V> V> V>
Passed: testkit/test/unit/autoUpdateVerify
Passed: testkit/test/unit/cleanupFeed
Failed: testkit/test/unit/doOnceLeakCheck
>>> For more information see: testkit/test/unit/doOnceLeakCheck.testout

Passed: testkit/test/unit/doOnce
Passed: testkit/test/unit/enableOmitEmptyRecords
Passed: testkit/test/unit/extendIndexOpt
Passed: testkit/test/unit/fileHandleOverload
Passed: testkit/test/unit/holdingsStoreManagement
Passed: testkit/test/unit/isonfeed
Passed: testkit/test/unit/itemList
Passed: testkit/test/unit/matchWithKeys
Passed: testkit/test/unit/membershipDefaults
Passed: testkit/test/unit/optimizationSanity
Passed: testkit/test/unit/utils
Passed: testkit/test/functional/aliasTest
Passed: testkit/test/functional/constituentData
Passed: testkit/test/functional/dataSetChanges
Passed: testkit/test/functional/defaultCurrency
Passed: testkit/test/functional/emptyTest
Passed: testkit/test/functional/expirations
Passed: testkit/test/functional/hierarchyFeed
Passed: testkit/test/functional/holdingsDeleteAndReload
Passed: testkit/test/functional/membershipDeletion
Passed: testkit/test/functional/membershipWithoutDates
Passed: testkit/test/functional/missingFile
Passed: testkit/test/functional/simpleHoldings
/home/osvadmin/vision-open-source/software/testtools/ivr [90]% batchvision < testkit/scripts/checkProposed
V> V> V> V> V> V> V> V> V> V> V> V> V> V> V> V> V> V> V> V> V> V> V> V> V> V> V> V> V> V> V> V> V> V> V> V> V> V>

Passed: testkit/test/unit/autoUpdateVerify
Passed: testkit/test/unit/cleanupFeed
Passed: testkit/test/unit/doOnceLeakCheck
Passed: testkit/test/unit/doOnce
Passed: testkit/test/unit/enableOmitEmptyRecords
Passed: testkit/test/unit/extendIndexOpt
Passed: testkit/test/unit/fileHandleOverload
Passed: testkit/test/unit/holdingsStoreManagement
Passed: testkit/test/unit/isonfeed
Passed: testkit/test/unit/itemList
Passed: testkit/test/unit/matchWithKeys
Passed: testkit/test/unit/membershipDefaults
Passed: testkit/test/unit/optimizationSanity
Passed: testkit/test/unit/utils
Passed: testkit/test/functional/aliasTest
Passed: testkit/test/functional/constituentData
Passed: testkit/test/functional/dataSetChanges
Passed: testkit/test/functional/defaultCurrency
Passed: testkit/test/functional/emptyTest
Passed: testkit/test/functional/expirations
Passed: testkit/test/functional/hierarchyFeed
Passed: testkit/test/functional/holdingsDeleteAndReload
Passed: testkit/test/functional/membershipDeletion
Passed: testkit/test/functional/membershipWithoutDates
Passed: testkit/test/functional/missingFile
Passed: testkit/test/functional/simpleHoldings
Diff passed: aliasTest
Diff passed: constituentData
Diff passed: dataSetChanges
Diff passed: defaultCurrency
Diff passed: emptyTest
Diff passed: expirations
Diff passed: hierarchyFeed
Diff passed: holdingsDeleteAndReload
Diff passed: membershipDeletion
Diff passed: membershipWithoutDates
Diff passed: missingFile
Diff passed: simpleHoldings
Diff passed: aliasTest
Diff passed: constituentData
Diff passed: dataSetChanges
Diff passed: defaultCurrency
Diff passed: emptyTest
Diff passed: expirations
Diff passed: hierarchyFeed
Diff passed: holdingsDeleteAndReload
Diff passed: membershipDeletion
Diff passed: membershipWithoutDates
Diff passed: missingFile
Diff passed: simpleHoldings
```
   
   
# Errors

## Missing `uuid.h`

If you see something like this

``` 
../kernel/VkUUID.cpp:58:23: fatal error: uuid/uuid.h: No such file or directory
 #include <uuid/uuid.h>
                       ^
compilation terminated.
make: *** [VkUUID.o] Error 1
```

You're missing `uuid.h` which you'll need to install.  On redhat or centos the file is a part of `libuuid-devel` (`sudo yum install libuuid-devel`) which is not installed by default.

## Missing C++ compiler in `gcc`

`C++` doesn't come with `gcc` by default.  If you get an error like this:

```
gcc: error trying to exec 'cc1plus': execvp: No such file or directory
gcc: error trying to exec 'cc1plus': execvp: No such file or directory
```

you'll have to add `C++` to `gcc` (`sudo yum install gcc-c++`)
