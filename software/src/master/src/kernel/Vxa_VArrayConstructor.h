#ifndef Vxa_VArrayConstructor_Interface
#define Vxa_VArrayConstructor_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VArray.h"

#include "Vxa_VCursor.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    template <
	typename Val_T, typename Var_T = typename Vca::VTypePattern<Val_T>::var_t
    > class VArrayConstructor : public VRolePlayer {
    public:
	typedef VArrayConstructor<Val_T,Var_T> this_t;
	DECLARE_CONCRETE_RTTLITE(this_t,VRolePlayer);

    //  Aliases
    public:
	typedef VArray<Val_T,Var_T> array_t;

	typedef Val_T val_t;
	typedef Var_T var_t;

    //  Construction
    public:
	VArrayConstructor (VCursor *pCursor) : m_pCursor (pCursor) {
	}

    //  Destruction
    protected:
	~VArrayConstructor () {
	}

    //  Update
    public:
	void setCurrentElementTo (val_t iValue) {
	}

    //  State
    private:
	VCursor::Reference m_pCursor;
    };
}


#endif
