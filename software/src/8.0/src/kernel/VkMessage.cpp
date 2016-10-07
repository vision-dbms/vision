/*****  Vision Kernel Message Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

#include "VkMessage.h"


/********************************
 ********************************
 *****  Message Controller  *****
 ********************************
 ********************************/

/**************************************
 *****  Default Message Handlers  *****
 **************************************/

void VkMessageController::PrepareIncomingMessage (
    VkMessage *pMessage, VkMessageReceivingAgent *pAgent
) {
}

void VkMessageController::ReceiveIncomingMessage (
    VkMessage *pMessage, VkMessageReceivingAgent *pAgent
) {
}

void VkMessageController::PrepareOutgoingMessage (
    VkMessage *pMessage, VkMessageShippingAgent *pAgent
) {
}

void VkMessageController::DiscardOutgoingMessage (
    VkMessage *pMessage, VkMessageShippingAgent *pAgent
) {
}


/**********************************
 **********************************
 *****  Message Target Index  *****
 **********************************
 **********************************/

/************************
 *****  Conversion  *****
 ************************/

PublicFnDef bool VkMessageTargetIndex_Convert (
    VkMessageTargetIndex *pThis, unsigned int xDataFormat
) {
    switch (xDataFormat) {
    case Vk_DataFormatReversed:
	Vk_ReverseSizeable (pThis);
	return true;

    case Vk_DataFormatNative:
	return true;

    default:
	return false;
    }
}


/**********************************
 **********************************
 *****  Message Target Table  *****
 **********************************
 **********************************/

/************************
 *****  Definition  *****
 ************************/

#define VkMessageTargetTable_InitialSize	1024
#define VkMessageTargetTable_GrowthIncrement	1024

/********************
 *****  Update  *****
 ********************/

VkMessageTargetIndex VkMessageTargetTable::Add (VkMessageController *pController) {
    VkMessageTargetIndex xFreeEntry = m_xFirstFree;

    if (xFreeEntry < m_sActiveArea)
	m_xFirstFree = m_pTable[xFreeEntry].m_xNextFree;
    else if (xFreeEntry < m_sTable)
	m_xFirstFree = ++m_sActiveArea;
    else {
	m_xFirstFree = ++m_sActiveArea;
	m_sTable += VkMessageTargetTable_GrowthIncrement;
	m_pTable = (Entry*)reallocate (m_pTable, m_sTable * sizeof (Entry));
    }

    m_pTable[xFreeEntry].construct (pController);

    return xFreeEntry;
}

void VkMessageTargetTable::Remove (VkMessageTargetIndex xTarget) {
    if (xTarget < m_sActiveArea && m_pTable[xTarget].IsValid ()) {
	m_pTable[xTarget].construct (m_xFirstFree);
	m_xFirstFree = xTarget;
    }
}


/********************
 *****  Access  *****
 ********************/

bool VkMessageTargetTable::GetTargetMessageController (
    VkMessageTargetIndex xTarget, VkMessageController **pTargetMessageController
) const {
    if (xTarget < m_sActiveArea && m_pTable[xTarget].IsValid ()) {
	*pTargetMessageController = m_pTable[xTarget].Controller ();
	return true;
    }

    return false;
}


/**************************
 *****  Construction  *****
 **************************/

VkMessageTargetTable::VkMessageTargetTable (VkMessageController *pRootObject) {
    Entry *pTable = (Entry*)allocate (
	VkMessageTargetTable_InitialSize * sizeof (Entry)
    );

    pTable[VkMessageTargetIndex_Root].construct (pRootObject);

    m_xFirstFree	=
    m_sActiveArea	= VkMessageTargetIndex_Root + 1; 
    m_sTable		= VkMessageTargetTable_InitialSize;
    m_pTable		= pTable;
}

/*************************
 *****  Destruction  *****
 *************************/

void VkMessageTargetTable::Destroy () {
    deallocate (m_pTable);
}


/****************************
 ****************************
 *****  Message Header  *****
 ****************************
 ****************************/

/************************
 *****  Conversion  *****
 ************************/

void VkMessageHeader::Convert () {
    union bitFields_t {
	struct forward_t {
	    unsigned int	m_xFormat	:  8,
				m_fUnused	:  8,
				m_xType		: 16;
	} inStructOrder;
	struct reverse_t {
	    unsigned int	m_xType		: 16,
				m_fUnused	:  8,
				m_xFormat	:  8;
	} inReversedOrder;
    } bitFields;

    switch (m_xFormat)
    {
    case Vk_DataFormatNative:
	break;

    case Vk_DataFormatReversed:
	Vk_ReverseSizeable (&m_xSender);
	Vk_ReverseSizeable (&m_xTarget);

	bitFields.inStructOrder.m_xFormat	= m_xFormat;
	bitFields.inStructOrder.m_fUnused	= m_fUnused;
	bitFields.inStructOrder.m_xType		= m_xType;
	Vk_ReverseSizeable (&bitFields);
	m_xFormat	= bitFields.inReversedOrder.m_xFormat;
	m_fUnused	= bitFields.inReversedOrder.m_fUnused;
	m_xType		= bitFields.inReversedOrder.m_xType;

	Vk_ReverseSizeable (&m_sBody);
	break;

    default:
	break;
    }
}

