#ifndef Vca_IBinding_Interface
#define Vca_IBinding_Interface

#ifndef Vca_IBinding
#define Vca_IBinding extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_IResource.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IBinding)

#ifndef Vca_INamespace
DECLARE_VcaINTERFACE (INamespace)
#endif

/**
 * @class Vca::IBinding
 *
 * Generic interface for bindings between a name within some namespace and some object.
 * Derived interfaces should probably provide some mechanism by which the bound object can be accessed, amongst other things.
 * @see Vca::IDirectoryEntry
 */
namespace Vca {
    VINTERFACE_ROLE (IBinding, IResource)
	VINTERFACE_ROLE_1 (GetNamespace, IVReceiver<INamespace*>*);
	VINTERFACE_ROLE_1 (GetName, IVReceiver<VString const&>*);
    VINTERFACE_ROLE_END

    VcaINTERFACE (IBinding, IResource);
        /**
         * Retrieves the namespace within which the name for this binding applies.
         *
         * @param p1 the receiver that should be used to return the namespace.
         */
	VINTERFACE_METHOD_1 (GetNamespace, IVReceiver<INamespace*>*);

        /**
         * Retrieves the name by which this binding is known.
         *
         * @param p1 the receiver that should be used to return the name.
         */
	VINTERFACE_METHOD_1 (GetName, IVReceiver<VString const&>*);
    VINTERFACE_END
}

#endif
