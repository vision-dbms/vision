#ifndef Vxa_VArray_Interface
#define Vxa_VArray_Interface

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VTypePattern.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    template <
	typename Val_T, typename Var_T = typename Vca::VTypePattern<Val_T>::var_t
    > class VArray {
    public:
	typedef VArray<Val_T,Var_T> this_t;
	DECLARE_FAMILY_MEMBERS (this_t,void);

    //  Aliases
    public:
	typedef Val_T val_t;
	typedef Var_T var_t;

    //  Construction
    public:
	VArray (this_t const &rValues) {
	}

    //  Destruction
    public:
	~VArray () {
	}

    //  State
    private:
    };
}


#endif
