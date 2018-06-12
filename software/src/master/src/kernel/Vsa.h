#ifndef Vsa_Interface
#define Vsa_Interface
 
/*************************
 *****  DLL Linkage  *****
 *************************/

#ifdef VSA_EXPORTS
#define Vsa_API DECLSPEC_DLLEXPORT

#else
#define Vsa_API DECLSPEC_DLLIMPORT

#ifdef _WIN32
#pragma comment(lib, "vsa.lib")
#endif

#endif

#define Vsa_NAMESPACE_ENTER namespace Vsa {
#define Vsa_NAMESPACE_EXIT }

#define DECLARE_VsaINTERFACE(ifName) DECLARE_API_VINTERFACE(Vsa,ifName)
#define VsaINTERFACE(ifName,ifBase) VINTERFACE_IN_API(ifName,ifBase,Vsa)


/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IDataSource.h"

/***********************
 *****  Namespace  *****
 ***********************/

namespace Vsa {
    typedef V::VString VString;

    typedef Vca::IObjectSource IObjectSource;
    typedef Vca::IObjectSink   IObjectSink;

    typedef ::IVUnknown IVUnknown;

    typedef Vca::U32 request_index_t;
}


#endif
