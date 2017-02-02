/*****  VConnection Implementation  *****/

/**************************
 **************************
 *****  Declarations  *****
 **************************
 **************************/

/********************
 *****  System  *****
 ********************/

#include "VStdLib.h"

/******************
 *****  Self  *****
 ******************/

#include "VConnection.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"
#include "VConnectionUse.h"
#include "VFormatSettings.h"
#include "VDatum.h"
#include "VAccessString.h"
#include "VQueryCache.h"
#include "VEvaluatorSinkWaiter.h"
#include "Vca_VcaGofer.h"
#include "Vca_IPipeSource_Gofers.h"
#include "Vsa_IEvaluator_Gofers.h"
#include "Vca_IDirectory.h"

/*************************
 *************************
 *****               *****
 *****  VConnection  *****
 *****               *****
 *************************
 *************************/

/***********************
 ***********************
 *****  Constants  *****
 ***********************
 ***********************/

Vca::U32 VConnection::g_cPortNumber                     = 512;
size_t VConnection::g_sAdaptiveReallocationDelay	= 2;
size_t VConnection::g_sAdaptiveReallocationMultiplier	= 4;
size_t VConnection::g_sMaxAdaptiveReallocationIncrement	= 16 * 1024 * 1024;

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VConnection::VConnection ()
: m_pFormatSettings	(VFormatSettings::GlobalSettings->newChild ())
, m_pUseListHead	(0)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VConnection::~VConnection () {
    disconnect ();

    while (m_pUseListHead)
	detach (m_pUseListHead);
}

/****************************
 ****************************
 *****  IEvaluator EKG  *****
 ****************************
 ****************************/

void VConnection::signalIEvaluatorEKG () {
	// Cancel the EKG.
	cancelIEvaluatorEKG ();

	// Clear the interface reference.
        m_pIEvaluator.clear ();
}

