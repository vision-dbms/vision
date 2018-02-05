/*****  Stream I/O Driver Implementation  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName IOMStreamFacility

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

#include "IOMStream.h"

/************************
 *****  Supporting  *****
 ************************/

#include "verr.h"

#include "IOMHandle.h"

#include "Vca_VBS.h"
#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"
#include "Vca_VDeviceFactory.h"
#include "Vca_VStatus.h"
#include "Vca_VTrigger.h"

#include "VkBuffer.h"

#include "VReceiver.h"

#include "V_VArgList.h"

/***********************
 *****  Constants  *****
 ***********************/

#define IOMStreamGetLineFailureLimit 100


/*******************************
 *******************************
 *****                     *****
 *****  IOMStreamProducer  *****
 *****                     *****
 *******************************
 *******************************/

/*******************************
 *----  IOMStreamProducer  ----*
 *******************************/

class IOMStreamProducer : public Vca::VRolePlayer {
    DECLARE_CONCRETE_RTT (IOMStreamProducer, Vca::VRolePlayer);

//  Aliases
public:
    typedef Vca::VBSProducer	BS;
    typedef Vca::IBSClient	IBSClient;
    typedef Vca::IClient	IClient;

//  Construction
public:
    IOMStreamProducer (IOMStream *pIOMStream, BS *pBS);

//  Destruction
protected:
    ~IOMStreamProducer () {
        traceInfo ("Destroying IOMStreamProducer");
    }

//  Base Roles
public:
    using BaseClass::getRole;

//  IBSClient Role
private:
    Vca::VRole<ThisClass,Vca::IBSClient> m_pIBSClient;
public:
    void getRole (VReference<IBSClient>&rpRole) {
	m_pIBSClient.getRole (rpRole);
    }

//  Callbacks
public:
    void OnTransfer (IBSClient *pRole, Vca::U32 sTransfer);
    virtual void OnEnd (IClient *pRole) OVERRIDE;
    virtual void OnError (IClient *pRole, Vca::IError *pError, VString const &rMessage) OVERRIDE;

//  Termination
public:
    void Close ();

//  Input
private:
    bool GetBufferedData (size_t sResultMinimum, VkStatus *pStatusReturn);
    bool GetBufferedData (
	size_t sResultMinimum, size_t sResultMaximum, VkStatus *pStatusReturn
    );

    bool StartRead (
	void *pData, size_t sResultMinimum, size_t sResultMaximum, VkStatus *pStatusReturn
    );
    bool Produce (size_t sTransfer);

public:
    size_t GetByteCount (VkStatus *pStatusReturn) const;

    size_t GetData (
	size_t sResultMinimum, size_t sResultMaximum, void  *pData, VkStatus *pStatusReturn
    );
    bool GetLine (char **ppResult, size_t *psResult, VkStatus *pStatusReturn);

    bool GetString (
	size_t	sResultMinimum,
	size_t	sResultMaximum,
	char**	ppResult,
	size_t*	psResult,
	VkStatus *pStatusReturn
    );

//  Query
protected:
    char const *ConsumerRegion () const {
	return m_iBuffer.ConsumerRegion ();
    }
    size_t ConsumerRegionSize () const {
	return m_iBuffer.ConsumerRegionSize ();
    }
    pointer_t ProducerRegion () const {
	return m_iBuffer.ProducerRegion ();
    }
    size_t ProducerRegionSize () const {
	return m_iBuffer.ProducerRegionSize ();
    }
public:
    bool isComplete (VkStatus *pStatusReturn) const {
	pStatusReturn->setTo (m_iStatus);
	return m_iStatus.isCompleted ();
    }
    bool isComplete () const {
	return m_iStatus.isCompleted ();
    }
    VkStatusType statusType () const {
	return m_iStatus.Type ();
    }

//  Update
protected:
    bool CompactAndGuarantee (size_t sSize) {
	m_iBuffer.CompactAndGuarantee (sSize);
	return true;
    }
public:
    void setCompleteStatus () {
	m_iStatus.MakeSuccessStatus ();
    }

//  State
protected:
    VReference<IOMStream> const	m_pIOMStream;
    VReference<BS>		m_pBS;
    VkBuffer			m_iBuffer;
    size_t			m_sMinTransferBalance;
    VkStatus		        m_iStatus;       
};


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (IOMStreamProducer);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

IOMStreamProducer::IOMStreamProducer (
    IOMStream *pIOMStream, BS *pBS
) : m_pIOMStream (pIOMStream), m_pBS (pBS), m_sMinTransferBalance (0), m_pIBSClient (this) {
    traceInfo ("Creating IOMStreamProducer");
}

/***************************************
 ***************************************
 *****  Interface Implementations  *****
 ***************************************
 ***************************************/

/******************************************************************************
 * Routine: OnTransfer
 * If the transferred size is less than the minimum that was required to be 
 * transferred, it implies that the area has been registered and is being processed
 * Hence on getting a transfer which is less than minimum we set the 
 * status as blocked to prevent further readings. Else a status is set as success
 * and Stream's triggerAll () method is invoked to schedule/resume the 
 * suspended ComputationUnits and process the arrived data.
 *****************************************************************************/

void IOMStreamProducer::OnTransfer (IBSClient *pRole, Vca::U32 sTransfer) {
    if (Produce (sTransfer))
	m_pIOMStream->triggerAll ();
}

void IOMStreamProducer::OnEnd (IClient *pRole) {
    traceInfo ("\nIOMStreamProducerClient End...");
    m_iStatus.MakeClosedStatus ();
    m_pIOMStream->triggerAll ();
}

void IOMStreamProducer::OnError (IClient *pRole, Vca::IError *pError, VString const &rMessage) {
//    display ("\n%s:OnError: %s\n", rttName ().content (), rMessage.content ());
    m_iStatus.MakeFailureStatus ();
    m_pIOMStream->triggerAll ();
}

/******************************************************************************
 * Routine: Close
 *	Producer's Close does a Close on the actual ByteStream producer. 
 * If the ByteStream is in the middle of some processing then it performs a
 * delayed Close on the ByteStream. In either ways, the Client delegates the 
 * Close operation to the ByteStream and clears ByteStream producer from the 
 * reference variable.
******************************************************************************/


void IOMStreamProducer::Close () {
    traceInfo ("\nClosing IOMStreamProducer...");
    m_iStatus.MakeClosedStatus ();
    m_pBS->Close ();
    m_pBS.clear ();
}

/********************
 *****  Access  *****
 ********************/

