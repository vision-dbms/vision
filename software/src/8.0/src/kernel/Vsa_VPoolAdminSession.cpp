/*****  VPoolAdminSession main  *****/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "Vsa_VPoolAdminSession.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VSimpleFile.h"

#include "Vsa_IEvaluator.h"
#include "Vsa_IEvaluatorPool_Ex4.h"
#include "Vsa_IPoolEvaluation.h"
#include "Vsa_IEvaluatorPoolSettings_Ex6.h"
#include "Vsa_IPoolWorker.h"
#include "Vsa_IPoolApplication_Ex2.h"

#include "Vca_IPipeFactory.h"
#include "Vca_VInterfaceQuery.h"
#include "Vca_VTimer.h"
#include "Vca_VTrigger.h"
#include "Vca_VcaGofer.h"

#include "Vsa_VPoolAdmin.h"

/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

char const *Vsa::VPoolAdminSession::g_pMessages [] = {
  "Error: This operation is not supported by this pool",
  "Error: Null Evaluator",
  "Error: End Of Data (EOD)",
  "Error: Query Timedout",
  "Connection Error: ",
  "Error: No Object At Address",
  "Error: Session has Null IPoolApplication Interface",
  "Error: Session has Null IPoolApplication_Ex1 Interface",
  "Error: Session has Null IEvaluatorPool Interface",
  "Error: Session has Null IEvaluator Interface",
  "Error: Session has Null IEvaluatorPoolSettings Interface",
  "Error: Session has Null Worker Interface",
  "Error: Invalid Broadcast File",
  "Error: Invalid Worker Starup File",
  "Error: Invalid Worker PID Query File",
  "Error: Invalid Query File",
  "Error: Operation Failed",
  "Error: Worker creation failure limit reached",
  "Message sent",
  "Operation Succeeded",
  "Worker creation failure limit  not reached",
  "Error: Session has Null IGetter Interface"
};

/*******************************************
 *******************************************
 *****  Vsa::VPoolAdminSession::Query  *****
 *******************************************
 *******************************************/

Vsa::VPoolAdminSession::Query::Query (
    Session *pSession, VString const &rQuery, Type iType, bool bAnyData,
    Vca::U64 iTimeOutSeconds, VString const& iWorkerId, bool bIsPID
) : m_iQuery    (rQuery)
  , m_iTimeOut  (iTimeOutSeconds)
  , m_bTimedOut (false)
  , m_bAnyData  (bAnyData)
  , m_iType     (iType)
  , m_iWorkerId (iWorkerId)
  , m_bIsPID (bIsPID)
  , m_pSession  (pSession)
  , m_pIEvaluatorClient (this) {
}

Vsa::VPoolAdminSession::Query::~Query () {
}


/********************
 ****   Start    ****
 ********************/

void Vsa::VPoolAdminSession::Query::start () {

    IEvaluator::Reference pEvaluator (m_pSession->evaluator ());
    IEvaluatorPool::Reference pEvaluatorPool (m_pSession->poolEvaluator ());
    IEvaluatorPoolSettings::Reference pEvaluatorPoolSettings (m_pSession->poolSettings ());

    IEvaluatorClient::Reference pClient;
    getRole (pClient);

    switch (m_iType) {

    case Query_Ping: {
        if (pEvaluator) {
            link ();
            setTimeout ();
            m_pSession->m_bSuspend = true;
            pEvaluator->EvaluateExpression (pClient, "", m_iQuery);
        }
        else
            m_pSession->displayResult (g_pMessages[Null_Evaluator]);
    }
        break;

    case Query_Broadcast:
        if (pEvaluatorPool) {
            if (IEvaluatorPool_Ex2 *const pPool = dynamic_cast<IEvaluatorPool_Ex2*>(pEvaluatorPool.referent ())) {
                link ();
                setTimeout ();
                m_pSession->m_bSuspend = true;
                pPool->BroadcastExpression (pClient, "", m_iQuery);
            }
            else
                m_pSession->displayResult (g_pMessages[Pool_Operation_Not_Supported]);
        }
        else
            m_pSession->displayResult (g_pMessages[Null_IEvaluatorPool]);
        break;

    case Query_Offline_Worker:
        if (pEvaluatorPool) {
            if (IEvaluatorPool_Ex3 *const pPool = dynamic_cast<IEvaluatorPool_Ex3*>(pEvaluatorPool.referent ())) {
                setTimeout ();
                m_pSession->m_bSuspend = true;
                pPool->EvaluateUsingOfflineWorker (m_iQuery, m_iWorkerId, m_bIsPID, pClient);
           }
           else
                m_pSession->displayResult (g_pMessages[Pool_Operation_Not_Supported]);
        }
        else
            m_pSession->displayResult (g_pMessages[Null_IEvaluatorPool]);
        break;

    case Query_WorkerStartup:
        if (pEvaluatorPoolSettings) {
            pEvaluatorPoolSettings->SetWorkerStartupQuery (m_iQuery);
            m_pSession->displayResult (g_pMessages[Message_Sent]);
        }
        else
            m_pSession->displayResult (g_pMessages[Null_IEvaluatorPoolSettings]);
        break;

    default:
        break;
    }
}

/***************************************
 ****   IEvaluatorClient Methods    ****
 ***************************************/

void Vsa::VPoolAdminSession::Query::OnResult (
    IEvaluatorClient  *pRole, IEvaluationResult *pResult, VString const &rOutput
) {
    if (!m_bTimedOut) {
        m_pSession->displayResult (rOutput.content ());

        if (m_pTimer) {
            m_pTimer->cancel ();
            m_pTimer.clear ();
        }
        m_pSession->onOperationComplete ();
    }
    unlink ();
}

void  Vsa::VPoolAdminSession::Query::OnAccept (
    IEvaluatorClient *pRole, IEvaluation *pEvaluation, Vca::U32 xQueuePosition
) {
    IPoolEvaluation::Reference const pPoolEvaluation (dynamic_cast <IPoolEvaluation*> (pEvaluation));
    if (pPoolEvaluation && m_bAnyData) {
        pPoolEvaluation->UseAnyGenerationWorker ();
    }
}

/*******************************
 ****    IClient Methods    ****
 *******************************/

void Vsa::VPoolAdminSession::Query::OnError (
    Vca::IClient *pRole, Vca::IError *pError, VString const &rMessage
) {
    VString iMsg ("Error:"); iMsg << rMessage;
    m_pSession->displayResult (iMsg);
    if (m_pTimer) {
        m_pTimer->cancel ();
        m_pTimer.clear ();
    }
    m_pSession->onOperationComplete ();
    unlink ();
}

void Vsa::VPoolAdminSession::Query::OnEnd (Vca::IClient *pRole) {
    m_pSession->displayResult (g_pMessages[End_Of_Data]);
    m_pSession->onOperationComplete ();
    unlink ();
}

/***********************
 ****    Linkage    ****
 ***********************/

void Vsa::VPoolAdminSession::Query::link () {
    if (m_pSession->m_pQueryHead) {
        m_pSession->m_pQueryHead->m_pPredecessor.setTo (this);
        m_pSuccessor.setTo (m_pSession->m_pQueryHead);
        m_pSession->m_pQueryHead.setTo (this);
    }
    else
        m_pSession->m_pQueryHead.setTo (this);
}

