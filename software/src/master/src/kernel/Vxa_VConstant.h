#ifndef Vxa_VConstant_Interface
#define Vxa_VConstant_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VMethod.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VTypePattern.h"

#include "Vxa_VCallType1.h"
#include "Vxa_VCallType2.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    template <
	typename Val_T, typename Var_T = typename Vca::VTypePattern<Val_T>::var_t
    > class VConstant : public VMethod {
	typedef VConstant<Val_T,Var_T> this_t;
	DECLARE_CONCRETE_RTTLITE (this_t, VMethod);

    //  Aliases
    public:
	typedef Val_T val_t;
	typedef Var_T var_t;

    //  Construction
    public:
	VConstant (VString const &rName, Val_T iVal) : BaseClass (rName), m_iVar (iVal) {
	}

    //  Destruction
    private:
	~VConstant () {
	}

    //  Access
    public:
	val_t value () const {
	    return m_iVar;
	}

    //  Invocation
    private:
	virtual bool invoke (VCallType1 const &rCallHandle, cardinality_t cTask, VCollection *pCluster) OVERRIDE {
	    return rCallHandle.returnConstant (value ());
	}
	virtual bool invoke (VCallType2 const &rCallHandle, cardinality_t cTask, VCollection *pCluster) OVERRIDE {
	    return rCallHandle.returnConstant (value ());
	}

    //  State
    private:
	var_t const m_iVar;
    };
} //  namespace Vxa


#endif
