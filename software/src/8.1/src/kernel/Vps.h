#ifndef Vps_Interface
#define Vps_Interface

/*********************
 *****  Linkage  *****
 *********************/

#ifdef VPS_EXPORTS
#define Vps_API DECLSPEC_DLLEXPORT

#else
#define Vps_API DECLSPEC_DLLIMPORT

#ifdef _WIN32
#pragma comment(lib, "vps.lib")
#endif

#endif

#define Vps_NAMESPACE_ENTER\
    Vca_NAMESPACE_ENTER\
    namespace Vps {

#define Vps_NAMESPACE_EXIT\
    }\
    Vca_NAMESPACE_EXIT

#define DECLARE_VpsINTERFACE(ifName) DECLARE_API_VINTERFACE(Vps,ifName)
#define VpsINTERFACE(ifName,ifBase) VINTERFACE_IN_API(ifName,ifBase,Vps)
#define VpsINTERFACE_NEST(ifName,ifBase) VINTERFACE_NEST_IN_API(ifName,ifBase,Vps)

/*----------------------------------------------------------------------------*
 *  This file introduces the Vps namespace and declares the public procedural
 *  interface to the Vision Publish Subscribe toolkit.  The Vps toolkit
 *  contains the definitions of the interfaces used and supported by vision
 *  to provide publish subscribe mechanism.
 *----------------------------------------------------------------------------*/

/*******************************
 *****  The Vps Namespace  *****
 *******************************/

namespace Vca {
    namespace Vps {
    }
}


#endif
