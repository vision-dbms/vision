#ifndef Vdht_VHT_Interface
#define Vdht_VHT_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vdht.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_Registry_IRegistry.h"
#include "Vca_Registry_IRegistration.h"

#include "VkMapOf.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vdht {
    /**
     * Distributed Hash Table
     */
    class Vdht_API VHT : virtual public Vca::VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (VHT, VRolePlayer);

    //  Aliases
    public:
	typedef Vca::Registry::IAuthority IAuthority;

	typedef Vca::Registry::IRegistration IRegistration;
	typedef Vca::Registry::IRegistrationTicket IRegistrationTicket;
	typedef Vca::Registry::IRegistrationTicketSink IRegistrationTicketSink;

	typedef Vca::Registry::IRegistry IRegistry;

	typedef Vca::Registry::VAuthority VAuthority;

    //  Entry
    public:
	class Entry : virtual public VRolePlayer {
	    DECLARE_CONCRETE_RTTLITE (Entry, VRolePlayer);

	//  Authority
	public:
	    class Authority : virtual public VRolePlayer {
		DECLARE_CONCRETE_RTTLITE (Authority, VRolePlayer);

	    //  Map
	    public:
		typedef VkMapOf<IAuthority::Reference, IAuthority*, IAuthority const*, Reference> Map;

	    //  TicketSink
	    public:
		class TicketSink;
		friend class TicketSink;

	    //  Construction
	    public:
		Authority (VHT *pHT, Entry *pEntry, VAuthority const &rAuthority);

	    //  Destruction
	    private:
		~Authority ();

	    //  IRegistration Role
	    private:
		Vca::VRole<ThisClass,IRegistration> m_pIRegistration;
	    public:
		void getRole (IRegistration::Reference &rpRole) {
		    m_pIRegistration.getRole (rpRole);
		}

	    //  IRegistration Callbacks
	    public:
		void Revoke (IRegistration *pRole);

	    //  Access
	    public:
		VHT *ht () const {
		    return m_pHT;
		}
		Entry *entry () const {
		    return m_pEntry;
		}

	    //  State
	    private:
		VHT::Reference   const m_pHT;
		Entry::Reference const m_pEntry;
		IAuthority::Reference const m_pAuthority;
	    };
	    typedef Authority::Map AuthorityMap;
	    friend class Authority;

	//  Authority Map
	public:

	//  Construction
	public:
	    Entry (VHT *pHT, VAuthority const &rAuthority, IVUnknown *pObject);

	//  Destruction
	protected:
	    ~Entry ();

	//  Authority
	public:
	    void addAuthority (VHT *pHT, VAuthority const &rAuthority);

	//  Callbacks
	private:
	    void onRevocationOf (Authority *pAuthority);

	//  State
	private:
	    IVUnknown::Reference const m_pObject;
	    AuthorityMap m_iAuthorityMap;
	};

    //  Entry Map
    public:
        /** Map data structure type. */
	typedef VkMapOf<IVUnknown::Reference, IVUnknown*, IVUnknown const*, Entry::Reference> EntryMap;

    //  Construction
    public:
        /** Constructor initializes roles. */
	VHT ();

    //  Destruction
    protected:
        /** Empty destructor restricts scope. */
	~VHT ();

    //  Base Roles
    public:
	using BaseClass::getRole;

    /// @name IRegistry Implementation
    //@{
    private:
        /** The IRegistry role for this hash table instance. */
	Vca::VRole<ThisClass,IRegistry> m_pIRegistry;
    public:
        /** Used to retrieve the IRegistry role for this Directory. */
	void getRole (IRegistry::Reference &rpRole) {
	    m_pIRegistry.getRole (rpRole);
	}

    //  IRegistry Methods
    public:
	void RegisterObject (IRegistry *pRole, VAuthority const &rAuthority, IVUnknown *pObject);
    //@}

    //  State
    private:
        /** Map data structure. */
	EntryMap m_iEntryMap;
    };
}


#endif
