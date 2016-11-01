/**
 * @file
 *
 * This file introduces the Vca_Main namespace.
 */


#ifndef Vca_Main_Interface
#define Vca_Main_Interface
#define Vca_Main_Interface_Touch 1

/*********************
 *****  Linkage  *****
 *********************/

#ifdef VCAMAIN_EXPORTS
#define Vca_Main_API DECLSPEC_DLLEXPORT

#else
#define Vca_Main_API DECLSPEC_DLLIMPORT

#ifdef _WIN32
#pragma comment(lib, "vcamain.lib")
#endif

#endif

#define Vca_Main_NAMESPACE_ENTER namespace Vca {
#define Vca_Main_NAMESPACE_EXIT }

/**
 * Defines a Vca_Main Interface, for use when defining interfaces that belong to the Vca_Main library. Should be followed by a series of <code>VINTERFACE_METHOD_*</code> lines and finally a VINTERFACE_END.
 * Handles inclusion of appropriate platform-specific import/export tags for use of interfaces inside and outside of the Vca_Main library.
 *
 * @param ifName the interface name.
 * @param ifBase the interface's base's name.
 *
 * @see VINTERFACE_IN_API
 */
#define VcaMainINTERFACE(ifName,ifBase) VINTERFACE_IN_API(ifName,ifBase,Vca_Main)
#define VcaMainINTERFACE_NEST(ifName,ifBase) VINTERFACE_NEST_IN_API(ifName,ifBase,Vca_Main)

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca.h"


#endif
