#ifndef Vsa_VEvaluation_Interface
#define Vsa_VEvaluation_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_VActivity.h"

#include "Vsa_VPathQuery.h"

#include "Vsa_IEvaluation.h"

#include "V_VTime.h"

#include "Vca_VInterfaceEKG.h"

#include "cam.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VcaGofer.h"

#include "Vsa_IEvaluatorClient.h"


/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    class IEvaluator;
    class VEvaluator;

    /*************************
     *----  VEvaluation  ----*
     *************************/

    class Vsa_API VEvaluation : public Vca::VActivity {
	DECLARE_ABSTRACT_RTTLITE (VEvaluation, VActivity);

    //  Aliases
    public:
	typedef Vca::VInterfaceEKG<ThisClass,IVUnknown> interface_monitor_t;

    //  GoferOrder(s)
    public:
	class Vsa_API GoferOrder;
	template <typename gofer_reference_t> class GoferOrder_;

    //  Manager
        class Manager;
      
    //  Query Type
    public:
	enum QueryType {
	    QueryType_Expression, QueryType_URL
	};
	
    //  Construction
    protected:
	VEvaluation (	//  ... Expression
	    VEvaluator*		pEvaluator,
	    IEvaluatorClient*	pClient,
	    VString const&	rPath,
	    VString const&	rQuery
	);
	VEvaluation (	//  ... URL
	    VEvaluator*		pEvaluator,
	    IEvaluatorClient*	pClient,
	    VString const&	rPath,
	    VString const&	rQuery,
	    VString const&	rEnvironment
	);

    //  Destruction
    protected:
	~VEvaluation ();

    //  IEvaluation Role
    public:
	virtual void getRole (IEvaluation::Reference& rpRole) = 0;

    //  IEvaluation Methods

    //  Cancellation
    public:
        bool isCancelled () const { return m_bCancelled; }
        void cancel ();
	void Cancel (Vca::IRequest *pRole) { cancel (); }

    //  Access
    protected:
	void getDescription_(VString &rResult) const;
    public:
	VPathQuery* query () const {
	    return m_pQuery;
	}
	VString const &environmentString () const {
	    return m_pQuery->environmentString ();
	}

	VString const &pathString () const {
	    return m_pQuery->pathString ();
	}

	VString const &queryString () const {
	    return m_pQuery->queryString ();
	}

	bool getValue (char const *pKey, VString &rResult) {
	    return m_pQuery->getValue (pKey, rResult);
	}

        bool getEvaluation (Reference &rpEvaluation, request_index_t xRequest);

	void getVisionExpression (VPathQuery::Formatter const &rFormatter, VString &rResult);

	request_index_t index () const {
	    return m_xRequest;
	}

	QueryType queryType () const {
	    return m_xQueryType;
	}

	IEvaluatorClient* evaluatorClient () const {
	    return m_pClient;
	}
	Vca::U32 evaluationAttempt () const {
	    return m_xEvaluationAttempt;
	}
	Vca::U64 evaluationTime () const;
	Vca::U64 queueTime() const;

	bool getArrivalTime (V::VTime &rArrivalTime) const;
	bool getStartTime (V::VTime &rStartTime) const;
	bool getEndTime (V::VTime &rEndTime) const; 
	
	void setStart();


    //  Update
    public:
	void incrementEvaluationAttempt () {
	    m_xEvaluationAttempt++;
	}

	virtual void setRetryable (bool bRetryable) {}

    //  Client Access
    public:
	template <typename gofer_reference_t> bool newClientObjectGofer (gofer_reference_t& rpGofer) {
	    rpGofer.setTo (
		new Vca::Gofer::Queried<typename gofer_reference_t::ReferencedClass::interface_t>(m_pClient.referent())
	    );
	    return true;
	}
	bool fulfill (GoferOrder &rOrder); 

    //  Delegation
    public:
	void delegateTo (IEvaluatorClient *pClient, IEvaluator *pEvaluator) const;

    //  Result Generation
    public:
	void onAccept (Vca::U32 xQueuePosition);

	void onChange (Vca::U32 xQueuePosition);

	void onError (Vca::IError *pError, VString const &rText);

	void onComplete ();

	void onResult (IEvaluationResult *pResult, VString const &rOutput);

    //  EKG
    public:
        void signalIEvaluatorClientEKG ();
    private:
        void cancelIEvaluatorClientEKG ();

    //  State
    protected:
	VReference<VEvaluator>		const	m_pEvaluator;
	request_index_t			const	m_xRequest;
	IEvaluatorClient::Reference	const	m_pClient;
	QueryType			const	m_xQueryType;
	VPathQuery::Reference		const	m_pQuery;

	V::VTime			const	m_iArrivalTime; 
	V::VTime				m_iStartTime;
	V::VTime				m_iEndTime;
	Vca::U32				m_xEvaluationAttempt;

        CAM::Operation                          m_iCamOp;

        /**
         * Keeps track of the IEvaluatorClient interface, notifies us upon disconnect.
         */
        interface_monitor_t::Reference          m_pIEvaluatorClientEKG;

        bool                                    m_bCancelled;
    };


    /*************************************
     *----  VEvaluation::GoferOrder  ----*
     *************************************/

    class VEvaluation::GoferOrder {
	DECLARE_NUCLEAR_FAMILY (GoferOrder);

    //  Construction
    protected:
	GoferOrder () {
	}

    //  Destruction
    protected:
	~GoferOrder () {
	}

    //  Use
    private:
	virtual bool fulfillUsing_(VEvaluation *pEvaluation) const = 0;
    public:
	bool fulfillUsing (VEvaluation *pEvaluation) const {
	    return fulfillUsing_(pEvaluation);
	}
    };

    /***********************************************************************************************
     *----  template <typename gofer_reference_t> VEvaluation::GoferOrder_<gofer_reference_t>  ----*
     ***********************************************************************************************/

    template <typename gofer_reference_t> class VEvaluation::GoferOrder_ : public GoferOrder {
	DECLARE_FAMILY_MEMBERS (GoferOrder_<gofer_reference_t>, GoferOrder);

    //  Construction
    public:
	GoferOrder_(gofer_reference_t &rpResult) : m_rpResult (rpResult) {
	}

    //  Destruction
    public:
	~GoferOrder_() {
	};

    //  Use
    protected:
	bool fulfillUsing_(VEvaluation *pEvaluation) const {
	    return pEvaluation->newClientObjectGofer (m_rpResult);
	}

    //  State
    protected:
	gofer_reference_t &m_rpResult;
    };
}

#endif
