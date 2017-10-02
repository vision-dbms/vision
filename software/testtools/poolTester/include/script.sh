#
# Retrieves a value from a line in a script file.
#
# Arguments:
#   $1 - The line from the script file.
#   $2 - The key of the value to retrieve.
#   $3 - The name of the variable to store the resultant value in.
#   $4 - The default value for the variable. Optional.
#
function runScript_getValue() {
    # Get our arguments.
    local line="$1"
    [[ -z "$line" ]] && error "Expected script line as first argument to runScript_getValue()."
    local searchKey="$2"
    [[ -z "$searchKey" ]] && error "Expected search key as second argument to runScript_getValue()."
    local toReturn="$3"
    [[ -z "$toReturn" ]] && error "Expected return variable name as third argument to runScript_getValue()."
    eval "$toReturn='$4'" # Set default value.
    
    debug_inform "Finding value for key '$searchKey' in '$line'."
    
    # TODO: enhance robustness of runScript_getValue()
    local -a kvps # Array of key-value pairs.
    lines_to_array "`echo $line | tr , '\n'`" kvps # Populate array
    for i in `seq 0 $((${#kvps[@]} - 1))`; do # Iterate over array
        # Check if this key-value-pair has the key we're looking for.
        [[ "${kvps[$i]}" =~ "[ 	]*$searchKey[ 	]*:(.*)" ]] || continue
	 
        # We've found the correct key. Output the value and return.
        local value=`trim "${BASH_REMATCH[1]}"`
	eval "$toReturn='$(echo `eval echo \"$value\"`)'"
	debug_inform "value found: '$value' -- '$toReturn'"
        break;
    done
}

#
# Cleans intermediate thread states.
# Intended to be called via EXIT signal trap.
#
function runScript_onThreadExit() {
    local finishMessage="Thread ($POOLTESTER_THREAD_ID) finished."
    grep -q "$finishMessage" $POOLTESTER_SCRIPT_LOGFILE || echo "Thread ($POOLTESTER_THREAD_ID) finished." >> $POOLTESTER_SCRIPT_LOGFILE
}

#
# Runs a script thread.
#
# Arguments:
#   $1 - The script file.
#   $2 - The thread ID to execute.
#
function runScript_runThread() {
    # Get our arguments.
    local scriptFile="$1"
    local threadID="$2"
    
    # Set up environment.
    export POOLTESTER_THREAD_ID=$threadID
    
    # Set up cleanup trap.
    trap 'runScript_onThreadExit' TERM EXIT
    
    # Find out how many repetitions to run for this thread.
    local threadLine=`grep "^thread:$threadID" $scriptFile | head -1`
    runScript_getValue "$threadLine" repetitions repetitions
    
    # Announce that we're starting.
    echo "Thread ($POOLTESTER_THREAD_ID) started. Will perform $repetitions iterations." >> $POOLTESTER_SCRIPT_LOGFILE
    
    # Wrap thread execution in a for loop.
    if [[ $repetitions != "infinite" ]]; then
        for i in `seq 1 $repetitions`; do
            runScript_runThreadIteration $scriptFile $threadID "$i/$repetitions"
        done
    else
        local i=0;
        while [[ "`grep "finished" $POOLTESTER_SCRIPT_LOGFILE | wc -l`" != "`grep "started" $POOLTESTER_SCRIPT_LOGFILE | grep -v infinite | wc -l`" ]]; do
            i=$((i+1))
            runScript_runThreadIteration $scriptFile $threadID "$i"
        done
    fi
    exit 0
}

