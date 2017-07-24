#
# Help handler function.
#
function help() {
    # Get our arguments.
    local operationName="$1"
    shift
    
    # Handle case when no operation name was provided.
    if [[ "$operationName" == "" ]]; then
        operationName=help
    fi
    
    # Make sure we have given operation in our registry.
    operation_exists $operationName || error "No operation '$operationName' found."
    
    # Find our help delegate function.
    fn_exists ${operationName}_help || error "No help found for $operationName."
    
    # Execute help delegate function.
    ${operationName}_help $@
}
register_operation help "Online help."

function help_help() {
    word_wrap <<EOD
Help usage: $POOLTESTER help <operationName>
For a list of operations, try: $POOLTESTER operations
EOD
}
