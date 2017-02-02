/*****  Vsa_VSmartEvaluatorSource Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "Vsa_VSmartEvaluatorSource.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VReceiver.h"
#include "V_VTime.h"
#include "Vca_VDirectory.h"
#include "VkSocketAddress.h"
#include "VTransientServices.h"

namespace Vsa {

/********************************************
 *****  VSmartEvaluatorSource::Request  *****
 ********************************************/

  class VSmartEvaluatorSource::Request : public Vca::VRolePlayer {

    //  Run Time Type
      DECLARE_CONCRETE_RTTLITE (Request, Vca::VRolePlayer);

    //  Typedef
    typedef VReceiver<ThisClass, ISubscription*> ISubscriptionSink;
    typedef VReceiver<ThisClass, IVUnknown*> IVUnknownSink;
	typedef VReceiver<ThisClass, Vca::U32>   U32Receiver;

    //  enum
    enum State {
      STATE_NEW,
      STATE_TRYING_PRIMARY,
      STATE_TRYING_PRIMARY_FAILOVERS,
	  STATE_TRYING_LEAST_BUSY,
      STATE_TRYING_LASTRESORTS,
      STATE_FAILED,
      STATE_SUCCEEDED
    };

    //  Constructor
  public:
    Request (
      Vca::IDirectory *pDirectory, VkDynamicArrayOf<VString> &rEntries, 
      IEvaluatorSink *pSink, VSmartEvaluatorSource *pSource
    );

    //  Destructor
  protected:
    ~Request ();

    //  Init
  protected:
    void init (VkDynamicArrayOf<VString> &rEntries);
    

  public:
    void start ();

    //  Implementation
  private:
    void process ();


    void tryEntry (VString const &rName);
    void tryNextEntry ();
    void enquireDirectory (VString const &rName);
	void ReturnLeastBusy();
	void CheckBusyness();
	void attemptReturn();
	bool shouldReturnEvaluator();


    //  ObjectSink Role
  private:
    Vca::VRole<ThisClass,ObjectSink> m_pIVUnknownReceiver;
  public:
    void getRole (VReference<ObjectSink>&rpRole) {
      m_pIVUnknownReceiver.getRole (rpRole);
    }
    
    //  ObjectSink Methods
  public:
    void OnData (ObjectSink *pRole, IVUnknown *pObject);
    void OnError_ (Vca::IError *pError, VString const &rMessage);

    //  IUpDownSubscriber Role
  private:
    Vca::VRole<ThisClass,IUpDownSubscriber> m_pIUpDownSubscriber;
  public:
    void getRole (VReference<IUpDownSubscriber>&rpRole) {
      m_pIUpDownSubscriber.getRole (rpRole);
    }
    
    //  IUpDownSubscriber Methods
  public:
    void OnUp (IUpDownSubscriber *pRole);
    void OnDown (IUpDownSubscriber *pRole);
    
    //  Sinks
  public:
    void onSubscription (ISubscriptionSink *pSink, ISubscription *pSubscription);
    void onSubscriptionError (ISubscriptionSink *pSink, Vca::IError *pError, VString const &rText);
    void onUpDownPublisher (IVUnknownSink *pSink, IVUnknown *pIUpDownPublisher);
    void onGetter (IVUnknownSink *pSink, IVUnknown *pIGetter);
    void onBusynessResult (U32Receiver *pReceiver, Vca::U32 rResult);
    void onBusynessError (U32Receiver *pReceiver, Vca::IError *pError, VString const &rText);

    //  Query
  protected:
    bool isPrimaryEntry (VString const &rName) const;
    bool isLastResortEntry (VString const &rName) const;
    bool isDone () const {
        return m_iState == STATE_FAILED || m_iState == STATE_SUCCEEDED;
    }

    //  Randomize
  protected:
    V::U32 getRandomArrayIndex (V::U32 iArraySize) const;

    //  State
  protected:
    State                              m_iState;

    //  Directory 
    Vca::IDirectory::Pointer           m_pDirectory; // soft reference to prevent circular reference
    VkDynamicArrayOf<VString>          m_iPrimaryFailoverEntries;
    VkDynamicArrayOf<VString>          m_iLastResortEntries;
    bool                               m_bHasPrimary;
    VString                            m_iPrimarySource;
    V::U32                             m_cPrimaryFailovers;
    V::U32                             m_cLastResorts;

    //  Attempts
    V::U32                             m_cPrimaryFailoversTried;
    V::U32                             m_cLastResortsTried;
    V::U32                             m_xNextEntryToTry;
	
    V::U32                             m_cBusynessesChecked;

    //  Current Try
    VString                            m_iEntryBeingTried;
    IEvaluator::Reference              m_pEvaluatorBeingProbed;
    ISubscription::Reference           m_pEvaluatorSubscription;
    ObjectHolder::Reference            m_pObjectHolderBeingProbed;
    ObjectHolder::Reference            m_pObjectHolderToReturn;
    V::U32                             m_xEvaluatorBusyness;

    IEvaluatorSink::Reference          m_pClient;
    VSmartEvaluatorSource::Reference   m_pSmartSource;
    
    //  Guard flags
    bool m_bProcessing;
    bool m_bWait;
  };  

}

