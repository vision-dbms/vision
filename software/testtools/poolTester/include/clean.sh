# Array of retained files.
declare -a PT_RETAINED
PT_RETAINED_NEXTIDX=1 # 1-based array so that we can return 0 as a DNE index.

#
# Retains a file, preventing subsequent clean operations (within the same runtime) from removing it.
#
function retainFile() {
    # Get our arguments.
    local file="$1"
    if [[ ! -f "$file" ]]; then
        error "File not found while retaining: $file"
    fi

    # Add to the retained list.
    PT_RETAINED[$PT_RETAINED_NEXTIDX]="$file"
    ((PT_RETAINED_NEXTIDX++))
    debug_inform "Retained file: $file"
}

#
# Releases a file, allowing subsequent clean operations (within the same runtime) to remove it.
#
function releaseFile() {
    # Get our arguments.
    local file="$1"

    # Find the index of the given file in our array.
    isntFileRetained $file
    local fileIdx=$?

    # Short-circuit if DNE.
    if [[ $fileIdx -eq 0 ]]; then
        return 1
    fi

    # Remove from array.
    unset PT_RETAINED[$fileIdx]
    debug_inform "Released file: $file"
}

#
# Checks if a file is retained.
#
# Returns the index of the file in the retained array if it exists therein, zero otherwise.
#
function isntFileRetained() {
    # Get our arguments.
    local file="$1"
    if [[ ! -f "$file" ]]; then # Non-existant files are never retained.
        return 0;
    fi

    # Iterate over array.
    local i
    for i in `seq 0 $PT_RETAINED_NEXTIDX`; do
        [[ -z "${PT_RETAINED[$i]}" ]] && continue
        [[ "${PT_RETAINED[$i]}" == "$file" ]] && return $i
    done
    return 0
}

#
# Cleans a pool's scratch directory.
#
function clean() {
    rmOptions="-rf"
    debug_on && rmOptions="$rmOptions -v"
    local file
    for file in "$scratchSpace/testpool$poolIdx"/*; do
        if isntFileRetained "$file"; then
            run rm $rmOptions "$file"
        else
            debug_inform "Skipping clean on retained file: $file"
        fi
    done
}
register_operation clean "Cleans a pool's scratch directory (of logs, failed queries, etc.)."

function clean_help() {
    cat <<EOD
The clean operation will destroy all logs, failed query results, server files, and any other risidual files within a test pool's scratch directory.

WARNING: Do not use the clean operation on a running pool, as this will cause Really Bad(tm) things to happen.
EOD
}
