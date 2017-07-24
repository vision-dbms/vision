#
# Hammer a pool with a set amount of queries all at once, each taking a set amount of seconds to complete.
#
# Arguments:
#   -c COUNT    - Hammers the pool with COUNT queries. Defaults to 1.
#   -d DURATION - Each query lasts DURATION seconds. Defaults to 30.
#   -w          - If provided, causes hammer to wait for children (queries) to exit (finish) before exiting.
# 
function hammer() {
    require_pool
    
    # Get our arguments.
    local count=1
    local duration=30
    local wait=false
	local iteration=0
    local vpromptOptions=""
    local OPTIND=1
    while getopts 'ac:d:wl:i:' OPTION; do
        case $OPTION in
            a)
                debug_inform "Setting -anyData option."
                vpromptOptions="$vpromptOptions -anyData"
                ;;
            c)
                debug_inform "Setting count option to $OPTARG."
                count=$OPTARG
                ;;
            d)
                debug_inform "Setting duration option to $OPTARG."
                duration=$OPTARG
                ;;
            w)
                debug_inform "Setting wait option."
                wait=true;
                ;; 
            l)
                debug_inform "Setting logging to $OPTARG.$$"
                vpromptOptions="$vpromptOptions -logSwitchOn -logFilePath=$OPTARG.$$"
                ;;
			i) 
				debug_inform "Setting iteration option to $OPTARG."
				iteration=$OPTARG
				;;
            ?)
                debug_inform "Unhandled option to hammer: '-$OPTION'. Ignoring."
                ;;
        esac
    done
    shift $(($OPTIND - 1))
    OPTIND=1
    
	# Set a loop with iteration number specified
	if [[ 0 != $iteration  ]]; then
		echo -e '!'"x <- 0;\n[ x <= $iteration ] whileTrue: [ :x <- x + 1;];\n x print;\n?g" | getVPrompt $vpromptOptions > /dev/null & 2>&1
	else
    # Use vprompt iteratively.
    for i in `seq 1 $count`; do
        echo -e "\"sleep $duration\" filterOutputOf: [];\n\"Slept $duration seconds.\" print;\n?g"
    done | getVPrompt $vpromptOptions >/dev/null & 2>&1
	fi
    local vpromptPID=$!

    # Handle -w option.
    $wait || return 0
    wait $vpromptPID
}
register_operation hammer 'Hammer a pool with n queries each lasting m seconds.'

function hammer_help() {
    word_wrap <<EOD
The hammer operation hammers a pool with a given number of queries, each lasting a given amount of time.

Optionally, the following arguments can be provided to the hammer operation:
    -c COUNT       Hammers the pool with COUNT queries. Defaults to 1.
    -d DURATION    Each query lasts DURATION seconds. Defaults to 30.
    -w             If provided, causes hammer to wait for child queries to complete before exiting.
    -a             If provided, causes hammer to call vprompt with the -anyData option.
    -l pathName    If provided, causes hammer to write log information to the file identified by pathName
	-i 			   If provided, causes hammer to start a loop with specified number. If iterations is specified, then hammer will disable the -c and -d options.

The exit status of hammer operations is always zero (success), even if some (or all) of its children have failed.
    
    Example 1: Hammer pool 1 with 200 queries that each take 4 seconds to process.
    $POOLTESTER hammer -c 200 -d 4
    
    Example 2: Hammer pool 3 with 15 queries that each take 60 seconds to process.
    $POOLTESTER -p 3 hammer -d 60 -c 15"
    
    Example 3: Hammer pool 1 with 100 queries that each take one second to process, and wait for all queries to finish before exiting.
    $POOLTESTER hammer -c 100 -d 4 -w
	
    Example 4: Hammer pool 1 with 1 queries that can loop 100 times, and even count and duration are specified, they are disabled.
    $POOLTESTER hammer -i 100 -c 100 -d 4
EOD
}
