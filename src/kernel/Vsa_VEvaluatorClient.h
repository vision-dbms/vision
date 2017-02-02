#ifndef Vsa_VEvaluatorClient_Interface
#define Vsa_VEvaluatorClient_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VActivity.h"

#include "Vca_VInterfaceEKG.h"

#include "Vsa_IEvaluatorClient.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    class IEvaluator;

    class Vsa_API VEvaluatorClient : public Vca::VActivity {
	DECLARE_ABSTRACT_RTTLITE (VEvaluatorClient, VActivity);

    //  Aliases
    public:
	typedef Vca::VInterfaceEKG<ThisClass,IVUnknown> interface_monitor_t;

    //  Construction
    protected:
	VEvaluatorClient (Vca::VCohort *pCohort);
	VEvaluatorClient ();

    //  Destruction
    protected:
	~VEvaluatorClient ();

    //  Base Roles
    public:
	using BaseClass::getRole;

    //  IEvaluatorClient Role
    private:
	Vca::VRole<ThisClass,IEvaluatorClient> m_pIEvaluatorClient;
    protected:
	void getRole (IEvaluatorClient::Reference &rpRole) {
	    m_pIEvaluatorClient.getRole (rpRole);
	}

    //  IEvaluatorClient Methods
    public:
	virtual void OnAccept (IEvaluatorClient *pRole, IEvaluation *pEvaluation, Vca::U32 xQueuePosition);
	virtual void OnChange (IEvaluatorClient *pRole, Vca::U32 xQueuePosition);
	virtual void OnResult (IEvaluatorClient *pRole, IEvaluationResult *pResult, VString const &rOutput);

    //  Override these virtuals to customize the behavior of your subclass.
    private:
	virtual void OnAccept_(IEvaluation *pEvaluation, Vca::U32 xQueuePosition);
	virtual void OnChange_(Vca::U32 xQueuePosition);
	virtual void OnResult_(IEvaluationResult *pResult, VString const &rOutput) = 0;

    //  IClient Methods
    protected:
	virtual void OnError_(Vca::IError *pInterface, VString const &rMessage);

    //  Evaluation Control
    public:
	void setUsingAnyDataTo (bool bValue) {
	    m_bUsingAnyData = bValue;
	}
	bool usingAnyData () const {
	    return m_bUsingAnyData;
	}

    //  Evaluation
    public:
	bool onQuery (IEvaluator *pEvaluator, VString const &rPath, VString const &rQuery, VString const &rEnvironment);
	bool onQuery (IEvaluator *pEvaluator, VString const &rPath, VString const &rQuery);
	bool onQuery (IEvaluator *pEvaluator, VString const &rQuery);

	void onError (Vca::IError *pInterface, VString const &rMessage);
    private:
	bool goodToGo (IEvaluator *pEvaluator, IEvaluatorClient::Reference &rpRole);

    //  Monitoring
    private:
	void monitorInterface (IVUnknown *pInterface);
	void cancelInterfaceMonitor ();
	void signalInterfaceMonitor ();

    //  State
    private:
	interface_monitor_t::Reference m_pInterfaceMonitor;
	bool m_bUsingAnyData;
    };
}


#endif