/******************************************************************************
 * Routine: GetByteCount
 *  Retrieves the number of bytes that is present in the local buffers and 
 * the OS buffers that can be read from this stream
 ******************************************************************************/


size_t IOMStreamProducer::GetByteCount (VkStatus *pStatusReturn) const {
    pStatusReturn->setTo (m_iStatus);
    return ConsumerRegionSize () + m_pBS->GetByteCount ();
}

/*******************
 *****  Input  *****
 *******************/

bool IOMStreamProducer::GetBufferedData (
    size_t sResultMinimum, VkStatus *pStatusReturn
) {
    return GetBufferedData (sResultMinimum, sResultMinimum, pStatusReturn);
}

/******************************************************************************
 * Routine: GetBufferedData
 * After ensuring that no Input operation is in progress and there is enough 
 * space in the buffer to allocate the maximum requirement, the StartRead method 
 * is invoked which basically does a VBS ByteStream get operation
 *****************************************************************************/

bool IOMStreamProducer::GetBufferedData (
    size_t sResultMinimum, size_t sResultMaximum, VkStatus *pStatusReturn
) {
    sResultMaximum = sResultMinimum > sResultMaximum ? sResultMinimum : sResultMaximum;
    return isComplete (pStatusReturn)
	&& CompactAndGuarantee (sResultMaximum)
	&& StartRead (ProducerRegion (), sResultMinimum, sResultMaximum, pStatusReturn);
}

/******************************************************************************
 * Routine: StartRead
 * Performs a get operation on the ByteStream with the minimum and maximum size
 *****************************************************************************/

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
    Calling the external version of OnTransfer from this internal routine without
    identifying the call as an internal one is incorrect.  OnTransfer is written
    as an external event handler and so calls triggerAll to fire the events that
    must be processed.  StartRead, on the other hand, is called from those events
    handlers, and so should not retrigger the events.
 *>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

bool IOMStreamProducer::StartRead (
    void *pData, size_t sResultMinimum, size_t sResultMaximum, VkStatus *pStatusReturn
) {
    m_sMinTransferBalance = sResultMinimum;

    VReference<IBSClient> pRole;
    getRole (pRole);
    bool bDone = Produce (m_pBS->get (pRole, pData, sResultMinimum, sResultMaximum));
    pStatusReturn->setTo (m_iStatus);
    return bDone;
}

bool IOMStreamProducer::Produce (size_t sTransfer) {
    m_iBuffer.Produce (sTransfer);

    if (m_sMinTransferBalance > sTransfer) {
	m_sMinTransferBalance -= sTransfer;
	if (m_iStatus.isntClosed () && m_iStatus.isntFailed ())
	    m_iStatus.MakeBlockedStatus ();
	return false;
    }

    m_sMinTransferBalance = 0;
    if (m_iStatus.isntClosed () && m_iStatus.isntFailed ())
	m_iStatus.MakeSuccessStatus ();

    return true;
}

/******************************************************************************
 * Routine: GetData
 *	(pResultBuffer) Memory is already allocated. If sResultMinimum data is
 * already available in buffer then uses it else tries to read again..
 * Copies the data from the internal buffer to the allocated memory located 
 * passed
 *****************************************************************************/

size_t IOMStreamProducer::GetData (
    size_t sResultMinimum, size_t sResultMaximum, void *pResultBuffer, VkStatus *pStatusReturn
) {

    pointer_t pResultLimit = (pointer_t)pResultBuffer;

    if (sResultMinimum > sResultMaximum)
	sResultMinimum = sResultMaximum;

    while (ConsumerRegionSize () < sResultMinimum 
	&& GetBufferedData (sResultMinimum - ConsumerRegionSize (), pStatusReturn) 
    );

    if (ConsumerRegionSize () >= sResultMinimum) {
        pStatusReturn->setTo (m_iStatus);
	do {
	    size_t cbToCopy = ConsumerRegionSize ();
	    if (cbToCopy > sResultMaximum)
		cbToCopy = sResultMaximum;
	
	    m_iBuffer.Consume (pResultLimit, cbToCopy);
	    pResultLimit    += cbToCopy;
	    sResultMaximum  -= cbToCopy;
	} while (sResultMaximum > 0 && GetBufferedData (1, sResultMaximum, pStatusReturn));
    }
    return pResultLimit - (pointer_t)pResultBuffer;
}

/******************************************************************************
 * Routine: GetLine
 *	Enters a while loop performing 2 operations
 * 1. Checking whether a newline character exists in the Buffer
 * 2. If not tries to read as much as data available.
 * 
 * If successful in getting a line of data into the internal buffer
 * then calls GetString which then allocates memory to copy the data from the
 * internal buffer to be sent to the requestor..
 *****************************************************************************/

bool IOMStreamProducer::GetLine (
    char **ppResult, size_t *psResult, VkStatus *pStatusReturn
) {
    pointer_t    pNewLine;
    size_t     sResult; 

    *ppResult = NilOf (char *);
    *psResult = 0;
    
    while (
	IsNil (pNewLine = (char *) memchr (ConsumerRegion (), '\n', ConsumerRegionSize ())) 
	  && GetBufferedData (1, ProducerRegionSize (), pStatusReturn)
    );

    if (IsntNil (pNewLine))
	sResult = pNewLine - ConsumerRegion () + 1;
    else if (ConsumerRegionSize () > 0 && isComplete (pStatusReturn))
	sResult = ConsumerRegionSize ();
    else return false;

    return GetString (sResult, sResult, ppResult, psResult, pStatusReturn);
}

/******************************************************************************
 * Routine: GetString
 *	Tries to read atleast sResultMinimum of data into the internal buffer
 * using GetBufferedData method.... If unsuccessful returns false.
 * If successful in reading minimum data, then allocates memory and does a 
 * GetData to transfer the data from the internal buffer to the allocated 
 * space. Also appends a \0 at the end for usage by string methods.
 *****************************************************************************/

bool IOMStreamProducer::GetString (
    size_t	sResultMinimum,
    size_t	sResultMaximum,
    char**	ppResult,
    size_t*	psResult,
    VkStatus   *pStatusReturn
) {
   *ppResult = NilOf (char *);
   *psResult = 0;

   if (sResultMinimum > sResultMaximum) 
       while (sResultMinimum > (sResultMaximum = ConsumerRegionSize ())
	   && GetBufferedData (sResultMinimum - sResultMaximum, ProducerRegionSize (), pStatusReturn)
       );

    if (sResultMinimum > sResultMaximum)
	return false;

    *ppResult = (char *)allocate (sResultMaximum + 1);
    *psResult = GetData (sResultMinimum, sResultMaximum, *ppResult, pStatusReturn);

    if (0 == *psResult && *psResult < sResultMinimum) {
	deallocate (*ppResult);
	*ppResult = NilOf (char*);
	return false;
    }

    (*ppResult)[*psResult] = '\0';
    return true;
}


