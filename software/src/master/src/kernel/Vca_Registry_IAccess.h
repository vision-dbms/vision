#ifndef Vca_Registry_IAccess_Interface
#define Vca_Registry_IAccess_Interface

#ifndef Vca_Registry_IAccess
#define Vca_Registry_IAccess extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_Registry.h"

#include "Vca_Namespace_VTraits.h"

VINTERFACE_TEMPLATE_INSTANTIATIONS_IN_API (Vca_Registry,Vca::Registry,IAccess)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace Registry {
	/**
	 * @class IAccess
	 *
	 * An interface for obtaining an interface to a registered object.
	 */
	Vca_Registry_INTERFACE (IAccess, IVUnknown);
	    /**
	     * Obtain an interface from a registered object.
	     *
	     * @param p1[out]  A pointer to the IObjectSink that should receive the
	     *                 requested interface.  Convention has it that NULL
	     *                 is returned if the interface is not supported.
	     * @param p1[in]   The uuid identifying the registered object to be
	     *                 queried.
	     * @param p2[in]   A pointer to a VTypeInfo for the desired interface
	     *                 type.
	     */
	    VINTERFACE_METHOD_3 (GetObject,IObjectSink*,uuid_t const&,VTypeInfo*);
	VINTERFACE_END

	VINTERFACE_ROLE (IAccess, IVUnknown)
	    VINTERFACE_ROLE_3 (GetObject,IObjectSink*,uuid_t const&,VTypeInfo*);
	VINTERFACE_ROLE_END
    } // namespace Registry

    namespace Namespace {
	/**
	 *  Traits describing a namespace indexed by UUIDs and implemented by an interface overload of Vca::Gofer::Supply.
	 */
	template <> struct VTraits<Registry::IAccess> {
	    typedef Registry::IAccess interface_t;
	    typedef uuid_t const &name_t;
	};
    } // namespace Namespace
} // namespace Vca


#endif
