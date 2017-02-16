/*****  I/O Driver Implementation  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName IOM

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "vdsc.h"
#include "venvir.h"
#include "verr.h"
#include "vfac.h"

#include "RTptoken.h"
#include "RTvector.h"

#include "VDescriptor.h"

/*****  Self  *****/
#include "IOMHandle.h"


/***************************************************
 ***************************************************
 *****  Constants, Globals, and Private Types  *****
 ***************************************************
 ***************************************************/

PrivateVarDef bool		DriverTableCleanupNeeded = false;
PrivateVarDef unsigned int	MinSlotToClean = 0;
PrivateVarDef unsigned int	MaxSlotToClean = 0;

PrivateVarDef size_t		DriverTableSize	 = 0;
PrivateVarDef IOMDriver**	DriverTable	 = NilOf (IOMDriver**);

PublicVarDef M_CPD		*IOM_HandlerVector = NilOf (M_CPD*),
				*IOM_HandlerPToken = NilOf (M_CPD*);


/***********************
 ***********************
 *****  Utilities  *****
 ***********************
 ***********************/

/**************************
 *****  Descriptions  *****
 **************************/

PublicFnDef char const* IOM_ChannelTypeDescription (IOMChannelType xCode) {
    switch (xCode) {
    case IOMChannelType_Null:
	return "Null";
    case IOMChannelType_Service:
	return "Service";
    case IOMChannelType_Stream:
	return "Stream";
    case IOMChannelType_File:
	return "File";
    default:
	return "???";
    }
}

PublicFnDef char const* IOM_StateDescription (IOMState xCode) {
    switch (xCode) {
    case IOMState_Unused:
	return "Unused";
    case IOMState_Ready:
	return "Ready";
    case IOMState_Starting:
	return "Starting";
    case IOMState_Restarting:
	return "Restarting";
    case IOMState_Running:
	return "Running";
    case IOMState_Closing:
	return "Closing";
    default:
	return "???";
    }
}


PublicFnDef char const* IOM_OptionDescription (IOMOption xCode) {
    switch (xCode) {
    case IOMOption_None:
	return "None";
    case IOMOption_PromptFormat:
	return "Prompt Format";
    case IOMOption_BinaryFormat:
	return "Binary Format";
    case IOMOption_TrimFormat:
	return "Trim Format";
    case IOMOption_SeekOffset:
	return "Seek Offset";
    case IOMOption_TCPNODELAY:
	return "TCP NoDelay";
    case IOMOption_AutoMutex:
	return "Auto Mutex";
    default:
	return "???";
    }
}


PublicFnDef char const* IOM_PromptFormatDescription (IOMPromptFormat xCode) {
    switch (xCode) {
    case IOMPromptFormat_Normal:
	return "Normal";
    case IOMPromptFormat_Editor:
	return "Editor";
    default:
	return "???";
    }
}

PublicFnDef char const* IOM_BinaryFormatDescription (IOMBinaryFormat xCode) {
    switch (xCode) {
    case IOMBinaryFormat_Untranslated:
	return "Untranslated";
    case IOMBinaryFormat_PARisc:
	return "PARisc";
    case IOMBinaryFormat_Sparc:
	return "Sparc";
    case IOMBinaryFormat_Intel:
	return "Intel";
    default:
	return "???";
    }
}

PublicFnDef char const* IOM_TrimFormatDescription (IOMTrimFormat xCode) {
    switch (xCode) {
    case IOMTrimFormat_Untrimmed:
	return "Untrimmed";
    case IOMTrimFormat_TrimLeadingBlanks:
	return "Trim Leading Blanks";
    case IOMTrimFormat_TrimTrailingBlanks:
	return "Trim Trailing Blanks";
    case IOMTrimFormat_TrimBoundingBlanks:
	return "Trim Bounding Blanks";
    default:
	return "???";
    }
}


/********************
 ********************
 *****  Driver  *****
 ********************
 ********************/

