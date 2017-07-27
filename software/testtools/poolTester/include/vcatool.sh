#
# Use vcatool on a given pool.
#
# Arguments:
#   $1 - The vcatool argument.
#
function useTool() {
    require_pool
    runVcaApp vcatool $1
}
register_operation useTool 'Use vcatool on pool.'
require_exec vcatool

#
# Use vcatool's getString operation.
#
# Arguments:
#   $1 - The key to retrieve the value of.
#
function getString() {
    useTool "-getString=$1" | sed "1s/^$1: //"
}
register_operation getString 'Use vcatool to retrieve a string value for a given key from a pool.'

#
# Use vcatool's getU32 operation.
#
# Arguments:
#   $1 - The key to retrieve the value of.
#
function getU32() {
    useTool "-getU32=$1" | sed "1s/^$1: //"
}
register_operation getU32 'Use vcatool to retrieve a numeric value for a given key from a pool.'

#
# Use vcatool's getU64 operation.
#
# Arguments:
#   $1 - The key to retrieve the value of.
#
function getU64() {
    useTool "-getU64=$1" | sed "1s/^$1: //"
}
register_operation getU64 'Use vcatool to retrieve a numeric value for a given key from a pool.'

#
# Use vcatool's getF32 operation.
#
# Arguments:
#   $1 - The key to retrieve the value of.
#
function getF32() {
	useTool "-getF32=$1" | sed "1s/^$1: //"
}
register_operation getF32 'Use vcatool to retrieve a numeric value for a given key from a pool.'

#
# Use vcatool's getF64 operation.
#
# Arguments:
#   $1 - The key to retrieve the value of.
#
function getF64() {
    useTool "-getF64=$1" | sed "1s/^$1: //"
}
register_operation getF64 'Use vcatool to retrieve a numeric value for a given key from a pool.'