#
# Kills a specified pool using the 'kill' shell builtin.
#
# Arguments:
#   $1 - The pool Index (not PID).
#
function killPool() {
	debug_inform "kill pool called with: $@"
	
	local poolID
	local user
	local poolPID
	local toReturn
	
    # Get our pool Index.
    poolID=$1
    
    # Get current user.
	user=`whoami`

	poolPID=`ps aux| grep $user | grep vpool|grep testpool$poolID| grep -v grep| awk '{print $2}'`
	toReturn=$?

	# Kill pool.
	if [[ poolPID -ne 0 ]] && [[ $toReturn -eq 0 ]] 
	then
	  run kill $poolPID
	  return 0
	fi
	
    error "Unable to find pool with ID=$poolID" 
	return 1
	
}
register_operation killPool "Kills a pool provided a pool Index."

function killPool_help() {
    word_wrap <<EOD
The killPool operation kills a pool given a pool Index. The pool is killed by sending the TERM signal to the PID corresponding to the provided pool Index. 

    Example: Kill pool 1.
    $POOLTESTER killPool 1
EOD
}

#
# Start a vpooladmin to talk to pool
#
# For internal use only.
#
# Return:
#   0 - Pool can be connected
#	1 - Pool cannot be connected
#
function adminToPool () {
	debug_inform "adminToPool called"

	local result

	result=`runVcaApp vpooladmin -serverFile=$scratchSpace/testpool$poolIdx/serverFile`
	toReturn=$?

	[[ $toReturn -eq 0 ]] && [[ -z $result ]] && return 0
	
	warn "not connected"
	return 1
}
#register_operation adminToPool 'Evaluation if pool can be connected via vpooladmin.'
require_exec vpooladmin

#
# Waits until given pool connecting condition(s) met.
#
function poolConnectivity () {
	debug_inform "poolConncted called with: $@"
	
        local val
	local actual
	local timeout=10
        local conditionMet=false
	
	while getopts 'c:t:' OPTION; do
        case $OPTION in
        t)
                debug_inform "Setting timeout to '$OPTARG'."
                timeout=$OPTARG
                ;;
        c)
                [[ "$OPTARG" =~ "^(0|1)(.*)" ]] || error "Incorrect syntax for condition: '$OPTARG'."
                conditionMet=false

                val=${BASH_REMATCH[1]}

                # Perform check.
                debug_inform "Waiting up to $timeout seconds for pool connection evaluation ($val)."
                for i in `seq 1 $timeout`; do
                        actual=`runVcaApp adminToPool`
					
					if [[ $? -eq $val ]]; then
                        conditionMet=true
                        break
                    else
                        sleep 1
                    fi
                done

                $conditionMet && continue
				
				[[ $val -eq 1 ]] && actual="pool is still contactable"
				[[ $val -eq 0 ]] && actual="pool is still uncontactable"
                warn "Timeout of $timeout seconds expired; '$actual'."
                return 1
                ;;
        esac
    done
    shift $(($OPTIND - 1))
    OPTIND=1
	
    $conditionMet && return 0
    warn "No conditions provided."
    return 1
}
register_operation poolConnectivity 'Evaluation if pool can be connected.'

function poolConnectivity_help() {
    word_wrap <<EOD
Waits until a given pool connectivity condition is met, up to a given timeout. 

At least one of the following arguments must be specified:
    -c CONDITION Specify pool connectivity that should be checked for. Should be in the form of 0 or 1 where 1 indicate pool is not connectable while 0 indicates the opposite.
    -t TIMEOUT   The timeout of the wait operation in seconds. Only effects subsequent -c arguments. Defaults to 10.

Exit status is zero if condition(s) met, nonzero otherwise.

    Example: Wait for pool to be connectable.
    $POOLTESTER poolConnected -c 0
EOD
}

#
# Check if the process of a pool is gone
#
# Arguments
#    p - The pool Index (not PID).
#    c - The pool exists (0) or does not exist (1).
#
function doesPoolExist () {
debug_inform "doesPoolExist called"
	
	local poolID=1
	# Get current user.
	local user=`whoami`
	local poolPID
	local toReturn
	local val
	local retWarn
	local actual=9
	local conditionMet=false
	
	while getopts 'p:c:' OPTION; do
		case $OPTION in
			p)
				debug_inform "Setting poolIndex to '$OPTARG'."
				# Get our pool Index.
                poolID=$OPTARG
                ;;
			c)
				[[ "$OPTARG" =~ "^(0|1)(.*)" ]] || error "Incorrect syntax for condition: '$OPTARG'."
                conditionMet=false

                val=${BASH_REMATCH[1]}
				
				[[ $val -eq 1 ]] && retWarn="pool still exists"
                                [[ $val -eq 0 ]] && retWarn="pool does not exist"
				
                # Perform check.
                debug_inform "Waiting up to $timeout seconds for pool connection evaluation ($val)."
				
				poolPID=`ps aux|grep vpool| grep $user | grep testpool$poolID| grep -v grep| awk '{print $2}'`
				toReturn=$?

				[[ $poolPID -ne 0 ]] && [[ $toReturn -eq 0 ]] && actual=0 || actual=1
				
				[[ $actual -eq $val ]] && conditionMet=true && continue
				warn $retWarn
				return 1
				;;
		esac		
	done
	shift $(($OPTIND - 1))
	OPTIND=1
	
	$conditionMet && return 0
    return 1
}
register_operation doesPoolExist 'Evaluation if pool process exists.'

function doesPoolExist_help() {
    word_wrap <<EOD
Evaluate if pool exists. 

At least one of the following arguments must be specified:
    -c CONDITION Specify if pool process exists or not. Should be in the form 0 or 1 Where 1 indicates pool does not exist while 0 indicates the opposite.
    -p POOLINDEX Specify pool Index. Default is 1.

Exit status is zero if condition(s) met, nonzero otherwise.

Example: Check if pool 1 exists.
    $POOLTESTER doesPoolExist -c 0
EOD
}
