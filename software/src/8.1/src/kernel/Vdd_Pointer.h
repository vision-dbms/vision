#ifndef Vdd_Pointer_Interface
#define Vdd_Pointer_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vdd_Object.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vdd {
    class Poynter : public Object {
	DECLARE_FAMILY_MEMBERS (Poynter, Store);

    //  Aliases
    public:
	typedef VReference<ThisClass> Reference;

    //  Construction
    protected:
	Poynter (VReferenceable *pReferenceable) : BaseClass (pReferenceable) {
	}

    //  Destruction
    protected:
	~Poynter () {
	}
    };
}


#endif
