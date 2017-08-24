#
# Loads a config.
#
# For internal use only.
#
# Arguments:
#   $1 - Path to the configuration file to load.
#
function loadConfig() {
    # Get arguments.
    local configFile=$1

    # Check if the config file exists.
    if [[ ! -f $configFile ]]; then
        warn "Unable to find configuration file '$configFile'. Ignoring."
        return 1
    fi

    # Check if we've already loaded this config file.
    [[ ${#LOADED_CONFIGS[@]} -gt 0 ]] && for i in `seq 0 $((${#LOADED_CONFIGS[@]} - 1))`; do # Iterate over array
        [[ "${LOADED_CONFIGS[$i]}" == "$configFile" ]] || continue
        warn "Already loaded configuration file '$configFile'. Skipping."
        return 1
    done

    # Remember that we've loaded this config. Prevents cycles.
    LOADED_CONFIGS[${#LOADED_CONFIGS[@]}]=$configFile

    # Begin parsing.
    verbose_inform "Loading configuration file '$configFile'."
    local -a lines
    lines_to_array "`cat $configFile`" lines
    for i in `seq 0 $((${#lines[@]} - 1))`; do
        if [[ "${lines[$i]:0:1}" == "#" || ${#lines[$i]} -eq 0 ]]; then
            # Skip commented and or lines.
            continue
        elif [[ "${lines[$i]}" =~ "^(source|include)[ 	]*(.*)" ]]; then
            # Handle include/source directives.
            local includedConfig=${BASH_REMATCH[2]}
            if [[ "${includedConfig:0:1}" != "/" ]]; then
                includedConfig="`dirname $configFile`/$includedConfig"
            fi
            loadConfig $includedConfig
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
# Keep list of loaded config files.
declare -a LOADED_CONFIGS

#
# Start a pool.
#
function start() {
    # Unset our list of loaded configuration files.
    LOADED_CONFIGS=()

    # Get our options.
    local quickStart=false
    local plumbOpt=""
    local dirtyStart=false
    local stopExisting=false
    local killExisting=false
    local socketsDir=0
    local waitForWorkers=false
    local OPTIND=1
    while getopts 'c:dD:pqskuw' OPTION; do
        case $OPTION in
            c)
                loadConfig $OPTARG
                ;;
            D)
                if [[ "$OPTARG" =~ "([^=]+)=(.*)" ]]; then
                    # Handle assignments.
                    local configOpt=${BASH_REMATCH[1]}
                    local configVal=${BASH_REMATCH[2]}
                    verbose_inform "Setting $configOpt = '$configVal'."
                    export $configOpt="$configVal"
                else
                    warn "Invalid define syntax: '$OPTARG'. Ignoring."
                fi
                ;;
            d)
                debug_inform "Enabling dirtyStart option."
                dirtyStart=true
                ;;
            p)
                debug_inform "Enabling -plumbWorkers option."
                plumbOpt="-plumbWorkers"
                ;;
            q)
                debug_inform "Enabling quickStart option."
                quickStart=true 
                ;;
            s)
                debug_inform "Enabling stopExisting option."
                stopExisting=true
                ;;
            k)
                debug_inform "Enabling killExisting option."
                killExisting=true
                ;;
            u)
                debug_inform "Using UNIX domain socket option."
                socketsDir=$scratchSpace/sockets/
                ;;
            w)
                debug_inform "Enabling waitForWorkers option."
                waitForWorkers=true
                ;;
            ?)
                warn "Unhandled option to start: '-$OPTION'. Ignoring."
                ;;
        esac
    done
    shift $(($OPTIND - 1))
    OPTIND=1
    
    # Check if the pool is already started.
    if isAlive; then
        if $stopExisting; then
            verbose_inform "Stopping existing pool."
            stop
        elif $killExisting; then
            verbose_inform "Killing existing pool."
            stop -k
        else
            warn "Existing pool found. To start a new pool (stopping the existing one), provide the -s option. Aborting start."
            return 1
        fi
    fi
    
    # Clean up first, unless $dirtyStart is set to true.
    if ! $dirtyStart; then
        verbose_inform "Running clean operation before starting pool."
        clean
    fi
    
    # Make sure appropriate directories exist. 
    mkdir -p $scratchSpace/testpool$poolIdx $scratchSpace/sockets
    # Start vpool.
    
    verbose_inform "$plumbOpt -logFilePath=$scratchSpace/testpool$poolIdx/log -logSwitchOn $socketsDir epool >& $scratchSpace/testpool$poolIdx/testpool.log"

    sessionName='' runVcaAppBG `which vpool` $plumbOpt -logFilePath=$scratchSpace/testpool$poolIdx/log -logSwitchOn $socketsDir epool >& $scratchSpace/testpool$poolIdx/testpool.log
    
    # Store the PID.
    jobs -l %% | awk '{ print $2; }' > $scratchSpace/testpool$poolIdx/vpool.pid

    # Store the host.
    hostname > $scratchSpace/testpool$poolIdx/vpool.host

    # Store the user.
    whoami > $scratchSpace/testpool$poolIdx/vpool.user
    
    # Disown it so that subsequent wait calls won't wait for the vpool to exit.
    # TODO: Make this an option via argument.
    disown %%
    
    # Handle -q option.
    $quickStart && return 0
    
    # Attempt isAlive checks.
    for i in `seq 1 15`; do
        isAlive && break
        sleep 1
    done
    
    # Error if still not alive.
    isAlive || error "Unable to connect to newly started pool."
    
    # Wait for workers.
    $waitForWorkers || return 0
    local expectedWorkers=`getSetting WorkerMinimum`
    waitForStat -c OnlineWorkers\>=$expectedWorkers || ( warn "Timed out while waiting for workers."; return 1 )
}
register_operation start 'Start a pool.'
require_exec vpool