/*******************************
 *******************************
 *****                     *****
 *****  IOMStreamConsumer  *****
 *****                     *****
 *******************************
 *******************************/

/******************************************************************************
 * Notes:
 *  IOMStreamConsumer has two buffers that is used in tandem for sending
 *  data out.The two buffers are called Active and Transit Buffer
 *  Data is initally filled up in the Active Buffer and when it needs to be 
 *  written out, it is referenced as the Transit Buffer while the other buffer is 
 *  referenced as the Active Buffer. 
 *  The Transit Buffer's memory area and size is given to the VBSConsumer to 
 *  write. In the meanwhile the data is accepted into the Active buffer. 
 *  The buffers are swapped when appropriate and data is sent out to VBSConsumer
 *  Since all the writes are done in nonblocking mode, 
 *  this kind of approach (2 buffers) is used to allow data to be accepted 
 *  internally even if previous write hasnt yet completed.
 *******************************************************************************/

/*******************************
 *----  IOMStreamConsumer  ----*
 *******************************/

class IOMStreamConsumer : public Vca::VRolePlayer {
    DECLARE_CONCRETE_RTT (IOMStreamConsumer, Vca::VRolePlayer);

//  Aliases
public:
    typedef Vca::VBSConsumer	BS;
    typedef Vca::IBSClient	IBSClient;
    typedef Vca::IClient	IClient;

//  Constants
protected:
    enum {
	BufferGrain = VkBuffer::BufferGrain,
	BufferThreshold = BufferGrain * 3
    };

//  State
public:
    enum State {
	State_Open, State_Closing, State_Closed
    };

//  Construction
public:
    IOMStreamConsumer (BS *pBS);

//  Destruction
protected:
    ~IOMStreamConsumer ();

//  Base Roles
public:
    using BaseClass::getRole;

//  IBSClient Role
private:
    Vca::VRole<ThisClass,Vca::IBSClient> m_pIBSClient;
public:
    void getRole (VReference<IBSClient>&rpRole) {
	m_pIBSClient.getRole (rpRole);
    }

//  Callbacks
public:
    void OnTransfer (IBSClient *pRole, Vca::U32 sTransfer);
    virtual void OnEnd (IClient *pRole) OVERRIDE;
    virtual void OnError (IClient *pRole, Vca::IError *pError, VString const &rMessage) OVERRIDE;

//  State Update
protected:
    void setStateToClosing () {
	m_xState = State_Closing;
    }
    void setStateToClosed () {
	m_xState = State_Closed;
    }

//  State Query
public:
    bool isOpen () const {
	return m_xState == State_Open;
    }
    bool isntOpen () const {
	return m_xState != State_Open;
    }
    bool isClosing () const {
	return m_xState == State_Closing;
    }
    bool isClosed () const {
	return m_xState == State_Closed;
    }

//  Termination
public:
    void Close ();

//  Output
public:
    bool   PutBufferedData ();
    size_t PutData   (size_t sData, void const* pData);
    size_t PutString (char const *pString) {
	return PutData (strlen (pString), pString);
    }
    size_t PutFormattedString (size_t sData, char const *pFormat,va_list pArgList);
    size_t Print (size_t sData, char const *pFormat, ...); 

//  Query
protected:
    bool activeBufferIsEmpty () const {
	return m_pActiveBuffer->ConsumerRegionSize () == 0;
    }
    bool activeBufferIsntEmpty () const {
	return m_pActiveBuffer->ConsumerRegionSize () > 0;
    }

    bool transitBufferIsEmpty () const {
	return transitBuffer ()->ConsumerRegionSize () == 0;
    }
    bool transitBufferIsntEmpty () const {
	return transitBuffer ()->ConsumerRegionSize () > 0;
    }

    VkBuffer const *transitBuffer () const {
	return m_pActiveBuffer == &m_iBuffer [0]
	    ? &m_iBuffer [1] : &m_iBuffer [0];
    }
    VkBuffer *transitBuffer () {
	return m_pActiveBuffer == &m_iBuffer [0]
	    ? &m_iBuffer [1] : &m_iBuffer [0];
    }

//  CheckPoint
public:
    void createCheckPoint (bool bBlocking, Vca::ITrigger *pTrigger);
    void releaseBlockingCheckPoint ();

//  State
protected:
    VReference<BS>		m_pBS;
    State			m_xState;
    VkBuffer			m_iBuffer [2];
    VkBuffer*			m_pActiveBuffer;
};


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (IOMStreamConsumer);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

