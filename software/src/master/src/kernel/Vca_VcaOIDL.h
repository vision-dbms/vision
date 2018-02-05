#ifndef Vca_VcaOIDL_Interface
#define Vca_VcaOIDL_Interface 

/************************
 *****  Components  *****
 ************************/

#include "Vca_VcaOID.h"

#include "VkMapOf.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VCallbackTicket.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VcaOIDR;

    class VMessage;

    /*-------------------*
     *----  VcaOIDL  ----*
     *-------------------*/

    class VcaOIDL : public VcaOID {
	DECLARE_CONCRETE_RTTLITE (VcaOIDL, VcaOID);

	friend class ::IVUnknown;

    //  Evaluator
    public:
	class Evaluator : public VReferenceable {
	    DECLARE_CONCRETE_RTTLITE (Evaluator, VReferenceable);

	    friend class VcaOIDL;
	    friend class VcaPeer;

	//  Construction
	protected:
	    Evaluator (VcaOIDL *pOIDL, VcaSite *pSite);

	//  Destruction
	private:
	    ~Evaluator ();

	//  Access
	public:
	    VcaOIDL *oidl () const {
		return m_pOIDL;
	    }
	    VcaSite *site () const {
		return m_pSite;
	    }

	//  Evaluation
	private:
	    unsigned int evaluate (VMessage *pMessage);

	//  Peer Linkage
	private:
	    void onDefunctPeer ();

	    void removeFromSiteList ();

	//  Tracing and Display
	public:
	    void getInfo (VString &rResult, VString const &rPrefix) const OVERRIDE;

	//  State
	private:
	    VReference<VcaOIDL>	const	m_pOIDL;
	    VReference<VcaSite>	const	m_pSite;
	    Pointer			m_pSiteListPredecessor;
	    Pointer			m_pSiteListSuccessor;
	    VReference<VMessage>	m_pMessages;
	    VReference<VMessage>*	m_pMessagesToAck;
	    VReference<VMessage>*	m_pMessagesToRun;
	    count_t			m_xNextMessage;
	};
	friend class Evaluator;

    //  Local Identity Holder
    protected:
	/*----------------------------------------------------------------*
	 *  Vise exists primarily for its constructor.  It initializes
	 *	and holds a weak reference to the local identity that keeps
	 *	this interface alive whenever it is exported.
	 *----------------------------------------------------------------*/
	class Vise;
	friend class Vise;

    //  Construction
    public:
	VcaOIDL (VcaSSID const &rObjectSSID, IVUnknown *pObject);

    //  Destruction
    private:
	~VcaOIDL ();

    //  Connectivity
    public:
	bool isConnected () const OVERRIDE {
	    return true;
	}
	bool isntConnected () const {
	    return false;
	}

    //  Evaluation
    private:
	typedef VkMapOf<
	    VReference<VcaSite>, VcaSite*, VcaSite const*, VReference<Evaluator>
	> Evaluators;

	void evaluateIncomingFrom (VMessage *pMessage, VcaSite *pSite);

	void detach (Evaluator *pEvaluator);

    //  Evaluation Accounting
    private:
	bool allMessagesProcessed () const {
	    return m_cMessagesReported == m_cMessagesProcessed && m_cMessagesProcessed == m_cMessagesReceived;
	}

	void updateMessageCounts (count_t cReported, count_t cReceived, count_t cProcessed) OVERRIDE;

    //  Object Access
    private:
	uuid_t const& objectSiteUUID_() const OVERRIDE;
	VTypeInfo *objectType_() const OVERRIDE;

	void supplyInterface_(VReference<IVUnknown>&rpInterface) OVERRIDE;

    //  Object Export
    private:
	void onFinalExport () OVERRIDE;

    //  Object Import
    private:
        void createImportFrom (VcaPeer *pPeer, bool bWeak) OVERRIDE {
	}
	void deleteImportFrom (VcaPeer *pPeer) OVERRIDE {
	}
	bool weakenImportFrom (VcaPeer *pPeer) OVERRIDE {
	    return false;
	}

    //  Tracing and Display
    public:
	using BaseClass::displayInfo;
	void displayInfo () const OVERRIDE;
	void getInfo (VString &rResult, const VString &rPrefix) const OVERRIDE;
	void getEvaluatorTable (VString &rResult, VString const &rPrefix) const;

    //  State
    private:
	VReference<IVUnknown> const m_pObject;
	Evaluators		    m_iEvaluators;
	unsigned int		    m_cMessagesReceived;
	unsigned int		    m_cMessagesProcessed;
    };


    /*-------------------------*
     *----  VcaOIDL::Vise  ----*
     *-------------------------*/

    class VcaOIDL::Vise {
	friend class ::IVUnknown;

    //  Construction
    public:
	Vise () : m_pOIDL (0) {
	}

    //  Destruction
    public:
	~Vise () {
	}

    //  Access
    public:
	operator VcaOID* () const {
	    return m_pOIDL;
	}
	operator VcaOIDL* () const {
	    return m_pOIDL;
	}
	VcaOIDL *operator-> () const {
	    return m_pOIDL;
	}

    //  Evaluation
    private:
	void evaluateIncomingFrom (VMessage *pMessage, VcaSite *pSite) const {
	    m_pOIDL->evaluateIncomingFrom (pMessage, pSite);
	}

    //  Export
    public:
	Vca_API VcaOID::count_t exportCount () const;
	Vca_API VcaOID::count_t weakExportCount () const;
	
	Vca_API bool isExported () const;
	Vca_API bool isExportedTo (VcaPeer const *pPeer) const;

	bool isntExported () const {
	    return !isExported ();
	}
	bool isntExportedTo (VcaPeer const *pPeer) const {
	    return !isExportedTo (pPeer);
	}

	bool requestExportCountCallbacks (
	    VCallbackTicket::Reference &rpTicket, IVReceiver<count_t>* pSink, VcaOIDR *pOIDR
	) const {
	    return false;
	}
	template <class sink_t> bool requestExportCountCallbacks (
	    VCallbackTicket::Reference &rpTicket, sink_t *pSink, void (sink_t::*pSinkMember)(count_t), VcaOIDR *pOIDR
	) const {
	    return false;
	}

    //  OIDL
    private:
	void detach (VcaOIDL *pOIDL) {
	    m_pOIDL.interlockedClearIf (pOIDL);
	}
	void getOIDL (VReference<VcaOIDL>&rpOIDL) const;
	void getOIDL (VReference<VcaOIDL>&rpOIDL, IVUnknown *pObject);
	void getSSID (VcaSSID &rSSID);

    //  State
    private:
	VcaOIDL::Pointer m_pOIDL;
    };
}


#endif
