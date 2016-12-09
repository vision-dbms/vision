#ifndef VSNFTask_Interface
#define VSNFTask_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTask.h"

#include "VCachedArray.h"

#include "VExternalGroundStore.h"

#include "VSNFTaskParameters.h"
#include "VSNFTaskParametersList.h"

#include "Vxa_ICaller.h"
#include "Vxa_ICollection.h"
#include "Vxa_IVSNFTaskImplementation.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VkDynamicArrayOf.h"


/*************************
 *****  Definitions  *****
 *************************/

class VSNFTask : public VTask {
    DECLARE_CONCRETE_RTT (VSNFTask, VTask);

//  Aliases
public:
    typedef Vxa::object_reference_t object_reference_t;
    typedef Vxa::object_reference_array_t object_reference_array_t;

    typedef Vxa::ICaller ICaller;
    typedef Vxa::ICollection ICollection;
    typedef Vxa::ISingleton ISingleton;

    typedef Vxa::IVSNFTaskImplementation IVSNFTaskImplementation;

//  Types
public:
    typedef void (VSNFTask::*Continuation) ();
    typedef void (*GroundContinuation)(VSNFTask*);

//****************************************************************
//  Queue Class
public:
    class Queue : protected VSNFTaskParametersList {
    //  Construction
    public:
	Queue () : m_pEnqueueReference (this) {
	}

    //  Query
    public:
	operator VSNFTaskParameters* () const {
	    return VSNFTaskParametersList::operator VSNFTaskParameters* ();
	}
	bool isEmpty () const {
	    return VSNFTaskParametersList::isEmpty ();
	}
	bool isntEmpty () const {
	    return VSNFTaskParametersList::isntEmpty ();
	}

    //  Update
    public:
	inline void enqueue (VSNFTaskParameters* pSNFT);
	inline void dequeue (VSNFTaskParameters::Reference& rpSNFT);

	void resetEnqueueReference () {
	    m_pEnqueueReference = this;
	}

    //  State
    protected:
	VSNFTaskParameters::Reference* m_pEnqueueReference;
    };

//****************************************************************
//  Meta Maker
protected:
    static void MetaMaker ();

//  Construction
public:
    VSNFTask (ConstructionData const& rTCData, unsigned int const iSkipCount);

//  Destruction
private:
    ~VSNFTask ();

//  Query
public:
    bool allSegmentsReceived () const {
	return m_cSegmentsExpected == m_cSegmentsReceived;
    }

    bool datumAlterable_ () const;

//  Access
public:
    IVSNFTaskImplementation *implementation () const {
	return m_pSNFTaskImplementation;
    }

    unsigned int skipCount () const {
	return m_iSkipCount;
    }

//  Parameter Access
public:
    bool getParameter (unsigned int xParameter, VDescriptor& rDatum);
    bool getSelfReference (object_reference_t &rSelfReference);
    bool getSelfReference (object_reference_array_t &rSelfReference);

//  Resend
protected:
    void resendIn (VSNFTask *pResendTask); // ... see comments in .cpp

//  Continuations
protected:
    void DelegateToRecipient ();
    void ContinueOnTheGround ();
    void TriggerControlPoint ();
    void DoAnOldFashionedSNF ();

    void OnPopulateCompleted ();

//  Execution
public:
    void dismiss () {
	m_pContinuation = 0;
    }

    void setGroundContinuationTo (GroundContinuation pContinuation) {
	m_pGroundContinuation = pContinuation;
	m_pContinuation = pContinuation ? &VSNFTask::ContinueOnTheGround : 0;
    }

    void setContinuationTo (Continuation pContinuation) {
        m_pContinuation = pContinuation;
    }

protected:
    void run ();

//  Display and Description
public:
    void reportInfo	(unsigned int xLevel, VCall const* pContext = 0) const;
    void reportTrace	() const;

