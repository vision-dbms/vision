#
# Waits until given statistic condition(s) met.
#
function waitForStat() {
    debug_inform "waitForStat called with: $@"
    require_pool

    local stat
    local cond
    local val
    local actual
    local timeout=120
    local conditionMet=false
    local OPTIND=1
    while getopts 'c:t:' OPTION; do
        case $OPTION in
            c)
                [[ "$OPTARG" =~ "^([^><=\!]+)(<>|>=|>|<=|<|==|=|!=)(.*)" ]] || error "Incorrect syntax for condition: '$OPTARG'."
                conditionMet=false
                stat=${BASH_REMATCH[1]}
                cond=${BASH_REMATCH[2]}
                val=${BASH_REMATCH[3]}

                # Translate cond.
                case $cond in
                    '>')
                        cond='-gt'
                        ;;
                    '>=')
                        cond='-ge'
                        ;;
                    '<')
                        cond='-lt'
                        ;;
                    '<=')
                        cond='-le'
                        ;;
                    '=' )
                        cond='=='
                        ;;
                    '<>')
                        cond='!='
                        ;;
                esac

                # Perform check.
                debug_inform "Waiting up to $timeout seconds for '$stat' $cond '$val'."
                for i in `seq 1 $timeout`; do
                    actual=`getStat $stat`
                    if `eval "[[ '$actual' $cond '$val' ]]"`; then
                        conditionMet=true
                        break
                    else
			debug_inform "$i of $timeout: $actual $cond $val"
                        sleep 1
                    fi
                done
                $conditionMet && continue
                warn "Timeout of $timeout seconds expired; condition '$stat $cond $val' still not met."
                return 1
                ;;
            t)
                debug_inform "Setting timeout to '$OPTARG'."
                timeout=$OPTARG
                ;;
        esac
    done
    shift $(($OPTIND - 1))
    OPTIND=1

    $conditionMet && return 0
    warn "No conditions provided."
    return 1
}
register_operation waitForStat "Waits until a given pool statistic condition is met."

function waitForStat_help() {
    word_wrap <<EOD
Waits until a given pool statistic condition is met, up to a given timeout. For a list of possible statistics to set conditions against, see dumpStats.

At least one of the following arguments must be specified:
    -c CONDITION Specify statistical condition that should be checked for. Should be in the form <VAR><OPERATOR><VALUE> Where <VAR> is the statistic name, <OPERATOR> is the conditional operator (one of: <, <=, >, >=, =, !=) and <VALUE> is the condition's check value.
    -t TIMEOUT   The timeout of the wait operation in seconds. Only effects subsequent -c arguments. Defaults to 120.

Exit status is zero if condition(s) met, nonzero otherwise.

    Example: Wait for a pool to finish hiring workers.
    $POOLTESTER waitForStat -c WorkersBeingCreated=0
EOD
}
