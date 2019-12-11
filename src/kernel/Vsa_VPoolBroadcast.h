#ifndef Vsa_VPoolBroadcast_Interface
#define Vsa_VPoolBroadcast_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"
#include "Vsa_VPathQuery.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vsa_IEvaluatorClient.h"

namespace Vca {
    class IError;
    class IClient;
}

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {

    class IEvaluation;
    class IEvaluationResult;
    class VEvaluatorPool;
    class VPoolEvaluation;

    /****************************
     *----  VPoolBroadcast  ----*
     ****************************/

    class Vsa_API VPoolBroadcast : public Vca::VRolePlayer {
    //  Run Time Type
	DECLARE_CONCRETE_RTTLITE (VPoolBroadcast, Vca::VRolePlayer);

    //  Friends
	friend class VEvaluatorPool;
	friend class VPoolWorkerGeneration;

    //  Query Type
    public:
	enum QueryType {
	    QueryType_Expression, QueryType_URL
	};

    //  Construction
    public:
	VPoolBroadcast (	//  ... Expression
	    VEvaluatorPool*	pPool,
	    IEvaluatorClient*	pClient,
	    VString const&	rPath,
	    VString const&	rQuery
	);
	VPoolBroadcast (	//  ... URL
	    VEvaluatorPool*	pPool,
	    IEvaluatorClient*	pClient,
	    VString const&	rPath,
	    VString const&	rQuery,
	    VString const&	rEnvironment
	);

    //  Destruction
    private:
	~VPoolBroadcast ();

    //  IEvaluatorClient Role
    private:
	Vca::VRole<ThisClass,IEvaluatorClient> m_pIEvaluatorClient;
    public:
	void getRole (VReference<IEvaluatorClient>&rpRole);

    //  IEvaluatorClient Methods
    public:
	void OnAccept (
	    IEvaluatorClient *pRole, IEvaluation *pEvaluation, Vca::U32 xQueuePosition
	);
	void OnChange (
	    IEvaluatorClient *pRole, Vca::U32 xQueuePosition
	);
	void OnResult (
	    IEvaluatorClient *pRole, IEvaluationResult *pResult, VString const &rOutput
	);

    //  IClient Methods
	virtual void OnError (
	    Vca::IClient *pRole, Vca::IError *pError, VString const &rText
	) OVERRIDE;

    //  Query
    public:
	void getResult (VString &rResult) const {
	    rResult.setTo (m_iResult);
	}
	QueryType type () const {
	    return m_xQueryType;
	}
	VPathQuery* pathQuery () const {
	    return m_iPathQuery;
	}
	VEvaluatorPool* pool () const {
	    return m_pPool;
	}

    //  Update
    public:
	void incrementExpectedReplies () {
	    m_cExpectedReplies++;
	}
	void decrementExpectedReplies () {
	    m_cExpectedReplies--;
	}
    
    //  Callback    
    public:
	void OnEvaluationCancelled (VString const& rMessage);


    private:
    //  Creating Evaluations
	VPoolEvaluation* createBCEvaluationFor (Vca::U32 iWorkerId);

    //  Completion
	void onComplete ();


    //  State
    protected:
	VReference<VEvaluatorPool>	    m_pPool;
	VReference<IEvaluatorClient> const  m_pClient;
	VReference<VPathQuery>		    m_iPathQuery;
	QueryType		     const  m_xQueryType;

	VString				    m_iResult;
	Vca::U32			    m_cExpectedReplies;

	VReference<ThisClass>		    m_pSuccessor;
	VReference<ThisClass>		    m_pPredecessor;
    };
}


#endif