    char const* description () const;

//  External Invocation
public:
    void startExternalInvocation (ISingleton *pExternalObject);

//  External Invocation Support
private:
    VExternalGroundStore *remoteFactoryGS ();
    public:
    void createAndCopyToVariant (unsigned int xParameter);

    void returnImplementationHandle (
	IVSNFTaskImplementation *pImplementation, VkDynamicArrayOf<ISingleton::Reference> const &
    );
    void TurnBackSNFTask ();
    void SetOutput (VkDynamicArrayOf<VString> const & );
    void ContinueFromReturnError();

//  External Result Return Helpers
private:
    VFragment *createSegment (object_reference_array_t const &rInjector);
    bool       wrapupSegment ();

    template <typename Source_T, typename Result_T> void ProcessArray (
	VDescriptor &rResult, rtPTOKEN_Handle *pPPT, VkDynamicArrayOf<Source_T> const &rArray, Result_T const *&rpResultArray
    ) const;
    void ProcessObjects (
	VDescriptor &rResult, rtPTOKEN_Handle *pPPT, ICollection *pCluster, object_reference_t sCluster, object_reference_array_t const &rxObjects
    ) const;

//  External Result Return
public:
    template <typename Source_T, typename Result_T> void ReturnArray (
	VkDynamicArrayOf<Source_T> const &rSourceArray, Result_T const *&rpResultArray
    );

    template <typename Source_T, typename Result_T> bool ReturnSegment (
	object_reference_array_t const &rInjector, VkDynamicArrayOf<Source_T> const &rSourceArray, Result_T const *&rpResultArray
    );

    template <typename Source_T> void ReturnSingleton (Source_T iSingleton);

    bool ReturnNA (object_reference_array_t const &rInjector); // ... segment
    void ReturnNA () {
	loadDucWithNA ();
    }

    bool ReturnObjects (
	object_reference_array_t const &rInjector, ICollection *pCluster, object_reference_t sCluster, object_reference_array_t const &rxObjects
    );
    void ReturnObjects (ICollection *pCluster, object_reference_t sCluster, object_reference_array_t const &rxObjects);
    void ReturnObject (ICollection *pCluster, object_reference_t sCluster, object_reference_t xObject);

    void ReturnError (const VString &rMessage);

    bool SetSegmentCountTo (unsigned int cSegments);

//  State
private:
    VCachedArray<VDescriptor,1> m_iParameters;
    unsigned int const		m_iSkipCount;
    Continuation		m_pContinuation;
    GroundContinuation		m_pGroundContinuation;

    Queue				m_iQueue;

    IVSNFTaskImplementation::Reference
				m_pSNFTaskImplementation;
    ISingleton::Reference	m_pRemoteFactory;
    VExternalGroundStore::Reference
				m_pRemoteFactoryGS;

    unsigned int m_xParameter;
    unsigned int		m_cSegmentsExpected;
    unsigned int		m_cSegmentsReceived;

    VString m_pErrorString;
    bool			m_bRunning;
};

/*--------------------------------------*
 *----    VSNFTaskParametersList    ----*
 *--------------------------------------*/

void VSNFTaskParametersList::pop (VSNFTaskParameters::Reference& rpSNFT) {
    rpSNFT.claim (*this);
    claim (rpSNFT->m_pSuccessor);
}

/*---------------------------*
 *----  VSNFTask::Queue  ----*
 *---------------------------*/

void VSNFTask::Queue::enqueue (VSNFTaskParameters* pSNFT) {
    pSNFT->m_pSuccessor.claim (*m_pEnqueueReference);
    m_pEnqueueReference->setTo (pSNFT);
    m_pEnqueueReference = &pSNFT->m_pSuccessor;
}

void VSNFTask::Queue::dequeue (VSNFTaskParameters::Reference& rpSNFT) {
    VSNFTaskParametersList::pop (rpSNFT);
    resetEnqueueReference ();
}

#endif
