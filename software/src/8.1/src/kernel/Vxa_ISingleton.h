#ifndef Vxa_ISingleton_Interface
#define Vxa_ISingleton_Interface

#ifndef Vxa_ISingleton
#define Vxa_ISingleton extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vxa.h"

/***********************
 *****  Components *****
 ***********************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

#ifndef Vxa_IVSNFTaskHolder
    DECLARE_VxaINTERFACE (IVSNFTaskHolder)
#elif !defined(Vxa_IVSNFTaskHolder_Templates_Instantiated)
#   define Vxa_IVSNFTaskHolder_Templates_Instantiated
    VINTERFACE_TEMPLATE_INSTANTIATIONS(Vxa,IVSNFTaskHolder)
#endif

/*************************
 *****  Definitions  *****
 *************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS_AT_LEVEL (Vxa,ISingleton,2)

namespace Vxa {

    VINTERFACE_ROLE (ISingleton, IVUnknown)
	VINTERFACE_ROLE_4 (ExternalImplementation, IVSNFTaskHolder*, VString const&, Vca::U32, Vca::U32);
    VINTERFACE_ROLE_END

    VxaINTERFACE (ISingleton, IVUnknown);
	VINTERFACE_METHOD_4 (ExternalImplementation, IVSNFTaskHolder*, VString const&, Vca::U32, Vca::U32);
    VINTERFACE_END
}

/****************************
 *****  Instantiations  *****
 ****************************/

#if defined(USING_HIDDEN_DEFAULT_VISIBILITY) || defined(Vxa_ISingleton_Instantiations)

#ifndef Vxa_ISingleton_Instantiations
#define Vxa_ISingleton_Instantiations extern
#endif

Vxa_ISingleton template class Vxa_API Vca::VTypeInfoHolder<VkDynamicArrayOf<Vxa::ISingleton::Reference> const &>;
Vxa_ISingleton template class Vxa_API Vca::VTypeInfoHolder<VkDynamicArrayOf<Vxa::ISingleton::Reference> >;

#endif


#endif