/*************************************************
 *************************************************
 *****                                       *****
 *****  Vsa::VSmartEvaluatorSource::Request  *****
 *****                                       *****
 *************************************************
 *************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VSmartEvaluatorSource::Request::Request (
   Vca::IDirectory *pDirectory, VkDynamicArrayOf<VString> &rEntries, 
   IEvaluatorSink *pSink, VSmartEvaluatorSource *pSource
) : m_pDirectory (pDirectory)
  , m_bHasPrimary (false)
  , m_cPrimaryFailovers (0)
  , m_cLastResorts (0)
  , m_cPrimaryFailoversTried (0)
  , m_cLastResortsTried (0)
  , m_cBusynessesChecked(0)
  , m_xNextEntryToTry (0)
  , m_iState (STATE_NEW)
  , m_pClient (pSink)
  , m_pSmartSource (pSource)
  , m_bProcessing (false)
  , m_bWait (false)
  , m_pIVUnknownReceiver (this)
  , m_pIUpDownSubscriber (this) {
  traceInfo ("Creating Vsa::VSmartEvaluatorSource::Request");
  init (rEntries);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VSmartEvaluatorSource::Request::~Request () {
  traceInfo ("Destroying Vsa::VSmartEvaluatorSource::Request");
}

/******************
 ******************
 *****  Init  *****
 ******************
 ******************/

void Vsa::VSmartEvaluatorSource::Request::init (VkDynamicArrayOf<VString> &rEntries) {
  V::U32 iSize = rEntries.cardinality ();
  VString iHost; m_pSmartSource->getHostname (iHost);

  // scan the entries to see what we have ...
  for (V::U32 i=0;i<iSize; i++) {
    VString iEntry (rEntries[i].content ());
    
    VString iPrefix, iPostFix;
    if (m_bHasPrimary == false && iEntry.equalsIgnoreCase (iHost))
      m_bHasPrimary = true;
    else if (isLastResortEntry (iEntry))
      m_cLastResorts++;
    else
      m_cPrimaryFailovers++;
  }

  // organize the entries according to priority ...
  m_bHasPrimary = false;
  m_iPrimaryFailoverEntries.Append (m_cPrimaryFailovers);
  m_iLastResortEntries.Append (m_cLastResorts);

  for (V::U32 i=0, j=0, k=0;i<iSize; i++) {
    VString iEntry (rEntries[i].content ());
    
    VString iPrefix, iPostFix;
    if (m_bHasPrimary == false && iEntry.equalsIgnoreCase (iHost)) {
      m_iPrimarySource.setTo (iEntry);
      m_bHasPrimary = true;
    }
    else if (isLastResortEntry (iEntry))
      m_iLastResortEntries[j++].setTo (iEntry);
    else
      m_iPrimaryFailoverEntries[k++].setTo (iEntry);
  }
  
}

/*******************
 *******************
 *****  Start  *****
 *******************
 *******************/


