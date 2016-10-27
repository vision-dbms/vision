#ifndef Vxa_VError_Interface
#define Vxa_VError_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VCollectableObject.h"

#include "V_VString.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa { 
    class Vxa_API VError : virtual public VCollectableObject {
	DECLARE_ABSTRACT_RTTLITE (VError, VCollectableObject);

    //  Construction
    protected:
	VError ();
	
    //  Destruction
    protected:
	~VError ();

    public:
	virtual void getMessage	(VString & rResult) const = 0;
    };
}

#endif
