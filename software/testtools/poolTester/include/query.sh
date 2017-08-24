#
# Query a pool and compare the results with a set of expected results.
#
# Arguments:
#   -e QUERYEXPR   - Query expression.
#   -q QUERYFILE   - Path to query file.
#   -r RESULTSFILE - Path to expected results file, NA if results don't matter.
#   -u QUERYURL    - Query URL.
#
# Optional Arguments:
#   -a             - Pass -anyData argument to vprompt.
#
function query() {
    debug_inform "Running query via command: $@"
    # Get our arguments.
    local queryExpression
    local queryFile
    local queryURL
    local expectedResultsFile
    local vpromptOptions
    local OPTIND=1
    local wait=false
    while getopts 'ae:q:r:wu:' OPTION; do
        case $OPTION in
            a)
                debug_inform "Setting -anyData option."
                vpromptOptions="$vpromptOptions -anyData"
                ;;
            e)
                debug_inform "Setting query expression to '$OPTARG'."
                if [[ ! -z "$queryFile" ]]; then
                    warn "Both query file and query expression provided. Dropping previously provided query file."
                    queryFile=""
                fi
                if [[ ! -z "$queryURL" ]]; then
                    warn "Both query expression and query URL provided. Dropping previously provided query URL."
                    queryURL=""
                fi
                queryExpression="$OPTARG"
                ;;
            q)
                debug_inform "Setting query file to '$OPTARG'."
                if [[ ! -z "$queryExpression" ]]; then
                    warn "Both query file and query expression provided. Dropping previously provided query expression."
                    queryExpression=""
                fi
                if [[ ! -z "$queryURL" ]]; then
                    warn "Both query file and query URL provided. Dropping previously provided query URL."
                    queryURL=""
                fi
                queryFile="$OPTARG"
                ;;
            r)
                expectedResultsFile="$OPTARG"
                ;;
	
	    w)
		wait=true
		;;
            u)
                debug_inform "Setting query URL to '$OPTARG'."
                if [[ ! -z "$queryFile" ]]; then
                    warn "Both query file and query URL provided. Dropping previously provided query file."
                    queryFile=""
                fi
                if [[ ! -z "$queryExpression" ]]; then
                    warn "Both query expression and query URL provided. Dropping previously provided query expression."
                    queryExpression=""
                fi
                queryURL="$OPTARG"
                ;;
        esac
    done
    shift $(($OPTIND - 1))
    OPTIND=1
    
    # Make sure query file exists and is readable
    [[ ! -z "$queryFile" ]] && if [[ ! -f $queryFile || ! -r $queryFile ]]; then
        error "Query file '$queryFile' is not a readable file."
    fi
    
    # Set default expected results file if one wasn't specified.
    if [[ -z "$expectedResultsFile" ]]; then
        expectedResultsFile="`echo $queryFile | sed 's/.vis$//'`.out"
    fi
    if [[ "$expectedResultsFile" == "NA" ]]; then
        expectedResultsFile=""
    fi
    # Make sure expected results file exists and is readable 

    [[ ! -z "$expectedResultsFile" ]] && if [[ ! -f $expectedResultsFile || ! -r $expectedResultsFile ]]; then
        error "Expected results file '$expectedResultsFile' is not a readable file."
    fi
    
    # Set up output file.
    local actualResultsFile=/dev/null
    if [[ ! -z "$expectedResultsFile" ]]; then
        actualResultsFile=`mktemp -p $scratchSpace/testpool$poolIdx/ query.actualResults.XXXXXXXXXX` || error "Unable to create temporary output file."
    fi
    
    # Query the database.
    local toRun
    if [[ ! -z "$queryFile" ]]; then
        toRun="$queryFile"
        verbose_inform "Running query file: $queryFile expectedResults: $expectedResultsFile"
	verbose_inform "NDF env variable: $NDFPathName"
	verbose_inform "Workers startup query: $WorkerStartupQuery"
        getVPrompt $vpromptOptions < $queryFile > $actualResultsFile & 2>&1
	local vpromptPID=$!

	# Handle -w option.
	$wait || return 0
	wait $vpromptPID
    else
        toRun="$queryExpression"
        [[ ! -z "$queryURL" ]] && toRun="\"$queryURL\" asUrl;"
        verbose_inform "Running query: $toRun expectedResults: $expectedResultsFile"
        toRun="$toRun
?g"
	
        debug_inform "Running query: $toRun"
        echo "$toRun" | getVPrompt $vpromptOptions > $actualResultsFile
    fi
    # Compare results.
verbose_inform "Compare results: $actualResultsFile $expectedResultsFile"
    [[ -z "$expectedResultsFile" ]] && return 0
#    diff -q $actualResultsFile $expectedResultsFile >/dev/null
    diff $actualResultsFile $expectedResultsFile >/dev/null
    case $? in
        0)
            # Test passed. Delete the file and return. 
            verbose_inform "Query test '$toRun' PASSED."
            run rm $actualResultsFile
            return 0
            ;;
        1)
            warn "Query test '$toRun' FAILED."
            ;;
        *)
            warn "Error running diff."
            ;;
    esac
    
    # Failure. Inform the user as to where the results are and return.
    inform "Actual result file kept as '$actualResultsFile'. Expected results are in '$expectedResultsFile'."
    return 1
}
register_operation query "Query a pool, with expected/actual results comparison."

function query_help() {
    word_wrap <<EOD
The 'query' operation sends queries to a pool and records the results. It will then compare results against known expected results and report any errors.

Usage: $POOLTESTER query -q <queryFile> [-r <expectedResultsFile>] [-a]

Optional Arguments:
   -e QUERYEXPR   - Query expression to evaluate.
   -q QUERYFILE   - Path to query file.
   -a             - Pass -anyData argument to vprompt.
   -r RESULTSFILE - Path to expected results file. If not provided, it is assumed to be QUERYFILE, minus any '.vis' extension, plus '.out' extension. If 'NA', no results comparison is done.
   -u QUERYURL    - Query URL to evaluate.

The queryFile should be a file containing vision code that, under normal circumstances, generates output exactly identical to expectedResultsFile. In the event that the results of running queryFile against the given pool generates results not identical to expectedResultsFile, a warning will be generated. Except in debug mode, intermediate output is deleted after comparison; in debug mode, the location of intermediate output on disk is printed.
EOD
}