bool IOMDriver::getInputHandler (DSC_Descriptor& rInputHandler) {
    if (m_fHandlerWired)
	return false;

    DSC_Scalar handlerPointer;
    DSC_InitScalar (
	handlerPointer,
	IOM_HandlerPToken,
	RTYPE_C_RefUV,
	DSC_Scalar_Int,
	m_xDriver
    );
    rtVECTOR_RefExtract (
	IOM_HandlerVector, &handlerPointer, &rInputHandler
    );

    m_fHandlerWired = rInputHandler.storeCPD ()->NamesTheNAClass ();
    if (m_fHandlerWired) {
	rInputHandler.clear ();
	return false;
    }

    return true;
}


/********************
 ********************
 *****  Handle  *****
 ********************
 ********************/

/**************************
 *****  Construction  *****
 **************************/

void IOMHandle::construct () {
    m_xDriverValid	= false;
    m_xDriver		= VkStatusCode_Invalid;
    m_xOption		= IOMOption_None;
    m_xTrimFormat	= IOMTrimFormat_Untrimmed;
    m_iSeekOffset	= 0;
}

void IOMHandle::construct (VkStatus const* pStatus) {
    construct ();

    m_xDriverValid	= false;
    m_xDriver		= pStatus->Code ();
}

void IOMHandle::construct (IOMDriver const* pDriver) {
    construct ();

    m_xDriver		= pDriver->index ();
    m_xDriverValid	= true;
}

void IOMHandle::construct (unsigned int xDriver) {
    if (--xDriver < DriverTableSize && IsntNil (DriverTable[xDriver]))
	construct (DriverTable[xDriver]);
    else {
	VkStatus iStatus;
	iStatus.MakeFailureStatus ();
	construct (&iStatus);
    }
}


/********************
 *****  Access  *****
 ********************/

IOMDriver* IOMHandle::driver () const {
    return m_xDriverValid && m_xDriver < DriverTableSize
	? DriverTable[m_xDriver] : NilOf (IOMDriver*);
}


IOMChannelType IOMHandle::channelType () const {
    IOMDriver* pDriver = driver ();
    return pDriver ? pDriver->channelType () : IOMChannelType_Null;
}

IOMState IOMHandle::state () const {
    IOMDriver* pDriver = driver ();
    return pDriver ? pDriver->state () : IOMState_Unused;
}

VkStatusType IOMHandle::status () const {
    IOMDriver* pDriver = driver ();
    return pDriver ? pDriver->statusType () : VkStatusType_Failed;
}

VkStatusCode IOMHandle::statusCode () const {
    IOMDriver* pDriver = driver ();
    return pDriver ? pDriver->statusCode () : (VkStatusCode)m_xDriver;
}


int IOMHandle::getSocketNameString (char **ppString, size_t *psString) const {
    IOMDriver* pDriver = driver ();

    if (IsNil (pDriver) || pDriver->getSocketNameString (ppString, psString) < 0) {
	*ppString = NilOf (char *);
	*psString = 0;
	return -1;
    }

    return 0;
}

int IOMHandle::getPeerNameString (char **ppString, size_t *psString) const {
    IOMDriver* pDriver = driver ();

    if (IsNil (pDriver) || pDriver->getPeerNameString (ppString, psString) < 0) {
	*ppString = NilOf (char *);
	*psString = 0;
	return -1;
    }

    return 0;
}


/********************
 *****  Update  *****
 ********************/

int IOMHandle::enableHandler (VTask* pSpawningTask) const {
    IOMDriver* pDriver = driver ();
    return pDriver ? pDriver->EnableHandler (pSpawningTask) : false;
}

int IOMHandle::disableHandler () const {
    IOMDriver* pDriver = driver ();
    return pDriver ? pDriver->DisableHandler () : false;
}

unsigned int IOMHandle::retain () {
    IOMDriver* pDriver = driver ();
    return pDriver ? pDriver->IncrementUseCount () : 0;
}

unsigned int IOMHandle::release () {
    IOMDriver* pDriver = driver ();
    return pDriver ? pDriver->DecrementUseCount () : 0;
}

void IOMHandle::setOptionValue (unsigned int optionValue) {
    IOMDriver* pDriver = driver ();
    if (pDriver)
	pDriver->SetOptionValue (this, optionValue);
}

void IOMHandle::setWiredHandler (int value) const {
    IOMDriver* pDriver = driver ();
    if (pDriver)
	pDriver->SetWiredHandler (value);
}