void Vsa::VPoolAdminSession::Query::unlink () {
    if (m_pSuccessor)
        m_pSuccessor->m_pPredecessor = m_pPredecessor;

    if (m_pPredecessor)
        m_pPredecessor->m_pSuccessor = m_pSuccessor;
    else
        m_pSession->m_pQueryHead.setTo (m_pSuccessor);

    m_pSuccessor.clear ();
    m_pPredecessor.clear ();
}

/************************
 ************************
 **** Query Timeout  ****
 ************************
 ************************/

bool Vsa::VPoolAdminSession::Query::setTimeout () {
    if (m_iTimeOut != U64_MAX) {
        Vca::VTrigger<ThisClass>::Reference pTrigger (
            new Vca::VTrigger<ThisClass> (this, &ThisClass::onTimeout)
        );
        m_pTimer.setTo (new Vca::VTimer (pTrigger, m_iTimeOut*1000*1000));
        m_pTimer->start ();
        return true;
    }
    return false;
}

void Vsa::VPoolAdminSession::Query::onTimeout (Vca::VTrigger<ThisClass> *pTrigger) {
    m_pSession->displayResult (g_pMessages[Query_Timedout]);

    m_bTimedOut = true;
    m_pTimer->cancel ();
    m_pTimer.clear ();
    m_pSession->onOperationComplete ();
}

/************************************
 ************************************
 *****  Vsa::VPoolAdminSession  *****
 ************************************
 ************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

/*****************************************************************
 *****  The 'retain/untain' call pair ensures that the       *****
 *****  object being constructed is not reclaimed as part    *****
 *****  of the construction                                  *****
 *****************************************************************/

Vsa::VPoolAdminSession::VPoolAdminSession (
    VString const &rAddress, VString const &rSessionName, VPoolAdmin *pAdmin
) : m_iSessionName (rSessionName)
  , m_iAddress (rAddress)
  , m_pAdmin (pAdmin)
  , m_iState (State_Plumbing)
  , m_bSuspend (false)
{
    traceInfo ("Creating Session");
    retain (); {
        VPoolAdmin::server_gofer_t::Reference pSessionGofer;
        pAdmin->getServerGofer (pSessionGofer, rAddress);
        pSessionGofer->supplyMembers (this, &ThisClass::onConnect, &ThisClass::onConnectError);
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VPoolAdminSession::~VPoolAdminSession () {
    traceInfo ("Destroying Session");
}


/********************************
 ****   Interface Receivers  ****
 ********************************/

void Vsa::VPoolAdminSession::onConnectError (Vca::IError *pIError, VString const &rErrorText) {
    m_iState = State_Failure;

    VString iMessage;
    iMessage << g_pMessages [Connection_Error] << rErrorText;
    displayResult (iMessage);

    Reference iRetainer (this);
    clear ();
    m_pAdmin->setExitStatus (ErrorExitValue);
    m_pAdmin->removeSession (this);
    if (!m_pAdmin->g_bInteractive)
	m_pAdmin->stop ();
}

//  Note:
//  Reusing the same receiver (pReceiver) again for the next QI, has race condition problems between
//  the pool's proxy GC  and the message processing, resulting in the new QI message being ignored.
//  Hence, using new receivers each time a QI is done.

void Vsa::VPoolAdminSession::onConnect (IVUnknown *pObject) {
    if (!pObject)
        onConnectError (0, g_pMessages[No_Object_At_Address]);
    else {
        // Retrieve IPoolApplication.
        IVUnknownReceiver::Reference pNewReceiver (
           new IVUnknownReceiver (this, &ThisClass::onPoolApplication)
        );
        m_pFarside.setTo (pObject);
        m_pFarside->QueryInterface (IPoolApplication_Ex1::typeInfo (), pNewReceiver);
    }
}

void Vsa::VPoolAdminSession::onPoolApplication  (IVUnknownReceiver *pReceiver, IVUnknown *pUnknown) {
    if (pUnknown)
        m_pPoolApp.setTo (static_cast<IPoolApplication_Ex1*>(pUnknown));
    else
        displayResult (g_pMessages[Null_IPoolApplication_Ex1]);

    IVUnknownReceiver::Reference pNewReceiver (
        new IVUnknownReceiver (this, &ThisClass::onGetter)
    );
    m_pFarside->QueryInterface (Vca::IGetter::typeInfo (), pNewReceiver);

    pNewReceiver.setTo (
        new IVUnknownReceiver (this, &ThisClass::onEvaluatorPool)
    );
    m_pFarside->QueryInterface (IEvaluatorPool::typeInfo (), pNewReceiver);
}

void Vsa::VPoolAdminSession::onGetter (IVUnknownReceiver *pReceiver, IVUnknown *pUnknown) {
    if (pUnknown) {
        m_pGetter.setTo (static_cast<Vca::IGetter*>(pUnknown));
    }
}

void Vsa::VPoolAdminSession::onEvaluatorPool (IVUnknownReceiver *pRec, IVUnknown *pUnknown) {
    if (pUnknown) {
        m_pPool.setTo (static_cast<IEvaluatorPool*>(pUnknown));

        EvaluatorReceiver::Reference pReceiver (
            new EvaluatorReceiver (this, &ThisClass::onEvaluator)
        );
        m_pPool->GetEvaluator (pReceiver);
    }
    else {
        onQIComplete ();
        displayResult (g_pMessages[Null_IEvaluatorPool]);
    }
}

void Vsa::VPoolAdminSession::onEvaluator (EvaluatorReceiver *pRec, IEvaluator *pEvaluator) {
    if (pEvaluator)
        m_pEvaluator.setTo (pEvaluator);
    else
        displayResult (g_pMessages[Null_IEvaluator]);

    PoolSettingsReceiver::Reference pReceiver (
        new PoolSettingsReceiver (this, &ThisClass::onEvaluatorPoolSettings)
    );
    m_pPool->GetSettings (pReceiver);
}

void Vsa::VPoolAdminSession::onEvaluatorPoolSettings (
    PoolSettingsReceiver *pReceiver, IEvaluatorPoolSettings *pSettings
) {
    if (pSettings)
        m_pPoolSettings.setTo (pSettings);
    else
        displayResult (g_pMessages[Null_IEvaluatorPoolSettings]);
    onQIComplete ();
}


void Vsa::VPoolAdminSession::onQIComplete () {
    m_iState = State_Initialized;
    triggerQueuedOperations ();
}

/****************************
 ****   Session Control  ****
 ****************************/

void Vsa::VPoolAdminSession::Suspend () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_Suspend));
        return;
    }

    if (m_pPool) {
        m_bSuspend = true;
        BoolReceiver::Reference pReceiver;
        pReceiver.setTo (new BoolReceiver (this, &ThisClass::onOperationResult));
        m_pPool->Suspend (pReceiver);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPool]);
}

void Vsa::VPoolAdminSession::Resume () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_Resume));
        return;
    }

    if (m_pPool) {
        m_bSuspend = true;
        BoolReceiver::Reference pReceiver;
        pReceiver.setTo (new BoolReceiver (this, &ThisClass::onOperationResult));
        m_pPool->Resume (pReceiver);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPool]);
}

void Vsa::VPoolAdminSession::Restart () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_Restart));
        return;
    }

    if (m_pPool) {
        m_bSuspend = true;
        BoolReceiver::Reference pReceiver;
        pReceiver.setTo (new BoolReceiver (this, &ThisClass::onOperationResult));
        m_pPool->Restart (pReceiver);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPool]);
}

