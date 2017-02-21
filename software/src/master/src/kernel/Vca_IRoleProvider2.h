#ifndef Vca_IRoleProvider2_Interface
#define Vca_IRoleProvider2_Interface

#ifndef Vca_IRoleProvider2
#define Vca_IRoleProvider2 extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vca_IRoleProvider.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IRoleProvider2)

namespace Vca {
    VINTERFACE_ROLE (IRoleProvider2, IRoleProvider)
        VINTERFACE_ROLE_4 (QueryRole2, IVReceiver<bool>*, IVReceiver<IVUnknown*>*, VTypeInfo*, IVUnknown*);
    VINTERFACE_ROLE_END

    /**
     * @class IRoleProvider2
     *
     * Used to inherit (aggregate) interfaces from remote objects, this interface provides
     * an alternative implementation of QueryRole with better failure detection and recovery.
     * In particular, the protocol associated with this interface 'requires' the sending of
     * both positive (success) and negative (not found) acknowledgements to its consumers.
     * In contrast, IRoleProvider::QueryRole only requires the generation of the negative
     * acknowledgements needed to continue the interface search.
     *
     * Vca::IRoleProvider2 is currently implemented and used by Vca::VRolePlayer as part of
     * its interface aggregation support.
     *
     * @todo Migrate appropriate parts of this documentation to a page.
     */
    VcaINTERFACE (IRoleProvider2, IRoleProvider);
        /**
         * Queries this interface's implementor (typically a VRolePlayer) for a desired interface.
         *
         * @param p1                 A pointer to an IVReceiver<bool> whose 'OnData' member should
	 *                           be called with 'true' if the requested interface was found
	 *                           and 'false' if it wasn't.  In practice, implemented by an
	 *                           object associated with the query transaction.  Calling this
	 *                           interface's OnData with 'false' tells its implementor to
	 *                           continue looking elsewhere; calling it with 'true' gives
	 *                           the implementing transaction a chance to remove whatever
	 *                           monitoring it may have placed on this role provider.
         *
         * @param p2                 A pointer to an IVReceiver<IVUnknown*> that should be
         *                           fired (i.e., pInterfaceReceiver->OnData (pInterface))
         *                           if the interface is found.  Implemented by the client
         *                           that wants the interface.  If no other error recovery
         *                           machinery is negotiated by the implementor of this
         *                           interface, errors and exceptions should be sent to the
         *                           client using this interface's inherited 'OnError' so
         *                           that the client isn't left hanging.
         *
         * @param p3                 A pointer to a VTypeInfo identifying the desired
         *                           interface.
         *
         * @param p4                 A pointer to the IVUnknown aggregating the interface.
         *                           QueryInterface calls made using the interface returned
         *                           by this member must be delegated back to this aggregator
         *                           to prevent inconsistent and unpredictable results.  This
         *                           aggregator could also be notified of errors or other
         *                           exceptions encountered by the implementor of IRoleProvider2;
         *                           however, the protocol and interfaces for doing that are
         *                           are beyond the scope of the Vca::IRoleProvider2 interface.
         */
        VINTERFACE_METHOD_4 (QueryRole2, IVReceiver<bool>*, IVReceiver<IVUnknown*>*, VTypeInfo*, IVUnknown*);
    VINTERFACE_END
}

#endif