void IOMHandle::setErrorInput (IOMHandle const* pHandle) const {
    IOMDriver* pDriver = driver ();
    IOMDriver* pOtherDriver = pHandle->driver ();
    if (pDriver)
	pDriver->SetErrorInputDriver (pOtherDriver);
}

void IOMHandle::setErrorOutput (IOMHandle const* pHandle) const {
    IOMDriver* pDriver = driver ();
    IOMDriver* pOtherDriver = pHandle->driver ();
    if (pDriver)
	pDriver->SetErrorOutputDriver (pOtherDriver);
}


int IOMHandle::getTcpNodelay (int* fOnOff) const {
    *fOnOff = false;

    IOMDriver* pDriver = driver ();
    return pDriver ? pDriver->getTcpNodelay (fOnOff) : -1;
}

int IOMHandle::setTcpNodelay (int fOnOff) const {
    IOMDriver* pDriver = driver ();
    return pDriver ? pDriver->SetTcpNodelay (fOnOff) : -1;
}

int IOMHandle::endTransmission () const {
    IOMDriver* pDriver = driver ();

    return pDriver ? pDriver->EndTransmission () : -1;
}

int IOMHandle::endReception () const {
    IOMDriver* pDriver = driver ();
    return pDriver ? pDriver->EndReception () : -1;
}

unsigned int IOMHandle::close () const {
    IOMDriver* pDriver = driver ();
    if (pDriver) {
	//	pDriver->retain ();
	pDriver->Close ();
	pDriver->release ();
    }

    return 0;
}


/*******************
 *****  Input  *****
 *******************/

/***************
 *  GetStream  *
 ***************/

IOMDriver* IOMHandle::getConnection () const {
    IOMDriver* pDriver = driver ();
    return pDriver ? pDriver->GetConnection () : NilOf (IOMDriver*);
}

/***********************
 *  Byte Count Access  *
 ***********************/

size_t IOMHandle::getByteCount () const {
    IOMDriver* pDriver = driver ();
    return pDriver ? pDriver->GetByteCount (this) : 0;
}


/*******************
 *  Numeric Input  *
 *******************/

bool IOMHandle::getByte (unsigned char *value) const {
    IOMDriver* pDriver = driver ();
    return pDriver ? pDriver->GetByte (this, value) : false;
}

bool IOMHandle::getShort (unsigned short *value) const {
    IOMDriver* pDriver = driver ();
    return pDriver ? pDriver->GetShort (this, value) : false;
}

bool IOMHandle::getLong (unsigned int *value) const {
    IOMDriver* pDriver = driver ();
    return pDriver ? pDriver->GetLong (this, value) : false;
}

bool IOMHandle::getFloat (float *value) const {
    IOMDriver* pDriver = driver ();
    return pDriver ? pDriver->GetFloat (this, value) : false;
}

bool IOMHandle::getDouble (double *value) const {
    IOMDriver* pDriver = driver ();
    return pDriver ? pDriver->GetDouble (this, value) : false;
}


/******************
 *  String Input  *
 ******************/

VkStatusType IOMHandle::trimString (
    char **ppString, size_t *psString, VkStatusType resultStatus
) const {
    size_t span;

/*****  Return if no string exists to be trimmed, ...  *****/
    if (IsNil (*ppString))
	return resultStatus;

/*---------------------------------------------------------------------------
 *  ... end the string at the first NULL to avoid confusing the
 *      L-Store routines, ...
 *---------------------------------------------------------------------------
 */
    *psString = strlen (*ppString);

/*****  ... and trim the string:  *****/
    switch (m_xTrimFormat)
    {
    case IOMTrimFormat_Untrimmed:
    default:
	break;
    case IOMTrimFormat_TrimLeadingBlanks:
	span = strspn (*ppString, " ");
	if (span > 0)
	{
	    memmove (*ppString, *ppString + span, *psString - span + 1);
	    *psString -= span;
	}
	break;
    case IOMTrimFormat_TrimTrailingBlanks:
	for (span = *psString; span > 0 && ' ' == (*ppString)[span - 1]; span--);
	(*ppString)[span] = '\0';
	*psString = span;
	break;
    case IOMTrimFormat_TrimBoundingBlanks:
	span = strspn (*ppString, " ");
	if (span > 0)
	{
	    memmove (*ppString, *ppString + span, *psString - span + 1);
	    *psString -= span;
	}
	for (span = *psString; span > 0 && ' ' == (*ppString)[span - 1]; span--);
	(*ppString)[span] = '\0';
	*psString = span;
	break;
    }
    return resultStatus;
}


