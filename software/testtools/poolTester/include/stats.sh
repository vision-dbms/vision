#
# Get pool stats.
#
function getStats() {
    require_pool
    
	useSession=1
	
    # Use vpooladmin to get stat output.
    runVcaApp vpooladmin -stat
	
	useSession=""
}
register_operation getStats 'Show pool statistics.'
require_exec vpooladmin

#
# Dump pool stats (machine-readable).
#
function dumpStats() {
    getString statDump
}
register_operation dumpStats 'Dump pool statistics in a machine-readable format.'

#
# Get a particular statistic from pool stat dump.
#
# Arguments:
#   $1 - The name of the statistic to get.
#
function getStat() {
    dumpStats | grep "$1 | " | cut -d ' ' -f 3-
}
register_operation getStat 'Get pool statistic by name.'

#
# Helper function for workerStats, generationStats, quickStats.
# Handles the boilerplate code.
# Assumes that <statsKey>Stats retrieves table data and that <statsKey>StatsHeader retrieves header for table.
#
# Arguments:
#   -k KEY - The IGetter key to use for stats retrieval, sans 'Stats' suffix.
#   -d     - Data-only, do not print header.
#
function statsTable() {
    local dataOnly=false
    local key
    local OPTIND=1
    while getopts 'k:d' OPTION; do
        case $OPTION in
            k)
                debug_inform "Found key $OPTARG in statsTable()."
                key=$OPTARG
                ;;
            d)
                debug_inform "Enabling data-only option."
                dataOnly=true
                ;;
            ?)
                warn "Unhandled option to workerStats: '-$OPTION'. Ignoring."
                ;;
        esac
    done
    shift $(($OPTIND - 1))
    OPTIND=1
    
    # Make sure we have a stats key at least.
    [[ -z "$key" ]] && error "No stats key provided."
    
    # Show header unless dataOnly is true.
    $dataOnly || getString ${key}StatsHeader
    
    # Get our stats and print them.
    getString ${key}Stats
}

#
# Helper function for *Stats_help.
#
# Arguments:
#   $1 - The IGetter key to use for stats retrieval, sans 'Stats' suffix.
#
function statsTable_help() {
    local key=$1
    word_wrap <<EOD
Dumps $key statistics from a pool in tabular format with one line (row) per $key. Also prints a header line for the statistics dump detailing what each column represents.

Optionally, the following arguments can be provided to the ${key}Stats operation:
    -d Data-only mode; do not print header line.

    Example: Dump $key statistics for pool 3 and suppress the header line.
    $POOLTESTER -p 3 ${key}Stats -d
EOD
}

#
# Get pool stats (one-liner) with header.
#
# Arguments:
#    -d - Data-only, do not print header.
#
function quickStats() {
    statsTable -k quick $@
}
register_operation quickStats 'Show pool statistics all in one line.'
function quickStats_help() {
    word_wrap <<EOD
Retrieves a one-liner statistics dump from a pool. Also prints a header line for the statistics dump detailing what each column represents.

Optionally, the following arguments can be provided to the quickStats operation:
    -d Data-only mode; do not print header line.

    Example: Retrieve one-liner statistics for pool 2 and suppress the header line.
    $POOLTESTER -p 2 quickStats -d
EOD
}

#
# Get pool stats (one-liner) header.
#
function quickStatsHeader() {
    getString quickStatsHeader
}
register_operation quickStatsHeader 'Show pool statistics header for all-in-one-line quick pool statistics.'

#
# Get worker stats.
#
# Arguments:
#    -d - Data-only, do not print header.
#
function workerStats() {
    statsTable -k worker $@
}
register_operation workerStats 'Show worker statistics.'
function workerStats_help() {
    statsTable_help worker
}

#
# Get generation stats.
#
# Arguments:
#    -d - Data-only, do not print header.
#
function generationStats() {
    statsTable -k generation $@
}
register_operation generationStats 'Show generation statistics.'
function generationStats_help() {
    statsTable_help generation
}

