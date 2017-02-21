#ifndef Vca_VDirectory_Interface
#define Vca_VDirectory_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_VGoferInterface.h"

#include "Vca_IDirectory.h"
#include "Vca_IDirectoryEntry.h"

#include "VkMapOf.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VInterfaceQuery;

    /**
     * Generic string-to-interface map.
     */
    class Vca_API VDirectory : public VRolePlayer {
        DECLARE_CONCRETE_RTTLITE (VDirectory, VRolePlayer);

    //  Aliases
    public:
        /** Receiver for IBinding instances. */
        typedef IVReceiver<IBinding*>           BindingSink;
        /** Receiver for IDirectoryEntry instances. */
        typedef IVReceiver<IDirectoryEntry*>    EntrySink;
        /** In this class we use the term 'Object' as a generic for any interface. */
        typedef IVUnknown                       Object;
        /** Receiver for 'Objects' (interfaces). */
        typedef IVReceiver<IVUnknown*>          ObjectSink;
        /** Source for 'Objects' (interfaces). */
        typedef IDataSource<IVUnknown*>         ObjectSource;
        /** Gofer for 'Objects' (interfaces). */
        typedef VGoferInterface<IVUnknown>      ObjectGofer;

    //  Entry
    public:
        /**
         * Represents a single directory entry (name-interface pair).
         *
         * Entries directly contain the name and description associated with the entry.
	 * Entries store their value by way of a gofer. A new gofer is created for each
	 * distinct object or object source (i.e, each call to 'setObject' or 'setSource').
	 * Doing so ensures that the correct value for the entry is returned when the entry's
	 * value or source is changed before a previous source has returned its value.
         *
         * Entries are not roleplayers themselves to avoid a garbage collection cycle.
	 * Rather, we keep a weak reference to a RolePlayer within each Entry, breaking that
	 * cycle and allowing garbage collection of the directory once all IDirectoryEntry
	 * references (to RolePlayer instances) are destroyed. The reason for this complicated
	 * structure is the requirement that IDirectoryEntry::BaseClass::GetNamespace hold a
	 * pointer to the directory. If Entry were to store that pointer, a strong reference
	 * cycle results in which Directories and Entries point to each other reciprocally.
	 *
         * @dot
         *   digraph {
         *   graph [ rankdir="LR" ];
         *   node [ shape="rectangle" ];
         *   Directory -> Entry;
         *   RolePlayer -> Directory;
         *   RolePlayer -> Entry;
         *   Entry -> RolePlayer [ style="dashed" ];
         *   }
         * @enddot
         */
        class Entry : public VReferenceable {
            DECLARE_CONCRETE_RTTLITE (Entry, VReferenceable);

        //  Friends
            friend class VDirectory;

        //  RolePlayer
        public:
            class RolePlayer;
            friend class RolePlayer;

        //  Construction
        private:
            /**
             * Constructor expects a name for this entry.
             *
             * @param rName the name of the new entry.
             */
            Entry (VString const &rName);

        //  Destruction
        private:
            /** Empty destructor restricts scope. */
            ~Entry ();

            /**
             * Removes a given RolePlayer from this Entry. Used in destruction.
             *
             * @param pRolePlayer the RolePlayer that should be removed from this Entry.
             */
            void detach (RolePlayer *pRolePlayer);

        //  Role Support
        private:
            void supply (BindingSink *pBindingSink, VDirectory *pDirectory);
            void supply (EntrySink *pEntrySink, VDirectory *pDirectory);
            void supply (IDirectoryEntry::Reference &rpRole, VDirectory *pDirectory);

        //  Access
        public:
            /** Returns the name of this entry. */
            VString const &name () const {
                return m_iName;
            }
            /** Implements Vca::IDirectoryEntry::GetDescription() */
            VString const &getDescription () const {
                return m_iDescription;
            }
	private:
            /** Implements Vca::IDirectoryEntry::GetObject(). */
            Vca_API void getObject (VInterfaceQuery const &rQuery);

        //  Update
        public:
            /**
             * Removes this entry from a directory.
             *
             * @param pDirectory the directory from which this entry should be removed.
             */
            Vca_API void deleteFrom (VDirectory *pDirectory);

            /** Implements Vca::IDirectoryEntry::SetObject(). */
            Vca_API void setObject (EntrySink *pClient, VDirectory *pDirectory, Object *pObject);
	    Vca_API void setObject (EntrySink *pClient, VDirectory *pDirectory, Object *pObject, VString const& rDescription);

            /** Implements Vca::IDirectoryEntry::SetSource(). */
            Vca_API void setSource (EntrySink *pClient, VDirectory *pDirectory, ObjectGofer *pGofer);
	    Vca_API void setSource (EntrySink *pClient, VDirectory *pDirectory, ObjectGofer *pGofer, VString const& rDescription);

            Vca_API void setSource (EntrySink *pClient, VDirectory *pDirectory, ObjectSource *pSource);
	    Vca_API void setSource (EntrySink *pClient, VDirectory *pDirectory, ObjectSource *pSource, VString const& rDescription);

            /** Implements Vca::IDirectoryEntry::SetDescription(). */
            Vca_API void setDescription (EntrySink *pClient, VDirectory *pDirectory, VString const &rDescription);

        //  State
        private:
            /** The name of this entry. */
            VString const               m_iName;
            /** The human-readable description of this entry. */
            VString                     m_iDescription;
            /** The generation of this entry. Used to handle concurrent reads/writes. */
	    ObjectGofer::Reference      m_pBinding;
            /** The RolePlayer instance for this entry. Used to provide the IDirectoryEntry interface. */
            V::VPointer<RolePlayer>     m_pRolePlayer;
        };
        friend class Entry;

    //  EntryKey
    public:
        /**
         * The key type for VDirectory; a VString subclass whose comparison operators are case-insensitive.
         */
        class EntryKey : public VString {
            DECLARE_FAMILY_MEMBERS (EntryKey, VString);

        //  Construction
        public:
            /** Copy constructor. */
            EntryKey (VString const &rName) : BaseClass (rName) {
            }
            /** Empty default constructor. */
            EntryKey () {
            }

        //  Destruction
        public:
            /** Empty destructor. */
            ~EntryKey () {
            }

        // Update
        public:
            ThisClass &operator= (ThisClass const &rOther) {
                setTo (rOther);
                return *this;
            }
            ThisClass &operator= (VString const &rOther) {
                setTo (rOther);
                return *this;
            }
            ThisClass &operator= (char const *pOther) {
                setTo (pOther);
                return *this;
            }

        //  Comparison
        public:
            /** Case-insensitive comparison operator. */
            bool operator<  (char const *pOther) const {
                return strcasecmp (*this, pOther) <  0;
            }
            /** Case-insensitive comparison operator. */
            bool operator<= (char const *pOther) const {
                return strcasecmp (*this, pOther) <= 0;
            }
            /** Case-insensitive comparison operator. */
            bool operator== (char const *pOther) const {
                return strcasecmp (*this, pOther) == 0;
            }
            /** Case-insensitive comparison operator. */
            bool operator!= (char const *pOther) const {
                return strcasecmp (*this, pOther) != 0;
            }
            /** Case-insensitive comparison operator. */
            bool operator>= (char const *pOther) const {
                return strcasecmp (*this, pOther) >= 0;
            }
            /** Case-insensitive comparison operator. */
            bool operator>  (char const *pOther) const {
                return strcasecmp (*this, pOther) >  0;
            }
        };

    //  EntryMap
    public:
        /** Map data structure type. */
        typedef VkMapOf<
            EntryKey, VString const&, VString const&, Entry::Reference
        > EntryMap;

    //  Construction
    public:
        /** Constructor initializes roles. */
        VDirectory ();

    //  Destruction
    protected:
        /** Empty destructor restricts scope. */
        ~VDirectory ();

    //  Update
    public:
        /** Non-interface invoked versions of SetObject(). */
	void setObject (VString const &rName, IVUnknown* pUnknown);       
	void setObject (VString const &rName, IVUnknown* pUnknown, VString const& rDescription);

        /** Non-interface invoked versions of SetSource(). */
	void setSource (VString const &rName, ObjectGofer *pObjectGofer);
	void setSource (VString const &rName, ObjectGofer *pObjectGofer, VString const& rDescription);

	void setSource (VString const &rName, ObjectSource *pObjectSource);
	void setSource (VString const &rName, ObjectSource *pObjectSource, VString const& rDescription);

        /** Non-interface invoked version of SetDescription(). */
        void setDescription (VString const &rName, VString const &rDescription);	

    //  Query
    public:
        /**
         * Used to determine whether or not this directory has an entry for a given name.
         *
         * @param rName the name for which an entry should be searched for.
         * @return true if an entry exists for the given name, false otherwise.
         */
        bool hasEntry (VString const &rName) const;

        /**
         * Used to retrieve the description of this directory.
         *
         * @param[out] rDescription the reference by which the description should be returned.
         */
        void getDirectoryDescription (VString &rDescription);

    //  Base Roles
    public:
        using BaseClass::getRole;

    /// @name IDirectory Implementation
    //@{
    private:
        /** The IDirectory role for this Directory. */
        VRole<ThisClass,IDirectory> m_pIDirectory;
    public:
        /** Used to retrieve the IDirectory role for this Directory. */
        void getRole (IDirectory::Reference &rpRole) {
            m_pIDirectory.getRole (rpRole);
        }

    //  IDirectory Methods
    public:
        /** Implements Vca::IDirectory::GetObject(). */
        void GetObject (
            IDirectory *pRole, ObjectSink *pClient, VString const &rName, VTypeInfo *pType
        );
        /** Implements Vca::IDirectory::SetObject(). */
        void SetObject (
            IDirectory *pRole, EntrySink *pClient, VString const &rName, Object *pObject
        );
        /** Implements Vca::IDirectory::SetSource(). */
        void SetSource (
            IDirectory *pRole, EntrySink *pClient, VString const &rName, ObjectSource *pObjectSource
        );
        /** Implements Vca::IDirectory::SetDescription(). */
        void SetDescription (
            IDirectory *pRole, EntrySink *pClient, VString const &rName, VString const &rDescription
        );
        /** Implements Vca::IDirectory::GetDescription(). */
        void GetDescription (
            IDirectory *pRole, IVReceiver<VString const &>* pReceiver, VString const &rName
        );
        /** Implements Vca::IDirectory::GetEntries(). */
        void GetEntries (
            IDirectory *pRole, IVReceiver<VkDynamicArrayOf <VString> const & >* pReceiver
        );

    //  INamespace Methods
    public:
        /** Implements Vca::INamespace::GetBinding(). */
        void GetBinding (INamespace *pRole, BindingSink *pClient, VString const &rName);
    //@}

    //  Behavior Control support
    public:
	static bool TTLIsDisabled () {
	    return g_TTLDisabled;
	}
	static void DisableTTL () {
	    g_TTLDisabled = true;
	}
	static void EnableTTL () {
	    g_TTLDisabled = false;
	}
    //  Maintenance
    private:
        /**
         * Removes an entry from this directory.
         *
         * @param pEntry the entry to remove.
         */
        void detach (Entry const *pEntry);
    protected:
        /**
         * Finds, if it exists, an entry in a directory.
         *
         * Here, subclasses can provide implementations that locate items that don't actually exist (or at least not in the underlying map data structure). A practical UNIX example of this type of functionality is automount.
         *
         * @see Vca::Vps::VRelayPublicationDirectory
         *
         * @param[out] rpEntry the reference by which the found entry is returned.
         * @param[in] rName the name of the entry to be found.
         * @return true if an entry was found and returned, false otherwise.
         */
        virtual bool locate (Entry::Reference &rpEntry, VString const &rName);

        /**
         * Finds, or creates if none can be found, an entry in a directory.
         *
         * @param[out] rpEntry the reference by which the found/created entry is returned.
         * @param[in] rName the name of the entry to be found/created.
         * @return true if an entry was created, false if an existing entry was returned.
         */
        virtual bool supply (Entry::Reference &rpEntry, VString const &rName);

    //  State
    private:
        /** Map data structure. */
        EntryMap m_iEntries;
	static bool g_TTLDisabled;
    };
}


#endif