VkStatusType IOMHandle::trimLine (
    char **ppString, size_t *psString, VkStatusType resultStatus
) const {
/*****  Return if no string exists to be trimmed, ...  *****/
    if (IsNil (*ppString))
	return resultStatus;

/*****  ... and trim the string:  *****/
    return trimString (ppString, psString, resultStatus);
}


VkStatusType IOMHandle::getString (
    size_t sMinLength, size_t sMaxLength, char **ppString, size_t *psString
) const {
    IOMDriver* pDriver = driver ();

    *ppString = NilOf (char *);
    *psString = 0;

    return pDriver ? trimString (
	ppString, psString, pDriver->GetString (
	    this, sMinLength, sMaxLength, ppString, psString
	)
    ) : VkStatusType_Failed;
}


VkStatusType IOMHandle::getLine (
    char const* zPrompt, char **ppLine, size_t *psLine
) const {
    IOMDriver* pDriver = driver ();
    *ppLine = NilOf (char *);
    *psLine = 0;

    return pDriver ? trimLine (
	ppLine, psLine, pDriver->GetLine (this, zPrompt, ppLine, psLine)
    ) : VkStatusType_Failed;
}


/********************
 *****  Output  *****
 ********************/

/********************
 *  Numeric Output  *
 ********************/

size_t IOMHandle::putByte (unsigned char value) const {
    IOMDriver* pDriver = driver ();
    return pDriver ? pDriver->PutByte (this, value) : 0;
}

size_t IOMHandle::putShort (unsigned short value) const {
    IOMDriver* pDriver = driver ();
    return pDriver ? pDriver->PutShort (this, value) : 0;
}

size_t IOMHandle::putLong (unsigned int value) const {
    IOMDriver* pDriver = driver ();
    return pDriver ? pDriver->PutLong (this, value) : 0;
}

size_t IOMHandle::putFloat (float value) const {
    IOMDriver* pDriver = driver ();
    return pDriver ? pDriver->PutFloat (this, value) : 0;
}

size_t IOMHandle::putDouble (double value) const {
    IOMDriver* pDriver = driver ();
    return pDriver ? pDriver->PutDouble (this, value) : 0;
}


/*******************
 *  String Output  *
 *******************/

size_t IOMHandle::putString (char const *pString) const {
    IOMDriver* pDriver = driver ();
    return pDriver ? pDriver->PutData (this, pString, strlen (pString)) : 0;
}

/*********************
 *  Output Flushing  *
 *********************/

int IOMHandle::putBufferedData () const {
    IOMDriver* pDriver = driver ();
    if (pDriver) {
	pDriver->PutBufferedData ();
	return true;
    }
    return false;
}


/*******************************
 *******************************
 *****  Subsystem Manager  *****
 *******************************
 *******************************/

/***************************
 *****  Table Cleanup  *****
 ***************************/

PrivateFnDef void CleanupDriverTable () {
    if (DriverTableCleanupNeeded) {
	static VDescriptor iNA;

	if (iNA.isEmpty ()) {
	    iNA.setToNA (M_AttachedNetwork->KOT ());
	}

	unsigned int	cEmptySlots = 0;
	rtLINK_CType*	pEmptySlots = rtLINK_RefConstructor (IOM_HandlerPToken, -1);

	for (unsigned int xDriver = MinSlotToClean; xDriver <= MaxSlotToClean; xDriver++) {
	    if (IsNil (DriverTable[xDriver])) {
		rtLINK_AppendRange (pEmptySlots, xDriver, 1);
		cEmptySlots++;
	    }
	}

	M_CPD* pEmptySlotPPT = rtPTOKEN_New (M_AttachedNetwork->ScratchPad (), cEmptySlots);
	pEmptySlots->Close (pEmptySlotPPT);

	rtVECTOR_Assign   (IOM_HandlerVector, pEmptySlots, &iNA.contentAsMonotype ());
	rtVECTOR_AlignAll (IOM_HandlerVector, true);

	pEmptySlotPPT->release ();
	pEmptySlots->release ();

	DriverTableCleanupNeeded = false;
    }
}