/**************************
 *****  Construction  *****
 **************************/

void VkMessageHeader::construct (
    VkMessageTargetIndex	xSender,
    VkMessageTargetIndex	xTarget,
    unsigned short		xType,
    unsigned int		sBody
) {
    m_xSender		= xSender;
    m_xTarget		= xTarget;
    m_xFormat		= Vk_DataFormatNative;
    m_fUnused		= 0;
    m_xType		= xType;
    m_sBody		= sBody;
}


/*********************
 *********************
 *****  Message  *****
 *********************
 *********************/

/********************
 *****  Update  *****
 ********************/

void VkMessage::AttachBody (void *pBody) {
    DetachBody ();
    m_pBody = pBody;
}

void VkMessage::DetachBody () {
    if (m_fBodyAllocated) {
	m_fBodyAllocated = false;
	deallocate (m_pBody);
    }
    m_pBody = NilOf (void*);
}

bool VkMessage::RealizeBody () {
    if (IsntNil (m_pBody) || 0 == Size ())
	return true;

    if (IsntNil (m_pBody = allocate (Size ()))) {
	m_fBodyAllocated= true;
	return true;
    }

    return false;
}

/************************
 *****  Conversion  *****
 ************************/

void VkMessage::Convert () {
    m_iHeader.Convert ();
}

/***********************
 *****  Callbacks  *****
 ***********************/

void VkMessage::PrepareIncomingMessage (VkMessageReceivingAgent *pAgent) {
    m_pController->PrepareIncomingMessage (this, pAgent);
}

void VkMessage::ReceiveIncomingMessage (VkMessageReceivingAgent *pAgent) {
    m_pController->ReceiveIncomingMessage (this, pAgent);
}

void VkMessage::PrepareOutgoingMessage (VkMessageShippingAgent *pAgent) {
    m_pController->PrepareOutgoingMessage (this, pAgent);
}

void VkMessage::DiscardOutgoingMessage (VkMessageShippingAgent *pAgent) {
    m_pController->DiscardOutgoingMessage (this, pAgent);
}

/*************************
 *****  Destruction  *****
 *************************/

void VkMessage::Destroy () {
    DetachBody ();
}

/**************************
 *****  Construction  *****
 **************************/

void VkMessage::construct (
    VkMessageController	       *pController,
    VkMessageTargetIndex	xSender,
    VkMessageTargetIndex	xTarget,
    unsigned short		xType,
    unsigned int			sBody,
    void const*			pBody
)
{
    m_pController = pController;
    m_iHeader.construct (xSender, xTarget, xType, sBody);

    m_pBody		= (void*)pBody;
    m_fBodyAllocated	= false;
}


/*********************************
 *********************************
 *****  Message Queue Entry  *****
 *********************************
 *********************************/

/**************************
 *****  Construction  *****
 **************************/

VkMessageShippingAgent::QueueEntry::QueueEntry (
    VkMessageController	       *pController,
    VkMessageTargetIndex	xSender,
    VkMessageTargetIndex	xTarget,
    unsigned short		xType,
    unsigned int		sBody,
    void const*			pBody
)
{
    m_pNext = NilOf (QueueEntry*);
    m_iOutgoingMessage.construct (pController, xSender, xTarget, xType, sBody, pBody);
}


/***************************
 ***************************
 *****  Message Agent  *****
 ***************************
 ***************************/

/*********************************
 *****  Callback Processing  *****
 *********************************/

void VkMessageReceivingAgent::OnCompletion () {
    m_pIncomingCursor += m_sData;
}

void VkMessageShippingAgent::OnCompletion () {
    m_pOutgoingCursor += m_sData;
}


/*******************
 *****  Input  *****
 *******************/

bool VkMessageReceivingAgent::ReadData () {
    VkStatus iStatus;
    return Complete (&iStatus) && (
	m_pIncomingCursor >= m_pIncomingLimit || StartRead (
	    m_pIncomingCursor, V_Min (
		m_sIncomingChunk, (unsigned int)(m_pIncomingLimit - m_pIncomingCursor)
	    ), &iStatus
	)
    );
}


/********************
 *****  Output  *****
 ********************/

