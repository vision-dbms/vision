#ifndef Vfx_Interface
#define Vfx_Interface

/*************************
 *****  Compilation  *****
 *************************/

#ifdef Vfx_EXPORTS
#define Vfx_API DECLSPEC_DLLEXPORT

#else
#define Vfx_API DECLSPEC_DLLIMPORT

#ifdef _WIN32
#pragma comment(lib, "vfx.lib")
#endif

#endif

#define Vfx_NAMESPACE_ENTER namespace Vfx {
#define Vfx_NAMESPACE_EXIT }

#define DECLARE_VfxINTERFACE(ifName) DECLARE_API_VINTERFACE(Vfx,ifName)


/***********************
 *****  Namespace  *****
 ***********************/

namespace Vfx {
}


#endif