void Vsa::VSmartEvaluatorSource::Request::start () {  
  m_pSmartSource->refreshConnectivity ();

  VReference<ObjectHolder> pObjHolder;
  pObjHolder.setTo (m_pSmartSource->getActiveObjectHolder ());
  if (pObjHolder ) {
    m_pObjectHolderBeingProbed.setTo (pObjHolder);
    m_pEvaluatorBeingProbed.setTo(static_cast<IEvaluator*>(pObjHolder->getObject()));
    m_iEntryBeingTried.setTo(pObjHolder->getName());
    m_bWait = true;
    CheckBusyness();
    //if the busyness return works then we are done
    if(m_iState == STATE_SUCCEEDED) {
        log ("SmartEvaluatorSource::Request: Cached unbusy evaluator already exists...");
        return;
    }
  }	else {
	process ();
	}
}


/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/


void Vsa::VSmartEvaluatorSource::Request::process () {
  if (m_bProcessing == false){
    m_bProcessing = true;
    while (!isDone () && !m_bWait) {
        tryNextEntry ();
    }
    m_bProcessing = false;
  }
}

void Vsa::VSmartEvaluatorSource::Request::tryNextEntry () {
  //  Perform state transitions and required "edge" actions
  switch (m_iState) {
  case STATE_NEW:
      if (m_bHasPrimary) {
	  m_iState = STATE_TRYING_PRIMARY;
	  break;
      }
  case STATE_TRYING_PRIMARY:
      if (m_cPrimaryFailovers > 0) {
	  m_iState = STATE_TRYING_PRIMARY_FAILOVERS;
	  m_xNextEntryToTry = getRandomArrayIndex (m_cPrimaryFailovers);
	  break;
      }
  case STATE_TRYING_PRIMARY_FAILOVERS:
      if (m_cPrimaryFailoversTried < m_cPrimaryFailovers && m_cBusynessesChecked <= m_pSmartSource->GetMaxBusynessChecks() )
	  ;// no state change needed
      else {
		m_iState = STATE_TRYING_LEAST_BUSY;
	  }
	  break;
  case STATE_TRYING_LEAST_BUSY:
	if (m_cLastResorts > 0) {
	  VString iErrMessage ("Warning: Trying the last resorts for the connection with existing sources");
	  notify (20, "#20: VSmartEvaluatorSource::Request::tryNextEntry: %s\n", iErrMessage.content ());
	  m_iState = STATE_TRYING_LASTRESORTS;
	  m_xNextEntryToTry = getRandomArrayIndex (m_cLastResorts);
      }
      else
		m_iState = STATE_FAILED;
      break;
  case STATE_TRYING_LASTRESORTS:
      if (m_cLastResortsTried < m_cLastResorts) 
	  ;// no state change needed
      else
	  m_iState = STATE_FAILED;
    break;
  case STATE_FAILED:
  default:
    break;
  }
  
  //  Perform the "node" actions associated with the current state
  switch (m_iState) {
  case STATE_TRYING_PRIMARY: 
    tryEntry (m_iPrimarySource);
    break;
    
  case STATE_TRYING_PRIMARY_FAILOVERS:
    {
    VString iCurrentEntry = m_iPrimaryFailoverEntries[m_xNextEntryToTry];
    m_xNextEntryToTry = m_xNextEntryToTry+1 < m_cPrimaryFailovers ? m_xNextEntryToTry+1 : 0;
    m_cPrimaryFailoversTried++;
    tryEntry (iCurrentEntry);
	}
    break;
	
  case STATE_TRYING_LEAST_BUSY: {
	ReturnLeastBusy();
	break;
  }
    
  case STATE_TRYING_LASTRESORTS: {
    VString iCurrentEntry = m_iLastResortEntries[m_xNextEntryToTry];
    m_xNextEntryToTry = m_xNextEntryToTry+1 < m_cLastResorts ? m_xNextEntryToTry+1 : 0; 
    m_cLastResortsTried++;
    tryEntry (iCurrentEntry);
  }
    break;
    
  case STATE_FAILED:
  default:
    VString iErrMessage ("Unable to provide valid connection with existing sources");
    notify (11, "#11: VSmartEvaluatorSource::Request::tryNextEntry: %s\n", iErrMessage.content ());
    log ("SmartEvaluatorSource::Request: %s", iErrMessage.content ());
    m_pClient->OnError (0, iErrMessage);
    break;
  }
}

