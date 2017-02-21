#ifndef Vca_IRoleProvider_Interface
#define Vca_IRoleProvider_Interface

#ifndef Vca_IRoleProvider
#define Vca_IRoleProvider extern
#endif

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

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IRoleProvider)

#ifndef Vca_ITrigger
DECLARE_VcaINTERFACE (ITrigger)
#endif

namespace Vca {
    VINTERFACE_ROLE (IRoleProvider, IVUnknown)
        VINTERFACE_ROLE_4 (QueryRole, ITrigger*, IVReceiver<IVUnknown*>*, VTypeInfo*, IVUnknown*);
    VINTERFACE_ROLE_END

    /**
     * @class IRoleProvider
     *
     * Used to inherit (aggregate) interfaces from remote objects.
     *
     * Any Vca interface can be asked for any other interface supported by its implementor.
     * Implementors that wish to support this type of polymorphism do so by providing an
     * appropriate implementation of IVUnknown::QueryInterface.
     *
     * The easiest way to support interfaces in Vca is to derive a class, directly or indirectly
     * from Vca::VRolePlayer.  Vca::VRolePlayer provides machinery for declaring and implementing
     * interfaces coupled with a standard implementation of QueryInterface that can return those
     * interfaces.  It also provides machinery for offering the interfaces implemented elsewhere
     * as though they were implemented directly by the VRolePlayer derived class.  This form of
     * 'delegation' based polymorphism is also called 'aggregation', after the similar notion
     * implemented in Microsoft's COM technology (without the restrictions, of course :-)
     *
     * Interfaces 'inherited' from other implementors different from other interfaces in one
     * significant respect.  They must appear to be interfaces implemented by their aggregator,
     * not their implementor.  In practical terms, that means they must delegate QueryInterface
     * requests back to the aggregator, not the implementor, of the interface.  Failure to do
     * so would have the undesirable effect of producing different answers to otherwise
     * identical queries.
     *
     * The details discussed above are essentially implementation details.  If interface
     * aggregation were limited to objects implemented in the same process as the aggregator,
     * there would be no need for an interface.  Remote, out of process, interfaces can also
     * be aggregated, however.  Interfaces from those objects must be obtained asynchronously
     * using an interface.  Vca::IRoleProvider is that interface.
     *
     * Vca::IRoleProvider is currently implemented and used by Vca::VRolePlayer so that all
     * classes derived from Vca::VRolePlayer can aggregate and be aggregated by other VRolePlayer
     * derived classes.
     *
     * @todo Migrate appropriate parts of this documentation to a page.
     */
    VcaINTERFACE (IRoleProvider, IVUnknown);
        /**
         * Queries this interface's implementor (typically a VRolePlayer) for a desired interface.
         *
         * @param p1                 A pointer to an ITrigger that should be fired (i.e,
         *                           pNotFoundTrigger->Process()) if the requested interface
         *                           is not found.  In practice, implemented by an object
         *                           associated with the query transaction.  Firing this
         *                           trigger tells the query to continue looking elsewhere.
         *                           If the requested interface is found, abandoning this
         *                           pointer will garbage collect the transaction.
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
         *                           exceptions encountered by the implementor of IRoleProvider;
         *                           however, the protocol and interfaces for doing that are
         *                           are beyond the scope of the Vca::IRoleProvider interface.
         */
        VINTERFACE_METHOD_4 (QueryRole, ITrigger*, IVReceiver<IVUnknown*>*, VTypeInfo*, IVUnknown*);
    VINTERFACE_END
}

#endif
