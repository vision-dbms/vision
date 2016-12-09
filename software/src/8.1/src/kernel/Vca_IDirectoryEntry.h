#ifndef Vca_IDirectoryEntry_Interface
#define Vca_IDirectoryEntry_Interface

#ifndef Vca_IDirectoryEntry
#define Vca_IDirectoryEntry extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_IBinding.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

/**
 * @class Vca::IDirectoryEntry
 *
 * Interface for entries within an IDirectory implementation.
 */

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IDirectoryEntry)

namespace Vca {
    VINTERFACE_ROLE (IDirectoryEntry, IBinding)
	VINTERFACE_ROLE_2 (GetObject, IVReceiver<IVUnknown*>*, VTypeInfo*);
	VINTERFACE_ROLE_1 (SetObject, IVUnknown*);
	VINTERFACE_ROLE_1 (SetSource, IDataSource<IVUnknown*>*);
	VINTERFACE_ROLE_1 (SetDescription, VString const &);
	VINTERFACE_ROLE_1 (GetDescription, IVReceiver<VString const &>*);	
    VINTERFACE_ROLE_END

    VcaINTERFACE (IDirectoryEntry, IBinding);
        /**
         * Queries the object at this entry for a particular interface type.
         *
         * @param p1 the receiver that the retirieved object should be sent to.
         * @param p2 the type information of the interface that should be queried for.
         */
	VINTERFACE_METHOD_2 (GetObject, IVReceiver<IVUnknown*>*, VTypeInfo*);

        /**
         * Sets this entry's object. Clears the object if one is set.
         *
         * @param p1 the new value for this entry.
         */
	VINTERFACE_METHOD_1 (SetObject, IVUnknown*);

        /**
         * Sets this entry's source. Clears the object if one is set.
         *
         * @param p1 the new source for this entry.
         */
	VINTERFACE_METHOD_1 (SetSource, IDataSource<IVUnknown*>*);

        /**
         * Sets this entry's description.
         *
         * @param p1 the new description for this entry.
         */
	VINTERFACE_METHOD_1 (SetDescription, VString const &);

        /**
         * Used to retrieve the descriptive information for this entry. The descriptive information is purely intended for human readability/usability.
         *
         * @param p1 the receiver that the retirieved object should be sent to.
         */
	VINTERFACE_METHOD_1 (GetDescription, IVReceiver<VString const &>*);	
    VINTERFACE_END
}

#endif
