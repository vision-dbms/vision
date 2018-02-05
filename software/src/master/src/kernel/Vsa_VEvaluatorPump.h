#ifndef Vsa_VEvaluatorPump_Interface
#define Vsa_VEvaluatorPump_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_VEvaluator.h"

#include "Vsa_VPathQuery.h"

#include "Vca_IBSConsumerClient.h"
#include "Vca_IBSProducerClient.h"

#include "V_VAdaptiveAllocator.h"
#include "V_VFifoLite.h"

#include "V_VString.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"

#include "Vsa_IEvaluatorPump.h"
#include "Vsa_IEvaluatorPumpSettings.h"

#include "Vsa_VEvaluation.h"


/*************************
 *****  Definitions  *****
 *************************/

#ifdef __VMS
#define PumpSettings Settings
#endif

namespace Vsa {
    class Vsa_API VEvaluatorPump : public VEvaluator {
	DECLARE_CONCRETE_RTTLITE (VEvaluatorPump, VEvaluator);

    //  Aliases
    public:
	typedef IVReceiver<IVUnknown*> IObjectReceiver;

    //  Byte Stream Types
    public:
	typedef Vca::IBSClient IBSClient;

	typedef Vca::VBSConsumer BSConsumer;
	typedef Vca::IBSConsumerClient BSConsumerClient;

	typedef Vca::VBSProducer BSProducer;
	typedef Vca::IBSProducerClient BSProducerClient;

    //  Incoming Queue
    public:
	class IQueue {
	//  Entry
	public:
	    class Entry : public VReferenceable {
		DECLARE_CONCRETE_RTTLITE (Entry, VReferenceable);

	    //  Friends
		friend class IQueue;

	    //  Construction
	    public:
		Entry (VEvaluation *pRequest, unsigned int cPrompts);

	    //  Destruction
	    private:
		~Entry ();

	    //  Access
	    public:
		Entry *get (VEvaluation::Reference &rpRequest, unsigned int &rcPrompts) const;

	    //  State
	    private:
		VEvaluation::Reference	const	m_pRequest;
		unsigned int		const	m_cPrompts;
		Reference			m_pSuccessor;
	    };
	    friend class Entry;

	//  Construction
	public:
	    IQueue () : m_sQueue (0) {
	    }

	//  Destruction
	public:
	    ~IQueue () {
	    }

	//  Query		
	private:
	    Vca::U32 length () const {
		return m_sQueue;
	    }

	//  Linkage
	public:
	    void enqueue (VEvaluation *pRequest, unsigned int cPrompts);
	    void dequeue (VEvaluation::Reference &rpRequest, unsigned int &rcPrompts);

	//  Error
	public:
	    void onError (Vca::IError *pError, VString const &rMessage);

	//  State
	private:
	    Entry::Reference m_pHead;
	    Entry::Reference m_pTail;
	    Vca::U32	     m_sQueue;
	};


    //  PumpSettings
    public:
	class Vsa_API PumpSettings : public VEvaluator::Settings {
	    DECLARE_CONCRETE_RTTLITE (PumpSettings, VEvaluator::Settings);

	//  Prompt Set
	public:
	    class Vsa_API PromptSet : public VTransient {
	    //  Prompt
	    public:
		class Vsa_API Prompt : public VTransient {
		//  Construction
		public:
		    Prompt (char const *pText)
			: m_pText (pText), m_sText (strlen (pText))
		    {
		    }

		//  Destruction
		public:
		    ~Prompt () {
		    }

		//  Access
		public:
		    size_t length () const {
			return m_sText;
		    }
		    char const *text () const {
			return m_pText;
		    }

		//  Location
		public:
		    bool locate (	// returns true if found, false otherwise
			char const *pText, size_t &rxOrigin, size_t &rsSpan
		    ) const;

		//  State
		private:
		    char const*	m_pText;
		    size_t	m_sText;
		};

	    //  Construction
	    private:
		Prompt* newPromptArray (char const *pPrompts[]);

	    public:
		PromptSet (unsigned int cPrompts, char const *pPrompts[]);

	    //  Destruction
	    public:
		~PromptSet () {
		}

	    //  Access
	    public:
		unsigned int cardinality () const {
		    return m_cPrompts;
		}
		size_t maximumSpan () const {
		    return m_sMaximumSpan;
		}
		size_t maximumSpanCalc () const;

