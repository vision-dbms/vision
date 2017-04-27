# Function registry.

#
# Checks if a function is defined.
#
# Returns 0 (success) if function defined, true otherwise.
#
# Arguments:
#   $1 - The function name to check existence of.
#
function fn_exists() {
    declare -f $1 >/dev/null
}

#
# Function to be called by any include when providing a new poolTester operation.
#
# Arguments:
#   $1 - Operation name (function name).
#   $2 - Operation description (one-liner).
#
function register_operation() {
    # Get our arguments.
    local operationName="$1"
    if [[ "$operationName" == "" ]]; then
        error "Expected operation name as first argument to register_operation()."
    fi
    local operationDescription="$2"
    if [[ "$operationDescription" == "" ]]; then
        error "Expected operation description as second argument to register_operation()."
    fi
    
    # Ensure that the operation actually is defined.
    fn_exists $operationName || error "No function for $operationName defined."
    
    # Add function to registry.
    registry=( "${registry[@]}" "$operationName" )
    registryDesc=( "${registryDesc[@]}" "$operationDescription" )
    debug_inform "Added $operationName to registry."
}

#
# Checks if an operation is defined.
#
# Returns 0 (success) if operation defined, true otherwise.
#
# Arguments:
#   $1 - The operation name to check existence of.
#
function operation_exists() {
    for i in ${registry[@]}; do
        [[ "$i" == "$1" ]] && return 0
    done
    return 1
}

#
# Lists operations in the registry.
#
# Implemented in an ass-backwards way for Solaris support. Can't wait till we dump it.
#
function operations() {
    # Find maximum operation width in characters.
    local maxWidth=0
    for operationName in ${registry[@]}; do
        [[ $maxWidth -lt ${#operationName} ]] && maxWidth=${#operationName}
    done
    
    echo -e "${#registry[@]} operations exist in registry. Operations denoted by * also include help accessible via '$POOLTESTER help'.\n"
    for operationName in `for i in ${registry[@]}; do echo $i; done | sort`; do
        local i=0
        while [[ "${registry[$i]}" != "$operationName" ]]; do
            let i++
            [[ $i -gt ${#registry[@]} ]] && error "Unable to find description for registry item: $operationName."
        done
        local operationDescription="${registryDesc[$i]}"
        fn_exists ${operationName}_help && operationName="$operationName*"
        printf "%-$(($maxWidth + 1))s - %s\n" "$operationName" "$operationDescription" | word_wrap
    done
}
register_operation operations "List available operations."
