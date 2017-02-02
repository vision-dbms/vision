#include <stdio.h>
#include <string.h>

#include "Vk.h"
#include "Vsa_VEvaluationResult.h"

int main (int argc, char *argv[]) {
// optional timeout in milliseconds...
    unsigned int const sTimeout = argc > 2 ? static_cast<unsigned int>(atof (argv[2]) * 1000) : 0;

//  optional output limit...
    unsigned int const sContentDisplayLimit = argc > 3 ? atoi (argv[3]) : UINT_MAX;

    bool bNotDone = true;
    while (bNotDone) {
        fprintf (stdout, "P> ");
        fflush (stdout);
        char aInput[65536];
        bNotDone = fgets (aInput, sizeof (aInput), stdin) ? true : false;
        if (bNotDone) {
	    Vsa::RequestMaker iRM (argc > 1 ? argv[1] : "Default");

            Vsa::VEvaluationResult::Reference pER;
            iRM.makeRequest (pER, aInput);

            if (sTimeout > 0)
                pER->waitForResult (sTimeout);
            else
                pER->waitForResult ();

            switch (pER->state ()) {
            default:
                printf ("+++  No Result\n");
                break;
            case Vsa::VEvaluationResult::State_Failed:
                printf ("+++  Error: %s\n", pER->errorMessage ().content ());
                break;
            case Vsa::VEvaluationResult::State_Succeeded:
		printf ("%.*s\n", sContentDisplayLimit, pER->evaluationOutput ().content ());
                break;
            }
        }
    }

    return NormalExitValue;
}
