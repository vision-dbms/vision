#ifndef Vca_VPublisher_Interface
#define Vca_VPublisher_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VSubscriptionManager.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    template <class Interface_T> class VPublisher : public Interface_T {
	DECLARE_CONCRETE_RTTLITE (VPublisher<Interface_T>,Interface_T);

    //  Aliases
    public:
	typedef Interface_T interface_t;

	typedef typename Interface_T::Reference InterfaceReference;
	typedef typename Interface_T::Subscription InterfaceSubscription;

    //  SubscriptionManager
    public:
	class SubscriptionManager : public VSubscriptionManager {
	    DECLARE_CONCRETE_RTTLITE (SubscriptionManager, VSubscriptionManager);

	//  Subscription
	public:
	    class Subscription : public InterfaceSubscription {
		DECLARE_CONCRETE_RTTLITE (Subscription, InterfaceSubscription);

		friend class SubscriptionManager;
		
	    //  Construction
	    public:
		Subscription (Interface_T *pTarget) : m_pTarget (pTarget) {
		}

	    //  Destruction
	    private:
		~Subscription () {
		}

	    public:
		ThisClass* successor () const OVERRIDE {
		    return NextUsable (m_pSuccessor);
		}
		Interface_T* target () const OVERRIDE {
		    return m_pTarget;
		}
    	    
	    //  Referenceage
	    private:
		bool unlink (Reference*& rppMySelf, VSubscriptionManager::Subscription const* pCandidate) {
		//  No match?  Tell my caller to try my successor ...
		    if (rppMySelf->referent () != pCandidate) {
			rppMySelf = &m_pSuccessor;
			return false;
		    }

		//  Found me?  Splice me out of the list ...
		    rppMySelf->setTo (m_pSuccessor);
		    return true;
		}

	    //  State
	    private:
		const InterfaceReference m_pTarget;
		mutable Reference m_pSuccessor;
	    };
	    typedef typename Subscription::Reference SubscriptionReference;

	//  Construction
	public:
	    SubscriptionManager () {
	    }

	//  Destruction
	private:
	    ~SubscriptionManager () {
	    }

	//  Subscription Access
	public:
	    static Subscription* NextUsable (SubscriptionReference& rpCandidate) {
		while (rpCandidate.isntNil () && (!rpCandidate->target() || rpCandidate->target()->isntUsable ()))
		    rpCandidate.setTo (rpCandidate->m_pSuccessor);
		return rpCandidate;
	    }
	public:
	    Subscription* subscription () const {
		return NextUsable (m_pSubscriptions);
	    }	
	    Interface_T* target () const {
		return m_pSubscriptions ? m_pSubscriptions->target () : 0;
	    }

	//  Subscription Query
	public:
	    bool hasSubscriptions () const {
		return m_pSubscriptions.isntNil ();
	    }
	    bool hasSubscriptionsNot () const {
		return m_pSubscriptions.isNil ();
	    }

	//  Subscription Management
	//  ... creation:
	private:
	    void makeSubscription (SubscriptionReference& rpSubscription, Interface_T* pTarget) {
		rpSubscription.setTo (new Subscription (pTarget));
		m_pSubscriptions.interlockedPush (rpSubscription, &Subscription::m_pSuccessor);
	    }
	public:
	    template <typename ticket_receiver_t> void addSubscriber (ticket_receiver_t rTicketReceiver, Interface_T* pTarget) {
		SubscriptionReference pSubscription;
		makeSubscription (pSubscription, pTarget);
		BaseClass::makeTicket (rTicketReceiver, pSubscription);
	    }

	//  Subscription Management
	//  ... cancelation:
	private:
	    void cancel_(VSubscriptionManager::Subscription const* pSubscription) OVERRIDE {
		SubscriptionReference* ppCandidate (&m_pSubscriptions);
		while (ppCandidate->isntNil () && !(*ppCandidate)->unlink (ppCandidate, pSubscription));
	    }

	//  State
	private:
	    SubscriptionReference mutable m_pSubscriptions;
	};
	typedef typename SubscriptionManager::Subscription Subscription;

    //  Construction
    public:
	VPublisher () : m_pManager (new SubscriptionManager ()) {
	}

    //  Destruction
    private:
	~VPublisher () {
	}

    //  Access
    public:
	Subscription* subscription () const OVERRIDE {
	    return m_pManager->subscription ();
	}	
	Interface_T* target () OVERRIDE {
	    return m_pManager->target ();
	}

    //  Query
    public:
	bool hasSubscribers () const {
	    return m_pManager->hasSubscriptions ();
	}
    protected:
	bool isConnected_() const OVERRIDE {
	    return m_pManager->hasSubscriptions ();
	}

	bool isUsable_() const OVERRIDE {
	    return true;
	}

    //  Message Handling
    private:
        bool defersTo (VMessageScheduler &rScheduler) OVERRIDE {
            return false;
        }

    //  Subscription Management
    public:
	template <typename ticket_receiver_t> void addSubscriber (ticket_receiver_t rTicketReceiver, Interface_T* pTarget) const {
	    m_pManager->addSubscriber (rTicketReceiver, pTarget);
	}

    //  State
    private:
	typename SubscriptionManager::Reference const m_pManager;
    };
}


#endif
