#
# Drop into an interactive vpooladmin session with a given pool.
#
function administrate() {
    require_pool

    # Use vpooladmin interactively.
    runVcaApp vpooladmin -interactive
}
register_operation administrate 'Administrate pool interactively with vpooladmin.'
require_exec vpooladmin

#
# Use interactive vpooladmin commands with a given pool.
#
# TODO: Hangs because of how vpooladmin starts up. Find workaround.
#
function sendCmd() {
    require_pool
    echo -e "$@\nexit" | runVcaApp vpooladmin -interactive
}
#register_operation sendCmd 'Set an interactive vpooladmin command non-interactively.'
require_exec vpooladmin

#
# Get interactive gdb session to vpool.
#
# Assumes that debug symbols exist in vpool binary.
#
# Arguments are passed directly to gdb as command-line options.
#
function debugVPool() {
    local vpoolPath=`which vpool`
    local SrcDir=`dirname $(strings "$vpoolPath" | grep AUDITINFO:AuditFile: | cut -d : -f 3)`
    local directoryArg=""
    [[ -d "$SrcDir" ]] && directoryArg="--directory=$SrcDir"
    run gdb $@ "$directoryArg" "$vpoolPath" `cat $scratchSpace/testpool$poolIdx/vpool.pid`
}
if has_exec gdb; then
    register_operation debugVPool "Obtain interactive gdb session to a running vpool."
    require_exec vpool
    require_exec gdb
fi

#
# Get interactive gdb session to a particular worker.
#
# Assumes that debug symbols exist in worker binary and that a working PID string is available from the pool.
#
function debugWorker() {
    # Get parameters.
    local workerBin=batchvision
    local workerID=""
    while getopts 'b:i:' OPTION; do
        case $OPTION in
            b)
                workerBin="$OPTARG"
                debug_inform "Debugging binary: $workerPath"
                ;;
            i)
                workerID="$OPTARG"
                ;;
        esac
    done
    [[ -z "$workerID" ]] && error "No worker ID provided."
    
    # Get worker path.
    require_exec $workerBin
    local workerPath=`which $workerBin`
    
    # Get directory argument.
    local SrcDir=`dirname $(strings "$workerPath" | grep AUDITINFO:AuditFile: | cut -d : -f 3)`
    local directoryArg=""
    [[ -d "$SrcDir" ]] && directoryArg="--directory=$SrcDir"
    
    # Get worker PID from worker ID.
    local workerPID=`workerStats -d | cut -d '|' -f 2-3 | grep " $workerID " | cut -d '|' -f 2`
    [[ -z "$workerPID" ]] && error "Worker not found in pool."
    [[ "`ps -p $workerPID -o comm=`" == "$workerBin" ]] || error "Worker process not found."

    # Run GDB.
    run gdb "$directoryArg" "$workerPath" $workerPID
}
if has_exec gdb; then
    register_operation debugWorker "Obtain interactive gdb session to a vpool worker."
    require_exec gdb
fi
