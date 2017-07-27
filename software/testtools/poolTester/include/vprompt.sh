#
# Get a VPrompt to a given pool.
#
# Arguments:
#   $@ - Arguments passed directly to vprompt.
#
function getVPrompt() {
    require_pool
    
    # Use vprompt interactively.
    runVcaApp vprompt $@
}
register_operation getVPrompt 'Obtain a vprompt to pool.'

function getVPrompt_help() {
    word_wrap <<EOD
Obtains an interactive vprompt session via a pool. Optionally, any arguments passed to the getVPrompt operation will be forwarded to the actual vprompt call.

    Example: Get a prompt using a worker from the current generation of pool 3.
    $POOLTESTER -p 3 getVPrompt
    
    Example: Get a prompt using a worker from any generation of pool 1.
    $POOLTESTER getVPrompt -anyData
EOD
}
require_exec vprompt