function start_help() {
    word_wrap <<EOD
Starts a pool. By default, this operation will delay return until the pool starts, or fifteen seconds, whichever comes first. If the pool is still not contactable after said timeout has expired, an error is reported.
Additionally, as a pre-execution step to starting, the clean operation is inferred unless the -d option is provided.

Optionally, the following arguments can be provided to the start operation:
    -c CONFIGFILE Specify configuration file to be used when starting the pool. Configuration file format described below.
    -D VAR=VALUE  Specify configuration (environment variable) value.
    -d            Dirty start; do not run 'clean' before starting.
    -p            Plumb workers; start the pool with the -plumbWorkers option for VCA goodness.
    -q            Quick start; do not wait for the pool to gracefully start before exiting.
    -s            Stop existing pools; if found, an already running pool will be stopped before starting a new one. Without this option, start will warn and abort if it encounters an already running pool.
    -u            Use UNIX domain sockets instead of TCP/IP sockets.
    -w            Wait for workers to finish starting. Invalid if quick start is enabled.

Exit status is nonzero if the pool is not contactable within 60 seconds, unless the -w argument is provided, in which case the exit status is always zero.

    Example: Start pool 1.
    $POOLTESTER start
    
    Example: Start pool 4 without waiting until the pool is started before exiting.
    $POOLTESTER -p 4 start -q
    
    Example: Start pool 2 with plumbed workers, using configuration file myConfig, without waiting until the pool is started before exiting.
    $POOLTESTER -p 2 start -p -c myConfig -q
    
    Example configuration file:
    # Include baseline configuration using absolute path.
    source /path/to/baseline.ptc
    # Include shared configuration from same directory as this configuration file.
    source shared.ptc
    # Set worker maximum a bit higher.
    MaxWorkers=10
EOD
}

#
# Check if a pool is started.
# 
# Returns nonzero if the pool is not started or is not contactable, unless we are running a sessionName
#
function isAlive() {
	[[ "$sessionName" == "" ]] || return 0
    [[ -z "`DEBUG='' sessionName='' runVcaApp vpooladmin 2>&1`" ]] && return 0
    return 1
}
register_operation isAlive 'Check if a pool is started and contactable.'
require_exec vpooladmin

function isAlive_help() {
    word_wrap <<EOD
Checks if a pool is started and contactable via vpooladmin. If vpooladmin cannot connect to the pool for whatever reason, this operation will return with nonzero exit status.

This operation is largely used as an internal operation, but is provided to the user as a convenience for scripting.
EOD
}

#
# Check if a pool is local.
#
# Returns nonzero if the pool is not local.
#
function isLocal() {
    local hostFile="`dirname $(getServerFile)/vpool.host`"
    [[ -f "$hostFile" ]] || return 1
    [[ "`cat $hostFile`" == "`hostname`" ]] || return 1
    return 0
}

#
# Stop a pool.
#
function stop() {
    # Get our options.
    local quickStop=false
    local timeout=15
    local stopType="-stop"
    local OPTIND=1
    while getopts 'qkt:' OPTION; do
        case $OPTION in
            q)
                debug_inform "Enabling quickStop option."
                quickStop=true
                ;;
            k)
                debug_inform "Enabling hardStop option."
                stopType="-hardStop"
                ;;
            t)
                debug_inform "Setting timeout to '$OPTARG'."
                timeout=$OPTARG
                ;;
            ?)
                warn "Unhandled option to start: '-$OPTION'. Ignoring."
                ;;
        esac
    done
    shift $(($OPTIND - 1))
    OPTIND=1
    
    # Make sure that we have a pool.
    require_pool

    # Send the stop and filter the output.
    local output=`runVcaApp vpooladmin "$stopType"`
    local toReturn=$?
    echo "$output" | grep -v '^Message sent$'
    if [[ $toReturn -ne 0 ]]; then
        error "Unable to send stop message."
    fi

    # Handle quick-stop option.
    $quickStop && return 0

    # Wait for the pool to stop.
    local pidFile="`dirname $(getServerFile)/vpool.pid`"
    if ( isLocal && [[ -f "$pidFile" ]] ); then
        local pid=`cat "$pidFile"`
        for i in `seq 1 $timeout`; do
            ps -p $pid >/dev/null 2>&1 || return 0;
            sleep 1
        done
    else
        for i in `seq 1 $timeout`; do
            isAlive || return 0;
            sleep 1
        done
    fi
    error "Timed out while waiting for pool to stop."
}
register_operation stop 'Stop a pool.'
require_exec vpooladmin

