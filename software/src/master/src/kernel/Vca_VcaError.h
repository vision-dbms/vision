#ifndef Vca_VcaError_Interface
#define Vca_VcaError_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_IError.h"


/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

/***************************
 *----  Vca::VcaError  ----*
 ***************************/

namespace Vca {

    class Vca_API VcaError : public IError {
    //  Run Time Type
	DECLARE_CONCRETE_RTTLITE (VcaError, IError);

    //  Construction
    public:
	VcaError () {
	}

    //  Destruction
    protected:
	~VcaError () {
	}

    //  IError Methods
    public:
     };
}

#endif

