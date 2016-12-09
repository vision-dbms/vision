#ifndef Vca_Registry_VRegistry_Interface
#define Vca_Registry_VRegistry_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_Registry_IAccess.h"

#include "Vca_IPassiveCall.h"
#include "Vca_IPassiveClient.h"
#include "Vca_IPassiveServer.h"
#include "Vca_ITrigger.h"

#include "Vca_VTimer.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IPassiveConnector.h"

#include "Vca_VGoferInstance.h"

#include "Vca_Registry_IUpdater.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VPassiveConnector;

    namespace Registry {
	class Vca_Registry_API VRegistry : virtual public VRolePlayer {
	    DECLARE_CONCRETE_RTTLITE (VRegistry, VRolePlayer);

	//  Aliases
	public:
	    typedef VGoferInstance<IVUnknown*> object_gofer_t;

	//  Contact
	public:
	    class Vca_Registry_API Contact : virtual public VRolePlayer {
		DECLARE_ABSTRACT_RTTLITE (Contact, VRolePlayer);

		friend class VRegistry;

	    //  Ticket
	    public:
		class Ticket;
		friend class Ticket;

	    //  Construction
	    protected:
		Contact (VRegistry* pRegistry, uuid_t const &rUUID, unsigned int cUses);

	    //  Destruction
	    protected:
		~Contact ();

	    //  Access/Query
	    public:
		bool expired () const {
		    return m_cUsesRemaining == 0;
		}
		uuid_t const &uuid () const {
		    return m_iUUID;
		}

	    //  Callbacks
	    private:
		virtual void onCallback (IPassiveCallback *pCallback);
		virtual void onQuery (IObjectSink* pObjectSink, VTypeInfo* pObjectType);

	    //  Implementation
	    private:
		void disconnectFrom (VRegistry* pRegistry);
		bool lastUse () {
		    return expired () || --m_cUsesRemaining == 0;
		}

	    //  State
	    private:
		VkUUIDHolder const m_iUUID;
		unsigned int m_cUsesRemaining;
	    };
	    friend class Contact;

	//  Callback Contact
	public:
	    class CallbackContact;

	//  Object Contact
	public:
	    class ObjectContact;
	    
	//  Construction
	public:
	    VRegistry ();

	//  Destruction
	protected:
	    ~VRegistry ();

	//  Roles
	public:
	    using BaseClass::getRole;

	//  IAccess
	private:
	    VRole<ThisClass,IAccess> m_pRegistry_IAccess;
	public:
	    void getRole (IAccess::Reference& rpRole) {
		m_pRegistry_IAccess.getRole (rpRole);
	    }

	//  IAccess Callbacks
	public:
	    void GetObject (IAccess* pRole, IObjectSink* pObjectSink, uuid_t const& rObjectUUID, VTypeInfo* pObjectType);

	//  IPassiveClient
	private:
	    VRole<ThisClass,IPassiveClient> m_pIPassiveClient;
	public:
	    void getRole (IPassiveClient::Reference &rpRole) {
		m_pIPassiveClient.getRole (rpRole);
	    }

	//  IPassiveClient Callbacks
	public:
	    void AcceptCallback (IPassiveClient *pRole, IPassiveCallback *pCallback, uuid_t const &rUUID);

	//  Callback Contact Creation
	public:
	    void requestCallback (
		VPassiveConnector *pConnector, IPassiveCallbackReceiver *pCallbackReceiver, IPassiveServer *pServer, U64 sTimeout, unsigned int cUses = 1
	    );
	private:
	    void getNewContact (
		Contact::Reference &rpContact, unsigned int cUses, VPassiveConnector *pConnector, IPassiveCallbackReceiver *pCallbackReceiver, IPassiveServer *pServer, U64 sTimeout
	    );
	    bool insertContact (
		Contact::Reference &rpContact, uuid_t const&rUUID, unsigned int cUses, VPassiveConnector *pConnector, IPassiveCallbackReceiver *pCallbackReceiver, IPassiveServer *pServer, U64 sTimeout
	    );

	//  Object Contact Creation
	public:
	    template <class object_t> void addObject (IUpdater *pUpdater, object_t *pObject, unsigned int cUses = UINT_MAX) {
		Contact::Reference pContact;
		do {
		    VkUUIDGenerate iUUID;
		    // ... when insertContact fails, the UUID is in use and we must try again
		    insertContact (pContact, iUUID, cUses, pUpdater, pObject);
		} while (pContact.isNil ());
	    }
	    template <class object_t> bool addObject (IUpdater *pUpdater, uuid_t const& rUUID, object_t *pObject, unsigned int cUses = UINT_MAX) {
		Contact::Reference pContact;
		return insertContact (pContact, rUUID, cUses, pUpdater, pObject);
	    }
	private:
	    bool insertContact (Contact::Reference &rpContact, uuid_t const&rUUID, unsigned int cUses, IUpdater *pUpdater, IVUnknown *pObject);
	    bool insertContact (Contact::Reference &rpContact, uuid_t const&rUUID, unsigned int cUses, IUpdater *pUpdater, object_gofer_t *pGofer);

	//  Contact Map Access/Deletion
	private:
	    bool getOldContact (Contact::Reference &rpContact, uuid_t const &rUUID);
	    bool deleteContact (uuid_t const& rUUID);

	//  State
	private:
	    VkMapOf<VkUUIDHolder,uuid_t const&,uuid_t const&,Contact::Reference> m_iContactMap;
	};
    }
}


#endif
