#ifndef VcaListener_Interface
#define VcaListener_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_IListener.h"
#include "Vca_IPipeSourceClient.h"

#include "Vca_VcaOffer.h"
#include "Vca_VInstanceSource.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VString.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class IError;
    class IListener;
    class ITrigger;

    class Vca_API VcaListener : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (VcaListener, VRolePlayer);

    //  Aliases
    public:
	typedef IVReceiver<VString const&> INameSink;
	typedef VInstanceSource<VString const&,VString> NameSource;

    //  Object Holder
    public:
	typedef VcaOffer::Offering Offering;

    //  Global VcaListener List
    private:
	static Reference g_pListenerListHead;

    //  Construction
    public:
	VcaListener (IListener *pListener, Offering const &rOffering);

    //  Destruction
    private:
	~VcaListener ();

    //  Query
    public:
        U32 activeOfferCount () const;

    //  INameSink Role
    private:
	VRole<ThisClass,INameSink> m_pINameSink;
	void getRole (INameSink::Reference &rpRole) {
	    m_pINameSink.getRole (rpRole);
	}

    //  INameSink Methods
    public:
	void OnData (INameSink *pRole, VString const &rName);

    //  IPipeSourceClient Role
    private:
	VRole<ThisClass,IPipeSourceClient> m_pIPipeSourceClient;
	void getRole (IPipeSourceClient::Reference &rpRole) {
	    m_pIPipeSourceClient.getRole (rpRole);
	}

    //  IPipeSourceClient Methods
    public:
	void OnData (
	    IPipeSourceClient *pRole, VBSConsumer *pBSToPeer, VBSProducer *pBSToHere, VBSProducer *pErrToHere
	);
 	void OnError (
	    IClient *pRole, IError *pError, VString const &rDescription
	);

    //  Name Query
    public:
	template <typename callback_t> void getName (callback_t const &rCallback) {
	    m_iNameSource.supply (rCallback);
	}
	template <typename class_t> void getName (
	    class_t *pConsumer,
	    void (class_t::*pDataMember)(VString const&),
	    void (class_t::*pErrorMember) (IError*, VString const&)
	) {
	    m_iNameSource.supplyMembers (pConsumer, pDataMember, pErrorMember);
	}
	void getName (INameSink *pSink) {
	    m_iNameSource.supplyReceiver (pSink);
	}

    //  Control
    public:
	void stop ();

    //  Listener Maintenance
    private:
	void addToListenerList	    ();
	void removeFromListenerList ();
    public:
	static void CloseListeners  ();

    //  State
    private:
	IListener::Reference	m_pListener;
	Offering const		m_iOffering;

	U32			m_cConnectionsProcessed;

	NameSource		m_iNameSource;

	Reference 		m_pSuccessor;
	Reference 		m_pPredecessor;
    };
}


#endif
