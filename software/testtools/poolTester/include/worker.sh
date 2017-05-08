#
# Retrieves a worker's PID from its ID.
#
# Arguments:
#   $1 - The worker ID (not PID).
#
function workerPID() {
    require_pool

    # Get our arguments.
    local workerID=$1
    
    # Get worker PIDs
    local -a PIDs
    lines_to_array "`workerStats withoutHeader | cut -d \| -f 2-3`" PIDs
    
    # Search for our worker ID.
    local curLine
    for i in `seq 0 $((${#PIDs[@]} - 1))`; do # Iterate over array.
        curLine=${PIDs[$i]}
        if [[ "`echo $curLine | awk '{ print $1; }'`" == "$workerID" ]]; then
            echo $curLine | awk '{ print $3; }'
            return 0
        fi
    done
    return 1
}

#
# Kills a specified worker using the 'kill' shell builtin.
#
# Arguments:
#   $1 - The worker ID (not PID).
#
function killWorker() {
    # Get our arguments.
    local workerID=$1
    
    # Get our worker PID.
    local workerPID=`workerPID $workerID`
    [[ $? -ne 0 ]] && error "Unable to find worker with ID=$workerID"
    
    # Kill worker.
    run kill $workerPID
}
register_operation killWorker "Kills a worker provided a worker ID."

function killWorker_help() {
    word_wrap <<EOD
The killWorker operation kills a worker given a worker ID. The worker is killed by sending the TERM signal to the PID corresponding to the provided worker ID. This operation assumes local workers.

    Example: Kill worker 3.
    $POOLTESTER killWorker 3
EOD
}

#
# Check if no batchvision exist
#
function isWorkerGone () {
	debug_inform "isWorkerGone called"
	
	local timetout=10
	local username=`whoami`
	local result
	local toReturn
	local gone=false
	
	while getopts 't:' OPTION; do
		case $OPTION in
			t)
				debug_inform "Setting timeout to '$OPTARG'."
                timeout=$OPTARG

				# Perform check.
                debug_inform "Waiting up to $timeout seconds for $cond."
				for i in `seq 0 $timeout`; do
					result=`ps -e -o user -o pid -o ppid -o comm|grep $username |grep batchvision|grep 1 -w|grep -v grep|awk '{print $3}'`
					toReturn=$?
					
					if [[ ! $result ]] && [[ $toReturn -eq 0 ]]
					then
						gone=true
						break
					else
						sleep 1
					fi
                done
				
				$gone && continue
				warn "Timeout of $timeout seconds expired; workers are still there."
				return 1
		esac
	done
	shift $(($OPTIND - 1))
    OPTIND=1
	
	$gone && return 0
	warn "No conditions provided."
    return 1
}
register_operation isWorkerGone 'Evaluation if workers are gone.'

function isWorkerGone_help() {
    word_wrap <<EOD
The isWorkerGone operation detects if workers in the pool are gone. This operation assumes local workers.

Following argument must be specified:
    -t TIMEOUT   The timeout of the wait operation in seconds.
	
    Example: Wait for 3 seconds to see if workers are gone.
    $POOLTESTER isWorkerGone -t 3
EOD
}