void Vsa::VSmartEvaluatorSource::Request::tryEntry (VString const &rEntryName) {
	  VReference<ObjectHolder> pObjHolder;
	  if (m_pSmartSource->getObjectHolder (rEntryName, pObjHolder)) {
		  if (pObjHolder->isObjectUp ()) {			
		    m_pObjectHolderBeingProbed.setTo (pObjHolder);
			m_pEvaluatorBeingProbed.setTo(static_cast<IEvaluator*>(pObjHolder->getObject()));
			m_iEntryBeingTried.setTo(rEntryName);
			m_bWait = true;
			CheckBusyness();
		  }
		  else if (pObjHolder->isObjectReadyForRetry ()) {
			enquireDirectory (rEntryName);
		  }
	  }
	  else {
		enquireDirectory (rEntryName);
	}
}

void Vsa::VSmartEvaluatorSource::Request::enquireDirectory (VString const &rName) {
  log ("SmartEvaluatorSource::Request: Enquiring VcaDirectory for entry %s", rName.content ());

  VReference<ObjectSink> pObjectSink;
  getRole (pObjectSink);
  m_iEntryBeingTried.setTo (rName);
  m_bWait = true;
  m_pDirectory->GetObject (pObjectSink, rName, Vsa::IEvaluator::typeInfo ());
  
}

/*****************************
 *****************************
 *****  ObjectSink Role  *****
 *****************************
 *****************************/
 
void Vsa::VSmartEvaluatorSource::Request::OnData (ObjectSink *pRole, IVUnknown *pObject) {
  if (pObject) {
	m_pEvaluatorBeingProbed.setTo (static_cast<IEvaluator*> (pObject));
	bool bIsEntryPrimary = (m_iEntryBeingTried == m_iPrimarySource);
    
    VReference<IVUnknownSink> pSink (new IVUnknownSink (this, &ThisClass::onUpDownPublisher));
    m_pEvaluatorBeingProbed->QueryInterface (Vsa::IUpDownPublisher::typeInfo (), pSink);
  }
  else {
	m_pEvaluatorBeingProbed = 0;
    log ("SmartEvaluatorSource::Request: Null evaluator obtained for entry %s", m_iEntryBeingTried.content ());
    m_bWait = false;
    process ();
  }
}

void Vsa::VSmartEvaluatorSource::Request::OnError_(Vca::IError *pError, VString const &rMessage) {
  VString iErrMsg;
  iErrMsg << "SmartEvaluatorSource::Request: Error obtained for entry " << m_iEntryBeingTried << ": " << rMessage;
  log (iErrMsg);
  m_bWait = false;
  process ();
}

/******************
 ******************
 *****  Sink  *****
 ******************
 ******************/

 void Vsa::VSmartEvaluatorSource::Request::onUpDownPublisher (IVUnknownSink *pSink, IVUnknown *pIUpDownPublisher) {
  VReference<IUpDownPublisher> pPublisher;
  pPublisher.setTo (static_cast<Vsa::IUpDownPublisher*> (pIUpDownPublisher));
  

  bool bIsEntryPrimary = (m_iEntryBeingTried == m_iPrimarySource);
  VReference<ObjectHolder> pObjHolder (
     new ObjectHolder (m_iEntryBeingTried, m_pSmartSource, m_pEvaluatorBeingProbed, pPublisher, bIsEntryPrimary)
  );
  m_pSmartSource->link (pObjHolder);
  m_pObjectHolderBeingProbed.setTo (pObjHolder);
  
  //  Get the current UP/DOWN status
  if (pPublisher) {

    //  create subscriber 
    VReference<IUpDownSubscriber> pSubscriber;
    getRole (pSubscriber);
    
    //  create subscription sink
    VReference<ISubscriptionSink> pSink (new ISubscriptionSink (this, &ThisClass::onSubscription, &ThisClass::onSubscriptionError));
    // subscribe
    pPublisher->Subscribe (pSubscriber, pSink);
  }
  else {
    CheckBusyness();
  }
}
 