bool VkMessageShippingAgent::WriteData () {
    VkStatus iStatus;
    return Complete (&iStatus) && (
	m_pOutgoingCursor >= m_pOutgoingLimit || StartWrite (
	    m_pOutgoingCursor, V_Min (
		m_sOutgoingChunk, (unsigned int)(m_pOutgoingLimit - m_pOutgoingCursor)
	    ), &iStatus
	)
    );
}


/******************************
 *****  Event Processing  *****
 ******************************/

bool VkMessageReceivingAgent::ProcessEvent () {
    bool notDone = true;

#ifdef TracingStartStop
    fprintf (stderr, "+++ Receiving Agent Started\n");
    fflush  (stderr);
#endif
    do {
	switch (m_xIncomingState) {
	case State_Ready:
	    m_pIncomingCursor = (pointer_t)&m_iIncomingMessage.m_iHeader;
	    m_pIncomingLimit  = m_pIncomingCursor + sizeof (VkMessageHeader);

	    m_xIncomingState = State_HeadArriving;
	    break;

	case State_HeadArriving:
	    notDone = ReadData ();
#ifdef TracingStartStop
	    if (!notDone) {
		fprintf (
		    stderr,
		    "+++ Receiving Agent Blocked: Head %u/%u\n",
		    (unsigned int)(m_pIncomingLimit - m_pIncomingCursor),
		    sizeof (VkMessageHeader)
		);
		fflush (stderr);
	    }
#endif
	    if (!notDone) {
		if (m_iDeferredStatus.isClosed () ||
		    (m_iDeferredStatus.isFailed () && EAGAIN != m_iDeferredStatus.Code ())) {
		    perror ("Receiving Agent (Head)");
		    return false;
		}
	    }
	    if (m_pIncomingCursor >= m_pIncomingLimit)
		m_xIncomingState = State_HeadArrived;
	    break;

	case State_HeadArrived:
	    m_iIncomingMessage.Convert ();
	    if (!m_pIncomingTargets->GetTargetMessageController (
		    m_iIncomingMessage.Target (), &m_iIncomingMessage.m_pController
		)
	    ) return false;

	    m_iIncomingMessage.PrepareIncomingMessage (this);

	    if (0 == m_iIncomingMessage.Size ())
		m_xIncomingState = State_BodyArrived;
	    else if (m_iIncomingMessage.RealizeBody ()) {
		m_pIncomingCursor = (pointer_t)m_iIncomingMessage.Body ();
		m_pIncomingLimit  = m_pIncomingCursor + m_iIncomingMessage.Size ();

		m_xIncomingState = State_BodyArriving;
	    }
	    break;

	case State_BodyArriving:
	    notDone = ReadData ();
#ifdef TracingStartStop
	    if (!notDone) {
		fprintf (
		    stderr,
		    "+++ Receiving Agent Blocked: Body %u/%u\n",
		    (unsigned int)(m_pIncomingLimit - m_pIncomingCursor),
		    m_iIncomingMessage.Size ()
		);
		fflush (stderr);
	    }
#endif
	    if (!notDone) {
		if (m_iDeferredStatus.isClosed () ||
		    (m_iDeferredStatus.isFailed () && EAGAIN != m_iDeferredStatus.Code ())) {
		    perror ("Receiving Agent (Body)");
		    return false;
		}
	    }
	    if (m_pIncomingCursor >= m_pIncomingLimit)
		m_xIncomingState = State_BodyArrived;
	    break;

	case State_BodyArrived:
	    m_iIncomingMessage.ReceiveIncomingMessage (this);
	    m_iIncomingMessage.Destroy ();

	    m_xIncomingState = State_Ready;
	    break;

	default:
	    notDone = false;
	    break;
	}
    } while (notDone);
    return true;
}


