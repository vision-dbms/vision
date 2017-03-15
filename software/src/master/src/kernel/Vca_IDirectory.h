#ifndef Vca_IDirectory_Interface
#define Vca_IDirectory_Interface

#ifndef Vca_IDirectory
#define Vca_IDirectory extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_INamespace.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IDirectoryEntry.h"

/*************************
 *****  Definitions  *****
 *************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IDirectory)

/**
 * @class Vca::IDirectory
 *
 * Interface for a map data structure of strings to interfaces. Interfaces may or may not be materialized.
 * @todo Extend this interface, providing GetDescriptions and other query methods.
 */

namespace Vca {
    VINTERFACE_ROLE (IDirectory, INamespace)
        VINTERFACE_ROLE_3 (
            GetObject, IVReceiver<IVUnknown*>*, VString const&, VTypeInfo*
        );
        VINTERFACE_ROLE_3 (
            SetObject, IVReceiver<IDirectoryEntry*>*, VString const&, IVUnknown*
        );
        VINTERFACE_ROLE_3 (
            SetSource,
            IVReceiver<IDirectoryEntry*>*, VString const&, IDataSource<IVUnknown*>*
        );
        VINTERFACE_ROLE_3 (
            SetDescription, IVReceiver<IDirectoryEntry*>*, VString const&, VString const&
        );
        VINTERFACE_ROLE_2 (
            GetDescription, IVReceiver<VString const &>*, VString const&
        );
        VINTERFACE_ROLE_1 (
            GetEntries, IVReceiver<VkDynamicArrayOf <VString> const & >*
        );
    VINTERFACE_ROLE_END

    VcaINTERFACE (IDirectory, INamespace);
        /**
         * Retrieve the object associated with a directory name.  Implementations
         * of this interface should return the same object from this method as is
         * returned from the GetObject method applied to the IDirectoryEntry
         * associated with this name.
         *
         * @param p1 the receiver that the retirieved object should be sent to.
         * @param p2 the name of the object being retrieved.
         * @param p3 the type information describing the object being retrieved.
         *
         * @see Vca::IDirectoryEntry::GetObject()
         */
        VINTERFACE_METHOD_3 (
            GetObject, IVReceiver<IVUnknown*>*, VString const&, VTypeInfo*
        );

        /**
         * Associate an object with a name in a directory.  If this operation is
         * successful, the directory entry that binds the name to the object is
         * returned to the client's receiver.
         *
         * @param p1 the receiver that should be used to return the newly created directory entry.
         * @param p2 the name of the object being set.
         * @param p3 the object being set.
         *
         * @see Vca::IDirectoryEntry::SetObject()
         */
        VINTERFACE_METHOD_3 (
            SetObject, IVReceiver<IDirectoryEntry*>*, VString const&, IVUnknown*
        );

        /**
         * Associate data source for an object with a name in a directory.  If this operation is
         * successful, the directory entry that binds the name to the object is
         * returned to the client's receiver.
         *
         * @param p1 the receiver that should be used to return the newly created directory entry.
         * @param p2 the name of the object being set.
         * @param p3 the data source for the object being set.
         *
         * @see Vca::IDirectoryEntry::SetSource()
         */
        VINTERFACE_METHOD_3 (
            SetSource,
            IVReceiver<IDirectoryEntry*>*, VString const&, IDataSource<IVUnknown*>*
        );

        /**
         * Sets descriptive information for a name in a directory. This descriptive information is purely intended for human readability/usability.
         *
         * @param p1 the receiver that should be used to return the newly created directory entry.
         * @param p2 the name of the object being set.
         * @param p3 the description of the object being set.
         *
         * @see Vca::IDirectoryEntry::SetDescription()
         */
        VINTERFACE_METHOD_3 (
            SetDescription, IVReceiver<IDirectoryEntry*>*, VString const&, VString const&
        );

        /**
         * Used to retrieve descriptive information for a name in a directory. This descriptive information is purely intended for human readability/usability.
         *
         * @param p1 the receiver that should be used to return the descriptive information.
         * @param p2 the name of the object whose descriptive information should be retrieved.
         *
         * @see Vca::IDirectoryEntry::GetDescription()
         */
        VINTERFACE_METHOD_2 (
            GetDescription, IVReceiver<VString const &>*, VString const&
        );

        /**
         * Used to retrieve the list of entry names in a directory.
         *
         * @param p1 the receiver that should be used to return the list of entry names in this directory.
         */
        VINTERFACE_METHOD_1 (
            GetEntries, IVReceiver<VkDynamicArrayOf <VString> const & >*
        );
    VINTERFACE_END
}

#endif