	    //  Location
	    public:
		bool locate (	// returns true if found, false otherwise
		    char const *pText, size_t &rxOrigin, size_t &rsSpan
		) const;

	    //  State
	    private:
		unsigned int const	m_cPrompts; //  must precede m_pPrompts and m_sMaximumSpan
		Prompt const*const	m_pPrompts;
		size_t const		m_sMaximumSpan;
	    };

	//  Globals
	private:
	    static PromptSet const *standardPromptSet ();
	    static PromptSet const *extendedPromptSet ();

	    static VPathQuery::Formatter const g_iQueryFormatter;

	//  Construction
	public:
	    PumpSettings (PumpSettings const &rOther);
	    PumpSettings ();

	//  Destruction
	public:
	    ~PumpSettings () {
	    }

	//  IEvaluatorPumpSettings Role
	private:
	    Vca::VRole<ThisClass,IEvaluatorPumpSettings> m_pIEvaluatorPumpSettings;
	public:
	    void getRole (IEvaluatorPumpSettings::Reference &rpRole) {
		m_pIEvaluatorPumpSettings.getRole (rpRole);
	    }

	//  IEvaluatorPumpSettings Methods
	public:
	    void MakeControl (
		IEvaluatorPumpSettings*		pRole,
		IVReceiver<IEvaluatorPump*>*	pClient,
		Vca::VBSConsumer*		pPipeToPeer,
		Vca::VBSProducer*		pPipeToHere
	    );
	    void MakeSource (
		IEvaluatorPumpSettings*		pRole,
		IVReceiver<IEvaluatorSource*>*	pClient,
		Vca::IPipeSource*		pPipeSource
	    );
	    void GetURLClass (
		IEvaluatorPumpSettings *pRole, IVReceiver<VString const&> *pClient
	    );
	    void SetURLClass (IEvaluatorPumpSettings *pRole, VString const &rValue);
	    void GetURLTemplate (
		IEvaluatorPumpSettings *pRole, IVReceiver<VString const&> *pClient
	    );
	    void SetURLTemplate (IEvaluatorPumpSettings *pRole, VString const &rValue);
	    void GetUsingExtendedPrompts (
		IEvaluatorPumpSettings *pRole, IVReceiver<bool> *pClient
	    );
	    void SetUsingExtendedPrompts (IEvaluatorPumpSettings *pRole, bool bValue);

	//  Access
	public:
	    void getSetupExpression (VString &rResult) const;

	    VString const &urlClass () const {
		return m_iQueryFormatter.getClass ();
	    }
	    VString const &urlTemplate () const {
		return m_iQueryFormatter.getTemplate ();
	    }

	    bool usingStandandPrompts () const {
		return !m_bUsingExtendedPrompts;
	    }
	    bool usingExtendedPrompts () const {
		return m_bUsingExtendedPrompts;
	    }

	    PromptSet const *promptSet () const {
		return m_bUsingExtendedPrompts ? extendedPromptSet () : standardPromptSet ();
	    }

	//  Update
	public:
	    void setURLClassTo (char const *pValue) {
		m_iQueryFormatter.setClassTo (pValue);
	    }
	    void setURLClassTo (VString const &rValue) {
		m_iQueryFormatter.setClassTo (rValue);
	    }
	    void setURLTemplateTo (char const *pValue) {
		m_iQueryFormatter.setTemplateTo (pValue);
	    }
	    void setURLTemplateTo (VString const &rValue) {
		m_iQueryFormatter.setTemplateTo (rValue);
	    }

	    void setUsingExtendedPromptsTo (bool bValue) {
		m_bUsingExtendedPrompts = bValue;
	    }

	//  Use
	public:
	    void getVisionExpression (VEvaluation *pEvaluation, VString &rExpression) const;

	//  State
	protected:
	    VPathQuery::Formatter	m_iQueryFormatter;
	    bool			m_bUsingExtendedPrompts;
	};
	typedef PumpSettings::PromptSet PromptSet;


    //  Construction
    public:
	VEvaluatorPump (
	    PumpSettings *pSettings, Vca::VBSConsumer *pPipeToPeer, Vca::VBSProducer *pPipeToHere
	);

    //  Destruction
    protected:
	~VEvaluatorPump ();

    //  Base Class Roles
    public:
	using BaseClass::getRole;

