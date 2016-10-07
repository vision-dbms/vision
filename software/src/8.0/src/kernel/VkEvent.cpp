/*****  Vision Kernel Event Implementation  *****/

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

#include "VkEvent.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_ITrigger.h"

#include "VpSocket.h"

/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

static VkEvent* g_pFirstActiveEvent = NilOf (VkEvent*);


/*********************
 *********************
 *****           *****
 *****  VkEvent  *****
 *****           *****
 *********************
 *********************/

/************************
 ************************
 *****  State Name  *****
 ************************
 ************************/

char const *VkEvent::nameFor (State xState) {
    switch (xState) {
    case State_Failed:
	return "Failed";
    case State_Signaled:
	return "Signaled";
    case State_AcceptWait:
	return "AcceptWait";
    case State_ReadWait:
	return "ReadWait";
    case State_WriteWait:
	return "WriteWait";
    default:
	return string ("State %u", xState);
    }
}

/***************************
 ***************************
 *****  Construction   *****
 ***************************
 ***************************/

VkEvent::VkEvent (HANDLE hEvent)
: m_hEvent	(hEvent)
, m_xState	(State_Signaled)
, m_pPredecessor(NilOf (VkEvent*))
, m_pSuccessor	(NilOf (VkEvent*))
, m_bTriggered	(false)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VkEvent::~VkEvent () {
#if defined(_WIN32)
    CloseHandle (m_hEvent);
#endif
}


/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

bool VkEvent::isSignaled (size_t sTimeout) {
    if (m_bTriggered)
	return true;

#if defined(_WIN32)
/**************************
 *  WIN32 Implementation  *
 **************************/

    switch (m_xState) {
    default:
	return false;

    case State_Failed:
    case State_Signaled:
	return true;

    case State_AcceptWait:
    case State_ReadWait:
    case State_WriteWait:
	break;
    }

    switch (WaitForSingleObject (m_hEvent, sTimeout)) {
    case WAIT_TIMEOUT:
	return false;

    case WAIT_ABANDONED_0:
    case WAIT_OBJECT_0:
	return true;

    default:
	setStateTo (State_Failed);
	return true;
    }


#else
/*************************
 *  UNIX Implementation  *
 *************************/

    fd_set	iSSet;
    fd_set*	pRSet = NilOf (fd_set*);
    fd_set*	pWSet = NilOf (fd_set*);

    switch (m_xState) {
    default:
	return false;

    case State_Failed:
    case State_Signaled:
	return true;

    case State_AcceptWait:
    case State_ReadWait:
	pRSet = &iSSet;
	break;

    case State_WriteWait:
	pWSet = &iSSet;
	break;
    }

    struct timeval iTimeout;
    iTimeout.tv_sec	= sTimeout / 1000;
    iTimeout.tv_usec	= sTimeout % 1000 * 1000;

    FD_ZERO	(&iSSet);
    FD_SET	(m_hEvent, &iSSet);

    int nFound = select (
	m_hEvent + 1,
	pRSet,
	pWSet,
	NilOf (fd_set*),
	sTimeout < Vk_InfiniteWait ? &iTimeout : NilOf (struct timeval *)
    );

    switch (nFound) {
    case 0:
	return false;
    case 1:
	setTrigger ();
	return true;
    default:
	setStateTo (State_Failed);
	return true;
    }
#endif
}


/********************************
 ********************************
 *****  Handler Management  *****
 ********************************
 ********************************/

void VkEvent::callHandler () {
    if (m_iHH)
	m_iHH->Process ();
}

void VkEvent::claimHandler (HandlerHolder &rHH, State xState) {
#if defined(EVENT_DEBUG_TRACE)
    printf (
	"+++ %p: claim: [%-10.10s, %p]->[%-10.10s, %p]\n",
	this, stateName (), m_iHH.referent (), nameFor (xState), rHH.referent ()
    );
    fflush (stdout);
#endif

    m_xState = xState;
    m_bTriggered = false;

    if (rHH.isntEmpty ()) {
	m_iHH.claim (rHH);
	m_pSuccessor = g_pFirstActiveEvent;
	if (g_pFirstActiveEvent)
	    g_pFirstActiveEvent->m_pPredecessor = this;
	g_pFirstActiveEvent = this;
    }
}

void VkEvent::clearHandler (HandlerHolder &rHH, State xState) {
#if defined(EVENT_DEBUG_TRACE)
    printf (
	"+++ %p: clear: [%-10.10s, %p]->[%-10.10s, %p]\n",
	this, stateName (), m_iHH.referent (), nameFor (xState), rHH.referent ()
    );
    fflush (stdout);
#endif

    m_xState = xState;
    m_bTriggered = false;

    if (m_iHH.isntEmpty ()) {
	rHH.claim (m_iHH);
	if (m_pSuccessor)
	    m_pSuccessor->m_pPredecessor = m_pPredecessor;
	if (m_pPredecessor)
	    m_pPredecessor->m_pSuccessor = m_pSuccessor;
	else
	    g_pFirstActiveEvent = m_pSuccessor;
	m_pPredecessor = m_pSuccessor = NilOf (VkEvent*);
    }
}


/**************************
 **************************
 *****                *****
 *****  VkEvent::Set  *****
 *****                *****
 **************************
 **************************/

/**********************
 *****  Addition  *****
 **********************/

unsigned int VkEventSet::AddActiveEvents () {
    unsigned int cEventsAdded = 0;
    for (VkEvent* pEvent = g_pFirstActiveEvent; pEvent; pEvent = pEvent->m_pSuccessor)
	cEventsAdded += Add (pEvent) ? 1 : 0;

    return cEventsAdded;
}