// Check the busyness of our current m_pEvaluatorBeingProbed
// First check if there's an override active
// If we can check its busyness and its the lowest one checked yet, set it as m_pEvaluatorToReturn and store its busyness
// If we can't check its busyness but we don't have m_pEvaluatorToReturn stored yet, store this one with a max busyness
void Vsa::VSmartEvaluatorSource::Request::CheckBusyness () {
	m_cBusynessesChecked++;
	Vca::U32 busynessOverride;
	if(m_pSmartSource->getBusynessOverride(m_iEntryBeingTried, busynessOverride)) {
		log("SmartEvaluatorSource::Request: Found a busyness override for %s", m_iEntryBeingTried.content());
		onBusynessResult(0, busynessOverride);
	} else {	
		VReference<IVUnknownSink> pSink (new IVUnknownSink (this, &ThisClass::onGetter));
		m_pEvaluatorBeingProbed->QueryInterface (Vca::IGetter_Ex2::typeInfo (), pSink);
	}
}
	
void Vsa::VSmartEvaluatorSource::Request::onGetter (IVUnknownSink *pSink, IVUnknown *pIGetter) {
	VReference<Vca::IGetter> pGetter;
	pGetter.setTo (static_cast<Vca::IGetter*> (pIGetter));
	if(pGetter) {
		U32Receiver::Reference pReceiver;
        pReceiver.setTo (new U32Receiver (this, &ThisClass::onBusynessResult, &ThisClass::onBusynessError));
		pGetter->GetU32Value (pReceiver, "Busyness");
	} else {
		if(!m_pObjectHolderToReturn) {
			m_pObjectHolderToReturn.setTo(m_pObjectHolderBeingProbed);
			m_xEvaluatorBusyness = 0;
			attemptReturn();
		} else {
			m_bWait = false;
			process();
		}
	}
}

void Vsa::VSmartEvaluatorSource::Request::onBusynessResult (U32Receiver *pReceiver, Vca::U32 rResult) {
	if(!m_pObjectHolderToReturn || rResult < m_xEvaluatorBusyness) {
		m_pObjectHolderToReturn.setTo(m_pObjectHolderBeingProbed);
		m_xEvaluatorBusyness = rResult;
		attemptReturn();
	} else {
		m_bWait = false;
		process();
	}
}

void Vsa::VSmartEvaluatorSource::Request::onBusynessError (U32Receiver *pReceiver, Vca::IError *pError, VString const &rText) {
	if(!m_pObjectHolderToReturn) {
		m_pObjectHolderToReturn.setTo(m_pObjectHolderBeingProbed);
		m_xEvaluatorBusyness = 0;
		attemptReturn();
	} else {
		m_bWait = false;
		process();
	}
}

void Vsa::VSmartEvaluatorSource::Request::attemptReturn () {
	if (shouldReturnEvaluator()) {
		m_pSmartSource->setActiveObjectHolder (m_pObjectHolderToReturn);
		IEvaluator::Reference const pEvaluator (
		static_cast <IEvaluator*> (m_pObjectHolderToReturn->getObject ())
		);
		m_iState = STATE_SUCCEEDED;
		m_bWait = false;
		m_pClient->OnData (pEvaluator);
	} else {
		m_bWait = false;
		process();
	}
}

void Vsa::VSmartEvaluatorSource::Request::ReturnLeastBusy () {
	if(m_pObjectHolderToReturn) {
		m_pSmartSource->setActiveObjectHolder (m_pObjectHolderToReturn);
		m_iState = STATE_SUCCEEDED;
		IEvaluator::Reference const pEvaluator (
		static_cast <IEvaluator*> (m_pObjectHolderToReturn->getObject ())
		);
		m_bWait = false;
		m_pClient->OnData (pEvaluator);
	} else {
		m_bWait = false;
		process();
	}
}

bool Vsa::VSmartEvaluatorSource::Request::shouldReturnEvaluator() {
	return (m_pObjectHolderToReturn && m_xEvaluatorBusyness < m_pSmartSource->GetBusynessThreshold()) || 
		(m_cBusynessesChecked >= m_pSmartSource->GetMaxBusynessChecks()) ||
		(m_iState == STATE_TRYING_LASTRESORTS);
}

void Vsa::VSmartEvaluatorSource::Request::onSubscription (ISubscriptionSink *pSink, ISubscription *pSubs) {
  m_pEvaluatorSubscription.setTo (pSubs);
  m_bWait = false;
} 

void Vsa::VSmartEvaluatorSource::Request::onSubscriptionError (ISubscriptionSink *pSink, Vca::IError *pError, VString const &rText) {
	m_bWait = false;
	process();
}

