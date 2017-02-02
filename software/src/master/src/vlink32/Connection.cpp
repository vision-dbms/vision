/////////////////////////////////////////////////////////////////////////////
//
//  CConnection Implementation
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//  Interfaces
/////////////////////////////////////////////////////////////////////////////

//  System...
#include "stdafx.h"
#include "stdext.h"

//  Supporting...
#include "Agent.h"
#include "ForwardingService.h"
#include "history.h"
#include "LocatorAgent.h"
#include "Request.h"
#include "SocketAgent.h"
#include "TextBuffer.h"
#include "vlinkapp.h"

//  Self...
#include "Connection.h"

/////////////////////////////////////////////////////////////////////////////
//  Globals
/////////////////////////////////////////////////////////////////////////////

const CString	CConnection::g_launchDirective	= "?g\n";
const long	CConnection::g_defaultTimeout	=  60;

CCollectionKernel CConnection::Instances;


/////////////////////////////////////////////////////////////////////////////
//  Message Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CConnection, CNetworkClient)
    //{{AFX_MSG_MAP(CConnection)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//  Dispatch Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_DISPATCH_MAP(CConnection, CNetworkClient)
    //{{AFX_DISPATCH_MAP(CConnection)
    DISP_PROPERTY_EX(CConnection, "Timeout", OAGetTimeout, OASetTimeout, VT_I4)
    DISP_FUNCTION(CConnection, "Hostname", OAHostname, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CConnection, "Username", OAUsername, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CConnection, "Password", OAPassword, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CConnection, "Database", OADatabase, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CConnection, "ObjectSpace", OAObjectSpace, VT_I2, VTS_NONE)
    DISP_FUNCTION(CConnection, "Options", OAOptions, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CConnection, "Executable", OAExecutable, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CConnection, "Command", OACommand, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CConnection, "Description", OADescription, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CConnection, "LocalAddress", OALocalAddress, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CConnection, "RemoteAddress", OARemoteAddress, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CConnection, "WaitForCompletion", OAWaitForCompletion, VT_EMPTY, VTS_I4)
    DISP_FUNCTION(CConnection, "CompletedWithin", OACompletedWithin, VT_BOOL, VTS_I4)
    DISP_FUNCTION(CConnection, "WaitForConnection", OAWaitForConnection, VT_EMPTY, VTS_I4)
    DISP_FUNCTION(CConnection, "IsUpWithin", OAIsUpWithin, VT_BOOL, VTS_I4)
    DISP_FUNCTION(CConnection, "IsUp", OAIsUp, VT_BOOL, VTS_NONE)
    DISP_FUNCTION(CConnection, "IsDown", OAIsDown, VT_BOOL, VTS_NONE)
    DISP_FUNCTION(CConnection, "IsStable", OAIsStable, VT_BOOL, VTS_NONE)
    DISP_FUNCTION(CConnection, "IsUnstable", OAIsUnstable, VT_BOOL, VTS_NONE)
    DISP_FUNCTION(CConnection, "Submit", OASubmit, VT_DISPATCH, VTS_BSTR)
    DISP_FUNCTION(CConnection, "RequestCount", OARequestCount, VT_I4, VTS_NONE)
    DISP_FUNCTION(CConnection, "RequestHistory", OARequestHistory, VT_DISPATCH, VTS_NONE)
    DISP_FUNCTION(CConnection, "StoreTemplate", OAStoreTemplate, VT_BOOL, VTS_BSTR)
    DISP_FUNCTION(CConnection, "CreateAgent", OACreateAgent, VT_DISPATCH, VTS_NONE)
    //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
//  Construction
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CConnection, CNetworkClient)

CConnection::CConnection (CConnectionData& rConnectionData)
: m_iController		(rConnectionData)
, m_iPrompt		(rConnectionData.m_useEPrompt ? "V> \005\n" : "V> ")
, m_pForwardingService	(NULL)
, m_pDSocket		(NULL)
, m_pRequestHistory	(new CHistory ())
, m_pConnectionRequest	(NULL)
, m_pReceiver		(NULL)
, m_pSender		(NULL)
, m_pTail		(NULL)
, m_pReplyCursor	(m_iReplyBuffer)
, m_pReplyLimit		(m_iReplyBuffer)
{
// ... create and start a locator for the requested service, ...
    TRY {
	CLocatorAgent* pLocator = new CLocatorAgent (this);
	pLocator->SetHost (m_iController.m_hostname);
	pLocator->SetPort ("exec", "tcp");
	pLocator->Release ();
    } CATCH (CMemoryException, pMemoryException) {
    } END_CATCH

//  ... create and submit the connection request, ...
    CString connectString;
    m_iController.GetConnectString (connectString);
    m_pConnectionRequest = Submit (connectString, 1);

//  ... and add this instance to the class instance list:
    Instances.AddItem (this);
}