void Vsa::VPoolAdminSession::HardRestart () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_HardRestart));
        return;
    }

    if (m_pPool) {
        if (IEvaluatorPool_Ex1 *const pPool = dynamic_cast<IEvaluatorPool_Ex1*>(m_pPool.referent ())) {
            m_bSuspend = true;
            BoolReceiver::Reference pReceiver;
            pReceiver.setTo (new BoolReceiver (this, &ThisClass::onOperationResult));

            pPool->HardRestart (pReceiver);
        }
        else
            displayResult (g_pMessages[Pool_Operation_Not_Supported]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPool]);
}

/********
 * Stop Operation on a Pool (Session), gracefully stops the pool
 * i.e. retires all available workers, allows current workers to finish
 * and also processes the current pending queries, then makes the
 * Pool Application to stop listening any more. Terminates the connection between
 * this PoolAdmin and the Pool application
 ****/

void Vsa::VPoolAdminSession::Stop () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_Stop));
        return;
    }

    if (dynamic_cast<IPoolApplication_Ex2*>(m_pPoolApp.referent ())) {
        m_pPoolApp->Stop ();
        displayResult (g_pMessages[Message_Sent]);
        clear ();
    }
    else
        Stop_Old ();
}

void Vsa::VPoolAdminSession::Stop_Old () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_Stop));
        return;
    }

    if (m_pPool) {
        m_bSuspend = true;
        BoolReceiver::Reference pReceiver;
        pReceiver.setTo (new BoolReceiver (this, &ThisClass::onOperationResult));
        m_pPool->Stop (pReceiver);
        if (m_pPoolApp)
            m_pPoolApp->Stop ();
        displayResult (g_pMessages[Message_Sent]);
        clear ();
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPool]);
}



/********
 * HardStop Operation on a Pool (Session), hard stops the pool
 * i.e. retires all available workers, allows current workers to finish
 * Sends error messages for all pending queries in the queue.
 * Makes the Pool Application to stop listening any more.
 * Terminates the connection between this PoolAdmin and the Pool application
 ****/

void Vsa::VPoolAdminSession::HardStop () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_HardStop));
        return;
    }

    if (IPoolApplication_Ex2 *const pPoolApp = dynamic_cast<IPoolApplication_Ex2*>(m_pPoolApp.referent ())) {
        pPoolApp->HardStop ();
        displayResult (g_pMessages[Message_Sent]);
        clear ();
    }
    else
        HardStop_Old ();
}

void Vsa::VPoolAdminSession::HardStop_Old () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_HardStop));
        return;
    }

    if (m_pPool) {
        if (IEvaluatorPool_Ex1 *const pPool = dynamic_cast<IEvaluatorPool_Ex1*>(m_pPool.referent ())) {
            m_bSuspend = true;
            BoolReceiver::Reference pReceiver;
            pReceiver.setTo (new BoolReceiver (this, &ThisClass::onOperationResult));
            pPool->HardStop (pReceiver);
            if (m_pPoolApp)
                m_pPoolApp->Stop ();
            displayResult (g_pMessages[Message_Sent]);
            clear ();
        }
        else
            displayResult (g_pMessages[Pool_Operation_Not_Supported]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPool]);
}

void Vsa::VPoolAdminSession::FlushWorkers () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_FlushWorkers));
        return;
    }

    if (m_pPool) {
        // Make sure that we only wait on pools that send a result; older ones didn't used to.
        if (dynamic_cast<IEvaluatorPool_Ex3*>(m_pPool.referent ()))
            m_bSuspend = true;

        BoolReceiver::Reference pReceiver;
        pReceiver.setTo (new BoolReceiver (this, &ThisClass::onOperationResult));
        m_pPool->FlushWorkers (pReceiver);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPool]);
}

void Vsa::VPoolAdminSession::ServiceStatistics  () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_Statistics));
        return;
    }

    if (m_pPool) {
        m_bSuspend = true;
        VStringReceiver::Reference pReceiver;
        pReceiver.setTo (new VStringReceiver (this, &ThisClass::onResult));
        m_pPool->GetStatistics (pReceiver);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPool]);
}

void Vsa::VPoolAdminSession::QueryDetails (Vca::U32 iID) {
    if (m_pPool)  {
	if (IEvaluatorPool_Ex4 *const pPool = dynamic_cast<IEvaluatorPool_Ex4*>(m_pPool.referent ())) {
            VStringReceiver::Reference pReceiver;
            pReceiver.setTo (new VStringReceiver (this, &ThisClass::onResult));

            pPool->QueryDetails (iID, pReceiver);
      }
      else
            displayResult (g_pMessages[Pool_Operation_Not_Supported]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPool]);
}

void Vsa::VPoolAdminSession::StatisticsDump () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_StatisticsDump));
        return;
    }

    if (m_pGetter) {
        m_bSuspend = true;
        VStringReceiver::Reference pReceiver;
        pReceiver.setTo (new VStringReceiver (this, &ThisClass::onResult, &ThisClass::onError));
        m_pGetter->GetStringValue (pReceiver, "statDump");
    }
    else
        displayResult (g_pMessages[Null_IGetter]);
}

void Vsa::VPoolAdminSession::QuickStatDumpHeader () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_QuickStatDumpHeader));
        return;
    }

    if (m_pGetter) {
        m_bSuspend = true;
        VStringReceiver::Reference pReceiver;
        pReceiver.setTo (new VStringReceiver (this, &ThisClass::onResult, &ThisClass::onError));
        m_pGetter->GetStringValue (pReceiver, "quickStatsHeader");
    }
    else
        displayResult (g_pMessages[Null_IGetter]);
}

void Vsa::VPoolAdminSession::QuickStatDump () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_QuickStatDump));
        return;
    }

    if (m_pGetter) {
        m_bSuspend = true;
        VStringReceiver::Reference pReceiver;
        pReceiver.setTo (new VStringReceiver (this, &ThisClass::onResult, &ThisClass::onError));
        m_pGetter->GetStringValue (pReceiver, "quickStats");
    }
    else
        displayResult (g_pMessages[Null_IGetter]);
}

void Vsa::VPoolAdminSession::PingSession (
    VString const &rQuery, bool bAnyData, Vca::U64 iTimeOutSecs
) {
    Query::Reference pQuery (new Query (this, rQuery, Query::Query_Ping, bAnyData, iTimeOutSecs));
    QuerySession (pQuery);
}

void Vsa::VPoolAdminSession::QuerySession (Query *pQuery) {
    if (isntInitialized ())
        enqueue (new QueryOperation (pQuery));
    else
        pQuery->start ();
}

void Vsa::VPoolAdminSession::Broadcast (VString const &rQuery, Vca::U64 iTimeOutSecs) {
    Query::Reference pQuery (new Query (this, rQuery, Query::Query_Broadcast, false, iTimeOutSecs));
    QuerySession (pQuery);
}

void Vsa::VPoolAdminSession::BroadcastFile (VString const &rQueryFile, Vca::U64 iTimeOutSecs) {
    VSimpleFile iFile;
    VString iContents;
    if (iFile.GetContents (iContents, rQueryFile))
        Broadcast (iContents, iTimeOutSecs);
    else
        displayResult (g_pMessages[Invalid_Broadcast_File]);
}

