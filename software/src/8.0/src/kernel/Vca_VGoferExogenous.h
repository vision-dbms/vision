#ifndef Vca_VGoferExogenous_Interface
#define Vca_VGoferExogenous_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VGoferInstance.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    template <typename Val_T, typename Base_T = VGoferInstance<Val_T> > class VGoferExogenous : public Base_T {
	typedef BOGUS_TYPENAME VGoferExogenous<Val_T,Base_T> this_t;
	DECLARE_CONCRETE_RTTLITE (this_t, Base_T);

    //  Construction
    public:
	VGoferExogenous (Val_T iValue) : BaseClass (iValue) {
	}
	VGoferExogenous () {
	}

    //  Destruction
    protected:
	~VGoferExogenous () {
	}

    //  Callbacks
    private:
	void onData () {
	}

    //  Update
    public:
	void setTo (Val_T iValue) {
	    BaseClass::setTo (iValue);
	}
	void setTo (IError *pIError, VString const &rMessage) {
	    BaseClass::setTo (pIError, rMessage);
	}
    };


/*********************
 *----  Aliases  ----*
 *********************/

    typedef VGoferExogenous<bool> VGoferExogenous_bool;

    typedef VGoferExogenous<F32>	VGoferExogenous_F32;
    typedef VGoferExogenous<F64>	VGoferExogenous_F64;

    typedef VGoferExogenous<S08>	VGoferExogenous_S08;
    typedef VGoferExogenous<S16>	VGoferExogenous_S16;
    typedef VGoferExogenous<S32>	VGoferExogenous_S32;

    typedef VGoferExogenous<U08>	VGoferExogenous_U08;
    typedef VGoferExogenous<U16>	VGoferExogenous_U16;
    typedef VGoferExogenous<U32>	VGoferExogenous_U32;

    typedef VGoferExogenous<VString const&> VGoferExogenous_String;
}


#endif
