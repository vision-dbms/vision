#ifndef Vsa_VEvaluationResult_Interface
#define Vsa_VEvaluationResult_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vsa_IEvaluation.h"
#include "Vsa_IEvaluationResult.h"

#include "V_VCondvar.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vsa_VEvaluatorClient.h"
#include "Vsa_VRequestMaker_.h"
#include "Vsa_VResultBuilder_.h"


/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    /**********************************
     *---  Vsa::VEvaluationResult  ---*
     **********************************/

    class Vsa_API VEvaluationResult : public Vca::VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (VEvaluationResult, Vca::VRolePlayer);

    //  Aliases
    public:
	typedef ThisClass Result;
	typedef Reference ResultReference;

    //  Builder Aliases
    public:
	typedef VEvaluatorClient BuilderClient;
	typedef VResultBuilder_<ThisClass> Builder;
	friend class VResultBuilder_<ThisClass>;

    //  State
    public:
	enum State {
	    State_New,
	    State_Running,
	    State_Failed,
	    State_Succeeded
	};

    //  Waiter
    public:
	class Vsa_API Waiter : public Vca::VRolePlayer {
	    DECLARE_CONCRETE_RTTLITE (Waiter, Vca::VRolePlayer);

	    friend class VEvaluationResult;

	//  Construction
	public:
	    Waiter (Result* pResult);

	//  Destruction
	protected:
	    ~Waiter ();		// ... allow derived classes

	//  Access
	public:
	    Result* result () const {
		return m_pResult;
	    }

	//  Query
	public:
	    bool hasCompleted () const {
		return m_pResult->hasCompleted ();
	    }
	    bool notCompleted () const {
		return m_pResult->notCompleted ();
	    }

	//  Result Wait
	public:
	    bool wait (unsigned int sMillisecondTimeout);
	protected:
	    void signal ();
	private:
	    void complete (State xState);

	//  Result Wait Callbacks
	private:
	    virtual void onWaitStarted_();
	    virtual void onWaitSignaled_();
	    virtual void onWaitFinished_();

	//  State
	private:
	    ResultReference	const	m_pResult;
	    V::VCondvar			m_iResultWaitCV;
	};
	friend class Waiter;

    //  Construction
    public:
	VEvaluationResult ();

    //  Destruction
    protected:
	~VEvaluationResult ();

    //  Access
    public:
	State state () const {
	    return m_xState;
	}
	IEvaluation *evaluation () const {
	    return m_pEvaluation;
	}
	IEvaluationResult *evaluationObject () const {
	    return m_pEvaluationResult;
	}
	VString const &evaluationOutput () {
	    return m_iEvaluationOutput;
	}
	Vca::IError *error () {
	    return m_pError;
	}
	VString const &errorMessage () {
	    return m_iErrorMessage;
	}

    //  Query
    public:
	bool isNew () const {
	    return m_xState == State_New;
	}
	bool isRunning () const {
	    return m_xState == State_Running;
	}
	bool hasCompleted () const {
	    return m_xState > State_Running;
	}
	bool hasFailed () const {
	    return m_xState == State_Failed;
	}
	bool hasSucceeded () const {
	    return m_xState == State_Succeeded;
	}

	bool notCompleted () const {
	    return !hasCompleted ();
	}

    //  Builder Callbacks
    protected:
	void onAccept (IEvaluation *pEvaluation, Vca::U32 xQueuePosition);
	void onChange (Vca::U32 xQueuePosition);
	void onError  (Vca::IError *pErrorObject, VString const &rErrorMessage);
	void onResult (IEvaluationResult *pResult, VString const &rOutput);
    private:
	void onCompletion (State xCompletionState);
	void setStateTo (State xCompletionState);

    //  Result Wait
    public:
	bool waitForResult (unsigned int sMillisecondTimeout = UINT_MAX);
    private:
	virtual Waiter *newWaiter ();

    //  State
    private:
	State				m_xState;
	unsigned int			m_xQueuePosition;
	IEvaluation::Reference		m_pEvaluation;
	IEvaluationResult::Reference	m_pEvaluationResult;
	VString				m_iEvaluationOutput;
	Vca::IError::Reference		m_pError;
	VString				m_iErrorMessage;
	Waiter::Reference 		m_pWaiter;
    };


    /******************************
     *---  Vsa::VRequestMaker  ---*
     ******************************/

    typedef VRequestMaker_<VEvaluationResult> RequestMaker;
}


#endif
