#ifndef Vxa_VPolytypeMap_Interface
#define Vxa_VPolytypeMap_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VMap.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class Vxa_API VPolytypeMap : public VMap {
	DECLARE_CONCRETE_RTTLITE (VPolytypeMap, VMap);

    //  Construction
    public:
	VPolytypeMap (VSet *pDomain) : BaseClass (pDomain) {
	}

    //  Destruction
    protected:
	~VPolytypeMap () {
	}
    };
}


#endif