bool VkEventSet::Add (VkEvent* pEvent) {
    unsigned int xArray;

    switch (pEvent->state ()) {
    case VkEvent::State_AcceptWait:
    case VkEvent::State_ReadWait:
    case VkEvent::State_WriteWait:
	break;
    default:
	return false;
    }

    if (m_sEventSet >= MAXIMUM_WAIT_OBJECTS)
	return false;

    xArray = m_sEventSet++;
    m_pEvent[xArray] = pEvent;
#if defined(_WIN32)
    if (pEvent->handle () == INVALID_HANDLE_VALUE)
	return false;

    m_hEvent[xArray] = pEvent->handle ();
#endif

#if defined(EVENT_DEBUG_TRACE)
    printf ("+++ %p: %s\n", pEvent, pEvent->stateName ());
    fflush (stdout);
#endif

    return true;
}


/***********************
 *****  Selection  *****
 ***********************/

VkEvent *VkEventSet::getNextTriggeredEvent (size_t sTimeout) {
    VkEvent *pEvent = NilOf (VkEvent*);
    getNextEvent (sTimeout, pEvent);
    return pEvent;
}

VkEventSet::Got VkEventSet::getNextEvent (size_t sTimeout, VkEvent *&rpEvent) {
//  First look for events that are already signaled, ...
    for (unsigned int xEvent = 0; xEvent < m_sEventSet; xEvent++) {
	if (m_pEvent[xEvent]->isTriggered ()) {
	    rpEvent = remove (xEvent);
	    return Got_Event;
	}
    }

#if 0 // defined(__VMS)
/**************************
 *  Alpha Implementation  *
 **************************/



#elif defined(_WIN32)
/**************************
 *  WIN32 Implementation  *
 **************************/

/*---------------------------------------------------------------------------------*
 *  Special treatment for empty event sets is provided because the documentation   *
 *  for WaitForMultipleObjects does not specify how empty event sets are handled.  *
 *---------------------------------------------------------------------------------*/
    if (m_sEventSet > 0) {
	DWORD xEvent = WaitForMultipleObjects (m_sEventSet, m_hEvent, false, sTimeout);
	if (xEvent >= WAIT_OBJECT_0 && xEvent < WAIT_OBJECT_0 + m_sEventSet)
	    xEvent -= WAIT_OBJECT_0;
	else if (xEvent >= WAIT_ABANDONED_0 && xEvent < WAIT_ABANDONED_0 + m_sEventSet)
	    xEvent -= WAIT_ABANDONED_0;
	else return xEvent == WAIT_TIMEOUT
	    ? Got_Timeout : xEvent == WAIT_OBJECT_0 + m_sEventSet
	    ? Got_Message : Got_Error;

	rpEvent = remove (xEvent);
	return Got_Event;
    }
    if (Vk_InfiniteWait == sTimeout)
	return Got_EmptySet;

    static HANDLE hSleeper = INVALID_HANDLE_VALUE;
    if (SystemHandleIsInvalid (hSleeper))
	hSleeper = CreateEvent (NULL, true, false, NULL);

    return SystemHandleIsValid (hSleeper) && WAIT_TIMEOUT == WaitForSingleObject (hSleeper, sTimeout)
	? Got_Timeout : Got_Error;


#else
/*************************
 *  UNIX Implementation  *
 *************************/

    fd_set iRSet;
    FD_ZERO (&iRSet);

    fd_set iWSet;
    FD_ZERO (&iWSet);

    int nfds = 0;
    for (unsigned int xEvent = 0; xEvent < m_sEventSet; xEvent++) {
	VkEvent* pEvent = m_pEvent[xEvent];
	switch (pEvent->m_xState) {
	case VkEvent::State_AcceptWait:
	case VkEvent::State_ReadWait:
	    FD_SET (pEvent->m_hEvent, &iRSet);
	    if (nfds < pEvent->m_hEvent + 1)
		nfds = pEvent->m_hEvent + 1;
	    break;
	case VkEvent::State_WriteWait:
	    FD_SET (pEvent->m_hEvent, &iWSet);
	    if (nfds < pEvent->m_hEvent + 1)
		nfds = pEvent->m_hEvent + 1;
	    break;
	}
    }
    if (0 == nfds && Vk_InfiniteWait == sTimeout)
	return Got_EmptySet;

    struct timeval iTimeout;
    iTimeout.tv_sec	= sTimeout / 1000;
    iTimeout.tv_usec	= sTimeout % 1000 * 1000;

    nfds = ::select (
	nfds,
	&iRSet,
	&iWSet,
	NilOf (fd_set*),
	sTimeout < Vk_InfiniteWait ? &iTimeout : NilOf (struct timeval *)
    );
    if (nfds <= 0)
	return nfds < 0 ? Got_Error : Got_Timeout;

    Got xResult = Got_Timeout;
    for (unsigned int xEvent = 0; xEvent < m_sEventSet; xEvent++) {
	VkEvent *pEvent = m_pEvent[xEvent];
	if (FD_ISSET (pEvent->m_hEvent, &iRSet) || FD_ISSET (pEvent->m_hEvent, &iWSet)) {
	    pEvent->setTrigger ();
	    if (xResult != Got_Event) {
		rpEvent = remove (xEvent);
		xResult = Got_Event;
	    }
	}
    }
    return xResult;
#endif
}


/*********************
 *****  Removal  *****
 *********************/

VkEvent *VkEventSet::remove (unsigned int xEvent) {
    VkEvent *pEvent = m_pEvent[xEvent];
    if (xEvent < --m_sEventSet) {
	m_pEvent[xEvent] = m_pEvent[m_sEventSet];
#if defined(_WIN32)
	m_hEvent[xEvent] = m_hEvent[m_sEventSet];
#endif
    }
    return pEvent;
}
