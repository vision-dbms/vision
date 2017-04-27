#
# Dump pool settings in a machine-readable format.
#
function dumpSettings() {
    require_pool
    getString settingsDump
}
register_operation dumpSettings 'Dump pool settings in a machine-readable format.'

#
# Get a particular setting from pool settings dump.
#
# Arguments:
#   $1 - The name of the setting to get.
#
function getSetting() {
    dumpSettings | grep "$1 | " | cut -d ' ' -f 3-
}
register_operation getSetting 'Get pool setting by name.'

#
# Evaluate if a particular setting from pool mets given condition
#
function poolSetting() {
	debug_inform "waitForStat called with: $@"
    require_pool

    local stat
    local cond
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
                    actual=`getSetting $stat`
                    if `eval "[[ '$actual' $cond '$val' ]]"`; then
                        conditionMet=true
                        break
                    else
                        sleep 1
                    fi
                done
                $conditionMet && continue
                warn "Timeout of $timeout seconds expired; condition '$stat $cond $val' still not met."
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
register_operation poolSetting "Waits until a given pool setting condition is met."

function poolSetting_help() {
    word_wrap <<EOD
Waits until a given pool setting condition is met, up to a given timeout. For a list of possible settings to set conditions against, see dumpSettings.

At least one of the following arguments must be specified:
    -c CONDITION Specify setting condition that should be checked for. Should be in the form <VAR><OPERATOR><VALUE> Where <VAR> is the setting name, <OPERATOR> is the conditional operator (one of: <, <=, >, >=, =, !=) and <VALUE> is the condition's check value.
    -t TIMEOUT   The timeout of the wait operation in seconds. Only effects subsequent -c arguments. Defaults to 10.

Exit status is zero if condition(s) met, nonzero otherwise.

    Example: Wait for a pool to get worker maximum in setting.
    $POOLTESTER poolSetting -c WorkerMaximum=0
EOD
}