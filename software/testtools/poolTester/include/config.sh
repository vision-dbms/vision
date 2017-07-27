# Configuration variables.
#
# Funky syntax here allows assigning default values, does not override existing definitions (from shell) if they already exist.
# See http://www.ibm.com/developerworks/library/l-bash-parameters.html#N102E4 for syntax details.

# Set up the poolTester environment.
POOLTESTER="$0"
COMMANDLINE="$POOLTESTER $@"
: ${poolIdx:=1}
: ${COLORIZE:=false}
 
# Set up the vpool environment.
#: ${NDFPathName:=$HOME/vision-testtools/scratch/testNetwork-`uname`/NDF}
: ${UserOSI:=3}
: ${VisionAdm:=1}
: ${WorkerSource:="batchvision"}
: ${MinWorkers:=1}
: ${MinWorkersAvailable:=1}
: ${MaxWorkers:=5}
: ${WorkersInCreation:=2}
: ${WorkerPIDQueryStr:=Utility processId}
: ${VisionMaxSeriousErrors:=0}

verbose_inform "NFD..... : $NDFPathName"
for i in NDFPathName UserOSI VisionAdm WorkerSource MinWorkers MinWorkersAvailable MaxWorkers WorkersInCreation WorkerPIDQueryStr VisionMaxSeriousErrors; do
    export $i;
done

# Mangle our path here to add $Tools to it.
export PATH="$Tools:$PATH"