/************************************
 ************************************
 *****  IUpDownSubscriber Role  *****
 ************************************
 ************************************/

void Vsa::VSmartEvaluatorSource::Request::OnUp (IUpDownSubscriber *pRole) {
  CheckBusyness();
  m_pEvaluatorSubscription->Unsubscribe ();
  m_bWait = false;
}

void Vsa::VSmartEvaluatorSource::Request::OnDown (IUpDownSubscriber *pRole) {
  m_pEvaluatorSubscription->Unsubscribe ();
  m_bWait = false;
  process ();
}

/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

bool Vsa::VSmartEvaluatorSource::Request::isPrimaryEntry (VString const &rName) const {
  return rName == m_iPrimarySource;
}
 

bool Vsa::VSmartEvaluatorSource::Request::isLastResortEntry (VString const &rName) const {
  bool result = rName.equalsIgnoreCase ("LASTRESORT");
  if (!result) {
    VString iPrefix, iPostfix;
    rName.getPrefix ('_', iPrefix, iPostfix);
    result = iPrefix.equalsIgnoreCase ("LASTRESORT");
  }
  return result;
}

/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

V::U32 Vsa::VSmartEvaluatorSource::Request::getRandomArrayIndex (V::U32 iArraySize) const {
  Vca::U32 iRand = rand ();
  return iArraySize > 0 ? iRand % iArraySize: 0;
}

/******************************************************
 ******************************************************
 *****                                            *****
 *****  Vsa::VSmartEvaluatorSource::ObjectHolder  *****
 *****                                            *****
 ******************************************************
 ******************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VSmartEvaluatorSource::ObjectHolder::ObjectHolder (
    VString const &rName, VSmartEvaluatorSource *pSource, 
    IVUnknown *pObject, 
    IUpDownPublisher *pPublisher, 
    bool bIsPrimarySource
) : m_iName (rName)
  , m_pObject (pObject)
  , m_pSmartSource (pSource)
  , m_iStatus (pPublisher ? STATUS_BEING_PROBED : STATUS_NOT_SUPPORTED)
  , m_bIsPrimarySource (bIsPrimarySource)
  , m_bReadyForRetry (false)
  , m_pIUpDownSubscriber (this) {
  traceInfo ("Creating Vsa::VSmartEvaluatorSource::ObjectHolder");
  
  retain (); {
    start (pPublisher);
  } 
  untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VSmartEvaluatorSource::ObjectHolder::~ObjectHolder () {
  traceInfo ("Destroying Vsa::VSmartEvaluatorSource::ObjectHolder");
}

/*********************
 *********************
 *****  Process  *****
 *********************
 *********************/

void Vsa::VSmartEvaluatorSource::ObjectHolder::start (IUpDownPublisher *pPublisher) {
  if (pPublisher) {
    //  create subscriber 
    VReference<IUpDownSubscriber> pSubscriber;
    getRole (pSubscriber);
    
    //  create subscription sink
    VReference<ISubscriptionSink> pSink (new ISubscriptionSink (this, &ThisClass::onSubscription));
    
    // subscribe
    pPublisher->Subscribe (pSubscriber, pSink);
  }
}


void Vsa::VSmartEvaluatorSource::ObjectHolder::onSubscription (
  ISubscriptionSink *pSink, ISubscription *pSubs
) {
  m_pSubscription.setTo (pSubs);
} 

/******************
 ******************
 *****  Role  *****
 ******************
 ******************/

void Vsa::VSmartEvaluatorSource::ObjectHolder::OnUp (IUpDownSubscriber *pRole) {
  m_iStatus = STATUS_UP;
  
  // set cache in VSmartEvaluatorSource primary source
  if (m_bIsPrimarySource)
    m_pSmartSource->setActiveObjectHolder (this);
}

void Vsa::VSmartEvaluatorSource::ObjectHolder::OnDown (IUpDownSubscriber *pRole) {
  m_iStatus = STATUS_DOWN;

  //  clear cache in VSmartEvaluatorSource if required (currently in use)
  VReference<ObjectHolder> pObjHolder (m_pSmartSource->getActiveObjectHolder ());

  if (pObjHolder.referent () == this) 
    m_pSmartSource->clearActiveObjectHolder ();
}

/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

