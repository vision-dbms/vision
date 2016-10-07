#!/bin/bash

###
#
# $LocalVisionRoot/adminScripts/parallel.cmd
#
# This script is used to run multiple copies of a program in parallel, using a
#   list of arguments that comes from running a user-specified command.
#
# The arguments must be specified in the following order:
#   <max-procs> - max amount of processes to run in parallel
#   <sleep-sec> - time to wait before checking to see if a slot is open
#         <cmd> - command to run in parallel (quote if needed)
#   <param-cmd> - command to generate the list of parameters (quote if needed)
#
###

if (($# != 4)); then
    echo "Usage: $0 <max-procs> <sleep-sec> <cmd> <param-cmd>"
    exit 1
fi

echo
echo "Started parallel.cmd at: `date`"
echo "  Parallel execution of: $3"
echo "  Using parameters derived from: $4"

maxProcs=$1
sleepSeconds=$2
cmd=$3  # quote argument when there is a command with spaces
params=(`$4`)  # run param-cmd to get array of parameters for cmd

echo
echo "Preparing to execute the following in parallel:"
for param in ${params[@]}; do
    echo "* $cmd $param"
done

results=`mktemp -t parallel.XXXXXXXXXX`

echo
for param in ${params[@]}; do
    fullCmd="$cmd $param"
    ( # execute commands within background subshell
        output="Starting: '$cmd $param' at `date`"
        output+=$'\n'
        output+=`$fullCmd`
        echo -e "$?\t$fullCmd" >> $results
        output+=$'\n'
        output+="Finished: '$cmd $param' at `date`"
        output+=$'\n'
        echo "$output"
    ) &

    # if at max processes then sleep for a bit
    jobList=(`jobs -rp`)
    while ((${#jobList[@]} >= $maxProcs)); do
        sleep $sleepSeconds
        jobList=(`jobs -rp`)
    done
done

# wait for remaining processes to finish
wait

# check result of all runs
echo
succeeded=0
while read result fullCmd; do
    if (($result == 0)); then
        let succeeded++
    else
        echo "ERROR: Execution of '$fullCmd' returned $result"
    fi
done < $results

# remove temporary file without prompting
\rm $results

echo "Finished parallel.cmd at: `date`"

# return amount of jobs that failed, zero means success
let failed=(${#params[@]} - $succeeded)
exit $failed

