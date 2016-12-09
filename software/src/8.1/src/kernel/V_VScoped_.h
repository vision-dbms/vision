#ifndef V_VScoped__Interface
#define V_VScoped__Interface

/************************
 *****  Components  *****
 ************************/

#include "V_VScopedResource.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    /**********************
     *----  VScoped_  ----*
     **********************/

    template <typename T>
    class VScoped_ : public VScopedResource_<VScoped_<T> >, public T {
	DECLARE_CONCRETE_RTTLITE (VScoped_<DerivedClass>, VScopedResource_<VScoped_<T> >);

    //  Construction
    protected:
	VScoped_(T const &rT) : T (rT) {
	}

    //  Destruction
    protected:
	~VScoped_() {
	}
    };
}


#endif