void Vsa::VPoolAdminSession::SetWorkerStartupQuery (VString const &rQuery) {
    Query::Reference pQuery (new Query (this, rQuery, Query::Query_WorkerStartup));
    QuerySession (pQuery);
}


void Vsa::VPoolAdminSession::SetWorkerStartupFile (VString const &rStartupFile) {
    VSimpleFile iFile;
    VString iContents;
    if (iFile.GetContents (iContents, rStartupFile))
        SetWorkerStartupQuery (iContents);
    else
        displayResult (g_pMessages[Invalid_Worker_Startup_File]);
}


void Vsa::VPoolAdminSession::SetWorkerPIDQueryFromFile (VString const &rFile) {
    VSimpleFile iFile; VString iContents;

    if (iFile.GetContents (iContents, rFile)) {
        if (m_pPoolSettings.isntNil ())  {
            if (IEvaluatorPoolSettings_Ex5 *const pSettings = dynamic_cast<IEvaluatorPoolSettings_Ex5*>(m_pPoolSettings.referent ())) {
                pSettings->SetWorkerPIDQuery (iContents);
                displayResult (g_pMessages[Message_Sent]);
            }
            else
                displayResult (g_pMessages[Pool_Operation_Not_Supported]);
        }
        else
          displayResult (g_pMessages[Null_IEvaluatorPoolSettings]);
    }
    else
      displayResult (g_pMessages[Invalid_Workerpid_Query_File]);
}

void Vsa::VPoolAdminSession::WorkerStatistics (VString const &rID, bool bIsPID) {
    if (isntInitialized ()) {
	enqueue (new WorkerOperation (Command_WorkerStat, rID, bIsPID));
	return;
    }

    if (m_pPool)  {
	if (IEvaluatorPool_Ex3 *const pPool3 = dynamic_cast<IEvaluatorPool_Ex3*>(m_pPool.referent ())) {
            IVUnknownReceiver::Reference pReceiver;
            pReceiver.setTo (new IVUnknownReceiver (this, &ThisClass::onWorkerInterface, &ThisClass::onWorkerError));

            pPool3->GetWorker_Ex (rID, bIsPID, pReceiver);
	} else if (IEvaluatorPool_Ex1 *const pPool1 = dynamic_cast<IEvaluatorPool_Ex1*>(m_pPool.referent ())) {
            IVUnknownReceiver::Reference pReceiver;
            pReceiver.setTo (new IVUnknownReceiver (this, &ThisClass::onWorkerInterface, &ThisClass::onWorkerError));

            Vca::U32 iID = atoi (rID.content ());

            pPool1->GetWorkerInterface (pReceiver, iID);
        }
        else
            displayResult (g_pMessages[Pool_Operation_Not_Supported]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPool]);
}

void Vsa::VPoolAdminSession::WorkersStatistics () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_Workers));
        return;
    }

    if (m_pPool)  {
	if (IEvaluatorPool_Ex3 *const pPool = dynamic_cast<IEvaluatorPool_Ex3*>(m_pPool.referent ())) {
            VStringReceiver::Reference pReceiver;
            pReceiver.setTo (new VStringReceiver (this, &ThisClass::onResult));

            pPool->GetWorkersStatistics (pReceiver);
      }
      else
            displayResult (g_pMessages[Pool_Operation_Not_Supported]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPool]);
}

void Vsa::VPoolAdminSession::WorkerStatDumpHeader () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_WorkerStatDumpHeader));
        return;
    }

    if (m_pGetter) {
        m_bSuspend = true;
        VStringReceiver::Reference pReceiver;
        pReceiver.setTo (new VStringReceiver (this, &ThisClass::onResult, &ThisClass::onError));
        m_pGetter->GetStringValue (pReceiver, "workerStatsHeader");
    }
    else
        displayResult (g_pMessages[Null_IGetter]);
}

void Vsa::VPoolAdminSession::WorkerStatDump () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_WorkerStatDump));
        return;
    }

    if (m_pGetter) {
        m_bSuspend = true;
        VStringReceiver::Reference pReceiver;
        pReceiver.setTo (new VStringReceiver (this, &ThisClass::onResult, &ThisClass::onError));
        m_pGetter->GetStringValue (pReceiver, "workerStats");
    }
    else
        displayResult (g_pMessages[Null_IGetter]);
}

void Vsa::VPoolAdminSession::GenerationStatistics (Vca::U32 iID) {
    if (m_pPool)  {
	if (IEvaluatorPool_Ex3 *const pPool = dynamic_cast<IEvaluatorPool_Ex3*>(m_pPool.referent ())) {
            VStringReceiver::Reference pReceiver;
            pReceiver.setTo (new VStringReceiver (this, &ThisClass::onResult));

            pPool->GetGenerationStatistics (iID, pReceiver);
      }
      else
            displayResult (g_pMessages[Pool_Operation_Not_Supported]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPool]);
}

void Vsa::VPoolAdminSession::GenerationStatDumpHeader () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_GenerationStatDumpHeader));
        return;
    }

    if (m_pGetter) {
        m_bSuspend = true;
        VStringReceiver::Reference pReceiver;
        pReceiver.setTo (new VStringReceiver (this, &ThisClass::onResult, &ThisClass::onError));
        m_pGetter->GetStringValue (pReceiver, "generationStatsHeader");
    }
    else
        displayResult (g_pMessages[Null_IGetter]);
}

void Vsa::VPoolAdminSession::GenerationStatDump () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_GenerationStatDump));
        return;
    }

    if (m_pGetter) {
        m_bSuspend = true;
        VStringReceiver::Reference pReceiver;
        pReceiver.setTo (new VStringReceiver (this, &ThisClass::onResult, &ThisClass::onError));
        m_pGetter->GetStringValue (pReceiver, "generationStats");
    }
    else
        displayResult (g_pMessages[Null_IGetter]);
}

void Vsa::VPoolAdminSession::GetTotalWorkers () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_TotalWorkers));
        return;
    }

    if (m_pPool) {
        if (IEvaluatorPool_Ex1 *const pPool = dynamic_cast<IEvaluatorPool_Ex1*>(m_pPool.referent ())) {
            m_bSuspend = true;
            U32Receiver::Reference pReceiver;
            pReceiver.setTo (new U32Receiver (this, &ThisClass::onU32Result));

            pPool->GetTotalWorkers (pReceiver);
        }
        else
            displayResult (g_pMessages[Pool_Operation_Not_Supported]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPool]);
}

void Vsa::VPoolAdminSession::IsWorkerCreationFailureLmtReached () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_IsWorkerCreationFailureLmtReached));
        return;
    }

    if (m_pPool) {
        if (IEvaluatorPool_Ex1 *const pPool = dynamic_cast<IEvaluatorPool_Ex1*>(m_pPool.referent ())) {
            m_bSuspend = true;
            BoolReceiver::Reference pReceiver;
            pReceiver.setTo (new BoolReceiver (
                this, &ThisClass::onWorkerCreationFailureLmtResult
                )
            );

            pPool->IsWorkerCreationFailureLmtReached (pReceiver);
        }
        else
            displayResult (g_pMessages[Pool_Operation_Not_Supported]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPool]);
}