/////////////////////////////////////////////////////////////////////////////
//  Destruction
/////////////////////////////////////////////////////////////////////////////

CConnection::~CConnection()
{
//  Cease operations, ...
    Stop ();

//  ... destroy the connection's locator and sockets if they exist, ...
    if (m_pDSocket)
	m_pDSocket->Release ();

//  ... release all remembered requests, ...
    if (m_pConnectionRequest)
	m_pConnectionRequest->Release ();
    m_pRequestHistory->Release ();

//  ... and remove this instance from the class instance list.
    Instances.DeleteItem (this);
}


/////////////////////////////////////////////////////////////////////////////
//  Diagnostics
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

void CConnection::Dump (CDumpContext& dc) const
{
//  CString description;
//  m_iController.GetDescription (description);
    CNetworkClient::Dump (dc);
//  dc << "(" << description << ")";
}
#endif


/////////////////////////////////////////////////////////////////////////////
//  Lookup
/////////////////////////////////////////////////////////////////////////////

CConnection* CConnection::GetConnection(
    LPCSTR hostname, LPCSTR username, LPCSTR password, LPCSTR database, short objectSpace
)
{
    CConnectionData pattern(hostname, username, password, database, objectSpace);
    return GetConnection (pattern);
}

CConnection* CConnection::GetConnection(CConnectionData& pattern)
{
    CConnection* pConnection;
#ifdef _DEBUG
    CString description;
    pattern.GetDescription (description);
#endif

    for (
	pConnection = NewestInstance ();
	pConnection;
	pConnection = pConnection->NextNewestInstance ()
    )
    {
	if (!pConnection->Stopped() && pConnection->m_iController == pattern)
	{
#ifdef _DEBUG
	    afxDump << "Get Connection: Found Match For " << description << "\n";
#endif
	    pConnection->Retain ();
	    return pConnection;
	}
    }
#ifdef _DEBUG
    afxDump << "GetConnection: Creating " << description << "\n";
#endif
    return new CConnection(pattern);
}


/////////////////////////////////////////////////////////////////////////////
//  Operations
/////////////////////////////////////////////////////////////////////////////

////////////////
//  Stop
////////////////

void CConnection::Stop ()
{
#ifdef _DEBUG
    afxDump << this << ">>> Stop Called!!!\n";
#endif

//  Disconnect from the forwarding service, ...
    if (m_pForwardingService)
	m_pForwardingService->Detach  (this);

//  Abort all pending requests, ...
    m_pSender = NULL;
    while (m_pReceiver)
    {
	m_pReceiver->Fail ();
	NextReceiver ();
    }

//  ... and invoke the superclass' stop method.
    CNetworkClient::Stop ();	
}


////////////////
//  Submit
////////////////

CRequest* CConnection::Submit (CString& rRequestText, unsigned int nAcksExpected)
{
    return Submit (new CTextBuffer (rRequestText), nAcksExpected);
}

CRequest* CConnection::Submit (LPCSTR pRequestText, unsigned int sRequestText)
{
    return Submit (new CTextBuffer (pRequestText, sRequestText));
}

CRequest* CConnection::Submit (LPCSTR pRequestText)
{
    return Submit (new CTextBuffer (pRequestText));
}

CRequest* CConnection::Submit (
    CTextBuffer* pRequestText, unsigned int nAcksExpected, CForwardingAgent* pSubmittingAgent
)
{
    CRequest *pRequest;

// Create a new request, ...
    TRY {
	pRequest = new CRequest (this, pRequestText, nAcksExpected, pSubmittingAgent);
    } CATCH (CMemoryException, pMemoryException) {
	pRequestText->Release ();
	return NULL;
    } END_CATCH;

// ... append it to request history, ...
    m_pRequestHistory->Append (pRequest);

// ... and fail immediately if the connection was stopped:
    if (m_fStopped)
    {
	pRequest->Fail ();
	return pRequest;
    }

// Otherwise, add it to the request queue, ...
    pRequest->Retain ();

    if (m_pTail)
	m_pTail->SetQueueSuccessor (pRequest);
    m_pTail = pRequest;

    if (NULL == m_pReceiver)
    {
	m_pReceiver = pRequest;
	EnableTimeouts ();
    }
    if (NULL == m_pSender)
	SetSender (pRequest);

#ifdef _DEBUG
    afxDump
	<< this
	<< "Submit " << (LPVOID)pRequest
	<< ", Receiver = " << (LPVOID)m_pReceiver
	<< ", Sender = " << (LPVOID)m_pSender
	<< ", Tail = " << (LPVOID)m_pTail
	<< "\n";
    CRequest* pCursor;
    for (pCursor = m_pReceiver; pCursor; pCursor = pCursor->QueueSuccessor()) afxDump
	<< "     "
	<< (LPVOID)pCursor
	<< (pRequest	== pCursor ? "/N" : "")
	<< (m_pReceiver	== pCursor ? "/R" : "")
	<< (m_pSender	== pCursor ? "/S" : "")
	<< (m_pTail	== pCursor ? "/T" : "")
	<< "\n";
#endif

// ... drain the queue, ...
    if (m_pDSocket)
    {
	SendRequests ();
	ReceiveReplies ();
    }

// ... and return the request object.
    return pRequest;
}