IOMStreamConsumer::IOMStreamConsumer (BS *pBS)
:   m_pBS			(pBS)
,   m_pActiveBuffer		(&m_iBuffer[0])
,   m_xState			(State_Open)
,   m_pIBSClient		(this)
{
    traceInfo ("Creating IOMStreamConsumer");
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

IOMStreamConsumer::~IOMStreamConsumer () {
    traceInfo ("Destroying IOMStreamConsumer");

    if (m_pBS)
	m_pBS->Close ();
}


/***************************************
 ***************************************
 *****  Interface Implementations  *****
 ***************************************
 ***************************************/

/******************************************************************************
 * Routine: OnTransfer
 *	Invoked upon transferring sTransfer bytes of data from the 
 * Transit buffer to the Byte Stream side. If the entire Transit buffer  
 * has been transferred, forces the buffered data on the byte stream side to 
 * be flushed to ensure that the data is actually out and not just stored in 
 * stream's internal buffers.
 ******************************************************************************/

void IOMStreamConsumer::OnTransfer (IBSClient *pRole, Vca::U32 sTransfer) {
    transitBuffer ()->Consume (sTransfer);
    PutBufferedData ();
}

void IOMStreamConsumer::OnEnd (IClient *pRole) {
    traceInfo ("\nIOMStreamConsumerClient End...");
    Close ();
}

void IOMStreamConsumer::OnError (IClient *pRole, Vca::IError *pError, VString const &rMessage) {
//    display ("\n%s:OnError: %s\n", rttName ().content (), rMessage.content ());
    Close ();
}


/********************
 ********************
 *****  Output  *****
 ********************
 ********************/

/******************************************************************************
 * Method: PutBufferedData
 * This method tries to flush the ActiveBuffer. This is possible only if the
 * Transit Buffer is completely flushed.....
 * If the Active Buffer is already empty just returns ...no action....
 *
 * If the earlier transfer has completed then swap the active buffer
 * and the transit buffer and do a put on byte stream consumer for sending 
 * the data in the transit buffer.
 *
 * Returns true if all data has been flushed, false otherwise.
******************************************************************************/

bool IOMStreamConsumer::PutBufferedData () {
//  If a transfer is currently in progress, nothing more can be done at the moment, ...
    if (transitBufferIsntEmpty ())
	return false;

//  If a transfer isn't in progress and the active buffer has content, ...
    if (activeBufferIsntEmpty ()) {
    //  ... switch buffers, ...
	VkBuffer *pTransitBuffer = m_pActiveBuffer;
	m_pActiveBuffer = transitBuffer ();

    //  ... start a transfer, ...
	VReference<IBSClient> pRole;
    	getRole (pRole);
	pTransitBuffer->Consume (
	    m_pBS->put (
		pRole, pTransitBuffer->ConsumerRegion (), pTransitBuffer->ConsumerRegionSize ()
	    )
	);

    //  ... and report the result:
	return pTransitBuffer->ConsumerRegionSize () == 0;
    }

//  If both buffers are empty, there is no buffered data !!!
    return true;
}


/******************************************************************************
 * Method: PutData
 *	If the data to be written is bigger than active buffers producer region
 * size then grows the active buffer accordingly and copies the data into it. 
 * To allow the data to be periodically flushed from this clients buffer to 
 * the byte stream we have a buffer size check made after copying the data...
 * If the condition is satisfied then a PutBufferedData is initiated to 
 * transfer the buffered data
******************************************************************************/

size_t IOMStreamConsumer::PutData (size_t sData, void const* pData) {
    if (isntOpen ())
	return 0;

    if (sData > m_pActiveBuffer->ProducerRegionSize ()) 
        m_pActiveBuffer->CompactAndGuarantee (sData);

    m_pActiveBuffer->Produce (pData, sData);

/////////////////////////////////////////////////////////////////////////////////
//  Introduced 'sProducerRegion' and 'sConsumerRegion' local variables to fix 
//  apparent NT compiler bug which causes 'ProducerRegionSize () < BufferGrain' and
//  'ConsumerRegionSize () > BufferThreshold' to always return true, 
//  thereby causing overly aggressive buffer flushing.
/////////////////////////////////////////////////////////////////////////////////

    size_t sProducerRegion = m_pActiveBuffer->ProducerRegionSize ();
    size_t sConsumerRegion = m_pActiveBuffer->ConsumerRegionSize ();

    //  buffer size check
    if (sConsumerRegion > BufferThreshold && sProducerRegion < BufferGrain)	
 	PutBufferedData ();

    return sData;    
}

size_t IOMStreamConsumer::PutFormattedString (
    size_t sData, char const *pFormat, va_list pArgList
) {
    if (isntOpen ())
	return 0;

    size_t sResult;
#if defined(_WIN32)
    while (
	(int)(
	    sResult = _vsnprintf (
		m_pActiveBuffer->ProducerRegion (), m_pActiveBuffer->ProducerRegionSize (), pFormat, pArgList
	    )
	) < 0
    )
    {

	if (sData < m_pActiveBuffer->ProducerRegionSize () + 1)
	    sData = m_pActiveBuffer->ProducerRegionSize () + 1;

	m_pActiveBuffer->CompactAndGuarantee (sData);
	sData += BufferGrain;
    }
    m_pActiveBuffer->Produce (sResult);

/////////////////////////////////////////////////////////////////////////////////
//  Introduced 'sProducerRegion' and 'sConsumerRegion' local variables to fix 
//  apparent NT compiler bug which causes 'ProducerRegionSize () < BufferGrain' and
//  'ConsumerRegionSize () > BufferThreshold' to always return true, 
//  thereby causing overly aggressive buffer flushing.
/////////////////////////////////////////////////////////////////////////////////

    size_t sProducerRegion = m_pActiveBuffer->ProducerRegionSize ();
    size_t sConsumerRegion = m_pActiveBuffer->ConsumerRegionSize ();

    if (sConsumerRegion > BufferThreshold && sProducerRegion < BufferGrain)
	PutBufferedData ();
#else
    size_t sGuess;
    if (sData > 0)
	sGuess = sData;
    else {
	FILE *devnull = fopen ("/dev/null", "a");
	if (NULL != devnull) {
	    V::VArgList pArgListCopy (pArgList);
	    sGuess = STD_vfprintf (devnull, pFormat, pArgListCopy);
	    fclose (devnull);
	}
	else
	    sGuess = 2048;
    }
    sGuess++;// some callers forget to account  for the null character

    if (sGuess > m_pActiveBuffer->ProducerRegionSize ()) {
	m_pActiveBuffer->CompactAndGuarantee (sGuess);
    }

    sResult = STD_vsprintf (m_pActiveBuffer->ProducerRegion (), pFormat, pArgList);
    m_pActiveBuffer->Produce (sResult);

    if (m_pActiveBuffer->ConsumerRegionSize () > BufferThreshold && m_pActiveBuffer->ProducerRegionSize () < BufferGrain) {
	PutBufferedData ();
    }
#endif

    return sResult;
}

size_t IOMStreamConsumer::Print (size_t sData, char const *pFormat, ...) {
    if (isntOpen ())
	return 0;

    V_VARGLIST (pArgList, pFormat);
    return PutFormattedString (sData, pFormat, pArgList);
}


/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

/******************************************************************************
 * Routine: Close
 *	If already in the closing state returns else invokes the close method
 * on the ByteStream Consumer which flushes the data and sends an EndTransmission
 * signal
******************************************************************************/

void IOMStreamConsumer::Close () {
    if (isntOpen ())
	return;

    setStateToClosing ();
    if (m_pBS) {
	m_pBS->Close ();
//  deleted 9/1/04 by mjc - causing in progress transfers to fail.
//	m_pBS.clear ();
    }
}

/******************************************************************************
 * Method: createCheckPoint
 *	Consumers allow for data to be written with CheckPointing
 * mechanism. i.e. CheckPoints could be inserted allowing for 
 * data sequencing..This method creates a checkpoint in the bytestream 
 * associated with this consumer client....
 *	Determines the size of data currently present in its internal buffer
 * and the ByteStream Consumer buffer. Creates a CheckPoint object and adds it
 * to the checkpoint list maintained by the ByteStream consumer.
 ******************************************************************************/

void IOMStreamConsumer::createCheckPoint (bool bBlocking, Vca::ITrigger *pTrigger) {
    if (m_pBS) {
	VReference<Vca::VCheckPoint> pCheckPoint (
	    new Vca::VCheckPoint (
		m_pActiveBuffer->ConsumerRegionSize () + m_pBS->transferPending (), bBlocking, pTrigger
	    )
	);
	m_pBS->setCheckPoint (pCheckPoint);
    }
}

void IOMStreamConsumer::releaseBlockingCheckPoint () {
    if (m_pBS)
	m_pBS->releaseBlockingCheckPoint ();
}


/************************************
 ************************************
 *****                          *****
 *****  IOMStream::WindowFrame  *****
 *****                          *****
 ************************************
 ************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

IOMStream::WindowFrame::WindowFrame (WindowFrame *pPredecessor, VReference<Producer> &rpProducer)
    : m_pPredecessor (pPredecessor), m_pProducer (rpProducer)
{
    rpProducer.clear ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

IOMStream::WindowFrame::~WindowFrame () {
}

IOMStream::WindowFrame *IOMStream::WindowFrame::Pop (VReference<Producer> &rpProducer) {
    WindowFrame *const pPriorFrame = m_pPredecessor;
    rpProducer.setTo (m_pProducer);
    delete this;

    return pPriorFrame;
}


/***********************
 ***********************
 *****             *****
 *****  IOMStream  *****
 *****             *****
 ***********************
 ***********************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (IOMStream);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

IOMStream::IOMStream (
    Options const& rOptions, VBSProducer *pBSProducer, VBSConsumer *pBSConsumer
) : IOMDriver		(rOptions)
,   m_pInputStack	(0)
,   m_cGetLineFailures	(0)
,   m_bOutputGenerated	(false)
,   m_bOutputRedirected	(false)
,   m_bPromptNeeded	(true)
,   m_pOutputFile	(NilOf (FILE *))
,   m_pStartupExpression(0)
{
    traceInfo ("Creating IOMStream");
    enableCheckpoints ();
    retain (); {
	plumb (pBSProducer);
	plumb (pBSConsumer);
    } untain ();
}


/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

IOMStream::~IOMStream () {
    traceInfo ("Destroying IOMStream");
    Close ();
}


/**********************
 **********************
 *****  Plumbing  *****
 **********************
 **********************/

void IOMStream::onConnect (VBSProducerSink *pReceiver, VBSProducer *pBS) {
    plumb (pBS);
}

void IOMStream::onConnect (VBSConsumerSink *pReceiver, VBSConsumer *pBS) {
    plumb (pBS);
}

void IOMStream::onError (
    VBSProducerSink *pReceiver, Vca::IError *pError, VString const &rMessage
) {
    PopInput ();
}

void IOMStream::onError (
    VBSConsumerSink *pReceiver, Vca::IError *pError, VString const &rMessage
) {
}

void IOMStream::plumb (VBSProducer *pBS) {
    if (m_pProducer.isNil () && pBS)	// This is correct - Push now clears the producer!!!
	m_pProducer.setTo (new Producer (this, pBS));
}

void IOMStream::plumb (VBSConsumer *pBS) {
    if (m_pConsumer.isNil () && pBS)
	m_pConsumer.setTo (new Consumer (pBS));
}


/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

IOMChannelType IOMStream::channelType () const {
    return IOMChannelType_Stream;
}

char const* IOMStream::consumedStartupExpression () {
    char const* pStartupExpression = m_pStartupExpression;
    m_pStartupExpression = 0;
    return pStartupExpression ? pStartupExpression : "";
}

int IOMStream::getTcpNodelay (int* fOnOff) {
    *fOnOff = false;
    m_iStatus.MakeFailureStatus ();
    return -1;
}


/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

/*************************
 *****  Input Stack  *****
 *************************/

void IOMStream::PushInput () {
    m_pInputStack = new WindowFrame (m_pInputStack, m_pProducer);
}

void IOMStream::PopInput () {
    if (m_pInputStack)
        m_pInputStack = m_pInputStack->Pop (m_pProducer);
    else {
        m_pProducer.clear ();
    }
}

void IOMStream::PopAllInput () {
    while (m_pInputStack) {
	if (m_pProducer)
	    m_pProducer->Close ();
        PopInput ();
    }
}


/******************
 *****  Seek  *****
 ******************/

int IOMStream::Seek (IOMHandle const* pHandle) {
    if (0 == pHandle->seekOffset ())
	return 0;

    m_iStatus.MakeFailureStatus ();
    return -1;
}


/**********************
 *****  Shutdown  *****
 **********************/

void IOMStream::Close () {
//  Do nothing if the channel is already closed, ...
    if (isClosing ())
	return;

//  ... otherwise, flush buffered output if we can, ...
    PutBufferedData ();

//  ... mark the channel as closing, ...
    SetStateToClosing ();

/*****  ... pop all input windows, ...  *****/
    PopAllInput ();


/*****  ... close the primary input window ...  *****/
    if (m_pProducer.isntNil ()) {
	m_pProducer->Close ();
	m_pProducer.clear ();
    }

/*****  ... close the output files and window ...  *****/
    CloseOutputFile ();

    if (m_pConsumer.isntNil ()) {
	m_pConsumer->Close ();
	m_pConsumer.clear ();
    }
}


int IOMStream::EndTransmission () {
    if (isClosing ()) {
	m_iStatus.MakeClosedStatus ();
	return -1;
    }

    if (m_pConsumer.isntNil ()) {
	m_pConsumer->Close ();
	m_pConsumer.clear ();
    }

    return 0;
}

int IOMStream::EndReception () {
    if (isClosing ()) {
	m_iStatus.MakeClosedStatus ();
	return -1;
    }

    PopAllInput ();

    if (m_pProducer.isntNil ()) {
	m_pProducer->Close ();
	m_pProducer.clear ();
    }

    return 0;
}


/*****************
 *****  TCP  *****
 *****************/

int IOMStream::SetTcpNodelay (int fOnOff) {
    m_iStatus.MakeFailureStatus ();
    return -1;
}


/***********************************
 ***********************************
 *****  Driver Input Handlers  *****
 ***********************************
 ***********************************/

PrivateFnDef char *InputFileName (char *inputLine) {
    static char const *const whiteSpace = " \t\n\014\015";

    size_t dropCount = '\001' == *inputLine
    ? 1 : 0 == strncmp (inputLine, "??", 2)
    ? 2 : 0;

    if (dropCount < 1)
	return NilOf (char *);

    inputLine += dropCount;
    inputLine += strspn (inputLine, whiteSpace);
    inputLine  [strcspn (inputLine, whiteSpace)] = '\0';

    return inputLine;
}

VkStatusType IOMStream::InputOperationStatus () const {
    return m_pProducer ? m_pProducer->statusType () : VkStatusType_Failed;
}


/*---------------------------------------------------------------------------
 *****  'GetByteCount' Handler.
 *
 *  Arguments:
 *	pHandle			- the handle used to access this driver.
 *				  Contains seek information.  Ignored by this
 *				  driver.
 *
 *  Returns:
 *	The number of bytes pending in the input buffer.
 *
 *****/
size_t IOMStream::GetByteCount (IOMHandle const* pHandle) {
    if (Seek (pHandle) < 0)
     	return 0;

    if (m_pProducer.isNil ()) {
	m_iStatus.MakeFailureStatus ();
	return 0;
    }

    return m_pProducer->GetByteCount (&m_iStatus);
}


/*---------------------------------------------------------------------------
 *****  'GetData' Handler.
 *
 *  Arguments:
 *	pHandle			- the handle used to access this driver.
 *				  Contains seek information.  Ignored by this
 *				  driver.
 *	pBuffer			- the address of the buffer to fill.
 *	cbMin			- the minimum number of bytes that must be
 *				  read.  No data is transferred and this
 *				  handler returns zero if at least 'cbMin'
 *				  bytes of data cannot be read.
 *	cbMax			- the maximum number of bytes that can be
 *				  read.
 *
 *  Returns:
 *	The actual number of bytes returned in '*pBuffer'.
 *
 *****/

size_t IOMStream::GetData (
    IOMHandle const* pHandle, void *pBuffer, size_t cbMin, size_t cbMax
) {
    if (Seek (pHandle) < 0)
	return 0;

    if (m_pProducer.isNil ()) {
	m_iStatus.MakeFailureStatus ();
	return 0;
    }
    return m_pProducer->GetData (cbMin, cbMax, pBuffer, &m_iStatus);
}


/*---------------------------------------------------------------------------
 *****  'GetString' Handler.
 *
 *  Arguments:
 *	pHandle			- the handle used to access this driver.
 *				  Contains seek information.  Ignored by this
 *				  driver.
 *	cbMin			- the minimum number of bytes that must be
 *				  read.  No data is transferred and this
 *				  handler returns a failure code if at least
 *				  'cbMin' bytes of data cannot be read.
 *	cbMax			- the maximum number of bytes that can be
 *				  read.  If 'cbMax' is less than 'cbMin', all
 *				  currently available data will be read and
 *				  and returned.
 *	ppString		- the address of a 'char *' that will be set to
 *				  point a newly allocated string containing the
 *				  data read by this call.  The string will be
 *				  NULL terminated and must be freed by calling
 *				  'deallocate' on its address when it is no
 *				  longer needed.  '*ppString' is set to NULL if
 *				  no data was read.
 *	psString		- the length of the string returned in
 *				  '**ppString', exclusive of the terminating
 *				  NULL.
 *
 *  Returns:
 *	The driver status at the end of the call.
 *
 *****/
VkStatusType IOMStream::GetString (
    IOMHandle const*		pHandle,
    size_t			cbMin,
    size_t			cbMax,
    char **			ppString,
    size_t *			psString
)
{
    *ppString = NilOf (char *);
    *psString = 0;

    if (Seek (pHandle) < 0)
	return VkStatusType_Failed;

    if (m_pProducer.isNil ()) {
	m_iStatus.MakeFailureStatus ();
	return m_iStatus.Type ();
    }
    
    m_pProducer->GetString (cbMin, cbMax, ppString, psString, &m_iStatus);
    return m_iStatus.Type ();
}


/*---------------------------------------------------------------------------
 *****  'GetLine' Handler.
 *
 *  Arguments:
 *	pHandle			- the handle used to access this driver.
 *				  Ignored by this driver.
 *	zPrompt			- the address of a NULL terminated string used
 *				  as an input prompt.  May be NULL if no prompt
 *				  is desired.
 *	ppLine			- the address of a 'char *' that will be set to
 *				  point a newly allocated string containing the
 *				  line read by this call.  The string will be
 *				  NULL terminated and must be freed by calling
 *				  'deallocate' on its address when it is no
 *				  longer needed.  '*ppLine' is set to NULL if
 *				  no data was read.
 *	psLine			- the length of the string returned in
 *				  '**ppLine', exclusive of the terminating
 *				  NULL.
 *
 *  Returns:
 *	The driver status at the end of the call.
 *
 *  Notes:
 *	This routine takes responsibility for generating the handshake prompts
 *	needed by the 'curses' based editor, 'vlink', and miscellaneous other
 *	'rexec'd applications.  The following constraints apply:
 *
 *	     1) the prompt must be at the beginning of the line for the
 *		editor to notice it.  Therefore, we are putting a \n
 *		front of the prompt.  Sometimes this \n is the newline for the
 *		previous line and sometimes the previous line had a newline so
 *		this \n shows up as an extra blank line.
 *	     2) the extra \n at the end of the prompt is for the editor.  The
 *		editor uses 'gets' which does not recognize a line as being done
 *		until it sees a \n.  If this extra \n at the end is not sent,
 *		the editor hangs forever waiting for the line to be finished.
 *
 *****/
VkStatusType IOMStream::GetLine (
    IOMHandle const *pHandle, char const *zPrompt, char **ppLine, size_t *psLine
) {
/*****  Initialize the return arguments, ...  *****/
    *ppLine = NilOf (char *);
    *psLine = 0;

/*****  ... fail if an input window is not available, ...  *****/
    if (m_pProducer.isNil ()) {
	m_iStatus.MakeFailureStatus ();
	return m_iStatus.Type ();
    }

/*****  ... turn off output redirection, ...  *****/
    if (m_bOutputRedirected) {
	CloseOutputFile ();
	m_bOutputRedirected = false;
    }

/*****  ...  verify the seek offset, ...  *****/
    if (Seek (pHandle) < 0)
	return VkStatusType_Failed;

/*****  ... and look for a line:  *****/
    char *pLine; size_t sLine;
    while (m_pProducer.isntNil ()) {
    	bool bInputClosed = false;
	bool bRescanning = false;
	do {
    /***  ... emit a prompt if talking to the top-level connection, ...  ***/
	    if (m_bPromptNeeded && !m_pInputStack && m_pConsumer.isntNil () && zPrompt) {
		m_bPromptNeeded = false;
		if (m_bOutputGenerated) {
		    m_pConsumer->PutString ("\n");
		    m_bOutputGenerated = false;
		}
		switch (promptFormat ()) {
		case IOMPromptFormat_Editor:
		    m_pConsumer->Print (
			strlen (zPrompt) + 2, "%s\005\n", zPrompt
		    );
		    break;
		default:
		    m_pConsumer->PutString (zPrompt);
		    break;
		}

		m_pConsumer->PutBufferedData ();
		m_pProducer->setCompleteStatus ();
	    }

    /***  ... read the next line of input, ...  ***/ 
	    bRescanning = false;
	    m_pProducer->GetLine (&pLine, &sLine, &m_iStatus);

	    switch (m_iStatus.Type ()) {
	    case VkStatusType_Completed:
		break;
	    case VkStatusType_Blocked:
		m_cGetLineFailures = 0;
		return VkStatusType_Blocked;
	    case VkStatusType_Failed:
		if (m_fTracingEnabled) {
		    VString iMessage;
		    m_iStatus.getDescription (iMessage);
		    Report (
			0, "+++  IOMStream::GetLine: %s\n", iMessage.content ()
		    );
		}
		if (m_cGetLineFailures++ < IOMStreamGetLineFailureLimit)
		    return VkStatusType_Failed;
	    case VkStatusType_Closed:
		if (m_pInputStack) {
                    PopInput ();
		    bRescanning = true;
		}
		else {
		    bInputClosed = true;
		}
		break;
	    }
	    m_cGetLineFailures = 0;
	} while (bRescanning);

    /***  ... got something, adjust the driver and remember the line, ...  ***/
	m_bPromptNeeded = true;

    /***  ... check for end of input, ...  ***/
	if (bInputClosed || sLine >= 4 && 0 == strncmp (pLine, "QUIT", 4)) {
	    if (m_fTracingEnabled)
		Report (0, "+++  IOMStream::GetLine: Closed\n");
	    deallocate (pLine);

	    m_iStatus.MakeClosedStatus ();
	    return VkStatusType_Closed;
	}

    /***  ... check for normal input, ...  ***/
	char const *pInputFileName = 0;
	if (IsNil (zPrompt) || IsNil (pInputFileName = InputFileName (pLine))) {
	    if (m_fTracingEnabled)
		Report (0, "+++  IOMStream::GetLine: Got <%s>\n", pLine);

	    *ppLine = pLine;
	    *psLine = sLine;

	    return VkStatusType_Completed;
	}

    /***  ... otherwise, push a new redirection, ...  ***/
	VkStatus iStatus; VReference<VBSProducer> pBS;
	if (!Vca::VDeviceFactory::Supply (iStatus, pBS, pInputFileName)) Report (
	    0, ">>> Error opening file %s: %s\n", pInputFileName, iStatus.CodeDescription ()
	);
	else {
	    m_iStatus.MakeBlockedStatus ();
            PushInput ();
	    plumb (pBS);
	}

    /***  ... free the line buffer and do it again... ***/
	deallocate (pLine);
    }	/*** while true ***/
    return m_iStatus.Type ();
}


/********************
 ********************
 *****  Output  *****
 ********************
 ********************/

/*************************
 *****  Redirection  *****
 *************************/

void IOMStream::OpenOutputFile (char const *filename, bool fOutputRedirected) {
/*****  If a file is already open - close it ... *****/
    CloseOutputFile ();

/*****  Open the new file ... *****/
    if (IsNil (m_pOutputFile = fopen (filename, "w"))) Report (
	0, ">>> Echo To File: Error opening file '%s'.\n", filename
    );

    m_bOutputRedirected = fOutputRedirected;
}

void IOMStream::CloseOutputFile () {
    if (IsntNil (m_pOutputFile)) {
	fclose (m_pOutputFile);
	m_pOutputFile = NilOf (FILE *);
    }
}

/*******************
 *****  Flush  *****
 *******************/

bool IOMStream::PutBufferedData () {
    return m_pConsumer.isntNil () && m_pConsumer->PutBufferedData (); 
}

/*****************
 *****  Raw  *****
 *****************/

size_t IOMStream::PutData (IOMHandle const* pHandle, void const* pBuffer, size_t sBuffer) {
    if (Seek (pHandle) < 0)
	return 0;

    if (IsntNil (m_pOutputFile))
	write (fileno (m_pOutputFile), pBuffer, sBuffer);

    if (m_bOutputRedirected || m_pConsumer.isNil ())
	return 0;

    m_bOutputGenerated = true;

    return m_pConsumer->PutData (sBuffer, pBuffer);

}


/***********************
 *****  Character  *****
 ***********************/

size_t IOMStream::PutString (char const *pString) {
    if (IsntNil (m_pOutputFile)) {
	fputs (pString, m_pOutputFile);
	fflush (m_pOutputFile);
    }

    if (m_bOutputRedirected || m_pConsumer.isNil ())
	return 0;

    m_bOutputGenerated = true;

    return m_pConsumer->PutString (pString);
}

size_t IOMStream::VPrint (size_t sData, char const *fmt, va_list pArgList) {
    if (IsntNil (m_pOutputFile)) {
	V::VArgList pArgListCopy (pArgList);

	STD_vfprintf (m_pOutputFile, fmt, pArgListCopy);
	fflush (m_pOutputFile);
    }

    if (m_bOutputRedirected || m_pConsumer.isNil ())
	return 0;

    m_bOutputGenerated = true;

    return m_pConsumer->PutFormattedString (sData, fmt, pArgList);
}

size_t IOMStream::VReport (size_t sData, char const *fmt, va_list pArgList) {

    if (m_pErrorOutputDriver &&	this != m_pErrorOutputDriver.operator->() && m_pOutputFile) {
	V::VArgList pArgListCopy (pArgList);

	STD_vfprintf (m_pOutputFile, fmt, pArgListCopy);
	fflush (m_pOutputFile);
    }
    return IOMDriver::VReport (sData, fmt, pArgList);
}


/***************************
 *****  CheckPointing  *****
 ***************************/

/******************************************************************************
 * createCheckPoint and releaseBlockingCheckPoint methods just delegate the work
 * to the Consumer Client associated with this stream.
******************************************************************************/

void IOMStream::createCheckPoint (bool bBlocking, Vca::ITrigger *pTrigger) {
    if (m_pConsumer)
	m_pConsumer->createCheckPoint (bBlocking, pTrigger);
}

void IOMStream::releaseBlockingCheckPoint (Vca::VTrigger<IOMDriver>* pTrigger) {
    if (m_pConsumer)
	m_pConsumer->releaseBlockingCheckPoint ();
}

/*********************************
 *********************************
 *****                       *****
 *****  Facility Definition  *****
 *****                       *****
 *********************************
 *********************************/

FAC_DefineFacility {
    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (IOMStream);
    FAC_FDFCase_FacilityDescription ("I/O Manager Stream Driver");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  mainbody.c 1 replace /users/mjc/system
  860226 16:23:48 mjc create

 ************************************************************************/
/************************************************************************
  mainbody.c 2 replace /users/mjc/system
  860325 11:33:23 mjc Implemented first version of read-eval-print loop

 ************************************************************************/
/************************************************************************
  mainbody.c 3 replace /users/jad
  860326 19:52:50 jad add live calls to RTstring routines and a mainbody
facility complete with methods

 ************************************************************************/
/************************************************************************
  mainbody.c 4 replace /users/jad
  860417 15:56:32 jad added hooks to make the rs work from the editor

 ************************************************************************/
/************************************************************************
  mainbody.c 5 replace /users/jck/source
  860417 23:58:23 jck Added capital letter options

 ************************************************************************/
/************************************************************************
  mainbody.c 6 replace /users/jad
  860421 16:58:31 jad used io module now

 ************************************************************************/
/************************************************************************
  mainbody.c 7 replace /users/jad/system
  860425 12:05:17 jad now uses prompting input in its read-eval-print loop

 ************************************************************************/
/************************************************************************
  mainbody.c 8 replace /users/mjc/makework
  860503 16:38:29 mjc Added handlers for hangup signal, message receiver
on signal 17 (SIGUSR2), input display, and debugger disable

 ************************************************************************/
/************************************************************************
  mainbody.c 9 replace /users/mjc/system
  860506 17:35:13 mjc Disabled all but '?g' in an end user system

 ************************************************************************/
/************************************************************************
  mainbody.c 10 replace /users/jck/system
  860522 12:03:20 jck Temporary Addition of MethodDictionary Creation
 as interface to the compiler

 ************************************************************************/
/************************************************************************
  mainbody.c 11 replace /users/mjc/system
  860526 20:32:52 mjc Return vmachine invocation changes

 ************************************************************************/
/************************************************************************
  mainbody.c 12 replace /users/mjc/system
  860528 11:43:26 mjc Added '?t' timer test case

 ************************************************************************/
/************************************************************************
  mainbody.c 13 replace /users/ees/system
  860528 18:13:30 ees modified call to input routine: ees 5-28-86

 ************************************************************************/
/************************************************************************
  mainbody.c 14 replace /users/mjc/system
  860531 10:43:03 mjc Changed 'RSLANG_Compile' to pick up its method dictionary from 'envir'

 ************************************************************************/
/************************************************************************
  mainbody.c 15 replace /users/mjc/system
  860607 12:15:48 mjc Changed calls to V, eliminated lost string on error bug

 ************************************************************************/
/************************************************************************
  mainbody.c 16 replace /users/mjc/system
  860610 13:27:45 mjc Added compile and execute timing code

 ************************************************************************/
/************************************************************************
  mainbody.c 17 replace /users/mjc/system
  860612 17:29:26 mjc Added 'M_{Duplicate,Allocate,Standard}CPD...M_CPD_DecrementReferenceCount' timing tests (?[0-4])

 ************************************************************************/
/************************************************************************
  mainbody.c 18 replace /users/mjc/system
  860624 10:52:49 mjc Moved CPD allocation timing tests from 'mainbody' to messages to Q-Registers

 ************************************************************************/
/************************************************************************
  mainbody.c 19 replace /users/mjc/system
  860803 00:17:19 mjc Added '?w' option to save current environment

 ************************************************************************/
/************************************************************************
  mainbody.c 20 replace /users/mjc/system
  860803 21:23:12 mjc Added compile only option

 ************************************************************************/
/************************************************************************
  mainbody.c 21 replace /users/jck/system
  861015 09:05:02 jck Added option to allow user to terminate session using quit signal

 ************************************************************************/
/************************************************************************
  mainbody.c 22 replace /users/jad/system
  861027 14:49:55 jad added ?l and ?e

 ************************************************************************/
/************************************************************************
  mainbody.c 23 replace /users/jad/system
  870317 12:29:34 jad modify interrupt handling

 ************************************************************************/
/************************************************************************
  mainbody.c 24 replace /users/jad/system
  870324 13:11:29 jad modified interrupt handling to only be on during execution

 ************************************************************************/
/************************************************************************
  mainbody.c 25 replace /users/mjc/system
  870326 19:26:01 mjc Eliminated 'UserSystem' option, added 'Developer' and 'Administrator'

 ************************************************************************/
/************************************************************************
  mainbody.c 26 replace /users/mjc/translation
  870524 09:40:16 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  mainbody.c 27 replace /users/mjc/system
  870526 16:45:01 mjc Convert 'IOMStream::ProcessCommand' to a function

 ************************************************************************/
/************************************************************************
  mainbody.c 28 replace /users/mjc/system
  870614 20:04:43 mjc Convert UNIX specific include files to 'stdoss.h'

 ************************************************************************/
/************************************************************************
  mainbody.c 29 replace /users/jck/system
  871210 14:01:47 jck Added a repeated, timed compile and run option (?r[count])

 ************************************************************************/
/************************************************************************
  mainbody.c 30 replace /users/mjc/Software/system
  871222 16:41:10 mjc Alter 'help' message to shorten '?r' and other descriptions

 ************************************************************************/
/************************************************************************
  mainbody.c 31 replace /users/jad/system
  880711 16:14:10 jad Implemented Fault Remembering and 4 New List Primitives

 ************************************************************************/
/************************************************************************
  mainbody.c 32 replace /users/jck/system
  881128 10:19:13 jck Now Record time after system errors

 ************************************************************************/
/************************************************************************
  mainbody.c 33 replace /users/jck/system
  881212 13:44:35 jck Raise precision of time delta output

 ************************************************************************/
/************************************************************************
  mainbody.c 34 replace /users/m2/backend
  890828 17:04:58 m2 Added command '?> file' for compiling and redirecting to a file.

 ************************************************************************/
