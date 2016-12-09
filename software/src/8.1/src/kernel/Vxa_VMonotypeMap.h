#ifndef Vxa_VMonotypeMap_Interface
#define Vxa_VMonotypeMap_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VMap.h"

#include "Vxa_VArray.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VTypePattern.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class Vxa_API VMonotypeMapBase : public VMap {
	DECLARE_ABSTRACT_RTTLITE (VMonotypeMapBase, VMap);

    //  Construction
    protected:
	VMonotypeMapBase (VSet *pDomain, VSet *pCodomain) : BaseClass (pDomain), m_pCodomain (pCodomain) {
	}

    //  Destruction
    protected:
	~VMonotypeMapBase () {
	}

    //  Access
    public:
	VSet *codomain () const {
	    return m_pCodomain;
	}

    //  State
    private:
	VSet::Reference m_pCodomain;
    };

    template <
	typename Val_T, typename Var_T = typename Vca::VTypePattern<Val_T>::var_t
    > class VMonotypeMap : public VMonotypeMapBase {
	typedef VMonotypeMap<Val_T,Var_T> this_t;
	DECLARE_CONCRETE_RTTLITE (this_t,VMonotypeMapBase);

    //  Aliases
    public:
	typedef Val_T val_t;
	typedef Var_T var_t;
	typedef VArray<Val_T,Var_T> array_t;

    //  Construction
    public:
	VMonotypeMap (
	    VSet *pDomain, VSet *pCodomain, array_t const &rCoValues
	) : BaseClass (pDomain, pCodomain) {
	}

    //  Destruction
    protected:
	~VMonotypeMap () {
	}

    //  State
    private:
	array_t m_aCoValues;
    };
}


#endif
