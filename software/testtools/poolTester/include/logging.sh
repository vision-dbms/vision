#
# Switch on logging for a pool.
#
function enableLogging() {
    require_pool
    runVcaApp vpooladmin -poolLogSwitchOn
}
register_operation enableLogging 'Enable logging for pool.'

enableLogging_help() {
    word_wrap <<EOD
Enables logging within an already running vpool. Note that by default, pools started via poolTester automatically have logging enabled.
EOD
}

#
# Switch off logging for a pool.
#
function disableLogging() {
    require_pool
    runVcaApp vpooladmin -poolLogSwitchOff
}
register_operation disableLogging 'Disable logging for pool.'

disableLogging_help() {
    word_wrap <<EOD
Disables logging within an already running vpool. Note that by default, pools started via poolTester automatically have logging enabled.
EOD
}

#
# Show the pool's log.
#
# WARNING: This function uses =~ extensively and requires bash >=3.2 
#
# Optional Arguments:
#   -s         - Strip uniqueness.
#   -f LOGFILE - Use logfile instead of the pool's log.
#
function dumpLog() {
    local logFile="$scratchSpace/testpool$poolIdx/log"
    local strip=false
    local OPTIND=1
    while getopts 'sf:' OPTION; do
        case $OPTION in
            s)
                debug_inform "Setting option to strip uniqueness from log dump."
                strip=true;
                ;;
            f)
                debug_inform "Using '$OPTARG' instead of pool log."
                logFile="$OPTARG"
                ;;
        esac
    done
    shift $(($OPTIND - 1))
    OPTIND=1
    
    if [[ "$strip" == "false" ]]; then
        cat $logFile
    else
        local line
        cat $logFile | while read line; do
            # If there's no timestamp on the line, it's not a single-line log message; no filtering.
            if [[ ! "$line" =~ "[0-9]{2}/[0-9]{2}/[0-9]{4}:[0-9]{2}:[0-9]{2}:[0-9]{2}.[0-9]{3}.*" ]]; then
                echo $line
                continue
            fi

            # Strip timestamp.
            line=`echo $line | cut -c 25-`

            # Strip negligible lines.
            [[ "$line" =~ "(New|Zero) offers \(connections\)\.\.\." ]] && continue
            [[ "$line" =~ "V[cs]aDirectoryBuilder::(This)?Order::.*" ]] && continue
            [[ "$line" =~ "Gracefully stopping server.*" ]] && continue

            # Strip worker pid's.
            [[ "$line" =~ "(.*[wW]orker)( [0-9]{2}[0-9]+ *)(.*)" ]] && line="${BASH_REMATCH[1]} ## ${BASH_REMATCH[3]}"

            # Strip worker id's.
            [[ "$line" =~ "(.*[wW]orker)( ?#?#? ?.?[0-9]+.? *)(.*)" ]] && line="${BASH_REMATCH[1]} ## ${BASH_REMATCH[3]}"

            # Output the line.
            echo "$line"
        done
    fi
}
register_operation dumpLog "Dump the pool's log."

dumpLog_help() {
    word_wrap <<EOD
Dumps the pool's log file.

Optional arguments:
    -s         - Strip uniqueness (timestamps, worker PIDs, etc.) from dump.
    -f LOGFILE - Dump from LOGFILE instead of the pool log.
EOD
}

#
# Compare pool's log with existing log.
#
# Arguments:
#   -e EXPECTEDLOG - Expected log file.
#
# Optional Arguments:
#   -f POOLLOG     - Use POOLLOG instead of this pool's log.
#
function compareLog() {
    local logFile="$scratchSpace/testpool$poolIdx/log"
    local expectedLogFile
    local OPTIND=1
    while getopts 'e:f:' OPTION; do
        case $OPTION in
            e)
                debug_inform "Using expected log file '$OPTARG' for comparison."
                expectedLogFile="$OPTARG"
                ;;
            f)
                debug_inform "Using '$OPTARG' instead of pool log."
                logFile="$OPTARG"
                ;;
        esac
    done
    shift $((OPTIND - 1))
    OPTIND=1
    [[ -z "$expectedLogFile" ]] && error "Path to expected log file must be provided via -e option."
    
    # Strip our logs of timestamps.
    local logFileStripped="$logFile.stripped"
    local expectedLogFileStripped="$logFile.expected.stripped"
    dumpLog -s > $logFileStripped
    dumpLog -f $expectedLogFile -s > $expectedLogFileStripped
    
#    diff -q $logFileStripped $expectedLogFileStripped >/dev/null
    diff $logFileStripped $expectedLogFileStripped >/dev/null
    case $? in
        0)
            verbose_inform "Pool log matches '$expectedLogFile'."
            
            run rm $logFileStripped $expectedLogFileStripped
            return 0
            ;;
        1)
            warn "Pool log does not match '$expectedLogFile'."
            ;;
        *)
            warn "Error running diff."
            ;;
    esac
    
    # Failure. Inform the user as to where the diffed files are and return.
    inform "Stripped (compared) log files kept as '$logFileStripped' and '$expectedLogFileStripped'."
    return 1
}
register_operation compareLog "Compare this pool's log file with a given expected log file."
