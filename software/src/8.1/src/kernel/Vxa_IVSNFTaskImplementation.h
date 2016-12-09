#ifndef Vxa_IVSNFTaskImplementation_Interface
#define Vxa_IVSNFTaskImplementation_Interface

#ifndef Vxa_IVSNFTaskImplementation
#define Vxa_IVSNFTaskImplementation extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vxa.h"

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VkDynamicArrayOf.h"

#include "Vxa_ISingleton.h"

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vxa,IVSNFTaskImplementation)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    typedef VkDynamicArrayOf<int> i32_array_t;
    typedef VkDynamicArrayOf<double> f64_array_t;
    typedef VkDynamicArrayOf<VString> str_array_t;
    typedef VkDynamicArrayOf<ISingleton::Reference> obj_array_t;

    VINTERFACE_ROLE (IVSNFTaskImplementation, IVUnknown)
	VINTERFACE_ROLE_2 (SetToInteger, unsigned int, i32_array_t const &);
	VINTERFACE_ROLE_2 (SetToDouble, unsigned int, f64_array_t const &);
	VINTERFACE_ROLE_2 (SetToVString, unsigned int, str_array_t const &);
	VINTERFACE_ROLE_2 (SetToObjects, unsigned int, obj_array_t const &);	
	VINTERFACE_ROLE_0 (PopulateVariantCompleted);	
    VINTERFACE_ROLE_END

    VxaINTERFACE (IVSNFTaskImplementation, IVUnknown);
	VINTERFACE_METHOD_2 (SetToInteger, unsigned int, i32_array_t const &);
	VINTERFACE_METHOD_2 (SetToDouble, unsigned int, f64_array_t const &);
	VINTERFACE_METHOD_2 (SetToVString, unsigned int, str_array_t const &);
	VINTERFACE_METHOD_2 (SetToObjects, unsigned int, obj_array_t const &);
	VINTERFACE_METHOD_0 (PopulateVariantCompleted);	
    VINTERFACE_END
}


#endif
