/*****  Batchvision_Interface  *****/
#ifndef Batchvision_Interface
#define Batchvision_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VServerApplication.h"
#include "VComputationScheduler.h"

#include "VTransientServicesForBatchvision.h"
#include "VReadEvalPrint.h"

#include "VEvaluatorPump.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IDirectoryEntry.h"

namespace Vca {
    class IStdPipeSource;
}

/*************************
 *****  Definitions  *****
 *************************/

class Batchvision : public Vca::VServerApplication, public VComputationScheduler::Manager {
    DECLARE_CONCRETE_RTT (Batchvision, Vca::VServerApplication);
    DECLARE_ROLEPLAYER_CLASSINFO ();

//  Aliases
public:
    typedef Vca::VBSConsumer			VBSConsumer;
    typedef Vca::VBSProducer			VBSProducer;

    typedef Vca::IDirectoryEntry		IDirectoryEntry;
    typedef IVReceiver<IDirectoryEntry*>	IDirectoryEntrySink;
    typedef Vca::IStdPipeSource			IStdPipeSource;

    typedef Vsa::IEvaluator			IEvaluator;
    typedef Vsa::IEvaluatorClient		IEvaluatorClient;

//  Instance
private:
    static Reference g_pTheInstance;
public:
    static ThisClass* Instance () {
	return g_pTheInstance;
    }

//  Main
public:
    class Main;
    friend class Main;

//  Plumber
public:
    class Plumber;
    friend class Plumber;

//  Construction
public:
    Batchvision (Context *pContext);

//  Destruction
private:
    ~Batchvision ();

//  Roles
public:
    using BaseClass::getRole;

//  IDirectoryEntrySink
private:
    Vca::VRole<ThisClass,IDirectoryEntrySink> m_pIDirectoryEntrySink;
public:
    void getRole (IDirectoryEntrySink::Reference &rpRole) {
	m_pIDirectoryEntrySink.getRole (rpRole);
    }

//  IDirectoryEntrySink Callbacks
public:
    void OnData (IDirectoryEntrySink *pRole, IDirectoryEntry* pSubscription);

//  IEvaluator
private:
    Vca::VRole<ThisClass,IEvaluator> m_pIEvaluator;
public:
    void getRole (IEvaluator::Reference &rpRole) {
	m_pIEvaluator.getRole (rpRole);
    }

//  IEvaluator Callbacks
public:
    void EvaluateExpression (
	IEvaluator *pRole, IEvaluatorClient *pClient, VString const &rPath, VString const &rExpression
    );
    void EvaluateURL (
	IEvaluator *pRole, IEvaluatorClient *pClient, VString const &rPath, VString const &rQuery, VString const &rEnvironment
    );

//  Evaluator Materialization
private:
    bool evaluatorMaterialized ();

//  Access
public:
    static IOMDriver* ActiveChannel () {
	return g_pTheInstance ? g_pTheInstance->activeChannel () : 0;
    }
    static size_t BlockingWait () {
	return g_pTheInstance ? g_pTheInstance->blockingWait () : 0;
    }
    static unsigned int PollingInterval () {
	return g_pTheInstance ? g_pTheInstance->pollingInterval () : 0;
    }

//  Update
public:
    static void SetPollingIntervalTo (unsigned int sPollingInterval) {
	if (g_pTheInstance)
	    g_pTheInstance->setPollingIntervalTo (sPollingInterval);
    }

//  Control
private:
    void doConnect (IStdPipeSource *pStdPipeSource);

    bool start_();
    bool stop_(bool bHardstop);

//  Process Management
public:
    static void RestartProcess ();
    void restartProcess ();

    int getExitValue ();

    void setExitValue (int value);

//  Logging Control
private:
    VReadEvalPrintController::Reference m_pREP;
public:
    static void registerREPController (VReadEvalPrintController *pREP);
    void setREPController (VReadEvalPrintController *pREP);

    static void completeUsageLog ();
    void logUsage ();

//  Debugger Management
public:
    static void ToggleDebuggerEnabled ();

//  State
private:
    VEvaluatorPump::Reference m_pEvaluator;
    IEvaluator::Reference m_pEvaluatorImplementation;
};

extern VTransientServicesForBatchvision ThisProcess;


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  main.h 1 replace /users/jad
  860417 15:58:50 jad added hooks to make the rs work from the editor

 ************************************************************************/
/************************************************************************
  main.h 2 replace /users/jck/source
  860417 23:58:57 jck Corrected spelling of 'Delimiter'

 ************************************************************************/
/************************************************************************
  main.h 3 replace /users/jad/system
  860425 12:06:43 jad removed the MAIN_Ed_Delimiter define - no longer needed

 ************************************************************************/
/************************************************************************
  main.h 4 replace /users/jad/system
  870317 12:29:12 jad modify interrupt handling

 ************************************************************************/
/************************************************************************
  main.h 5 replace /users/mjc/translation
  870524 09:40:07 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