/**************************
 *****  Table Update  *****
 **************************/

void IOMDriver::AttachToDriverTable () {
    for (m_xDriver = 0; m_xDriver < DriverTableSize; m_xDriver++)
	if (IsNil (DriverTable[m_xDriver]))
	    break;

    if (m_xDriver >= DriverTableSize) {
	unsigned int xDriver = DriverTableSize;

	DriverTableSize += 16;

	if (DriverTable) {
	    M_CPD* newHandlerPToken = rtPTOKEN_NewShiftPTConstructor (
		IOM_HandlerPToken, -1
	    )->AppendAdjustment (xDriver, DriverTableSize - xDriver)->ToPToken ();
	    IOM_HandlerPToken->release ();
	    IOM_HandlerPToken = newHandlerPToken;

	    DriverTable = (IOMDriver**)reallocate (
		DriverTable, DriverTableSize * sizeof (IOMDriver**)
	    );
	}
	else {
	    IOM_HandlerPToken = rtPTOKEN_New (
		M_AttachedNetwork->ScratchPad (), DriverTableSize
	    );
	    IOM_HandlerVector = rtVECTOR_New (IOM_HandlerPToken);

	    DriverTable = (IOMDriver**)allocate (
		DriverTableSize * sizeof (IOMDriver**)
	    );
	}

	while (xDriver < DriverTableSize)
	    DriverTable[xDriver++] = NilOf (IOMDriver*);
    }
    else if (DriverTableCleanupNeeded)
	CleanupDriverTable ();

    DriverTable[m_xDriver] = this;
}


void IOMDriver::DetachFromDriverTable () {
/*----------------------------------------------------------------------------------*
 *  Drivers with user defined handlers leave those handlers behind in the handler
 *  vector when they are detached from the driver table.  The handler cannot be set
 *  to NA here because the vector routines used to do so are not reentrant and this
 *  routine is subject to recursive call via the IOMDriver destructor which calls it.
 *  To work around this, newly constructed drivers are set to ignore this orphaned
 *  handler with driver table cleanup postponed to a safer place.
 *----------------------------------------------------------------------------------*/
    if (DriverTable[m_xDriver] == this) {
	DriverTable[m_xDriver] = NilOf (IOMDriver*);

	if (m_fHandlerWired) {	// No cleanup needed for wired handlers.
	}
	else if (DriverTableCleanupNeeded) {
	    if (MinSlotToClean > m_xDriver)
		MinSlotToClean = m_xDriver;
	    if (MaxSlotToClean < m_xDriver)
		MaxSlotToClean = m_xDriver;
	}
	else {
	    MinSlotToClean = MaxSlotToClean = m_xDriver;
	    DriverTableCleanupNeeded = true;
	}
    }
}


/*************************
 *****  Table Query  *****
 *************************/

PublicFnDef size_t IOM_DriverTableSize () {
    return DriverTableSize;
}


/********************************
 *****  Event Loop Helpers  *****
 ********************************/

PublicFnDef void IOM_PutBufferedData () {
    if (DriverTableCleanupNeeded)
	CleanupDriverTable ();

    for (unsigned int xDriver = 0; xDriver < DriverTableSize; xDriver++) {
	IOMDriver* pDriver = DriverTable[xDriver];
	if (IsntNil (pDriver) && !pDriver->isClosing ())
	    pDriver->PutBufferedData ();  
    }
}


/*********************************
 *********************************
 *****  Facility Definition  *****
 *********************************
 *********************************/

FAC_DefineFacility {
    static bool alreadyInitialized = false;

    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (IOManager);
    FAC_FDFCase_FacilityDescription ("I/O Manager");

    case FAC_DoStartupInitialization:
	if (alreadyInitialized) {
	    FAC_CompletionCodeField = FAC_RequestAlreadyDone;
	    break;
	}

	alreadyInitialized = true;
	FAC_CompletionCodeField = FAC_RequestSucceeded;
	break;

    case FAC_DoShutdownCleanup:
	FAC_CompletionCodeField = FAC_RequestSucceeded;
	break;

    default:
	break;
    }
}
