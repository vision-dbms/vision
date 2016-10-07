#ifndef Vca_MetaData_Interface
#define Vca_MetaData

/*********************
 *****  Linkage  *****
 *********************/

#ifdef VCA_EXPORTS
#define Vca_MetaData_API DECLSPEC_DLLEXPORT

#else
#define Vca_MetaData_API DECLSPEC_DLLIMPORT
#endif

#define DECLARE_Vca_MetaData_INTERFACE(ifName) DECLARE_API_VINTERFACE(Vca_MetaData,ifName)

/**
 * Defines a Vca Interface, for use when defining interfaces that belong to the Vca library. Should be followed by a series of <code>VINTERFACE_METHOD_*</code> lines and finally a VINTERFACE_END.
 * Handles inclusion of appropriate platform-specific import/export tags for use of interfaces inside and outside of the Vca library.
 *
 * @param ifName the interface name.
 * @param ifBase the interface's base's name.
 *
 * @see VINTERFACE_IN_API
 */
#define Vca_MetaData_INTERFACE(ifName,ifBase) VINTERFACE_IN_API(ifName,ifBase,Vca_MetaData)
#define Vca_MetaData_INTERFACE_NEST(ifName,ifBase) VINTERFACE_NEST_IN_API(ifName,ifBase,Vca_MetaData)


#endif
