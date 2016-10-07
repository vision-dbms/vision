#ifndef Vca_VInterfaceSource__Interface
#define Vca_VInterfaceSource__Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_VInstanceSource.h"
#include "Vca_VInterfaceQuery.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    template<class I> class VInterfaceSource_ : public VInstanceSource<I*,typename I::Reference> {
	typedef BOGUS_TYPENAME VInstanceSource<I*,typename I::Reference> base_t;
	DECLARE_FAMILY_MEMBERS (VInterfaceSource_<I>, base_t);

    //  Receiver
    public:
	typedef IVReceiver<IVUnknown*> Receiver;
	
    //  IQ
    public:
	typedef void (I::*IQ)(VTypeInfo*, Receiver*, VMessageManager*);

    /******************************************************************************/
    //  QueryInterface Sink
    public:
	class QISink : public VInterfaceQuery {
	    DECLARE_FAMILY_MEMBERS (QISink, VInterfaceQuery);

	//  Construction
	public:
	    QISink(VInterfaceQuery const &rQuery, IQ pIQ) : BaseClass (rQuery), m_pIQ (pIQ) {
	    }
	    QISink (ThisClass const &rOther) : BaseClass (rOther), m_pIQ (rOther.m_pIQ) {
	    }

	//  Destruction
	public:
	    ~QISink() {
	    }

	//  Callback
	public:
	    void operator () (I *pInterface) const {
		if (!pInterface)
		    onZero ();
		else {
		    typeinfo_reference_t pTypeInfo;
		    receiver_reference_t pReceiver;
		    manager_reference_t pManager;
		    getMessageParameters (pInterface, pReceiver, pTypeInfo, pManager);
		    (pInterface->*m_pIQ) (pTypeInfo, pReceiver, pManager);
		}
	    }
	    void operator () (IError *pInterface, VString const &rMessage) const {
		onError (pInterface, rMessage);
	    }

	//  State
	private:
	    IQ const m_pIQ;
	};

    /******************************************************************************/
    //  Construction
    public:
	VInterfaceSource_(IError *pError, VString const &rMessage) : BaseClass (pError, rMessage) {
	}
	VInterfaceSource_(I *pI) : BaseClass (pI) {
	}
	VInterfaceSource_() {
	}

    //  Destruction
    public:
	~VInterfaceSource_ () {
	};

    //  Use
    public:
	void supply (VInterfaceQuery const &rQuery) {
	    supply (rQuery, &I::QueryInterface);
	}
	void supply (VInterfaceQuery const &rQuery, IQ pIQ) {
	    QISink iSink (rQuery, pIQ);
	    BaseClass::supply (iSink);
	}
    };
}


#endif
