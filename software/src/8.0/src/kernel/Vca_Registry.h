#ifndef Vca_Registry_Interface
#define Vca_Registry_Interface

/*********************
 *****  Linkage  *****
 *********************/

#ifdef VCA_EXPORTS
#define Vca_Registry_API DECLSPEC_DLLEXPORT

#else
#define Vca_Registry_API DECLSPEC_DLLIMPORT
#endif

#define Vca_Registry_NAMESPACE_ENTER\
    Vca_NAMESPACE_ENTER\
    namespace Registry {

#define Vca_Registry_NAMESPACE_EXIT\
    }\
    Vca_NAMESPACE_EXIT

#define DECLARE_Vca_Registry_INTERFACE(ifName) DECLARE_API_VINTERFACE(Vca_Registry,ifName)

/**
 * Defines a Vca Interface, for use when defining interfaces that belong to the Vca library. Should be followed by a series of <code>VINTERFACE_METHOD_*</code> lines and finally a VINTERFACE_END.
 * Handles inclusion of appropriate platform-specific import/export tags for use of interfaces inside and outside of the Vca library.
 *
 * @param ifName the interface name.
 * @param ifBase the interface's base's name.
 *
 * @see VINTERFACE_IN_API
 */
#define Vca_Registry_INTERFACE(ifName,ifBase) VINTERFACE_IN_API(ifName,ifBase,Vca_Registry)
#define Vca_Registry_INTERFACE_NEST(ifName,ifBase) VINTERFACE_NEST_IN_API(ifName,ifBase,Vca_Registry)

/***********************
 *****  Namespace  *****
 ***********************/

namespace Vca {
    namespace Registry {

	class VAuthority;

    } // namespace Registry
} // namespace Vca


#endif
