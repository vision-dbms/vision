#ifndef Vca_VInstanceOfInterface_Interface
#define Vca_VInstanceOfInterface_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VInstanceOf.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    template <class Interface_T> class VInstanceOfInterface : public VInstanceOf<Interface_T*> {
	DECLARE_FAMILY_MEMBERS (VInstanceOfInterface<Interface_T>, VInstanceOf<Interface_T*>);

    //  Aliases
    public:
	typedef Interface_T interface_t;

    //  Construction
    public:
	template <typename value_t> VInstanceOfInterface (value_t iValue) : BaseClass (iValue) {
	}
	VInstanceOfInterface (ThisClass const &rOther) : BaseClass (rOther) {
	}

    //  Destruction
    public:
	~VInstanceOfInterface () {
	}

    //  Access
    public:
	interface_t* operator-> () const {
	    return BaseClass::value ();
	}
	interface_t& operator* () const {
	    return *BaseClass::value ();
	}

    //  Update
    public:
	template <typename value_t> ThisClass& operator= (value_t iValue) {
	    BaseClass::setTo (iValue);
	    return *this;
	}
    };
}


#endif
