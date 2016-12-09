#ifndef Vxa_VFiniteSet_Interface
#define Vxa_VFiniteSet_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VSet.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class Vxa_API VFiniteSet : public VSet {
	DECLARE_CONCRETE_RTTLITE (VFiniteSet, VSet);

    //  Construction
    public:
	VFiniteSet (cardinality_t iCardinality);

    //  Destruction
    private:
	~VFiniteSet ();

    //  Cardinality
    private:
	cardinality_t cardinality_() const {
	    return cardinality ();
	}
    public:
	cardinality_t cardinality () const {
	    return m_iCardinality;
	}

    //  State
    private:
	cardinality_t const m_iCardinality;
    };
}


#endif
