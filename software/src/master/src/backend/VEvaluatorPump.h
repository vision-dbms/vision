#ifndef VEvaluatorPump_Interface
#define VEvaluatorPump_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vsa_VEvaluatorPump.h"
#include "Vsa_VGenericEvaluation.h"

/*************************
 *****  Definitions  *****
 *************************/

class VEvaluatorPump : public Vsa::VEvaluatorPump {
    DECLARE_CONCRETE_RTTLITE (VEvaluatorPump, Vsa::VEvaluatorPump);

//  Aliases
public:
    typedef V::VString VString;
    typedef Vsa::IEvaluatorClient IEvaluatorClient;
    typedef Vsa::VEvaluation VEvaluation;

/****************************************
 *----  VEvaluatorPump::Evaluation  ----*
 ****************************************/
public:
    class Evaluation : public Vsa::VGenericEvaluation {
	DECLARE_CONCRETE_RTTLITE (Evaluation, VGenericEvaluation);

    //  Construction
    public:
	Evaluation (
	    VEvaluator *pEvaluator, IEvaluatorClient *pClient, VString const &rPath, VString const &rExpression
	);
	Evaluation (
	    VEvaluator *pEvaluator, IEvaluatorClient *pClient, VString const &rPath, VString const &rQuery, VString const &rEnvironment
	);

    //  Destruction
    private:
	~Evaluation ();
    };


/****************************************
 *----  VEvaluatorPump (continued)  ----*
 ****************************************/

//  Construction
public:
    VEvaluatorPump (
        PumpSettings *pSettings, Vca::VBSConsumer *pPipeToPeer, Vca::VBSProducer *pPipeToHere
    );

//  Destruction
private:
    ~VEvaluatorPump ();

//  Evaluation
protected:
    virtual VEvaluation* createEvaluation (
	IEvaluatorClient *pClient, VString const &rPath, VString const &rExpression
    ) OVERRIDE;
    virtual VEvaluation* createEvaluation (
	IEvaluatorClient* pClient, VString const &rPath, VString const &rQuery, VString const &rEnvironment
    ) OVERRIDE;

//  Cancellation
protected:
    virtual void cancelCurrent () OVERRIDE;
};

#endif