////////////////
//  Send
////////////////

void CConnection::SendRequests ()
{
    while (m_pSender && IsWriteable ())
    {
	m_pDSocket->Send (m_iOutputBuffer);
	if (m_iOutputBuffer.IsEmpty ())
	    SetSender (m_pSender->QueueSuccessor ());
    }
}

void CConnection::SetSender (CRequest *pSender)
{
    m_pSender = pSender;
    if (m_pSender)
	m_pSender->GetRequestForWire (m_iOutputBuffer.GetBufferForUpdate ());
}


////////////////
//  Receive
////////////////

void CConnection::ReceiveReplies ()
{
    while (m_pReceiver && IsReadable ())
    {
	unsigned int bytesReceived;
	if (ReplyBufferIsEmpty		() &&
	    m_pDSocket->IsReadable	() &&
	    m_pDSocket->Receive		(bytesReceived, m_iReplyBuffer, sizeof (m_iReplyBuffer) - 1)
	)
	{
	    m_iReplyBuffer[bytesReceived] = '\0';
	    m_pReplyCursor	= m_iReplyBuffer;
	    m_pReplyLimit	= m_iReplyBuffer + bytesReceived;
	}
#ifdef _DEBUG
	afxDump << this << "ReceiveReplies: Bytes In Reply Buffer = " << strlen (m_pReplyCursor) << "\n";
#endif
	while (m_pReceiver->ReplyIsntComplete () && ReplyBufferIsntEmpty ())
	{
	    LPCSTR prompt = strstr (m_pReplyCursor, m_iPrompt);
	    if (prompt)
	    {
		m_pReceiver->GetReplyFromWire (m_pReplyCursor, prompt - m_pReplyCursor, 1);
		m_pReplyCursor = prompt + m_iPrompt.GetLength ();
	    }
	    else
	    {
		size_t replyDataLength = strlen (m_pReplyCursor);
		m_pReceiver->GetReplyFromWire (m_pReplyCursor, replyDataLength);
		m_pReplyCursor += replyDataLength + 1;
	    }
	}
	if (m_pReceiver->ReplyIsComplete ())
	{
	    m_pReceiver->Succeed ();
	    NextReceiver ();
	}
    }
}

void CConnection::NextReceiver ()
{
    CRequest *pSuccessor = m_pReceiver->QueueSuccessor ();
    m_pReceiver->Release ();
    m_pReceiver = pSuccessor;

    if (NULL == m_pReceiver)
    {
	m_pTail = NULL;
	DisableTimeouts ();
    }
}


////////////////
//  On...
////////////////

void CConnection::OnAddressData (CLocatorAgent *pLocator)
{
#ifdef _DEBUG
    afxDump << this << "+++ OnAddressValid\n";
#endif

    m_pDSocket = m_pNetwork->newSocketAgent (this, pLocator->address().sin_family);
    m_pDSocket->Connect (&pLocator->address ());
}

void CConnection::OnConnect (CSocketAgent *pSocket)
{
    if (pSocket != m_pDSocket)
	return;

#ifdef _DEBUG
    afxDump << this << "+++ OnConnect\n";
#endif

    Application()->Register (this);
}

void CConnection::OnClose (CSocketAgent *pSocket)
{
    if (pSocket != m_pDSocket)
	return;

#ifdef _DEBUG
    afxDump << this << "+++ OnClose\n";
#endif

    Stop ();

    if (m_pConnectionRequest && m_pConnectionRequest->Failed ())
	m_fFailed = TRUE;
}