void Vsa::VPoolAdminSession::GetCurrentGenerationTotalWorkers () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_CurrentGenerationTotalWorkers));
        return;
    }

    if (m_pPool) {
        if (IEvaluatorPool_Ex1 *const pPool = dynamic_cast<IEvaluatorPool_Ex1*>(m_pPool.referent ())) {
            m_bSuspend = true;
            U32Receiver::Reference pReceiver;
            pReceiver.setTo (new U32Receiver (this, &ThisClass::onU32Result));

            pPool->GetCurrentGenerationTotalWorkers (pReceiver);
        }
        else
            displayResult (g_pMessages[Pool_Operation_Not_Supported]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPool]);
}

void Vsa::VPoolAdminSession::GetMinWorkers () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_GetMinWorkers));
        return;
    }

    if (m_pGetter) {
        m_bSuspend = true;
        U32Receiver::Reference pReceiver;
        pReceiver.setTo (new U32Receiver (this, &ThisClass::onU32Result, &ThisClass::onU32Error));
        m_pGetter->GetU32Value (pReceiver, "MinWorkers");
    }
    else
        displayResult (g_pMessages[Null_IGetter]);
}

void Vsa::VPoolAdminSession::GetMaxWorkers () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_GetMaxWorkers));
        return;
    }

    if (m_pGetter) {
        m_bSuspend = true;
        U32Receiver::Reference pReceiver;
        pReceiver.setTo (new U32Receiver (this, &ThisClass::onU32Result, &ThisClass::onU32Error));
        m_pGetter->GetU32Value (pReceiver, "MaxWorkers");
    }
    else
        displayResult (g_pMessages[Null_IGetter]);
}

void Vsa::VPoolAdminSession::GetQueryCount () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_GetQueryCount));
        return;
    }

    if (m_pGetter) {
        m_bSuspend = true;
        U32Receiver::Reference pReceiver;
        pReceiver.setTo (new U32Receiver (this, &ThisClass::onU32Result, &ThisClass::onU32Error));
        m_pGetter->GetU32Value (pReceiver, "QueryCount");
    }
    else
        displayResult (g_pMessages[Null_IGetter]);
}

void Vsa::VPoolAdminSession::GetQueueLength () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_GetQueueLength));
        return;
    }

    if (m_pGetter) {
        m_bSuspend = true;
        U32Receiver::Reference pReceiver;
        pReceiver.setTo (new U32Receiver (this, &ThisClass::onU32Result, &ThisClass::onU32Error));
        m_pGetter->GetU32Value (pReceiver, "QueueLength");
    }
    else
        displayResult (g_pMessages[Null_IGetter]);
}

void Vsa::VPoolAdminSession::GetClientCount () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_GetClientCount));
        return;
    }

    if (m_pGetter) {
        m_bSuspend = true;
        U32Receiver::Reference pReceiver;
        pReceiver.setTo (new U32Receiver (this, &ThisClass::onU32Result, &ThisClass::onU32Error));
        m_pGetter->GetU32Value (pReceiver, "OfferCount");
    }
    else
        displayResult (g_pMessages[Null_IGetter]);
}

void Vsa::VPoolAdminSession::Shutdown () {
    if (isntInitialized ())
        enqueue (new Operation (Command_Shutdown));
    else if (m_pAdmin)
        m_pAdmin->stop ();
}

void Vsa::VPoolAdminSession::GetAppSettings () {
    if (isntInitialized ()) {
        enqueue (new Operation (Command_AppSettings));
        return;
    }

    if (m_pPoolApp.isntNil ()) {
	VStringReceiver::Reference pReceiver;
	pReceiver.setTo (new VStringReceiver (this, &ThisClass::onAppSettings));
	m_pPoolApp->GetStatistics (pReceiver);
    }
    else
        displayResult (g_pMessages[Null_IPoolApplication]);
}

/*****************************
 ****   Session Settings  ****
 *****************************/

void Vsa::VPoolAdminSession::SetWorkerMinimum (Vca::U32 workerCount) {
    if (isntInitialized ()) {
        enqueue (new ValueOperation (Command_SetWorkerMinimum, workerCount));
        return;
    }

    if (m_pPoolSettings.isntNil ())  {
        m_pPoolSettings->SetWorkerMinimum (workerCount);
        displayResult (g_pMessages[Message_Sent]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPoolSettings]);
}

void Vsa::VPoolAdminSession::SetWorkerMaximum (Vca::U32 workerCount) {
    if (isntInitialized ()) {
        enqueue (new ValueOperation (Command_SetWorkerMaximum, workerCount));
        return;
    }

    if (m_pPoolSettings.isntNil ())  {
        m_pPoolSettings->SetWorkerMaximum (workerCount);
        displayResult (g_pMessages[Message_Sent]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPoolSettings]);
}

void Vsa::VPoolAdminSession::SetWorkerMinimumAvailable (Vca::U32 workerCount) {
    if (m_pPoolSettings.isntNil ())  {
        m_pPoolSettings->SetWorkerMinimumAvailable (workerCount);
        displayResult (g_pMessages[Message_Sent]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPoolSettings]);
}

void Vsa::VPoolAdminSession::SetWorkersBeingCreatedMaximum (Vca::U32 workerCount) {
    if (m_pPoolSettings.isntNil ())  {
        m_pPoolSettings->SetWorkersBeingCreatedMaximum (workerCount);
        displayResult (g_pMessages[Message_Sent]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPoolSettings]);
}

void Vsa::VPoolAdminSession::SetWorkerCreationFailureHardLimit (Vca::U32 count) {
    if (m_pPoolSettings.isntNil ())  {
        m_pPoolSettings->SetWorkerCreationFailureLimit (count);
        displayResult (g_pMessages[Message_Sent]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPoolSettings]);
}
void Vsa::VPoolAdminSession::SetWorkerCreationFailureSoftLimit (Vca::U32 count) {
    if (m_pPoolSettings.isntNil ())
	if (IEvaluatorPoolSettings_Ex5 *const pSettings = dynamic_cast<IEvaluatorPoolSettings_Ex5*>(m_pPoolSettings.referent ())) {
	    pSettings->SetWorkerCreationFailureSoftLimit (count);
	    displayResult (g_pMessages[Message_Sent]);
	}
	else
	    displayResult (g_pMessages[Pool_Operation_Not_Supported]);
    else
        displayResult (g_pMessages[Null_IEvaluatorPoolSettings]);
}

void Vsa::VPoolAdminSession::SetLogFilePath (VString const &rPath) {
    if (m_pPoolApp.isntNil ()) {
	m_pPoolApp->SetLogFilePath (rPath);
        displayResult (g_pMessages[Message_Sent]);
    }
    else
        displayResult (g_pMessages[Null_IPoolApplication]);
}


void Vsa::VPoolAdminSession::SetLogSwitch (bool bSwitch) {
    if (isntInitialized ()) {
        bSwitch ? enqueue (new Operation (Command_SetLogSwitchOn)):
                  enqueue (new Operation (Command_SetLogSwitchOff)) ;
        return;
    }

    if (m_pPoolApp.isntNil ()) {
	m_pPoolApp->SetLogSwitch (bSwitch);
        displayResult (g_pMessages[Message_Sent]);
    }
    else
        displayResult (g_pMessages[Null_IPoolApplication]);
}

void Vsa::VPoolAdminSession::SetLogFileSize (Vca::U32 iSize) {
    if (m_pPoolApp.isntNil ()) {
	m_pPoolApp->SetLogFileSize (iSize);
	displayResult (g_pMessages[Message_Sent]);
    }
    else
        displayResult (g_pMessages[Null_IPoolApplication]);
}

