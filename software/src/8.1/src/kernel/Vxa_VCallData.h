#ifndef Vxa_VCallData_Interface
#define Vxa_VCallData_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VRolePlayer.h"

#include "Vxa_VFiniteSet.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class Vxa_API VCallData {
	DECLARE_NUCLEAR_FAMILY (VCallData);

    //  Construction
    public:
	VCallData (cardinality_t cParameters, cardinality_t cTask);
	VCallData (ThisClass const &rOther);

    //  Destruction
    public:
	~VCallData ();

    //  Access
    public:
	cardinality_t cardinality () const {
	    return m_pDomain->cardinality ();
	}
	VFiniteSet *domain () const {
	    return m_pDomain;
	}
	cardinality_t parameterCount () const {
	    return m_cParameters;
	}

    //  State
    private:
	VFiniteSet::Reference const m_pDomain;
	cardinality_t const m_cParameters;
    };
}


#endif
