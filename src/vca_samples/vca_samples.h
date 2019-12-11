#ifndef VcaSamples_Interface
#define VcaSamples_Interface

/*********************
 *****  Linkage  *****
 *********************/

#ifdef VCA_SAMPLES_EXPORTS
#define VcaSamples_API DECLSPEC_DLLEXPORT

#else
#define VcaSamples_API DECLSPEC_DLLIMPORT

#ifdef _WIN32
#pragma comment(lib, "vca_samples.lib")
#endif

#endif

#define VcaSamples_NAMESPACE_ENTER namespace VcaSamples {
#define VcaSamples_NAMESPACE_EXIT }

#define DECLARE_VcaSamplesINTERFACE(ifName) DECLARE_API_VINTERFACE(VcaSamples,ifName)
#define VcaSamplesINTERFACE(ifName,ifBase) VINTERFACE_IN_API(ifName,ifBase,VcaSamples)


/**************************
 *****  Declarations  *****
 **************************/

namespace V {
    class VString;
}

namespace VcaSamples {
    typedef V::VString VString;
}


#endif