void Vsa::VPoolAdminSession::SetLogFileBackups (Vca::U32 iCount) {
    if (m_pPoolApp.isntNil ()) {
	m_pPoolApp->SetLogFileBackups (iCount);
	displayResult (g_pMessages[Message_Sent]);
    }
    else
        displayResult (g_pMessages[Null_IPoolApplication]);
}

void Vsa::VPoolAdminSession::SetWorkerProcessSource (VString const &iWorkerSource) {
    if (m_pPoolApp.isntNil ()) {
        m_pPoolApp->SetPoolWorkerProcessSource (iWorkerSource);
        displayResult (g_pMessages[Message_Sent]);
    }
    else
        displayResult (g_pMessages[Null_IPoolApplication]);
}

void Vsa::VPoolAdminSession::SetEvaluationAttempts  (Vca::U32 cEvaluationAttempt) {
    if (m_pPoolSettings.isntNil ())  {
        m_pPoolSettings->SetEvaluationAttemptMaximum (cEvaluationAttempt);
        displayResult (g_pMessages[Message_Sent]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPoolSettings]);
}

void Vsa::VPoolAdminSession::SetWorkerMaximumAvailable (Vca::U32 workerCount) {
    if (m_pPoolSettings.isntNil ()) {
        if (IEvaluatorPoolSettings_Ex1 *const pSettings = dynamic_cast<IEvaluatorPoolSettings_Ex1*>(m_pPoolSettings.referent ())) {
            pSettings->SetWorkerMaximumAvailable (workerCount);
	    displayResult (g_pMessages[Message_Sent]);
        }
        else
            displayResult (g_pMessages[Pool_Operation_Not_Supported]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPoolSettings]);
}

void Vsa::VPoolAdminSession::SetShrinkTimeOut  (Vca::U32 iTimeOutMinutes) {
    if (m_pPoolSettings.isntNil ()) {
        if (IEvaluatorPoolSettings_Ex1 *const pSettings = dynamic_cast<IEvaluatorPoolSettings_Ex1*>(m_pPoolSettings.referent ())) {
            pSettings->SetShrinkTimeOut (iTimeOutMinutes);
            displayResult (g_pMessages[Message_Sent]);
        }
        else
            displayResult (g_pMessages[Pool_Operation_Not_Supported]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPoolSettings]);
}

void Vsa::VPoolAdminSession::SetBroadcastInProgressLimit (Vca::U32 iLimit) {
    if (isntInitialized ()) {
        enqueue (new ValueOperation (Command_SetBroadcastInProgLimit, iLimit));
        return;
    }

    if (m_pPoolSettings.isntNil ()) {
        if (IEvaluatorPoolSettings_Ex2 *const pSettings = dynamic_cast<IEvaluatorPoolSettings_Ex2*>(m_pPoolSettings.referent ())) {
            pSettings->SetBCEvaluationsInProgressLimit (iLimit);
            displayResult (g_pMessages[Message_Sent]);
        }
        else
            displayResult (g_pMessages[Pool_Operation_Not_Supported]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPoolSettings]);
}

void Vsa::VPoolAdminSession::SetWorkerQueryHistoryLimit (Vca::U32 iLimit) {

    if (m_pPoolSettings.isntNil ()) {
        if (IEvaluatorPoolSettings_Ex4 *const pSettings = dynamic_cast<IEvaluatorPoolSettings_Ex4*>(m_pPoolSettings.referent ())) {
            pSettings->SetWorkerQueryHistoryLimit (iLimit);
            displayResult (g_pMessages[Message_Sent]);
        }
        else
            displayResult (g_pMessages[Pool_Operation_Not_Supported]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPoolSettings]);
}


void Vsa::VPoolAdminSession::SetStopTimeOut  (Vca::U32 iTimeOutMins) {
    if (m_pPoolSettings.isntNil ()) {
        if (IEvaluatorPoolSettings_Ex3 *const pSettings = dynamic_cast<IEvaluatorPoolSettings_Ex3*>(m_pPoolSettings.referent ())) {
            pSettings->SetStopTimeOut (iTimeOutMins);
            displayResult (g_pMessages[Message_Sent]);
        }
        else
            displayResult (g_pMessages[Pool_Operation_Not_Supported]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPoolSettings]);
}

void Vsa::VPoolAdminSession::SetEvaluationTimeout (Vca::U64 iTimeoutSecs) {
  if (isntInitialized ()) {
    enqueue (new ValueOperation (Command_SetEvaluationTimeout, iTimeoutSecs));
    return;
  }

  if (m_pPoolSettings.isntNil ()) {
    //  check for overflow...
    Vca::U64 iTimeoutMicroSecs;
    if (iTimeoutSecs > U64_MAX/1000000)
      iTimeoutMicroSecs = U64_MAX;
    else
      iTimeoutMicroSecs = iTimeoutSecs * 1000000;

    //  set the timeout...
    m_pPoolSettings->SetEvaluationTimeOut (iTimeoutMicroSecs);
    displayResult (g_pMessages[Message_Sent]);
  }
  else
    displayResult (g_pMessages[Null_IEvaluatorPoolSettings]);
}

void Vsa::VPoolAdminSession::SetGenerationMaximum (Vca::U32 count) {
    if (isntInitialized ()) {
        enqueue (new ValueOperation (Command_SetGenerationMaximum, count));
        return;
    }

    if (m_pPoolSettings.isntNil ()) {
        if (IEvaluatorPoolSettings_Ex6 *const pSettings = dynamic_cast<IEvaluatorPoolSettings_Ex6*>(m_pPoolSettings.referent ())) {
            pSettings->SetGenerationMaximum (count);
            displayResult (g_pMessages[Message_Sent]);
        }
        else
            displayResult (g_pMessages[Pool_Operation_Not_Supported]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPoolSettings]);
}

void Vsa::VPoolAdminSession::CancelQuery (Vca::U32 iID) {
    if (isntInitialized ()) {
        enqueue (new ValueOperation (Command_CancelQuery, iID));
        return;
    }

    if (m_pPool.isntNil ()) {
        if (IEvaluatorPool_Ex4 *const pIPool = dynamic_cast<IEvaluatorPool_Ex4*>(m_pPool.referent ())) {
            m_bSuspend = true;
            BoolReceiver::Reference pReceiver;
            pReceiver.setTo (new BoolReceiver (this, &ThisClass::onOperationResult, &ThisClass::onOperationError));

            pIPool->CancelQuery (iID, pReceiver);
        }
        else
            displayResult (g_pMessages[Pool_Operation_Not_Supported]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPool]);
}

void Vsa::VPoolAdminSession::TakeWorkerOffline (VString const &iWorkerId, bool bIsPID) {
  if (isntInitialized ()) {
    enqueue (new WorkerOperation (Command_TakeWorkerOffline, iWorkerId, bIsPID));
    return;
  }

    if (m_pPool) {
        if (IEvaluatorPool_Ex3 *const pIPool = dynamic_cast<IEvaluatorPool_Ex3*>(m_pPool.referent ())) {
            m_bSuspend = true;
            VStringReceiver::Reference pReceiver;
            pReceiver.setTo (new VStringReceiver (this, &ThisClass::onResult));

            pIPool->TakeWorkerOffline (iWorkerId, bIsPID, pReceiver);
        }
        else
            displayResult (g_pMessages[Pool_Operation_Not_Supported]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPool]);
}