function stop_help() {
    word_wrap <<EOD
    Stops a pool. By default, this operation will delay return until the pool stops, or fifteen seconds, whichever comes first. If the pool process is still running after said timeout has expired, an error is reported. In the case that the pool is on a remote host, connectivity to the pool is checked instead of process existence.

Optionally, the following arguments can be provided to the stop operation:
    -q         Quick stop; do not wait for the pool to stop before exiting.
    -t TIMEOUT Specify a timeout used when waiting for the pool to stop. Only valid when -q is not specified. Defaults to 15.
EOD
}

#
# Restart a pool.
#
function restart() {
    require_pool
    local waitForWorkers=false
    local OPTIND=1
    while getopts 'w' OPTION; do
        case $OPTION in
            w)
                debug_inform "Enabling waitForWorkers option."
                waitForWorkers=true
                ;;
            ?)
                warn "Unhandled option to start: '-$OPTION'. Ignoring."
                ;;
        esac
    done
    shift $(($OPTIND - 1))
    OPTIND=1
    local output=`runVcaApp vpooladmin -restart`
    local toReturn=$?
    echo "$output" | grep -v '^Operation Succeeded$'
    
    # Wait for workers.
    $waitForWorkers || return $toReturn
    waitForStat -c WorkersBeingCreated=0 || ( warn "Timed out while waiting for workers."; toReturn=1 )
    return $toReturn
}
register_operation restart 'Restart a pool.'
require_exec vpooladmin

function restart_help() {
    word_wrap <<EOD
Restarts a pool.

Optionally, the following arguments can be provided to the restart operation:
    -w            Wait for workers to finish starting.
EOD
}

#
# Flush workers within a pool
#
function flushWorkers() {
    require_pool
    local output=`runVcaApp vpooladmin -flushWorkers`
    local toReturn=$?
    echo "$output" | grep -v '^Operation Succeeded$'
    return $toReturn
}
register_operation flushWorkers 'Flush workers within a pool.'
require_exec vpooladmin

#
# Hard stop a pool.
#
function hardStop() {
    require_pool
    local output=`runVcaApp vpooladmin -hardStop`
    local toReturn=$?
    echo "$output" | grep -v '^Message sent$'
    return $toReturn
}
register_operation hardStop 'Hard stop a pool.'
require_exec vpooladmin

#
# Hard restart a pool.
#
function hardRestart() {
    require_pool
    local waitForWorkers=false
    while getopts 'w' OPTION; do
        case $OPTION in
            w)
                debug_inform "Enabling waitForWorkers option."
                waitForWorkers=true
                ;;
            ?)
                warn "Unhandled option to start: '-$OPTION'. Ignoring."
                ;;
        esac
    done
    shift $(($OPTIND - 1))
    OPTIND=1
    local output=`runVcaApp vpooladmin -hardRestart`
    local toReturn=$?
    echo "$output" | grep -v '^Operation Succeeded$'
    
    # Wait for workers.
    $waitForWorkers || return $toReturn
    waitForStat -c WorkersBeingCreated=0 || ( warn "Timed out while waiting for workers."; toReturn=1 )
    return $toReturn
}
register_operation hardRestart 'Hard restart a pool.'
require_exec vpooladmin

#
# Suspend a pool.
#
function suspend() {
    require_pool
    local output=`runVcaApp vpooladmin -suspend`
    local toReturn=$?
    echo "$output" | grep -v '^Operation Succeeded$'
    return $toReturn
}
register_operation suspend 'Suspend a pool.'
require_exec vpooladmin

#
# Resume a pool.
#
function resume() {
    require_pool
    local output=`runVcaApp vpooladmin -resume`
    local toReturn=$?
    echo "$output" | grep -v '^Operation Succeeded$'
    return $toReturn
}
register_operation resume 'Resume a pool.'
require_exec vpooladmin

#
# Prints a list of live pools that poolTester knows about.
#
function list() {
    local i
    prefix="$scratchSpace/testpool"
    for poolDir in "$prefix"*; do 
        i=${poolDir:${#prefix}}
        poolIdx=$i isAlive && echo $i
    done
}
register_operation list 'List pools.'
