function loadConfig() {
    # Get arguments.
    local configFile=$1

    # Check if the config file exists.
    if [[ ! -f $configFile ]]; then
        warn "Unable to find configuration file '$configFile'. Ignoring."
        return 1
    fi
	
    # Begin parsing.
    verbose_inform "Loading configuration file '$configFile'."
    local -a lines
    lines_to_array "`cat $configFile`" lines
    for i in `seq 0 $((${#lines[@]} - 1))`; do
        if [[ "${lines[$i]:0:1}" == "#" || ${#lines[$i]} -eq 0 ]]; then
            # Skip commented and or lines.
            continue
        elif [[ "${lines[$i]}" =~ "([^=]+)=(.*)" ]]; then 
            # Handle assignments.
            local configOpt=${BASH_REMATCH[1]}
            local configVal=${BASH_REMATCH[2]}
            debug_on || verbose_inform "Setting $configOpt = '$configVal'."
            debug_inform "Setting $configOpt = '$configVal' ($configFile:$i)."
            export $configOpt="$configVal"
        fi
    done
    return 0
}



#
# Outputs a sessions.cfg entry for a pool.
#
function getSession() {
    # Get arguments.
    local outputName=""
    local hasSSH=true
    local -a fallbacks
	local forceSSH=false
    [[ ! -z "$sessionName" ]] && hasSSH=false

    while getopts 'e:n:s:f' OPTION; do
        case $OPTION in
            e)
                fallbacks[${#fallbacks[*]}]="$OPTARG"
                ;;
			f)
				forceSSH="$OPTARG"
				;;
            n)
                outputName="$OPTARG"
                ;;
            s)
                hasSSH=false
                ;;
        esac
    done
    shift $(($OPTIND - 1))
    OPTIND=1

    # Assign defaults as necessary.
    local exposedSessionDefinition
    if [[ -z "$sessionName" ]]; then
        # Using server file.
        [[ -z "$outputName" ]] && outputName="poolTester_tp$poolIdx"
        exposedSessionDefinition="ServerFile `getServerFile`"
    else
        # Using session name.
        [[ -z "$outputName" ]] && outputName="poolTester_tp$sessionName"
        exposedSessionDefinition="Use ${sessionName}"
    fi
    debug_inform "Has SSH: $hasSSH"
    # Detect SSH as necessary.
    if $hasSSH; then
		$forceSSH && mkdir -p `dirname $(getServerFile)`
        # Find host file.
        local hostFile=""
        if [[ -z "$sessionName" ]]; then
            hostFile="`dirname $(getServerFile)`/vpool.host"
            debug_inform "Using hostFile=$hostFile"			
			$forceSSH && hostname > $hostFile
            [[ -r "$hostFile" ]] || hostFile=""
        fi

        # Find user file.
        local userFile=""
        if [[ -z "$sessionName" ]]; then
            userFile="`dirname $(getServerFile)`/vpool.user"
            debug_inform "Using userFile=$userFile"
			$forceSSH && whoami > $userFile
            [[ -r "$userFile" ]] || userFile=""
        fi

        # Output ssh fallback session as necessary.
        if [[ -z "$hostFile" || -z "$userFile" ]]; then
            hasSSH=false
			debug_inform "Abort! Abort!!!!"
        else
            verbose_inform "SSH connectivity detected for $outputName; enabling."
            getSession -n ${outputName}_serverFile -s -f false
            echo ""
            echo "Connection_Template Begin"
            echo "Name ${outputName}_ssh"
            echo "Program vpassthru -serverFile=`getServerFile`"
            echo "Hostname `cat $hostFile`"
            echo "Username `cat $userFile`"
            echo "Option ssh passiveAgent"
            echo "Connection_Template End"
            echo ""
            exposedSessionDefinition="Use ${outputName}_serverFile\nElseUse ${outputName}_ssh"
        fi
    fi

    # Output exposed session.
    echo "Connection_Template Begin"
    echo "Name $outputName"
    echo -e "$exposedSessionDefinition"
    for fallback in ${fallbacks[*]}; do
        echo "ElseUse $fallback"
    done
    echo "Connection_Template End"
}
register_operation getSession "Print a Vca session entry for a pool."
function getSession_help() {
    word_wrap <<EOD
Prints a Vca session entry for a pool. 

If invoked via poolTester for a pool by index (the default behavior), a session template using the pool's server file will be generated. Additionally, if the appropriate necessary information (host, username) about the pool can be found inside the scratch directory, an appropriate SSH-based session templatewill be generated as well. This and the serverFile template will then be referenced from a third session template by ElseUse and Use respectively.

If invoked via poolTester for a particular session (poolTester -s SESSION), an alias to that session will be generated.

Optionally, the following arguments may be included to govern which types of session(s) are generated:
    -e SESSION  Specify a fallback for the generated session. Multiple -e arguments may be used.
    -n SESSION  The generated session will be named SESSION. If not provided, a default value is automatically generated.
    -s          Bypasses SSH connectivity detection thus generating only a single session entry. Has no effect if invoked for a session alias instead of a server file.
EOD
}

#
# Retrieve a sessions.cfg template for a pool-of-pools. Will also set VcaGlobalSessionsFile to point to that file
#
# Arguments:
#   -n outputName - The name of the session template to create.
#   -p poolIdx - The pool index to use as a worker for the pool of pools (specify multiple -p options as necessary).
#   -s poolSession - The pool session to use as a worker for the pool of pools (specify multiple -s options as necessary).
#	-w forceSSH - The sessions.cfg should include SSH templates for all pools and create user/host files for them as needed.
#	-f file - The output will be written to an appropriate file and directory in the scratch space, useful for automated test scripts.
#
function getPOPSession() {
    # Get arguments.
    local outputName=""
    local -a workers
    local -a workerTypes
    local workerIdx=-1
	local forceSSH=false
	local file=""
	
	## config values
	export MaxWorkers=1
	export MinWorkers=0
	export MinAvailWorkers=0
	export MaxAvailWorkers=0
	export WorkersBeingCreated=1
	export EvaluationTimeout=90
	export EvaluationAttempts=1
	export WorkerCreationFailureSoftLimit=5
	export WorkerCreationFailureHardLimit=11
	export StopTimeOut=0
	export ShrinkTimeOut=0
	export BusynessThreshold=3
	export MaxBusynessChecks=5000
	
    while getopts 'n:p:s:f:w:c:' OPTION; do
        case $OPTION in
            n)
                outputName="$OPTARG"
				mkdir -p $scratchSpace/sessions/$OPTARG
				file=$scratchSpace/sessions/$OPTARG/sessions.cfg
                ;;
			w)
				forceSSH=true
				;;
            p|s)
                let workerIdx++
                workers[$workerIdx]="$OPTARG"
                workerTypes[$workerIdx]="$OPTION"
                ;;
			c)
				loadConfig "$OPTARG"
				;;
        esac
    done
    shift $(($OPTIND - 1))
    OPTIND=1

    # Sanitize arguments.
    [[ -z "$outputName" ]] && error "Output name not specified. Expected -n argument."
    [[ $workerIdx -lt 1 ]] && error "No workers provided. Expected at least one -p or -s argument."
	
    # Output pool connection template.
	# Do not append in the first write so as to overwrite anything already there
    echo "#" > $file
    echo "# Pool of Pools connection template automatically generated using: $COMMANDLINE" >> $file
    echo "#" >> $file
    echo "PoolConnection_Template Begin" >> $file
    echo "Name $outputName" >> $file
	echo "MaxWorkers $MaxWorkers" >> $file
	echo "MinWorkers $MinWorkers" >> $file
	echo "MinAvailWorkers $MinAvailWorkers" >> $file
	echo "MaxAvailWorkers $MaxAvailWorkers" >> $file
	echo "WorkersBeingCreated $WorkersBeingCreated" >> $file
	echo "EvaluationTimeout $EvaluationTimeout" >> $file
	echo "EvaluationAttempts $EvaluationAttempts" >> $file
	echo "WorkerCreationFailureSoftLimit $WorkerCreationFailureSoftLimit" >> $file
	echo "WorkerCreationFailureHardLimit $WorkerCreationFailureHardLimit" >> $file
	echo "StopTimeOut $StopTimeOut" >> $file
	echo "ShrinkTimeOut $ShrinkTimeOut" >> $file
	echo "BusynessThreshold $BusynessThreshold" >> $file
	echo "MaxBusynessChecks $MaxBusynessChecks" >> $file
    for i in `seq 0 $workerIdx`; do
        echo "" >> $file
        #case ${workerTypes[$i]} in
         #   p)
          #      sessionName="poolTester_tp${workers[$i]}" getSession -n "${workers[$i]}_popWorker"
           #     ;;
            #s)
             #   sessionName="${workers[$i]}" getSession -n "${workers[$i]}_popWorker"
             #   ;;
            sessionName="${workers[$i]}_$outputName" getSession -n "${workers[$i]}" -f false >> $file
        #esac
    done
    echo "" >> $file
    echo "PoolConnection_Template End" >> $file
	
    # Output workers.
    for i in `seq 0 $workerIdx`; do
        [[ "${workerTypes[$i]}" != "p" ]] && continue
        echo "" >> $file
        poolIdx="${workers[$i]}_$outputName" getSession -f -n "${workers[$i]}_$outputName" $forceSSH >> $file
    done
	export VcaGlobalSessionsFile=$file
}
register_operation getPOPSession "Prints a sessions.cfg template for a pool-of-pools."
function getPOPSession_help() {
    word_wrap <<EOD
Prints a sessions.cfg template for a pool-of-pools.

The workers within the pool-of-pools, amongst other settings can be specified by way of the following arguments:
    -n NAME     The pool-of-pools will be named NAME (required).
    -p INDEX    Specify a pool worker by poolTester index. The associated session templates will be printed alongside the pool connection templates as well. This argument may be specified as many times as necessary.
    -s SESSION  Specify a pool worker by session name. The session is assumed to be defined elsewhere. This argument may be specified as many times as necessary.
EOD
}
