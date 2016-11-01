/**
 * @file
 *
 * This file introduces the Vca_Main namespace.
 */


#ifndef Vxa_Main_Interface
#define Vxa_Main_Interface
#define Vxa_Main_Interface_Touch 1

/*********************
 *****  Linkage  *****
 *********************/

#ifdef VXAMAIN_EXPORTS
#define Vxa_Main_API DECLSPEC_DLLEXPORT

#else
#define Vxa_Main_API DECLSPEC_DLLIMPORT

#ifdef _WIN32
#pragma comment(lib, "vcamain.lib")
#endif

#endif

#define Vxa_Main_NAMESPACE_ENTER namespace Vxa {
#define Vxa_Main_NAMESPACE_EXIT }

/**
 * Defines a Vxa_Main Interface, for use when defining interfaces that belong to the Vxa_Main library. Should be followed by a series of <code>VINTERFACE_METHOD_*</code> lines and finally a VINTERFACE_END.
 * Handles inclusion of appropriate platform-specific import/export tags for use of interfaces inside and outside of the Vxa_Main library.
 *
 * @param ifName the interface name.
 * @param ifBase the interface's base's name.
 *
 * @see VINTERFACE_IN_API
 */
#define VxaMainINTERFACE(ifName,ifBase) VINTERFACE_IN_API(ifName,ifBase,Vxa_Main)
#define VxaMainINTERFACE_NEST(ifName,ifBase) VINTERFACE_NEST_IN_API(ifName,ifBase,Vxa_Main)

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa.h"


#endif