void VkMessageShippingAgent::ProcessEvent () {
    bool notDone = true;

#ifdef TracingStartStop
    fprintf (stderr, "+++ Shipping Agent Started\n");
    fflush  (stderr);
#endif
    do {
	switch (m_xOutgoingState) {
	case State_Ready:
	    m_iOutgoingMessage.PrepareOutgoingMessage (this);
	    m_pOutgoingCursor = (pointer_t)&m_iOutgoingMessage.m_iHeader;
	    m_pOutgoingLimit  = m_pOutgoingCursor + sizeof (VkMessageHeader);

	    m_xOutgoingState = State_HeadDeparting;
	    break;

	case State_HeadDeparting:
	    notDone = WriteData ();
#ifdef TracingStartStop
	    if (!notDone) {
		fprintf (
		    stderr,
		    "+++ Shipping Agent Blocked: Head %u/%u\n",
		    (unsigned int)(m_pOutgoingLimit - m_pOutgoingCursor),
		    sizeof (VkMessageHeader)
		);
		fflush (stderr);
	    };
#endif
	    if (m_pOutgoingCursor >= m_pOutgoingLimit)
		m_xOutgoingState = State_HeadDeparted;
	    break;

	case State_HeadDeparted:
	    if (0 == m_iOutgoingMessage.Size ())
		m_xOutgoingState = State_BodyDeparted;
	    else {
		m_pOutgoingCursor = (pointer_t)m_iOutgoingMessage.Body ();
		m_pOutgoingLimit  = m_pOutgoingCursor + m_iOutgoingMessage.Size ();

		m_xOutgoingState = State_BodyDeparting;
	    }
	    break;

	case State_BodyDeparting:
	    notDone = WriteData ();
#ifdef TracingStartStop
	    if (!notDone) {
		fprintf (
		    stderr,
		    "+++ Shipping Agent Blocked: Body %u/%u\n",
		    (unsigned int)(m_pOutgoingLimit - m_pOutgoingCursor),
		    m_iOutgoingMessage.Size ()
		);
		fflush (stderr);
	    }
#endif
	    if (m_pOutgoingCursor >= m_pOutgoingLimit)
		m_xOutgoingState = State_BodyDeparted;
	    break;

	case State_BodyDeparted:
	    m_iOutgoingMessage.DiscardOutgoingMessage (this);
	    m_iOutgoingMessage.Destroy ();

	    if (IsNil (m_pOutgoingQueueHead))
		m_xOutgoingState = State_Idle;
	    else {
		QueueEntry *pQueueEntry	= m_pOutgoingQueueHead;
		m_iOutgoingMessage	= pQueueEntry->m_iOutgoingMessage;
		m_pOutgoingQueueHead	= pQueueEntry->m_pNext;
		delete pQueueEntry;

		if (IsNil (m_pOutgoingQueueHead))
		    m_pOutgoingQueueTail = NilOf (QueueEntry*);

		m_xOutgoingState = State_Ready;
	    }
	    break;

	default:
	    notDone = false;
	    break;
	}
    } while (notDone);
}


/**************************
 *****  Transmission  *****
 **************************/

bool VkMessageShippingAgent::Send (
    VkMessageController	       *pController,
    VkMessageTargetIndex	xSender,
    VkMessageTargetIndex	xTarget,
    unsigned short		xType,
    unsigned int		sBody,
    void const*			pBody
)
{
    switch (m_xOutgoingState) {
    case State_Closed:
	return false;

    case State_Idle:
	m_iOutgoingMessage.construct (
	    pController, xSender, xTarget, xType, sBody, pBody
	);
	m_xOutgoingState = State_Ready;
	break;

    default: {
	    QueueEntry *pQueueEntry = new QueueEntry (
		pController, xSender, xTarget, xType, sBody, pBody
	    );

	    if (m_pOutgoingQueueTail)
		m_pOutgoingQueueTail->m_pNext = pQueueEntry;
	    else
		m_pOutgoingQueueHead = pQueueEntry;
	    m_pOutgoingQueueTail = pQueueEntry;
	}
	break;
    }

    return true;
}


/**************************
 *****  Construction  *****
 **************************/

VkMessageReceivingAgent::VkMessageReceivingAgent (
    VkStream*			pIncomingStream,
    unsigned int		sIncomingChunk,
    VkStream*			pOutgoingStream,
    unsigned int		sOutgoingChunk,
    VkMessageTargetTable*	pIncomingTargets
) : VkStreamReceivingAgent	(pIncomingStream)
, m_sIncomingChunk		(sIncomingChunk)
, m_pShippingAgent		(
    new VkMessageShippingAgent (pOutgoingStream, sOutgoingChunk)
) , m_pIncomingTargets		(pIncomingTargets)
, m_xIncomingState		(State_Ready)
{
    m_iIncomingMessage.construct (
	NilOf (VkMessageController*),
	VkMessageTargetIndex_Nil,
	VkMessageTargetIndex_Nil,
	0,
	0,
	NilOf (void const*)
    );
}

VkMessageShippingAgent::VkMessageShippingAgent (
    VkStream *pOutgoingStream, unsigned int sOutgoingChunk
)
: VkStreamShippingAgent	(pOutgoingStream)
, m_sOutgoingChunk	(sOutgoingChunk)
, m_xOutgoingState	(State_Idle)
, m_pOutgoingQueueHead	(0)
, m_pOutgoingQueueTail	(0)
{
    m_iOutgoingMessage.construct (
	NilOf (VkMessageController*),
	VkMessageTargetIndex_Nil,
	VkMessageTargetIndex_Nil,
	0,
	0,
	NilOf (void const*)
    );
}
