#ifndef Vxa_VMap_Interface
#define Vxa_VMap_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VRolePlayer.h"
#include "Vxa_VSet.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class Vxa_API VMap : public VRolePlayer {
	DECLARE_ABSTRACT_RTTLITE (VMap, VRolePlayer);

    //  Construction
    protected:
	VMap (VSet *pDomain) : m_pDomain (pDomain) {
	}

    //  Destruction
    protected:
	~VMap () {
	}

    //  Access
    public:
	VSet *domain () const {
	    return m_pDomain;
	}

    //  State
    private:
	VSet::Reference const m_pDomain;
    };
}


#endif
