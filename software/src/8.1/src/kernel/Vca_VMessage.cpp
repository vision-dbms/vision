/*****  Vca_VMessage Implementation  *****/

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

#include "Vca_VMessage.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "IVUnknown.h"


/***************************
 ***************************
 *****                 *****
 *****  Vca::VMessage  *****
 *****                 *****
 ***************************
 ***************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VMessage::VMessage (
   Starter *pStarter, Manager *pMgr
) : Schedulable (this), m_xMessage (pStarter->nextMessageIndex ()), m_xStatus (Status_New), m_pManager (pMgr) {
}

Vca::VMessage::VMessage (
    unsigned int xMessage, Manager *pMgr
) : Schedulable (this), m_xMessage (xMessage), m_xStatus (Status_Arriving), m_pManager (pMgr) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VMessage::~VMessage () {
}


/************************
 ************************
 *****  Evaluation  *****
 ************************
 ************************/

void Vca::VMessage::evaluate (Reference *&rppSuccessor) {
    evaluate ();
    rppSuccessor = &m_pSuccessor;
}

void Vca::VMessage::evaluate () {
    switch (m_xStatus) {
    case Status_New:
    case Status_Received:
	m_xStatus = Status_Run;
	evaluate_();
	break;
    }
}

void Vca::VMessage::evaluateIncomingFrom (VcaSite *pSite) {
    setStatusToReceived ();
    if (recipient ())
        recipient ()->evaluateIncomingFrom (this, pSite);
}


/*********************
 *********************
 *****  Linkage  *****
 *********************
 *********************/

/*---------------------------------------------------------------------------*
 *  'addToList' operates on the pointer that references the first element
 *  of the list.  This double indirection enables the insertion of the
 *  message anywhere in the list, including the beginning.  'evaluate'
 *  and 'successor' assist by returning the address of 'm_pSuccessor', the
 *  member that points to the list of invocations that follow this one.
 *---------------------------------------------------------------------------*/
void Vca::VMessage::addToList (Reference *ppListHead) {
    count_t xListHead = SequenceNumber (*ppListHead);
    while (m_xMessage > xListHead) {
	ppListHead = &(*ppListHead)->m_pSuccessor;
	xListHead = SequenceNumber (*ppListHead);
    }

    if (m_xMessage != xListHead) {	// suppress duplicates
	m_pSuccessor.setTo (*ppListHead);
	(*ppListHead).setTo (this);
    }
}

/*---------------------------------------------------------------------------*
 *  'successor' returns the list of invocations that follow the recipient.
 *  This routine is written to use the same double indirection convention
 *  employed by 'evaluate' and 'addToList'.  Because this routine returns
 *  the address of one of its members, the object containing that member
 *  must be protected from reclamation.  That is accomplished by setting
 *  'rpSuccessorContainer' to the object containing the member.
 *
 *  The handling of NULL 'this' pointers allows the representation and
 *  management of empty lists by using 'rpSuccessorContainer' as the
 *  head of that empty list.
 *---------------------------------------------------------------------------*/
Vca::VMessage::Reference *Vca::VMessage::Successor (
    ThisClass *pMessage, Reference &rpSuccessorContainer
) {
    rpSuccessorContainer.setTo (pMessage);
    return pMessage ? &pMessage->m_pSuccessor : &rpSuccessorContainer;
}


/************************
 ************************
 *****  Scheduling  *****
 ************************
 ************************/

void Vca::VMessage::schedule_() {
    if (m_pStarter.isNil ())
	evaluate ();
    else {
	Starter::Reference pStarter;
	pStarter.claim (m_pStarter);
	pStarter->start (this);
    }
}

/***************************
 ***************************
 *****  Serialization  *****
 ***************************
 ***************************/

Vca::VcaSerializer *Vca::VMessage::serializer (VcaSerializer *pCaller, unsigned int xParameter) {
    return arguments_().serializer (pCaller, xParameter);
}


/******************************
 ******************************
 *****  Status Callbacks  *****
 ******************************
 ******************************/

void Vca::VMessage::onError (Vca::IError *pError, VString const &rMessage) {
  setStatusToError ();
  if (isManaged ()) 
    m_pManager->onError (pError, rMessage);
}

void Vca::VMessage::onSent () {
  setStatusToSent ();
  if (isManaged ())
    m_pManager->onSent ();
}

/*********************
 *********************
 *****  Manager  *****
 *********************
 *********************/

bool Vca::VMessage::isManaged () const {
  return m_pManager.isntNil ();
}

/************************
 ************************
 *****  SubManager  *****
 ************************
 ************************/

bool Vca::VMessage::subManager (VMessageManager::Reference &rpManager) const {
    if (isManaged ()) {
	rpManager.setTo (new SubManager (m_pManager));
	return true;
    }
    rpManager.clear ();
    return false;
}

/********************
 ********************
 *****  Status  *****
 ********************
 ********************/

bool Vca::VMessage::setStatusToSent () {
    switch (m_xStatus) {
    case Status_New:
	m_xStatus = Status_Sent;
	return true;
    }
    return false;
}

bool Vca::VMessage::setStatusToReceived () {
    switch (m_xStatus) {
    case Status_Arriving:
	m_xStatus = Status_Received;
	return true;
    }
    return false;
}

bool Vca::VMessage::setStatusToRun () {
    switch (m_xStatus) {
    case Status_New:
    case Status_Sent:
    case Status_Received:
	m_xStatus = Status_Run;
	return true;
    }
    return false;
}

bool Vca::VMessage::setStatusToConfirmed () {
    switch (m_xStatus) {
    case Status_Run:
	m_xStatus = Status_Confirmed;
	return true;
    }
    return false;
}

bool Vca::VMessage::setStatusToError () {
    switch (m_xStatus) {
    case Status_New:
    case Status_Arriving:
      m_xStatus = Status_Error;
      return true;
    }
    return false;
}


