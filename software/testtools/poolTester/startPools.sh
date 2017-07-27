#!/bin/bash
./poolTester -p LBTest_1 start -c configs/max1Baseline.ptc -p -w
./poolTester -p LBTest_2 start -c configs/max1Baseline.ptc -p -w
./poolTester -p LBTest_3 start -c configs/max1Baseline.ptc -p -w
./poolTester -p LBTest_LastResort start -c configs/max1Baseline.ptc -p -w
