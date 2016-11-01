#ifndef Vca_VInterfaceQuery_Interface
#define Vca_VInterfaceQuery_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "VTypeInfo.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class IDirectory;
    class IError;
    class IPassiveCallback;
    class IPipeFactory;

    class VMessageManager;

/****************************************************************/
    class Vca_API VInterfaceQuery {
	DECLARE_FAMILY_MEMBERS (VInterfaceQuery,void);

    //  Aliases
    public:
	typedef VMessageManager manager_t;
	typedef VReference<manager_t> manager_reference_t;

	typedef IVReceiver<IVUnknown*> receiver_t;
	typedef VReference<receiver_t> receiver_reference_t;

	typedef VTypeInfo typeinfo_t;
	typedef VTypeInfo::Reference typeinfo_reference_t;

    //  Receptionist
    public:
	class Receptionist;
	friend class Receptionist;

    //  Construction
    public:
	VInterfaceQuery (manager_t *pManager, receiver_t *pReceiver, typeinfo_t *pTypeInfo);
	VInterfaceQuery (receiver_t *pReceiver, typeinfo_t *pTypeInfo);	//  ... remove this constructor when managers are mandatory
	VInterfaceQuery (ThisClass const &rOther);

    //  Destruction
    public:
	~VInterfaceQuery ();

    //  Access
    public:
	template <typename InterfaceT> void getMessageParameters (
	    InterfaceT *pInterface,
	    receiver_reference_t &rpReceiver, typeinfo_reference_t &rpTypeInfo, manager_reference_t &rpManager
	) const {
	    getMessageParameters (static_cast <IVUnknown*>(pInterface), rpReceiver, rpTypeInfo, rpManager);
	}
	void getMessageParameters (
	    IVUnknown *pInterface,
	    receiver_reference_t &rpReceiver, typeinfo_reference_t &rpTypeInfo, manager_reference_t &rpManager
	) const;

	manager_t *manager () const {
	    return m_pManager;
	}
	typeinfo_t *typeInfo () const {
	    return m_pTypeInfo;
	}

    //  Query
    public:
	bool wantsResult () const {
	    return m_pReceiver.isntNil ();
	}
	bool wants (VTypeInfo const *pTypeInfo) const {
	    return pTypeInfo == m_pTypeInfo.referent ();
	}
	bool doesntWant (VTypeInfo const *pTypeInfo) const {
	    return pTypeInfo != m_pTypeInfo.referent ();
	}

    //  Use
    public:
	void getObject (IVUnknown *pSource) const;
	void getObject (IPassiveCallback *pSource) const;
	void getObject (VString const &rName, IDirectory *pSource) const;

	void onEnd () const;
	void onError (IError *pInterface, VString const &rMessage) const;
	void onZero () const;

	/**
	 * Make this class usable as a valid parameter to VInstanceSource::supply(template callback_t).
	 */
	void operator () (IVUnknown *pInterface) const {
	    getObject (pInterface);
	}
	void operator () (IError *pInterface, VString const &rMessage) const {
	    onError (pInterface, rMessage);
	}

    //  State
    private:
	manager_reference_t	const m_pManager;
	receiver_reference_t	const m_pReceiver;
	typeinfo_reference_t 	const m_pTypeInfo;
    };

/****************************************************************/
    template <typename Interface> class VQueryInterface : public VInterfaceQuery {
	DECLARE_FAMILY_MEMBERS (VQueryInterface<Interface>, VInterfaceQuery);

    //  Construction
    public:
	VQueryInterface (
	    manager_t *pManager, receiver_t *pReceiver
	) : BaseClass (pManager, pReceiver, Interface::typeInfo ()) {
	}
	VQueryInterface (	//  ... remove this constructor when managers are mandatory
	    receiver_t *pReceiver
	) : BaseClass (pReceiver, Interface::typeInfo ()) {
	}
	VQueryInterface (ThisClass const &rOther) : BaseClass (rOther) {
	}

    //  Destruction
    public:
	~VQueryInterface () {
	}
    };
}


#endif