    //  Byte Stream Client Roles
    private:
	Vca::VRole<ThisClass,Vca::IBSConsumerClient> m_pBSConsumerClient;
	void getRole (BSConsumerClient::Reference &rpRole) {
	    m_pBSConsumerClient.getRole (rpRole);
	}

	Vca::VRole<ThisClass,Vca::IBSProducerClient> m_pBSProducerClient;
	void getRole (BSProducerClient::Reference &rpRole) {
	    m_pBSProducerClient.getRole (rpRole);
	}

    //  IEvaluatorPump Role
    private:
	Vca::VRole<ThisClass,IEvaluatorPump> m_pIEvaluatorPump;
    public:
	void getRole (IEvaluatorPump::Reference &rpRole) {
	    m_pIEvaluatorPump.getRole (rpRole);
	}

    //  IEvaluatorPump Methods
    public:
	void GetSettings (IEvaluatorPump *pRole, IVReceiver<IEvaluatorPumpSettings*>*pClient);

    //  IEvaluatorControl Methods:
    public:
	void GetStatistics (
	    IEvaluatorControl *pRole, IVReceiver<VString const &> *pReceiver
	);

    //  Access/Query
    private:
	char *incomingFreeArea () const {
	    return m_pIncomingString + m_sIncomingString;
	}
	size_t incomingFreeAreaSize () const {
	    return m_iIncomingString.storageSize () - m_sIncomingString - 1;
	}

    public:
	VEvaluation *incomingClient () const {
	    return m_pIncomingClient;
	}

	bool incomingPumpIsBusy () const {
	    return m_pBSProducer.isNil ()
		|| m_cIncomingPrompts > 0
		|| m_pIncomingClient.isntNil ();
	}
	bool incomingPumpIsIdle () const {
	    return m_pBSProducer.isntNil ()
		&& m_cIncomingPrompts == 0
		&& m_pIncomingClient.isNil ();
	}

    public:
	bool outgoingPumpIsBusy () const {
	    return m_pBSConsumer.isNil () || m_iOutgoingFifo.consumerAreaSize () > 0;
	}
	bool outgoingPumpIsIdle () const {
	    return m_pBSConsumer.isntNil () && m_iOutgoingFifo.consumerAreaSize () == 0;
	}

    //  Callbacks
    public/*private*/:
	void OnTransfer (BSConsumerClient *pRole, size_t sTransfer);
	void OnTransfer (BSProducerClient *pRole, size_t sTransfer);

        void OnEnd (Vca::IClient *pRole) OVERRIDE {
            OnError (pRole, 0, "Process Terminated");
        }
	void OnError (
	    Vca::IClient *pRole, Vca::IError *pError, VString const &rDescription
	) OVERRIDE;

    //  Execution
    private:
	void onQueue_ () OVERRIDE;

	void primeOutgoingPump ();
	void crankOutgoingPump (IBSClient *pRole);

	void primeIncomingPump ();
	void crankIncomingPump (IBSClient *pRole, size_t sTransfer);

    //  Execution Support
    private:
	void resetIncomingData ();

	void resetIncomingFifo ();
	void resetOutgoingFifo ();

    //  Cancellation
    protected:
        bool cancel_ (VEvaluation *pEvaluation) OVERRIDE;
        virtual void cancelCurrent () {}

    //  State
    private:
	PumpSettings::Reference	m_pSettings;
	PromptSet const *const	m_pPromptSet;
	BSConsumer::Reference	m_pBSConsumer;
	BSProducer::Reference	m_pBSProducer;
	V::VFifoLite		m_iOutgoingFifo;
	VString			m_iOutgoingString;
	bool			m_bOutgoingBody;
	bool			m_bOutgoingBodyNeedsNewLine;
	bool			m_bCrankingIncomingPump;
	bool			m_bCrankingOutgoingPump;
	IQueue			m_iIncomingQueue;
	VEvaluation::Reference	m_pIncomingClient;
	V::VAdaptiveAllocator	m_iIncomingAllocator;
	V::VFifoLite		m_iIncomingFifo;
	VString			m_iIncomingOutput;
	VString			m_iIncomingString;
	char*			m_pIncomingString;
	size_t			m_sIncomingString;
	unsigned int		m_cIncomingPrompts;
	char			m_iIncomingBuffer[4096];
    };
}


#endif