void Vsa::VPoolAdminSession::MakeWorkerOnline (VString const &iWorkerId, bool bIsPID) {
  if (isntInitialized ()) {
    enqueue (new WorkerOperation (Command_BringWorkerOnline, iWorkerId, bIsPID));
    return;
  }

    if (m_pPool) {
        if (IEvaluatorPool_Ex3 *const pIPool = dynamic_cast<IEvaluatorPool_Ex3*>(m_pPool.referent ())) {
            m_bSuspend = true;
            VStringReceiver::Reference pReceiver;
            pReceiver.setTo (new VStringReceiver (this, &ThisClass::onResult));

            pIPool->MakeWorkerOnline (iWorkerId, bIsPID, pReceiver);
        }
        else
            displayResult (g_pMessages[Pool_Operation_Not_Supported]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPool]);
}

void Vsa::VPoolAdminSession::EvaluateUsingOfflineWorker (VString const &rQueryFile, VString const &iWorkerId, bool bIsPID) {
    if (m_pPool) {
        if (dynamic_cast<IEvaluatorPool_Ex3*>(m_pPool.referent ())) {
          VSimpleFile iFile;
          VString iContents;
          if (iFile.GetContents (iContents, rQueryFile)) {
            Query::Reference pQuery (new Query (this, iContents, Query::Query_Offline_Worker, false, U64_MAX, iWorkerId, bIsPID));
            QuerySession (pQuery);
          }
          else
            displayResult (g_pMessages[Invalid_Query_File]);
        }
        else
            displayResult (g_pMessages[Pool_Operation_Not_Supported]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPool]);
}

void Vsa::VPoolAdminSession::RetireOfflineWorker (VString const& iWorkerId, bool bIsPID) {
  if (isntInitialized ()) {
    enqueue (new WorkerOperation (Command_RetireOfflineWorker, iWorkerId, bIsPID));
    return;
  }

    if (m_pPool) {
        if (IEvaluatorPool_Ex3 *const pIPool = dynamic_cast<IEvaluatorPool_Ex3*>(m_pPool.referent ())) {
            m_bSuspend = true;
            VStringReceiver::Reference pReceiver;
            pReceiver.setTo (new VStringReceiver (this, &ThisClass::onResult));

            pIPool->RetireOfflineWorker (iWorkerId, bIsPID, pReceiver);
        }
        else
            displayResult (g_pMessages[Pool_Operation_Not_Supported]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPool]);
}

void Vsa::VPoolAdminSession::DumpWorker (VString const & iWorkerId, bool bIsPID, VString const &rFile) {
    if (m_pPool) {
	if (IEvaluatorPool_Ex3 *const pIPool = dynamic_cast<IEvaluatorPool_Ex3*>(m_pPool.referent ())) {
            m_bSuspend = true;
            VStringReceiver::Reference pReceiver;
            pReceiver.setTo (new VStringReceiver (this, &ThisClass::onResult));
            pIPool->DumpWorker (iWorkerId, bIsPID, rFile, pReceiver);
      }
       else
            displayResult (g_pMessages[Pool_Operation_Not_Supported]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPool]);
}

void Vsa::VPoolAdminSession::DumpWorkingWorkers (VString const &rFile) {
    if (m_pPool) {
	if (IEvaluatorPool_Ex3 *const pIPool = dynamic_cast<IEvaluatorPool_Ex3*>(m_pPool.referent ())) {
            m_bSuspend = true;
            VStringReceiver::Reference pReceiver;
            pReceiver.setTo (new VStringReceiver (this, &ThisClass::onResult));
            pIPool->DumpWorkingWorkers (rFile, pReceiver);
      }
        else
            displayResult (g_pMessages[Pool_Operation_Not_Supported]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPool]);
}

void Vsa::VPoolAdminSession::DumpPoolQueue (VString const &rFile, Vca::U32 cQueries) {
    if (m_pPool) {
	if (IEvaluatorPool_Ex3 *const pIPool = dynamic_cast<IEvaluatorPool_Ex3*>(m_pPool.referent ())) {
            m_bSuspend = true;
            VStringReceiver::Reference pReceiver;
            pReceiver.setTo (new VStringReceiver (this, &ThisClass::onResult));
            pIPool->DumpPoolQueue (rFile, cQueries, pReceiver);
      }
        else
            displayResult (g_pMessages[Pool_Operation_Not_Supported]);
    }
    else
        displayResult (g_pMessages[Null_IEvaluatorPool]);
}


void Vsa::VPoolAdminSession::clear () {
    m_pPool.clear ();
    m_pPoolSettings.clear ();
    m_pPoolApp.clear ();

    if (m_pFarside) {
        m_pFarside->closePeer ();
        m_pFarside.clear ();
    }
}

/***********************************
 ****   OperationQueue Methods  ****
 ***********************************/

/********
 * Note:
 *  VcaAdmin program can be run in both interactive as well as command line mode
 * In the command line mode the program will try to connect to a pool, and
 * perform the operations and exit. As connecting to a pool is an
 * asynchronous operation, the program needs to synchronize executing the
 * operations on the pool with the connection establishment result.
 * To accomodate this, the operations are queued, if the session is
 * in not in the Initialized state (i.e. in the Plumbing/QueryInterface state)
 * On successful connection with the pool, the queued commands are
 * triggered.
 ****/

void Vsa::VPoolAdminSession::enqueue (Operation *pOp) {
    if (m_pPendingOperation.isNil ()) {
        m_pPendingOperation.setTo (pOp);
        return;
    }

    Operation::Reference pOperation (m_pPendingOperation);
    while (pOperation->m_pSuccessor)
        pOperation.setTo (pOperation->m_pSuccessor);

    pOperation->m_pSuccessor.setTo (pOp);
}

