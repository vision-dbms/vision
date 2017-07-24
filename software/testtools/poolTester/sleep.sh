#!/bin/bash
#rm loadbalance.hammers.txt -f
num=$1
duration=$2
name=$3
: ${num:=1}
: ${duration:=1}
: ${name:=querypool}

echo "Hammering $name with $num  queries, each with duration $duration ..."
for i in `seq 1 $num`; do
    echo -e "\"sleep $duration\" filterOutputOf: [];\n\"Slept $duration seconds.\" print;\n?g"
done | $Tools/vprompt -directory -server=$name -logSwitchOn -logFilePath=loadbalance.hammers.txt >/dev/null & 2>&1 
