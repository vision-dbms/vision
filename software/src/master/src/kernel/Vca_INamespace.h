#ifndef Vca_INamespace_Interface
#define Vca_INamespace_Interface

#ifndef Vca_INamespace
#define Vca_INamespace extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

/**
 * @class Vca::INamespace
 *
 * Intended to be used with IBinding, this interface provides mechanisms to group names.
 * Derived interfaces should provide rules for defining the namespace.
 * @see Vca::IDirectory
 */

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,INamespace)

#ifndef Vca_IBinding
DECLARE_VcaINTERFACE (IBinding)
#endif

namespace Vca {
    VINTERFACE_ROLE (INamespace, IVUnknown)
	VINTERFACE_ROLE_2 (GetBinding, IVReceiver<IBinding*>*, VString const&);
    VINTERFACE_ROLE_END

    VcaINTERFACE (INamespace, IVUnknown);
        /**
	 * Return the binding associated with a name in a namespace.  Typical
	 * implementations of this method return specializations of IBinding
	 * appropriate to the implementation.
         *
         * @param p1 the receiver that should be used to return the binding.
         * @param p2 the name of the binding within this namespace that should be returned.
         */
	VINTERFACE_METHOD_2 (GetBinding, IVReceiver<IBinding*>*, VString const&);
    VINTERFACE_END
}

#endif
