#ifndef Vca_VProgramContext_Interface
#define Vca_VProgramContext_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca_Main.h"

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class Vca_Main_API VProgramContext : public VTransient {
	DECLARE_FAMILY_MEMBERS (VProgramContext, VTransient);

    //  Construction
    protected:
	VProgramContext ();

    //  Destruction
    protected:
	~VProgramContext ();
    };
}



#endif
