#
# Check for failed assertions.
#
function checkAssertions() {
    require_pool
    local assertionsFailed=`getU32 AssertionsFailed`
    if [[ "$assertionsFailed" -gt 0 ]]; then
        warn "Assertions FAILED: $assertionsFailed"
        return 1
    fi
}
register_operation checkAssertions "Check pool assertions."

#
# Check any given property with getString
# Arguments:
#   $1 - The property to look up
#
function checkGetString() {
	require_pool
	local property=`getString $1`
	if [[ ! "$property" ]]; then
		warn "getString FAILED: property $1"
		return 1
	fi
}
register_operation checkGetString "Check getString properties."

#
# Check any given property with getU32
# Arguments:
#   $1 - The property to look up
#
function checkGetU32() {
	require_pool
	local property=`getU32 $1`
	if [[ ! "$property" ]]; then
		warn "getU32 FAILED: property $1"
		return 1
	fi
}
register_operation checkGetU32 "Check getU32 properties."

#
# Check any given property with getU64
# Arguments:
#   $1 - The property to look up
#
function checkGetU64() {
	require_pool
	local property=`getU64 $1`
	if [[ ! "$property" ]]; then
		warn "getU64 FAILED: property $1"
		return 1
	fi
}
register_operation checkGetU64 "Check getU64 properties."

#
# Check any given property with getF32
# Arguments:
#   $1 - The property to look up
#
function checkGetF32() {
	require_pool
	local property=`getF32 $1`
	if [[ ! "$property" ]]; then
		warn "getF32 FAILED: property $1"
		return 1
	fi
}
register_operation checkGetF32 "Check getF32 properties."

#
# Check any given property with getF64
# Arguments:
#   $1 - The property to look up
#
function checkGetF64() {
	require_pool
	local property=`getF64 $1`
	if [[ ! "$property" ]]; then
		warn "getF64 FAILED: property $1"
		return 1
	fi
}
register_operation checkGetF64 "Check getF64 properties."

function checkClassInfo() {
	checkGetString classInfo
	local toReturn=$?
	[[ toReturn -ne 0 ]] && warn "getString classInfo failed"
}

function checkBusyness() {
	checkGetU32 busyness
	local toReturn=$?
	[[ toReturn -ne 0 ]] && warn "getString busyness failed"
}