#
# Runs a script thread iteration.
#
# Arguments:
#   $1 - The script file.
#   $2 - The thread ID to execute.
#   $3 - The iteration text (used for human-readable status updates).
#
function runScript_runThreadIteration() {
    # Get our arguments.
    local scriptFile="$1"
    local threadID="$2"
    local iteration="$3"

    # Begin iteration message.
    verbose_inform "Beginning iteration $iteration for thread $threadID."
    
    # Iterate over each operation line.
    local -a operationLines # Array of operations_on_thread:$threadID lines.
    local oldPoolIdx="$poolIdx"
    lines_to_array "`grep "^operation_on_thread:$threadID" $scriptFile`" operationLines # Populate array.
    for operationID in `seq 0 $((${#operationLines[@]} - 1))`; do # Iterate over array.
        local toRun
        poolIdx="$oldPoolIdx"
        runScript_getValue "${operationLines[$operationID]}" run toRun
        runScript_getValue "${operationLines[$operationID]}" poolIdx poolIdx $poolIdx
	runScript_getValue "${operationLines[$operationId]}" sessionName sessionName 
        verbose_inform "Running operation $threadID#$operationID, iteration $iteration, sessionName $sessionName, poolIdx $poolIdx: $toRun"
        
        # Start a subshell if it is runScript 
        if [[ $toRun == *"runScript"* ]] 
        then 
                (eval $toRun) 
        else
                eval $toRun
        fi
    done
}

#
# Cleans intermediate runscript state.
# Intended to be called via EXIT signal trap.
#
function runScript_onExit() {
    releaseFile $POOLTESTER_SCRIPT_LOGFILE
    run rm $POOLTESTER_SCRIPT_LOGFILE
}

#
# Kills running backgrounded threads.
# Intended to be called via TERM signal trap.
#
function runScript_onTerm() {
    for thread in `jobs -p`; do
        warn "Killing background thread (pid=$thread)."
        kill $thread
    done
}

#
# Runs a poolTester script.
#
# poolTester scripts support threads (parallelism) and repetitions.
#
# Arguments:
#   -s SCRIPTFILE  - The script to execute.
#
function runScript() {
    # Get our arguments.
    local scriptFile
    local expectedLog
    local OPTIND=1
    while getopts 's:l:' OPTION; do
        case $OPTION in
            s)
                scriptFile=$OPTARG
                ;;
            l)
                debug_inform "Setting expectedLog to '$OPTARG'."
                expectedLog="$OPTARG"
                ;;
        esac
    done
    shift $(($OPTIND - 1))
    if [[ ! -f "$scriptFile" ]]; then
        error "Expected path to script file as argument to runScript()."
    fi
    
    # Set our environment.
    mkdir -p $scratchSpace/testpool$poolIdx/
    export POOLTESTER_SCRIPT_LOGFILE=`mktemp -p $scratchSpace/testpool$poolIdx/`
    retainFile $POOLTESTER_SCRIPT_LOGFILE
    trap 'runScript_onExit' EXIT
    trap 'runScript_onTerm' SIGINT PIPE TERM
    
    # Find our expextedLog default.
    if [[ -z "$expectedLog" && -r "$scriptFile.log" ]]; then
        expectedLog="$scriptFile.log"
    fi
    
    # First do our pre_exec operations.
    local -a preExecLines # Array of pre_exec: lines.
    lines_to_array "`grep ^pre_exec: $scriptFile`" preExecLines # Populate array.
#	echo "precExecLines$precExecLines"
    local toRun
    for i in `seq 0 $((${#preExecLines[@]} - 1))`; do #
        # Parse.
        runScript_getValue "${preExecLines[$i]}" pre_exec toRun
        
        # Run.
        verbose_inform "Running pre-execution '$toRun'."
        eval $toRun
    done
    
    # Iterate over each thread ID.
    local -a threadLines # Array of thread: lines.
    lines_to_array "`grep ^thread: $scriptFile`" threadLines # Populate array.
    local threadID
    for i in `seq 0 $((${#threadLines[@]} - 1))`; do # Iterate over array.
        # Parse.
        runScript_getValue "${threadLines[$i]}" thread threadID
        
        # Start thread.
        debug_inform "Starting thread $threadID.($threadLines[$i])"
        runScript_runThread "$scriptFile" "$threadID" &
    done
    
    # Wait for children to finish.
    wait
    
    # Finally, do our post_exec operations.
    releaseFile $POOLTESTER_SCRIPT_LOGFILE
    local -a postExecLines # Array of post_exec: lines.
    lines_to_array "`grep ^post_exec: $scriptFile`" postExecLines # Populate array.
    for i in `seq 0 $((${#postExecLines[@]} - 1))`; do # Iterate over array
        # Parse.
        runScript_getValue "${postExecLines[$i]}" post_exec toRun
        
        # Run.
        verbose_inform "Running post-execution '$toRun'."
        eval $toRun
    done
    
    # If we have a .log file, compare our results.
    [[ -z "$expectedLog" || "$expectedLog" == "NA" ]] && return
    compareLog -e $expectedLog
}
register_operation runScript "Run a poolTester script."

