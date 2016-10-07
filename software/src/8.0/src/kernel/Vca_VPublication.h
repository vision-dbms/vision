#ifndef Vca_VPublication_Interface
#define Vca_VPublication_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VPublisher.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {

    /************************************
     *----  class VPublicationBase  ----*
     ************************************/

    class Vca_API VPublicationBase {
	DECLARE_NUCLEAR_FAMILY (VPublicationBase);
	friend class VRolePlayer;

    //  Aliases
    public:
	typedef IVReceiver<IRequest*> ITicketSink;

    //  Construction
    protected:
	VPublicationBase ();

    //  Destruction
    protected:
	~VPublicationBase () {
	}

    //  Access
    public:
	virtual VTypeInfo *typeInfo_() const = 0;

    protected:
	VPublicationBase *successor () const {
	    return m_pSuccessor;
	}

    //  Query
    public:
	bool publishes (VTypeInfo *pTypeInfo) const;

    //  State
    private:
	Pointer const m_pSuccessor;
    };


    /*************************************************************
     *----  template <class Interface_T> class VPublication  ----*
     *************************************************************/

    template <class Interface_T> class VPublication : public VPublicationBase {
	DECLARE_FAMILY_MEMBERS (VPublication<Interface_T>, VPublicationBase);

    //  Aliases
    public:
	typedef VPublisher<Interface_T> Implementation;

    //  Construction
    public:
	VPublication () {
	}
    
    //  Destruction
    public:
	~VPublication () {
	}

    //  Access
    private:
	Implementation* implementation () const {
	    return m_pImplementation;
	}
	Implementation* implementation () {
	    if (m_pImplementation.isNil ()) {
		typename Implementation::Reference const pImplementation (new Implementation ());
		m_pImplementation.interlockedSetIfNil (pImplementation);
	    }
	    return m_pImplementation;
	}
    public:
	operator Interface_T* () const {
	    return implementation ();
	}
	Interface_T* operator->() const {
	    return implementation ();
	}
	Interface_T& operator*() const {
	    return *implementation ();
	}

	bool hasSubscribers () const {
	    return m_pImplementation && m_pImplementation->hasSubscribers ();
	}
	VTypeInfo *typeInfo_() const {
	    return Interface_T::typeInfo ();
	}

    //  Subscription
    public:
	void addSubscriber (ITicketSink* pTicketSink, Interface_T* pObserver) {
	    implementation ()->addSubscriber (pTicketSink, pObserver);
	}

    //  State
    private:
	typename Implementation::Reference m_pImplementation;
    };
}


#endif
