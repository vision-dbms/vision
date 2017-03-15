#ifndef Vxa_IVSNFTaskHolder_Interface
#define Vxa_IVSNFTaskHolder_Interface

#ifndef Vxa_IVSNFTaskHolder
#define Vxa_IVSNFTaskHolder extern
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

DECLARE_VxaINTERFACE (IVSNFTaskImplementation)

#ifndef Vxa_IVSNFTaskHolder_Templates_Instantiated
#define Vxa_IVSNFTaskHolder_Templates_Instantiated
VINTERFACE_TEMPLATE_INSTANTIATIONS (Vxa,IVSNFTaskHolder)
#endif

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    VINTERFACE_ROLE (IVSNFTaskHolder, IVUnknown)
	VINTERFACE_ROLE_1 (SetOutput, VkDynamicArrayOf<VString> const &);
	VINTERFACE_ROLE_1 (ReturnString, char const *);
	VINTERFACE_ROLE_1 (ReturnInteger, int);
	VINTERFACE_ROLE_1 (GetParameter, Vca::U32);
	VINTERFACE_ROLE_0 (TurnBackSNFTask);
	VINTERFACE_ROLE_2 (ReturnImplementationHandle, IVSNFTaskImplementation *, VkDynamicArrayOf<ISingleton::Reference> const &);
	VINTERFACE_ROLE_1 (ReturnIntegerArray, VkDynamicArrayOf<int> const &);
	VINTERFACE_ROLE_1 (ReturnDoubleArray, VkDynamicArrayOf<double> const &);
	VINTERFACE_ROLE_1 (ReturnStringArray, VkDynamicArrayOf<VString> const &);
	VINTERFACE_ROLE_1 (ReturnObjectArray, VkDynamicArrayOf<ISingleton::Reference> const &);
	VINTERFACE_ROLE_1 (ReturnFloatArray, VkDynamicArrayOf<float> const &);
	VINTERFACE_ROLE_1 (ReturnBooleanArray, VkDynamicArrayOf<bool> const &);
    VINTERFACE_ROLE_END

    VxaINTERFACE (IVSNFTaskHolder, IVUnknown);
	VINTERFACE_METHOD_1 (SetOutput, VkDynamicArrayOf<VString> const &);
	VINTERFACE_METHOD_1 (ReturnString, char const*);
	VINTERFACE_METHOD_1 (ReturnInteger, int);
	VINTERFACE_METHOD_1 (GetParameter, Vca::U32);
	VINTERFACE_METHOD_0 (TurnBackSNFTask);
	VINTERFACE_METHOD_2 (ReturnImplementationHandle, IVSNFTaskImplementation *, VkDynamicArrayOf<ISingleton::Reference > const &);
	VINTERFACE_METHOD_1 (ReturnIntegerArray, VkDynamicArrayOf<int> const &);
	VINTERFACE_METHOD_1 (ReturnDoubleArray, VkDynamicArrayOf<double> const &);
	VINTERFACE_METHOD_1 (ReturnStringArray, VkDynamicArrayOf<VString> const &);
	VINTERFACE_METHOD_1 (ReturnObjectArray, VkDynamicArrayOf<ISingleton::Reference> const &);
	VINTERFACE_METHOD_1 (ReturnFloatArray, VkDynamicArrayOf<float> const &);
	VINTERFACE_METHOD_1 (ReturnBooleanArray, VkDynamicArrayOf<bool> const &);
    VINTERFACE_END
}

#endif
