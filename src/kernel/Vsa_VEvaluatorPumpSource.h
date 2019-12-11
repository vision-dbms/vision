#ifndef Vsa_VEvaluatorPumpSource_Interface
#define Vsa_VEvaluatorPumpSource_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vsa.h"

/************************
 *****  Components  *****
 ************************/

#include "Vsa_VEvaluatorSource.h"

#include "Vca_IPipeSourceClient.h"

/**************************
 *****  Declarations  *****
 **************************/

namespace Vca {
    class IPipeSource;
}

#include "Vsa_VEvaluatorPump.h"


/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    class Vsa_API VEvaluatorPumpSource : public VEvaluatorSource {
    //  Run Time Type
	DECLARE_CONCRETE_RTTLITE (VEvaluatorPumpSource, VEvaluatorSource);

    //  EvaluatorSettings
    public:
	typedef VEvaluatorPump::PumpSettings EvaluatorSettings;

    //  Agents
    public:
	class Agent;
	friend class Agent;

	class Plumber;
	friend class Plumber;

	class Pumper;
	friend class Pumper;

    //  Construction
    public:
	VEvaluatorPumpSource (
	    EvaluatorSettings *pEvaluatorSettings, Vca::IPipeSource *pPipeSource,
	    VString const &rStarupQuery, bool bPlumbed = false
	);
	VEvaluatorPumpSource (
	    EvaluatorSettings *pEvaluatorSettings, Vca::IPipeSource *pPipeSource,
	    bool bPlumbed = false
	);

    //  Destruction
    private:
	~VEvaluatorPumpSource ();

    //  Access/Query
    public:
	EvaluatorSettings *evaluatorSettings () const {
	    return m_pEvaluatorSettings;
	}
	Vca::IPipeSource *pipeSource () const {
	    return m_pPipeSource;
	}
	bool plumbed () const {
	    return m_bPlumbed;
	}

    //  Implementation
    public:
	virtual void Supply (IEvaluatorSource *pRole, IEvaluatorSink *pSink) OVERRIDE;

    //  Factory Callbacks
    protected:
	void onData (IEvaluatorSink *pEvaluatorSink, IEvaluator *pEvaluator);
	void onData (
	    IEvaluatorSink *pEvaluatorSink,
	    Vca::VBSConsumer *pPipeToPeer,
	    Vca::VBSProducer *pPipeToHere,
	    Vca::VBSProducer *pErrorPipeToHere
	);

    //  State
    private:
	EvaluatorSettings::Reference	m_pEvaluatorSettings;
	VReference<Vca::IPipeSource>	m_pPipeSource;
	bool				m_bPlumbed;
    };
}


#endif
