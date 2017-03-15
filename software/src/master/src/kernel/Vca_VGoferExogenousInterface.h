#ifndef Vca_VGoferExogenousInterface_Interface
#define Vca_VGoferExogenousInterface_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VGoferExogenous.h"
#include "Vca_VGoferInterface.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    template <typename Interface_T> class VGoferExogenousInterface : public VGoferExogenous<Interface_T*,VGoferInterface<Interface_T> > {
	typedef BOGUS_TYPENAME VGoferExogenous<Interface_T*,VGoferInterface<Interface_T> > base_t;
	DECLARE_CONCRETE_RTTLITE (VGoferExogenousInterface<Interface_T>,base_t);

    //  Construction
    public:
	VGoferExogenousInterface (Interface_T *pInterface) : BaseClass (pInterface) {
	}
	VGoferExogenousInterface () {
	}

    //  Destruction
    protected:
	~VGoferExogenousInterface () {
	}

    };
}


#endif