void VConnection::cancelIEvaluatorEKG () {
	if (m_pIEvaluatorEKG.isntNil ()) {
		m_pIEvaluatorEKG->cancel ();
		m_pIEvaluatorEKG.clear ();
	}
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

bool VConnection::getSessionNames(VAccessString &rSessionNames) {
    // Cohort Claim
    Vca::VCohort::Claim c;

    // Create our waiter.
    EntryNamesReceiver::Reference pNamesReceiverWaiter (new EntryNamesReceiver());

    // Get our IVReceiver.
    EntryNamesReceiver::ReceiverInterface::Reference pINamesReceiverWaiter;
    pNamesReceiverWaiter->getRole (pINamesReceiverWaiter);

    // Use our waiter.
    Vca::GetObjectNames (pINamesReceiverWaiter);
    c.clear ();
    pNamesReceiverWaiter->wait ();

    // Retrieve result.
    EntryNamesReceiver::DataReference pEntryNameArray;
    pNamesReceiverWaiter->getDatum (pEntryNameArray);

    // Detect errors.
    if (pEntryNameArray.isNil ()) {
        return false;
    }

    // Convert array to newline-delimited string.
    rSessionNames.clear ();
    for (int i = 0, end = pEntryNameArray->elementCount (); i < end; i++) {
        // Get our entry name. Skip if none found.
        const VString *entryName = pEntryNameArray->element(i);
        if (!entryName) continue;

        // Retrieve session description.
        Vca::VCohort::Claim c;
        EntryDescriptionReceiver::Reference pDescriptionReceiver (new EntryDescriptionReceiver);
        EntryDescriptionReceiver::ReceiverInterface::Reference pIDescriptionReceiver;
        pDescriptionReceiver->getRole (pIDescriptionReceiver);
        Vca::GetObjectDescription (pIDescriptionReceiver, *entryName);
        c.clear ();
        pDescriptionReceiver->wait ();
        EntryDescriptionReceiver::DataReference description;
        pDescriptionReceiver->getDatum (description);

        // Check if session is hidden. Skip if so.
        if (strstr (description->content (), "Hidden")) continue;

        // Add session to return variable.
        if (i != 0) rSessionNames.append("\n");
        rSessionNames.append(entryName->content ());
    }
    return true;
}

/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

bool VConnection::login (
    VAccessString &	rErrorMessage,
    VAccessString const & rHostName,
    VAccessString const & rUserName,
    VAccessString const & rPassword, 
    VAccessString const & rCommand
)
{
    disconnect ();
    rErrorMessage.clear ();

    // Figure out whethe or not we're using rexec. By default, use SSH.
    VConnection_ConnectionTypes connectionType = ConnectionType_SSH;
    VString sProtocol, sHostName_Actual;
    if (rHostName.getPrefix("://", sProtocol, sHostName_Actual)) {
        // Protocol was specified
        // TODO: Make case insensitive.
        if (sProtocol == "ssh") {
            connectionType = ConnectionType_SSH;
        } else if (sProtocol == "rexec") {
            connectionType = ConnectionType_Rexec;
        } else if (sProtocol == "session") {
            connectionType = ConnectionType_Session;
        } else {
            // TODO: Global variable for errors.
            rErrorMessage << "Unhandled protocol: " << sProtocol;
            return false;
        }
    } else {
        sHostName_Actual = rHostName;
    }

    // Cohort Claim
    Vca::VCohort::Claim c;

    // Get our IPipeSource.
    Vsa::IEvaluator_Gofer::gofer_base_t::Reference pGofer;
    switch (connectionType) {
        case ConnectionType_SSH:
            {
                VString sshCommand;
                sshCommand << "{w}plink.exe -batch -ssh -l " << rUserName << " -pw " << rPassword << " " << sHostName_Actual << " " << rCommand;
                Vca::VGoferInterface<Vca::IPipeSource>::Reference pPipeSourceGofer (new Vca::IPipeSource_Gofer::NewProcess(sshCommand, true, true, false));
                pGofer.setTo (new Vsa::IEvaluator_Gofer::FromPipeSource (pPipeSourceGofer));
            }
            break;
        case ConnectionType_Rexec:
            {
                Vca::VGoferInterface<Vca::IPipeSource>::Reference pPipeSourceGofer(new Vca::IPipeSource_Gofer::NewRexec(sHostName_Actual, rUserName, rPassword, rCommand, false));
                pGofer.setTo (new Vsa::IEvaluator_Gofer::FromPipeSource (pPipeSourceGofer));
            }
            break;
        case ConnectionType_Session:
            pGofer.setTo (new Vca::Gofer::Named<Vsa::IEvaluator,Vca::IDirectory>(sHostName_Actual));
            break;
        default:
            rErrorMessage << "Unhandled connection type.";
            return false;
    }

    VEvaluatorSinkWaiter::Reference pEvaluatorSinkWaiter (new VEvaluatorSinkWaiter ());
    pGofer->supplyMembers (pEvaluatorSinkWaiter.referent (), &VEvaluatorSinkWaiter::OnData, &VEvaluatorSinkWaiter::OnError);
    c.clear ();
    pEvaluatorSinkWaiter->wait ();
    pEvaluatorSinkWaiter->getEvaluator (m_pIEvaluator);

    // Detect error.
    if (m_pIEvaluator.isNil ()) {
        // No IEvaluator retrieved. Check for an error message.
        VString sMessage;
        pEvaluatorSinkWaiter->getErrorMessage(sMessage);
        if (sMessage.length () > 0) {
            rErrorMessage << sMessage;
            return false;
        }
    }

    setupConnection ();

    return isConnected();
}


/************************
 ************************
 *****  Connection  *****
 ************************
 ************************/

bool VConnection::connect (char const *pHostName, u_short xHostPort)
{
    disconnect ();

    // Cohort Claim
    Vca::VCohort::Claim c;

    // Get our IPipeSource.
    VString sAddress;
    sAddress << pHostName << ":" << xHostPort;
    Vca::VGoferInterface<Vca::IPipeSource>::Reference pPipeSourceGofer;
    pPipeSourceGofer = new Vca::IPipeSource_Gofer::NewConnection (sAddress, false);
    Vsa::IEvaluator_Gofer::FromPipeSource::Reference pGofer (new Vsa::IEvaluator_Gofer::FromPipeSource (pPipeSourceGofer));

    VEvaluatorSinkWaiter::Reference pEvaluatorSinkWaiter (new VEvaluatorSinkWaiter ());
    pGofer->supplyMembers (pEvaluatorSinkWaiter.referent (), &VEvaluatorSinkWaiter::OnData, &VEvaluatorSinkWaiter::OnError);
    c.clear ();
    pEvaluatorSinkWaiter->wait ();
    pEvaluatorSinkWaiter->getEvaluator (m_pIEvaluator);

    // Detect error.
    if (m_pIEvaluator.isNil ()) {
        // No IEvaluator retrieved.
        return false;
    }

    setupConnection ();

    return isConnected();
}


/***************************
 ***************************
 *****  Disconnection  *****
 ***************************
 ***************************/

void VConnection::disconnect ()
{
    // Close the IEvaluator connection.
    cancelIEvaluatorEKG ();
    m_pIEvaluator.clear ();

    // Propagate disconnect to connection uses.
    for (
	VConnectionUse *pUse = m_pUseListHead;
	pUse != 0;
	pUse = pUse->m_pNextUse
    ) pUse->handleDisconnectEvent ();
}


/********************************
 ********************************
 *****  Request Processing  *****
 ********************************
 ********************************/

/*********************
 *----  Include  ----*
 *********************/

bool VConnection::include (char const* pIncludeFileName)
{
    VString iIncludeReply;

    return include (pIncludeFileName, iIncludeReply);
}

bool VConnection::include (char const* pIncludeFileName, VString& iIncludeReply)
{
    VString iIncludeRequest;
    iIncludeRequest.quote (pIncludeFileName) << " asFileContents evaluate";

    return submit (iIncludeReply, iIncludeRequest);
}


/*********************
 *----  execute  ----*
 *********************/

bool VConnection::execute (
    char const*		pRequest,
    char const*		pDate,
    char const*		pCurrency
)
{
    VString iReply;
    return submit (iReply, pRequest, pDate, pCurrency);
}


/*****************
 *----  get  ----*
 *****************/

bool VConnection::getValue (
    VDatum&	iResult,
    char const*	pEntityType,
    char const*	pEntity,
    char const*	pItem,
    char const*	pDate,
    char const*	pCurrency
)
{
    VString iReply;

    return getValue (
	iReply, pEntityType, pEntity, pItem, pDate, pCurrency
    ) && iResult.parse (iReply);
}

bool VConnection::getValue (
    VString&	iResult,
    char const*	pEntityType,
    char const*	pEntity,
    char const*	pItem,
    char const*	pDate,
    char const*	pCurrency
)
{
    VString iRequest ("Named ");

    iRequest << pEntityType << " at: ";
    iRequest.quote (pEntity);
    iRequest << ". " << pItem;

    VString iReply;
    if (!submit (iReply, iRequest, pDate, pCurrency))
	return false;

    //  Trim leading white space for compatibility with VDatum::parse, ...
    char const* pReplyStart = iReply;
    pReplyStart += strspn (pReplyStart, VAccessString::WhiteSpace);

    //  Trim the trailing new line, ...
    char const* pReplyLimit = strrchr (pReplyStart, '\n');
    iResult.setTo (
	pReplyStart,
	pReplyLimit ? (size_t)(pReplyLimit - pReplyStart) : strlen (pReplyStart)
    );

    return true;
}

bool VConnection::getValue (
    double&	iResult,
    char const*	pEntityType,
    char const*	pEntity,
    char const*	pItem,
    char const*	pDate,
    char const*	pCurrency
)
{
    VString iReply;
    VDatum  iDatum;

    return getValue (
	iReply, pEntityType, pEntity, pItem, pDate, pCurrency
    ) && iDatum.parse (iReply, VDatumKind_Real) && iDatum.getValue (iResult);
}

bool VConnection::getValue (
    int&	iResult,
    char const*	pEntityType,
    char const*	pEntity,
    char const*	pItem,
    char const*	pDate,
    char const*	pCurrency
)
{
    VString iReply;
    VDatum  iDatum;

    return getValue (
	iReply, pEntityType, pEntity, pItem, pDate, pCurrency
    ) && iDatum.parse (iReply, VDatumKind_Integer) && iDatum.getValue (iResult);
}


/********************
 *----  Submit  ----*
 ********************/

bool VConnection::submit (
    VString&		iReply,
    char const*		pRequest,
    char const*		pDate,
    char const*		pCurrency
)
{
    static char const * const	pExecutionDirective = "\n?g\n";
    static size_t const		sExecutionDirective = strlen (
	pExecutionDirective
    );

    if (0 == this)
	return false;

//  Formulate the actual request to send, ...
    VString iAltRequest;
    bool bHasDate = pDate && strlen (pDate) > 0;
    bool bHasCurrency = pCurrency && strlen (pCurrency) > 0;
    if (bHasDate || bHasCurrency)
    {
	if (bHasDate)
	    iAltRequest.quote (pDate). append (" asDate evaluate: [");
	if (bHasCurrency)
	    iAltRequest.quote (pCurrency). append (" asCurrency evaluate: [");
	iAltRequest.quote (pRequest). append (" evaluate");
	if (bHasCurrency)
	    iAltRequest.append ("]");
	if (bHasDate)
	    iAltRequest.append ("]");

	pRequest = iAltRequest;
    }
    
    // Send request, retrieve result.
    Vca::VCohort::Claim c;
    VQueryCache::Reference rQueryCache (new VQueryCache (pRequest));
    c.release ();
    if (!rQueryCache->evaluateWith (m_pIEvaluator)) return false; // TODO: Add timeout.
    VString toReturn;
    rQueryCache->getResult (toReturn);

    // Strip out prompts and convert line endings.
    //toReturn.convertLFtoCRLF(iReply);
    iReply.setTo(toReturn);

    return true;
}

/************************
 ************************
 *****  Attachment  *****
 ************************
 ************************/

void VConnection::attach (VConnectionUse *pUse) {
    if (this != pUse->connection ())
	return;

    if (m_pUseListHead) {
	m_pUseListHead->m_pPrevUse = pUse;
	pUse->m_pNextUse = m_pUseListHead;
    }
    m_pUseListHead = pUse;

    if (isConnected ())
	pUse->handleConnectionEvent ();
}

void VConnection::detach (VConnectionUse *pUse) {
    if (this != pUse->connection ())
	return;

    if (pUse->m_pNextUse)
	pUse->m_pNextUse->m_pPrevUse = pUse->m_pPrevUse;

    if (pUse->m_pPrevUse)
	pUse->m_pPrevUse->m_pNextUse = pUse->m_pNextUse;
    else
	m_pUseListHead = pUse->m_pNextUse;

    pUse->m_pPrevUse = pUse->m_pNextUse = (VConnectionUse*)0;
}


/*******************
 *******************
 *****  Setup  *****
 *******************
 *******************/

void VConnection::setupConnection ()
{
	    for (
		VConnectionUse *pUse = m_pUseListHead;
		pUse != 0;
		pUse = pUse->m_pNextUse
	    ) pUse->handleConnectionEvent ();
}
