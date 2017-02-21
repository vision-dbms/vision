/**
 * @file
 * Defines templates used to declare Vca interface multi-cast subscribers.
 */

#ifndef Vca_VSubscription_Interface
#define Vca_VSubscription_Interface

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    /*---------------------*
     *----  Recursion  ----*
     *---------------------*/

    template <class Interface_T> class VSubscription : public VSubscription<typename Interface_T::BaseClass>::SubscriptionBase {
	DECLARE_ABSTRACT_RTTLITE (VSubscription<Interface_T>, typename VSubscription<typename Interface_T::BaseClass>::SubscriptionBase);

    //  Aliases
    public:
	typedef Interface_T interface_t;

	typedef ThisClass SubscriptionBase;
	typedef Reference SubscriptionReference;

    //  Enumerator
    public:
	class Enumerator {
	    DECLARE_NUCLEAR_FAMILY (Enumerator);

	//  Construction
	public:
	    Enumerator (
		Interface_T *pInterface
	    ) : m_pSubscription (pInterface->subscription ()), m_pTarget (pInterface->target ()) {
	    }

	//  Destruction
	public:
	    ~Enumerator () {
	    }

	//  Access
	public:
	    operator Interface_T* () const {
		return m_pTarget;
	    }
	    Interface_T* operator-> () const {
		return m_pTarget;
	    }

	//  Query
	public:
	    bool isntNil () const {
		return m_pTarget.isntNil ();
	    }

	//  Update
	public:
	    void next () {
		if (m_pSubscription)
		    m_pSubscription.setTo (m_pSubscription->successor ());
		m_pTarget.setTo (m_pSubscription ? m_pSubscription->target () : 0);
	    }

	//  State
	private:
	    SubscriptionReference		m_pSubscription;
	    typename Interface_T::Reference	m_pTarget;
	};

    //  Construction
    protected:
	VSubscription () {
	}

    //  Destruction
    protected:
	~VSubscription () {
	}

    //  Access
    public:
	virtual ThisClass* successor () const = 0;  // ... covariant return types must work.
	virtual Interface_T*  target () const = 0;  // ... covariant return types must work.
    };

    /*-----------------*
     *----  Basis  ----*
     *-----------------*/

    template <> class VSubscription<VReferenceable> {
    public:
	typedef VReferenceable SubscriptionBase;
    };
}


#endif
