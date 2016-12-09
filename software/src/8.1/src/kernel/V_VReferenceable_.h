#ifndef V_VReferenceable__Interface
#define V_VReferenceable__Interface

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    template<typename T> class VReferenceable_ : public VReferenceable, public T {
	DECLARE_CONCRETE_RTT (VReferenceable_<T>, VReferenceable);

    //  new/delete
    public:
	void *operator new (size_t sObject) {
	    return BaseClass::operator new (sObject);
	}
    // not protected: See comment in 'VReferenceable.h'
	void operator delete (void *pObject, size_t sObject) {
	}

    //  Construction
    public:
	VReferenceable_(T const &rT) : T (rT) {
	}
	VReferenceable_() {
	}

    //  Destruction
    protected:
	~VReferenceable_() {
	}
    };
}

/****************************
 *****  Implementation  *****
 ****************************/

template<typename T> typename DEFINE_CONCRETE_RTT (V::VReferenceable_<T>);


#endif
