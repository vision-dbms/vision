#include "Vk.h"

static void Abort (char const *programName, char const *complaint) {
    fprintf (
	stderr,
	"Usage:\n\t%s upperBound\n\t%s\n",
	programName,
	complaint
    );
    exit (ErrorExitValue);
}

int main (int argc, char *argv[]) {
    int
	upperBound, inputValue;
    char
	inputString[128];

    if (argc < 2)
    {
	Abort (argv[0], "Too Few Arguments Specified");
    }

    if (1 != sscanf (argv[1], "%d", &upperBound))
    {
	Abort (argv[0], "Upper Bound Not Numeric");
    }

    while (fgets (inputString, sizeof inputString, stdin))
    {
	if (1 == sscanf (inputString, " %d", &inputValue) &&
	    inputValue < upperBound)
	{
	    printf ("mv %d .dsegs\n", inputValue);
	}
    }

    return NormalExitValue;
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  mkmvscript.c 1 replace /vision/tools/source
  871017 22:11:09 insyte Install tool source in source control

 ************************************************************************/