void CConnection::OnReadable (CSocketAgent *pSocket)
{
    if (pSocket != m_pDSocket)
	return;

#ifdef _DEBUG
    afxDump << this << "+++ OnReadable\n";
#endif

    if (m_pReceiver)
	ReceiveReplies ();
    else while (m_pDSocket->IsReadable ())
    {
	unsigned int bytesReceived;
	m_pDSocket->Receive (bytesReceived, m_iReplyBuffer, sizeof (m_iReplyBuffer) - 1);
    }
}

void CConnection::OnWriteable (CSocketAgent *pSocket)
{
    if (pSocket != m_pDSocket)
	return;

#ifdef _DEBUG
    afxDump << this << "+++ OnWriteable\n";
#endif

    SendRequests ();
}


/////////////////////////////////////////////////////////////////////////////
//  Handlers
/////////////////////////////////////////////////////////////////////////////

BSTR CConnection::OADatabase()
{
    return m_iController.m_database.AllocSysString();
}

long CConnection::OAGetTimeout()
{
    return m_iController.m_timeout;
}

void CConnection::OASetTimeout(long nNewValue)
{
    m_iController.m_timeout = nNewValue;
}

BSTR CConnection::OAExecutable()
{
    return m_iController.m_executable.AllocSysString();}

BSTR CConnection::OAHostname()
{
    return m_iController.m_hostname.AllocSysString();
}

short CConnection::OAObjectSpace()
{
    return m_iController.m_osNumber;
}

BSTR CConnection::OAOptions()
{
    return m_iController.m_options.AllocSysString();
}

BSTR CConnection::OAPassword()
{
    return m_iController.m_password.AllocSysString();
}

BSTR CConnection::OAUsername()
{
    return m_iController.m_username.AllocSysString();
}

BSTR CConnection::OACommand()
{
    CString command;
    m_iController.GetCommandString (command);
    return command.AllocSysString();
}

BSTR CConnection::OADescription()
{
    CString command;
    m_iController.GetDescription (command);
    return command.AllocSysString();
}

BSTR CConnection::OALocalAddress()
{
    CString s;
    if (m_pDSocket)
	m_pDSocket->LocalAddress (s);
    return s.AllocSysString();
}

BSTR CConnection::OARemoteAddress()
{
    CString s;
    if (m_pDSocket)
	m_pDSocket->RemoteAddress (s);
    return s.AllocSysString();
}

void CConnection::OAWaitForConnection(long timeout)
{
    DWORD loopCount = 0;
    if (IsUnstable ())
    {
	DWORD tickCount = TickCount (timeout < 0 ? Timeout () : timeout);
	do {
	    ProcessNextNetworkEvent ();
	} while (IsUnstable () && tickCount > TickCount ());
    }
#ifdef _DEBUG
    afxDump << "\n+++++\n" << this << "WaitForConnection Completed In " << loopCount << (1 != loopCount ? " Passes\n" : "Pass\n");
#endif
}

BOOL CConnection::OAIsUpWithin(long timeout)
{
    WaitForConnection (timeout);
    return IsUp ();
}

void CConnection::OAWaitForCompletion(long timeout)
{
    OAWaitForConnection (timeout);
}

BOOL CConnection::OACompletedWithin(long timeout)
{
    return OAIsUpWithin (timeout);
}

BOOL CConnection::OAIsUp ()
{
    return !m_fStopped && m_pConnectionRequest->Succeeded ();
}

BOOL CConnection::OAIsDown ()
{
    return m_fStopped || !m_pConnectionRequest->Succeeded ();
}

BOOL CConnection::OAIsStable()
{
    return m_fStopped || !m_pConnectionRequest->InProgress ();
}

BOOL CConnection::OAIsUnstable()
{
    return !m_fStopped && m_pConnectionRequest->InProgress ();
}

LPDISPATCH CConnection::OASubmit(LPCSTR request)
{
    CRequest *pRequest = Submit (request);
    return pRequest ? pRequest->GetIDispatch (FALSE) : NULL;
}

long CConnection::OARequestCount()
{
    return m_pRequestHistory->Count();
}

LPDISPATCH CConnection::OARequestHistory()
{
    return m_pRequestHistory->GetIDispatch(TRUE);
}

BOOL CConnection::OAStoreTemplate(LPCSTR storedTemplateName)
{
    return m_iController.Store (storedTemplateName);
}

LPDISPATCH CConnection::OACreateAgent()
{
    CAgent* pAgent = new CAgent (this);
    return pAgent ? pAgent->GetIDispatch(FALSE) : NULL;
}
