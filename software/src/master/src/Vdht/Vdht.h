#ifndef Vdht_Interface
#define Vdht_Interface

/*********************
 *****  Linkage  *****
 *********************/

#ifdef VDHT_EXPORTS
#define Vdht_API DECLSPEC_DLLEXPORT

#else
#define Vdht_API DECLSPEC_DLLIMPORT

#ifdef _WIN32
#pragma comment(lib, "vdht.lib")
#endif

#endif

#define DECLARE_VdhtINTERFACE(ifName) DECLARE_API_VINTERFACE(Vdht,ifName)
#define VdhtINTERFACE(ifName,ifBase) VINTERFACE_IN_API(ifName,ifBase,Vdht)


/**************************
 *****  Declarations  *****
 **************************/

namespace Vdht {
}


#endif