void Vsa::VPoolAdminSession::triggerQueuedOperations () {
    retain ();
    while (m_pPendingOperation.isntNil () && !m_bSuspend) {
        switch (m_pPendingOperation->m_iCommand) {
        case Command_Stop:
            Stop ();
            break;
        case Command_HardStop:
            HardStop ();
            break;
        case Command_Suspend:
            Suspend ();
            break;
        case Command_Resume:
            Resume ();
            break;
        case Command_Restart:
            Restart ();
            break;
        case Command_HardRestart:
            HardRestart ();
            break;
        case Command_Statistics:
            ServiceStatistics ();
            break;
        case Command_StatisticsDump:
            StatisticsDump ();
            break;
        case Command_QuickStatDumpHeader:
            QuickStatDumpHeader ();
            break;
        case Command_QuickStatDump:
            QuickStatDump ();
            break;
        case Command_AppSettings:
            GetAppSettings ();
            break;
        case Command_FlushWorkers:
            FlushWorkers ();
            break;
        case Command_SetLogSwitchOn:
            SetLogSwitch (true);
            break;
        case Command_SetLogSwitchOff:
            SetLogSwitch (false);
            break;
        case Command_Query:  {
                QueryOperation::Reference pQueryOpn (
                    static_cast <QueryOperation*> (m_pPendingOperation.referent ())
                );
                QuerySession (pQueryOpn->query ());
            } break;
        case Command_TotalWorkers:
            GetTotalWorkers ();
            break;
        case Command_IsWorkerCreationFailureLmtReached:
            IsWorkerCreationFailureLmtReached ();
            break;
        case Command_CurrentGenerationTotalWorkers:
            GetCurrentGenerationTotalWorkers ();
            break;
        case Command_GetClientCount:
            GetClientCount ();
            break;
        case Command_GetMinWorkers:
            GetMinWorkers ();
            break;
        case Command_GetMaxWorkers:
            GetMaxWorkers ();
            break;
        case Command_GetQueryCount:
            GetQueryCount ();
            break;
        case Command_GetQueueLength:
            GetQueueLength ();
            break;
        case Command_WorkerStatDumpHeader:
            WorkerStatDumpHeader ();
            break;
        case Command_WorkerStatDump:
            WorkerStatDump ();
            break;
        case Command_GenerationStatDumpHeader:
            GenerationStatDumpHeader ();
            break;
        case Command_GenerationStatDump:
            GenerationStatDump ();
            break;
        case Command_Workers:
            WorkersStatistics ();
            break;
        case Command_SetBroadcastInProgLimit: {
                ValueOperation::Reference pOpn (
                    static_cast <ValueOperation*> (m_pPendingOperation.referent ())
                );
                SetBroadcastInProgressLimit ((Vca::U32)pOpn->value ());
                break;
            }
        case Command_SetEvaluationTimeout: {
                ValueOperation::Reference pOpn (
                    static_cast <ValueOperation*> (m_pPendingOperation.referent ())
                );
                SetEvaluationTimeout (pOpn->value ());
                break;
        }
        case Command_SetWorkerMaximum: {
                ValueOperation::Reference pOpn (
                    static_cast <ValueOperation*> (m_pPendingOperation.referent ())
                );
                SetWorkerMaximum (static_cast<Vca::U32>(pOpn->value ()));
                break;
        }
        case Command_SetWorkerMinimum: {
                ValueOperation::Reference pOpn (
                    static_cast <ValueOperation*> (m_pPendingOperation.referent ())
                );
                SetWorkerMinimum (static_cast<Vca::U32>(pOpn->value ()));
                break;
        }
        case Command_SetGenerationMaximum: {
                ValueOperation::Reference pOpn (
                    static_cast <ValueOperation*> (m_pPendingOperation.referent ())
                );
                SetGenerationMaximum (static_cast<Vca::U32>(pOpn->value ()));
                break;
        }
        case Command_CancelQuery: {
                ValueOperation::Reference pOpn (
                    static_cast <ValueOperation*> (m_pPendingOperation.referent ())
                );
                CancelQuery (static_cast<Vca::U32>(pOpn->value ()));
                break;
        }
        case Command_TakeWorkerOffline: {
                WorkerOperation::Reference pOpn (
                    static_cast <WorkerOperation*> (m_pPendingOperation.referent ())
                );
                TakeWorkerOffline (pOpn->workerId (), pOpn->isWorkerPID ());
        }
          break;
        case Command_BringWorkerOnline: {
                WorkerOperation::Reference pOpn (
                    static_cast <WorkerOperation*> (m_pPendingOperation.referent ())
                );
                MakeWorkerOnline (pOpn->workerId (), pOpn->isWorkerPID ());
        }
          break;
        case Command_RetireOfflineWorker: {
                WorkerOperation::Reference pOpn (
                    static_cast <WorkerOperation*> (m_pPendingOperation.referent ())
                );
                RetireOfflineWorker (pOpn->workerId (), pOpn->isWorkerPID ());
        }
          break;
        case Command_WorkerStat: {
                WorkerOperation::Reference pOpn (
                    static_cast <WorkerOperation*> (m_pPendingOperation.referent ())
                );
                WorkerStatistics (pOpn->workerId (), pOpn->isWorkerPID ());
        }
          break;
        case Command_Shutdown:
            Shutdown ();
            break;
        default:
            break;
        }
        if (m_pPendingOperation)
            m_pPendingOperation.setTo (m_pPendingOperation->m_pSuccessor);
    }
    release ();
}

void Vsa::VPoolAdminSession::onOperationComplete () {
    m_bSuspend = false;
    triggerQueuedOperations ();
}

/**********************
 ****   Receivers  ****
 **********************/

void Vsa::VPoolAdminSession::onWorkerInterface  (
    VReceiver<ThisClass, IVUnknown*> *pReceiver, IVUnknown *pUnknown
) {
    if (pUnknown) {
        VStringReceiver::Reference pReceiver;
        pReceiver.setTo (new VStringReceiver (this, &ThisClass::onResult));
        (static_cast<IPoolWorker*>(pUnknown))->GetStatistics (pReceiver);
    }
    else
        displayResult (g_pMessages[Null_WorkerInterface]);
}

void Vsa::VPoolAdminSession::onWorkerError (
   IVUnknownReceiver *pReceiver, Vca::IError *pError, VString const &rErrorTxt
   ) {
    displayResult (rErrorTxt);
}


void Vsa::VPoolAdminSession::onOperationResult  (
    BoolReceiver *pReceiver, bool operationResult
) {
    if (operationResult)
        displayResult (g_pMessages[Operation_Succeeded]);
    else
        displayResult (g_pMessages[Operation_Failed]);
    onOperationComplete ();
}

void Vsa::VPoolAdminSession::onOperationError (BoolReceiver *pReceiver, Vca::IError *pError, VString const &rText) {
    displayError (rText);
}

/********
 * Note:
 * In command line mode the program needs to wait till the result is
 * returned, before processing the next command or terminating the program.
 * The m_bSuspend flag is used to hold the processing of next command
 * until the result is obtained.
 ****/

void Vsa::VPoolAdminSession::onResult (
    VStringReceiver *pReceiver, VString const &rResult
) {
    displayResult (rResult);
    onOperationComplete ();
}

void Vsa::VPoolAdminSession::onError (
    VStringReceiver *pReceiver, Vca::IError *pError, VString const &rText
) {
    displayError (rText);
    onOperationComplete ();
}

void Vsa::VPoolAdminSession::onAppSettings  (
    VStringReceiver *pReceiver, VString const &rStat
) {
    displayResult (rStat);
    // TODO: Is there any reason there's no onOperationComplete() here?
    // How about replacing references to onAppSettings with references to onResult?
}

void Vsa::VPoolAdminSession::onWorkerCreationFailureLmtResult (
    BoolReceiver *pReceiver, bool rResult
) {
    if (rResult)
        display ("Error: WorkerCreationFailureLmt reached for current generation");
    else
        display ("WorkerCreationFailureLmt Not reached for current generation");
    onOperationComplete ();
}

void Vsa::VPoolAdminSession::onU32Result (
    U32Receiver *pReceiver, Vca::U32 rResult
) {
    display("%d\n", rResult);
    onOperationComplete();
}

void Vsa::VPoolAdminSession::onU32Error (
    U32Receiver *pReceiver, Vca::IError *pError, VString const &rText
) {
    VString iResult = "Error: "; iResult << rText;
    displayResult (iResult);
    onOperationComplete ();
}

/*********************
 ****   Display   ****
 *********************/

void Vsa::VPoolAdminSession::displayResult (VString const &rResult) {
    m_pAdmin->displayResult (rResult);
}

void Vsa::VPoolAdminSession::displayError (VString const &rText) {
    VString iResult = "Error: "; iResult << rText;
    displayResult (iResult);
}
