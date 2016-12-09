#ifndef Vxa_Interface
#define Vxa_Interface

/*********************
 *****  Linkage  *****
 *********************/

#ifdef VXA_EXPORTS
#define Vxa_API DECLSPEC_DLLEXPORT

#else
#define Vxa_API DECLSPEC_DLLIMPORT

#ifdef _WIN32
#pragma comment(lib, "vxa.lib")
#endif

#endif

#define Vxa_NAMESPACE_ENTER namespace Vxa {
#define Vxa_NAMESPACE_EXIT }

#define DECLARE_VxaINTERFACE(ifName) DECLARE_API_VINTERFACE(Vxa,ifName)
#define VxaINTERFACE(ifName,ifBase) VINTERFACE_IN_API(ifName,ifBase,Vxa)

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca.h"


/*******************************
 *****  The Vxa Namespace  *****
 *******************************/

namespace Vxa {
    typedef Vca::U32 cardinality32_t;
    typedef Vca::U64 cardinality64_t;

    typedef cardinality32_t cardinality_t;

    typedef unsigned int     object_reference_t;
    typedef VkDynamicArrayOf<object_reference_t> object_reference_array_t;

    class Unused {};
}


#endif