void Vsa::VSmartEvaluatorSource::ObjectHolder::getStatus (VString &iStatus) const {
  switch (m_iStatus) {
  case STATUS_BEING_PROBED:
    iStatus.setTo ("STATUS_BEING_PROBED");
    break;
  case STATUS_NOT_SUPPORTED:
    iStatus.setTo ("STATUS_NOT_SUPPORTED");
    break;
  case STATUS_UP:
    iStatus.setTo ("STATUS_UP");
    break;
  case STATUS_DOWN:
    iStatus.setTo ("STATUS_DOWN");
    break;
  case STATUS_NO_CONNECTION:
    iStatus.setTo ("STATUS_NO_CONNECTION");
    break;
  }
}

bool Vsa::VSmartEvaluatorSource::ObjectHolder::isObjectReadyForRetry () {
    if (m_bReadyForRetry) {
	m_bReadyForRetry = false;
	return true;
    }
    if (isObjectNotConnected ())
	m_bReadyForRetry = true;
    return false;
}

/*******************
 *******************
 *****  Update *****
 *******************
 *******************/

void Vsa::VSmartEvaluatorSource::ObjectHolder::refreshConnectivity () {
    if (m_pObject && m_pObject->isntConnected ())
	m_iStatus = STATUS_NO_CONNECTION;
}

/*************************
 *************************
 *****  Unsubscribe  *****
 *************************
 *************************/

void Vsa::VSmartEvaluatorSource::ObjectHolder::unsubscribe () {
    if (m_pSubscription) 
	m_pSubscription->Unsubscribe ();
}

/****************************************
 ****************************************
 *****                              *****
 *****  Vsa::VSmartEvaluatorSource  *****
 *****                              *****
 ****************************************
 ****************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VSmartEvaluatorSource::VSmartEvaluatorSource (Vca::IDirectory *pDirectory, Vca::U32 busynessThreshold, Vca::U32 maxBusynessChecks)
  : m_pDirectory (pDirectory), m_iBusynessThreshold(busynessThreshold), m_iMaxBusynessChecks(maxBusynessChecks) {
  traceInfo ("Creating Vsa::VSmartEvaluatorSource");
}


/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VSmartEvaluatorSource::~VSmartEvaluatorSource () {
    traceInfo ("Destroying Vsa::VSmartEvaluatorSource");
    //  send unsubscribe message to current publishers and unlink
    ObjectHolder::Reference pObjectHolder (m_pObjectHolderList);
    while (pObjectHolder) {
	pObjectHolder->unsubscribe ();
	unlink (pObjectHolder);
	pObjectHolder.setTo (m_pObjectHolderList);
    }
}

/******************
 ******************
 *****  Init  *****
 ******************
 ******************/

void Vsa::VSmartEvaluatorSource::init () {
  //  seed the random number generator
  V::VTime currentTime;
  time_t time = currentTime; 
  srand (time);
  
  VReference<StringArraySink> pSink (new StringArraySink (this, &ThisClass::onEntries));
  m_pDirectory->GetEntries (pSink);
}

void Vsa::VSmartEvaluatorSource::setBusynessMap (BusynessOverrideMap busynessOverrides) {
	// temporary until figure out better way to set the map
	Iterator iterator (busynessOverrides);
	while (iterator.isNotAtEnd ()) {
		Vca::U32 xIndex;
		Vca::U32 busynessOverride;
		VString session = iterator.key ();
		busynessOverrides.Locate (session.content(), xIndex);
		busynessOverride = busynessOverrides [xIndex];
		addBusynessMapEntry(session, busynessOverride);
		++iterator;
	}
}

void Vsa::VSmartEvaluatorSource::addBusynessMapEntry (VString &iName, Vca::U32 iBusynessOverride) {
	unsigned int xIndex;
	if(m_iBusynessOverrides.Insert(iName.content(), xIndex)) {
		m_iBusynessOverrides[xIndex] = iBusynessOverride;
	}
}

/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

/***
 * Note: getHostname in VMS returns in UpperCase
 * 
 ***/

