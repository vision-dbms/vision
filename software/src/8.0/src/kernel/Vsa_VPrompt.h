#ifndef Vsa_VPrompt_Interface
#define Vsa_VPrompt_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_VClientApplication.h"

#include "Vca_VGoferInterface.h"
#include "Vca_VLineGrabber.h"
#include "Vca_VOutputSequencer.h"

#include "Vsa_IEvaluator.h"

/**************************
 *****  Declarations  *****
 **************************/

namespace Vca {
    template <class Actor_T> class VTrigger;
}


/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    class VPrompt : public Vca::VClientApplication::Activity {
	DECLARE_CONCRETE_RTTLITE (VPrompt, Vca::VClientApplication::Activity);

	friend class Vca::VLineGrabber_<ThisClass>;

    //  Aliases
    public:
	typedef Vca::IClient		IClient;
	typedef Vca::IError		IError;
	typedef Vca::IRoleProvider	IRoleProvider;

	typedef Vca::VBSConsumer	VBSConsumer;
	typedef Vca::VClientApplication	VClientApplication;
	typedef Vca::VOutputSequencer	Sequencer;

	typedef Vca::VLineGrabber_<ThisClass> LineGrabber;
	typedef Vca::VTrigger<ThisClass> CallbackTrigger;

	typedef Vca::VGoferInterface<IEvaluator> EvaluatorGofer;
	typedef Vca::VGoferInterface<IRoleProvider> RoleGofer;

    //  Output Types
    public:
	class QueryOutput;
	friend class QueryOutput;

    //  Construction
    public:
	VPrompt (
	    VClientApplication* pApplication, EvaluatorGofer* pEvaluatorGofer, IEvaluator* pEvaluator, VBSConsumer* pStreamToPeer
	);

    //  Destruction
    private:
	~VPrompt ();

    //  Access/Query
    public:
	RoleGofer* queryRoleGofer() const;

	IEvaluator* evaluator () const {
	    return m_pEvaluator;
	}
	EvaluatorGofer *evaluatorGofer () const {
	    return m_pEvaluatorGofer;
	}
	Sequencer *outputSequencer () const {
	    return m_pOutputSequencer;
	}
	bool usingExtendedPrompts () const {
	    return m_bUsingExtendedPrompts;
	}
	bool usingAnyData () const {
	    return m_bUsingAnyData;
	}

    //  Control
    public:
	void setUsingAnyDataTo (bool bTrue) {
	    m_bUsingAnyData = bTrue;
	}
	void setUsingExtendedPromptsTo (bool bTrue) {
	    m_bUsingExtendedPrompts = bTrue;
	}

    //  Input Processing
    private:
	bool onInputLine (char const *pLine, size_t sLine);
	void onInputDone () {
	    outputEnd ();
	}

    //  Output Processing
    private:
	void onEndTrigger(CallbackTrigger* pCallbackTrigger);

	void outputEnd () const;
	void outputPrompt () const;
	void outputQuery (VString const &rQuery);
	void outputQueryTiming () const;
	void outputString (VString const &rString) const;

    //  State
    private:
	IEvaluator::Reference	  const	m_pEvaluator;
	EvaluatorGofer::Reference const	m_pEvaluatorGofer;
	Sequencer::Reference	  const	m_pOutputSequencer;
	VString                   const m_iQueryRoleProviderName;
	VString				m_iPath;
	VString				m_iThisQuery;
	VString				m_iLastQuery;
	VReference<QueryOutput>		m_pLastQuery;
	bool				m_bUsingExtendedPrompts;
	bool				m_bUsingAnyData;
    };
}


#endif
