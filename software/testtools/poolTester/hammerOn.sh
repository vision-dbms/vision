#!/bin/bash

for i in `seq 1 4`; do
    echo -e "\"sleep 200\" filterOutputOf: [];\n\"Slept 200 seconds.\" print;\n?g"
done | $Tools/vprompt -directory LoadBalanceTest1 -logSwitchOn -logFilePath=loadbalance.txt >/dev/null & 2>&1
