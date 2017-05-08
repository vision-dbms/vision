# Convenience functions for pool tester script.

#
# Word-wraps a message for terminal viewing.
#
# Arguments:
#   $1 - The message to be word-wrapped. If not provided, standard input is used.
#
function word_wrap() {
    # Make sure we have our COLUMNS variable set.
    if [[ -z "$COLUMNS" ]]; then
        local COLUMNS=`tput cols`
    fi
    
    # Do the appropriate thing based on argument existence.
    if [[ -z "$1" ]]; then
        # No argument. Use standard input.
        fold -s -w$COLUMNS
    else
        # Argument found. Use it as input to fold.
        echo "$1" | fold -s -w$COLUMNS
    fi
}

#
# Prints timestamp for logging purposes.
#
function log_timestamp() {
    date +'%F %T'
}

#
# Reports a fatal error then exits.
#
# Arguments:
#   $@ - The error message.
#
function error() {
    # Print error.
    echo -n "`log_timestamp` "
    $COLORIZE && tput setaf 1
    echo -n ">>>"
    $COLORIZE && tput sgr0
    echo " ERROR: $@"

    # Print stack.
    debug_on && print_stack

    # Print exit message.
    echo -n "`log_timestamp` "
    $COLORIZE && tput setaf 1
    echo -n ">>>"
    $COLORIZE && tput sgr0
    echo " Fatal error encountered. Exiting."
    exit 1
}

#
# Reports a warning.
#
# Arguments:
#   $@ - The warning message.
#
function warn() {
    # Print warning.
    echo -n "`log_timestamp` "
    $COLORIZE && tput setaf 3
    echo -n ">>>"
    $COLORIZE && tput sgr0
    echo " Warning: $@"

    # Print stack.
    debug_on && print_stack
}

#
# Reports an informational message.
#
# Arguments:
#   $@ - The informational message.
#
function inform() {
    echo -n "`log_timestamp` "
    $COLORIZE && tput setaf 2
    echo -n "***"
    $COLORIZE && tput sgr0
    echo " $@"
}

#
# Checks if verbose mode is on.
# Note that debug mode implies verbose mode.
#
# Returns 0 (success) if verbose is on.
#
function verbose_on() {
    debug_on && return 0
    if [[ "$VERBOSE" != "" ]]; then
        return 0
    else
        return 1
    fi
}

#
# Reports an informational message, but only if verbose mode is on.
#
# Arguments:
#   $@ - The informational message.
#
function verbose_inform() {
    verbose_on || return 0
    inform "$@"
}

#
# Checks if debug mode is on.
#
# Returns 0 (success) if debug is on.
#
function debug_on() {
    if [[ "$DEBUG" != "" ]]; then
        return 0
    else
        return 1
    fi
}

#
# Reports an informational message, but only if debug mode is on.
#
# Arguments:
#   $@ - The informational message.
#
function debug_inform() {
    debug_on || return 0
    echo -n "`log_timestamp` " 1>&2
    $COLORIZE && tput setaf 2 1>&2
    $COLORIZE && tput bold 1>&2
    echo -n "***" 1>&2
    $COLORIZE && tput sgr0 1>&2
    echo " DEBUG: $@" 1>&2
}

#
# Prints the (bash) stack.
#
function print_stack() {
    echo "Call stack:"
    for i in `seq 0 $((${#FUNCNAME[@]} - 1))`; do
        echo "    ${FUNCNAME[$i]}() called at ${BASH_SOURCE[$i]}:${BASH_LINENO[$i]}"
    done
}

#
# Trims whitespace from either side of a string.
#
# Arguments:
#   $1 - The string to trim whitespace from either side of.
#
function trim() {
    echo $1
}

#
# Reads a set of lines into an array.
#
# Arguments:
#   $1 - The string, containing lines, to read into an array.
#   $2 - The name of the array variable to store the resultant lines in.
#
function lines_to_array() {
    # Create array from lines.
    local IFS=$'\n'
    local -a toReturn=( $1 )
    
    # Copy to returned array.
    for i in `seq 0 $((${#toReturn[@]} - 1))`; do
        eval "$2[$i]='${toReturn[$i]}'"
    done
}

#
# Executes a program.
#
# Arguments:
#   $@ - The program and arguments to execute.
#
function run() {
    debug_inform "Running: $prefix$@"
    if [[ ! -z $remoteHost ]]; then
        ssh $remoteHost "$@"
    else
        $@
    fi
}

#
# Executes a program as a background process.
#
# Arguments:
#   $@ - The program and arguments to execute.
#
function runBG() {
        debug_inform "Running in background: $@"
    if [[ ! -z "$remoteHost" ]]; then
        ssh $remoteHost "$@" &
    else
        $@ &
    fi
}

#
# Returns the server file for a pool.
#
function getServerFile() {
    echo "$scratchSpace/testpool$poolIdx/serverFile"
}

#
# Executes a VCA program, automatically inserting the correct -serverFile argument at the front of the argument stream.
#
# Arguments:
#   $@ - The program and arguments to execute.
#
function runVcaApp() {
    local app="$1"
    shift
       # echo "sessionName:$sessionName"
       # echo "serverFile:$serverFile"

    if [[ -z "$sessionName" ]]; then
        vcaArgs="-serverFile=`getServerFile`"
    else
        vcaArgs="-directory -server=$sessionName"
    fi

    run $app $vcaArgs $@
}

#
# Executes a VCA program, automatically inserting the correct -serverFile argument at the front of the argument stream.
#
# Arguments:
#   $@ - The program and arguments to execute.
#
function runVcaAppBG() {
    local app="$1"
    shift

    if [[ -z "$sessionName" ]]; then
        vcaArgs="-serverFile=`getServerFile`"
    else
        vcaArgs="-directory -server=$sessionName"
    fi
	echo "sessionName:$sessionName"
	echo "serverFile:$serverFile"
    runBG $app $vcaArgs $@
}

#
# Should be called in any operation requiring use of a live pool.
#
function require_pool() {
    isAlive || error "Could not connect to vpool. Looks like one isn't running. Aborting."
}

#
# Checks for the presence of a program in the $PATH.
#
# Arguments:
#   $1 - The program required.
#
function has_exec() {
    type -P $1 &>/dev/null
}

#
# Checks for the presence of a program in the $PATH.
# Errors if said program was not found.
#
# Arguments:
#   $1 - The program required.
#
function require_exec() {
    has_exec $1 || error "Required executable $1 not found in \$PATH."
}