function runScript_help() {
    word_wrap <<EOD
Runs a poolTester script, which can be used to automate a series of pool test operations in a variety of fashions. Such scripts are written in a special poolTester domain-specific language, specified and described as follows.

Arguments:
    -s SCRIPTFILE  - Path to the script file to be run.

Optional Arguments:
    -l EXPECTEDLOG - Path to the expected log file. If provided, this log file will be compared against the pool's log file at the end of script execution using compareLog. If unset and SCRIPTFILE.log exists, this will be the assumed EXPECTEDLOG. If set to 'NA', no log comparison will occur.

poolTester scripts are made up of the following definitions:

    * Thread Operations: Any operation that can be executed using a single call to poolTester can also be executed from within a script as a thread operation. In addition to providing the operation to execute and all of its arguments, the thread ID to which said thread operation belongs must also be provided.
    * Threads: Each script is composed of one or more threads. Each thread is composed of one or more thread operations. Each thread executes (in parallel to other threads) for a specified number of repetitions and then exits. If the repetitions is 'infinite', the thread will run until all finite threads have executed to completion. In this case there must be at least one finite thread in the script. Execution of a thread simply entails serial execution of each of its thread operations.
    * Pre-Exec Operations: These are just like the aforementioned thread operations but instead of belonging to the thread, are executed before any threads are started. These operations typically entail starting pools and/or setting up a runtime environment.
    * Post-Exec Operations: Analogous to Pre-Exec operations, these operations are executed after all threads have finished execution. These operations typically entail stopping pools and dismantling/cleaning a runtime environment.
        
    Example Script: Start a pool, run some queries (three times) with interspersed restarts after a four second wait (twice), and stop the pool once all processing is complete.
    
    # Start the pool first.
    pre_exec: start
    
    # Query thread.
    thread: querier, reptitions: 3
    operation_on_thread: querier, run: query input1.vis expectedOutput1.txt
    operation_on_thread: querier, run: query input2.vis expectedOutput2.txt
    
    # Restart thread.
    thread: restarter, repetitions: 2
    operation_on_thread: restarter, run: sleep 4
    operation_on_thread: restarter, run: restart
    
    # Stop the pool when the threads are done executing.
    post_exec: stop
    
    Example Usage: Run the script 'testScript.pts' on pool 3.
    $POOLTESTER -p 3 runScript -s testScript.pts
EOD
}

#
# Describes a poolTester script based on in-file documentation of said script.
#
# Prints all comment lines from the beginning of the script until the first non-comment line.
#
# Arguments:
#   -s SCRIPTFILE  - The script to execute.
#
function describeScript() {
    # Get our arguments.
    local scriptFile
    local OPTIND=1
    while getopts 's:' OPTION; do
        case $OPTION in
            s)
                scriptFile=$OPTARG
                ;;
        esac
    done
    shift $(($OPTIND - 1))
    if [[ ! -f "$scriptFile" ]]; then
        error "Expected path to script file as argument to runScript()."
    fi
    
    # Iterate over each line in the script.
    cat $scriptFile | while read line; do
        [[ "${line:0:1}" != "#" ]] && return
        line="${line:1}"
        [[ "${line:0:1}" == " " ]] && line="${line:1}"
        echo $line
    done
}
register_operation describeScript "Describe a poolTester script."

function describeScript_help() {
    word_wrap <<EOD
Describes a poolTester script based on its in-file documentation.

Prints all comment lines from the beginning of the script until the first non-comment line.

Arguments:
    -s SCRIPTFILE  - Path to the script file to be run.
EOD
}