void Vsa::VSmartEvaluatorSource::getHostname (VString &rHostname) {
    static VString g_iHostName;
    if (g_iHostName.isEmpty ()) {
	char iBuffer[256]; 
	VString iHostFullName, iName, iDomain;
	iHostFullName.setTo (VkSocketAddress::hostname (iBuffer, sizeof (iBuffer)));
	if (iHostFullName.getPrefix ('.', iName, iDomain))
	    g_iHostName.setTo (iName);
	else
	    g_iHostName.setTo (iHostFullName);
    }
    rHostname.setTo (g_iHostName);
}

bool Vsa::VSmartEvaluatorSource::getObjectHolder (VString const &rName, VReference<ObjectHolder> &rpHolder) const {
  bool bFound = false;
  VReference<ObjectHolder> pObjHolder (m_pObjectHolderList);

  while (pObjHolder && !bFound) {
    VString iName (pObjHolder->getName ());
    if (iName.equalsIgnoreCase (rName)) {
      rpHolder.setTo (pObjHolder);
      bFound = true;
    }
    pObjHolder.setTo (pObjHolder->m_pSuccessor);
  }
  return bFound;
}


/**********************
 **********************
 *****  Callback  *****
 **********************
 **********************/

void Vsa::VSmartEvaluatorSource::onEntries (StringArraySink* pSink, VkDynamicArrayOf<VString> const &rArray) {
  V::U32 iSize = rArray.cardinality ();
  if (iSize>0) {

    m_iEntries.Append (iSize);
    for (V::U32 i=0;i<iSize; i++) {
      m_iEntries[i].setTo (rArray[i].content ());
    }
  }
}

bool Vsa::VSmartEvaluatorSource::getBusynessOverride(VString entry, Vca::U32 &busynessOverride) {
	Vca::U32 xIndex;
	if(m_iBusynessOverrides.Locate (entry.content(), xIndex)) {
		busynessOverride = m_iBusynessOverrides [xIndex];
		return true;
	}
	return false;
}

/*********************
 *********************
 *****  Linking  *****
 *********************
 *********************/

void Vsa::VSmartEvaluatorSource::link (ObjectHolder *pHolder) {
  if (pHolder) {
    pHolder->m_pSuccessor.setTo (m_pObjectHolderList);
    if (m_pObjectHolderList)
      m_pObjectHolderList->m_pPredecessor.setTo (pHolder);
    m_pObjectHolderList.setTo (pHolder);
  }
}

void Vsa::VSmartEvaluatorSource::unlink (ObjectHolder *pHolder) {
    if (pHolder) {
	//  if head
	if (pHolder == m_pObjectHolderList) {
	    m_pObjectHolderList.setTo (pHolder->m_pSuccessor);
	    if (m_pObjectHolderList)
		m_pObjectHolderList->m_pPredecessor.clear ();
	}
	else {
	    pHolder->m_pPredecessor->m_pSuccessor.setTo (pHolder->m_pSuccessor);
	    if (pHolder->m_pSuccessor)
		pHolder->m_pSuccessor->m_pPredecessor.setTo (pHolder->m_pPredecessor);
	}
	pHolder->m_pSuccessor.clear ();
	pHolder->m_pPredecessor.clear ();
  }
}

/******************
 ******************
 *****  Role  *****
 ******************
 ******************/

void Vsa::VSmartEvaluatorSource::Supply (IEvaluatorSource *pRole, IEvaluatorSink *pSink) {
    if (pSink)
	supply (pSink);
}

void Vsa::VSmartEvaluatorSource::supply (IEvaluatorSink *pSink) {
  if (pSink) {
    refreshConnectivity ();
      // otherwise create a request object to process request
      Request::Reference const pRequest (new Request (m_pDirectory, m_iEntries, pSink, this));
      pRequest->start ();
  }
}

/**********************
 **********************
 *****  Liveness  *****
 **********************
 **********************/

void Vsa::VSmartEvaluatorSource::refreshConnectivity () {
  if(m_pActiveObjectHolder) {
    m_pActiveObjectHolder->refreshConnectivity (); 
    if (!m_pActiveObjectHolder->isObjectUp ()) 
      clearActiveObjectHolder ();
  }
  
  VReference<ObjectHolder> pObjHolder (m_pObjectHolderList);
  while (pObjHolder) {
    pObjHolder->refreshConnectivity ();
    pObjHolder.setTo (pObjHolder->m_pSuccessor);
  }
}




