/////////////////////////////////////////////////////////////////////////////
//
//  CForwardingAgent Implementation
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//  Interfaces
/////////////////////////////////////////////////////////////////////////////

//  System
#include "stdafx.h"

//  Supporting
#include "Connection.h"
#include "SocketAgent.h"
#include "ForwardingService.h"
#include "Request.h"
#include "TextBuffer.h"
#include "vlinkapp.h"

//  Self
#include "ForwardingAgent.h"


/////////////////////////////////////////////////////////////////////////////
//  Diagnostics
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
//  Message Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CForwardingAgent, CVLinkCmdTarget)
    //{{AFX_MSG_MAP(CForwardingAgent)
	    // NOTE - the ClassWizard will add and remove mapping macros here.
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
//  Dispatch Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_DISPATCH_MAP(CForwardingAgent, CVLinkCmdTarget)
    //{{AFX_DISPATCH_MAP(CForwardingAgent)
	    // NOTE - the ClassWizard will add and remove mapping macros here.
    //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IForwardingAgent to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {CD03FA5B-0477-11D2-BE30-00A0C931D9D4}
static const IID IID_IForwardingAgent =
{ 0xcd03fa5b, 0x477, 0x11d2, { 0xbe, 0x30, 0x0, 0xa0, 0xc9, 0x31, 0xd9, 0xd4 } };

BEGIN_INTERFACE_MAP(CForwardingAgent, CVLinkCmdTarget)
    INTERFACE_PART(CForwardingAgent, IID_IForwardingAgent, Dispatch)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
//  Construction, Destruction, and Initialization
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CForwardingAgent, CVLinkCmdTarget)

CForwardingAgent::CForwardingAgent(CForwardingService* pService, SOCKET xSocket)
: CNetworkClient	()
, m_pService		(pService)
, m_pSocket		(NULL)
, m_pConnection		(NULL)
, m_pLastRequest	(NULL)
, m_pInputBuffer	(NULL)
, m_sInputAcquired	(0)
{
    pService->Retain ();

    m_pSocket = m_pNetwork->newSocketAgent (this, xSocket);

    SetStateToExpectingCommand ();
}

CForwardingAgent::~CForwardingAgent()
{
    if (m_pInputBuffer)
	m_pInputBuffer->Release ();
    if (m_pLastRequest)
	m_pLastRequest->Release ();
    if (m_pConnection)
	m_pConnection->Release ();
    if (m_pSocket)
	m_pSocket->Release ();

    m_pService->Release ();
}


/////////////////////////////////////////////////////////////////////////////
//  State Machine Execution
/////////////////////////////////////////////////////////////////////////////

void CForwardingAgent::CrankStateMachine ()
{
    if (m_sInputRequired > 0)
	return;

    unsigned int iSize;
    switch (m_xInputState)
    {
    case ExpectingCommand:
	switch (m_iInputBuffer[0])
	{
	case 'c':
	    SetStateToExpectingConnectionNameSize ();
	    break;
	case 'd':
	    ProcessDisconnect ();
	    break;
	case '<':
	    SetStateToExpectingInputChunkSize ();
	    break;
	case 'r':
	    ProcessRequest ();
	    break;
	default:
	    SetStateToExpectingCommand ();
	    break;
	}
	break;
    case ExpectingConnectionNameSize:
	if (GetSize (iSize, sizeof (m_iInputBuffer) - 1))
	    SetStateToExpectingConnectionName (iSize);
	else
	    SetStateToExpectingResync ();
	break;
    case ExpectingConnectionName:
	ProcessConnect ();
	break;
    case ExpectingInputChunkSize:
	if (GetSize (iSize))
	    SetStateToExpectingInputChunk (iSize);
	else
	    SetStateToExpectingResync ();
	break;
    case ExpectingInputChunk:
	ProcessInputChunk ();
	break;
    case ExpectingResync:
	if (0xffffffff == ntohl (*(u_long*)m_iInputBuffer))
	    SetStateToExpectingCommand ();
	else
	    SetStateToExpectingResync ();
	break;
    default:
	SetStateToExpectingResync ();
	break;
    }

    m_sInputAcquired = 0;
}


/////////////////////////////////////////////////////////////////////////////
//  State Machine Actions
/////////////////////////////////////////////////////////////////////////////

void CForwardingAgent::ProcessConnect ()
{
    m_iInputBuffer[m_sInputAcquired] = '\0';

    CConnection* pConnection = Application()->LookupConnection (m_iInputBuffer);
    if (pConnection)
    {
	pConnection->Retain ();
	if (m_pConnection)
	    m_pConnection->Release ();
	m_pConnection = pConnection;
	SendAck ("c");
    }
    else SendNack ("c");

    SetStateToExpectingCommand ();
}

void CForwardingAgent::ProcessDisconnect ()
{
    if (m_pConnection)
    {
	m_pConnection->Release ();
	m_pConnection = NULL;
	SendAck ("d");
    }
    else SendNack ("d");

    SetStateToExpectingCommand ();
}

void CForwardingAgent::ProcessInputChunk ()
{
    if (m_pInputBuffer)
	m_pInputBuffer->AppendLines (m_iInputBuffer, m_sInputAcquired, m_sInputDeferred > 0);
    else	
	m_pInputBuffer = new CTextBuffer (m_iInputBuffer, m_sInputAcquired, m_sInputDeferred > 0);

    if (m_sInputDeferred > 0)
	SetStateToExpectingInputChunk (m_sInputDeferred);
    else
    {
	SendAck ("<");
	SetStateToExpectingCommand ();
    }
}

void CForwardingAgent::ProcessRequest ()
{
    if (!m_pConnection)
	SendNack ("Connection");
    else if (!m_pInputBuffer)
	SendNack ("Request");
    else
    {
	m_pConnection->Submit (m_pInputBuffer, 0, this);
	m_pInputBuffer = NULL;
    }

    SetStateToExpectingCommand ();
}


BOOL CForwardingAgent::GetSize (unsigned int &rSize, unsigned int iMaxSize)
{
    rSize = ntohl (*(u_long*)m_iInputBuffer);
    return rSize <= iMaxSize;
}


void CForwardingAgent::SetStateToExpectingCommand ()
{
    m_sInputRequired = 1;
    m_xInputState = ExpectingCommand;
}

void CForwardingAgent::SetStateToExpectingConnectionNameSize ()
{
    m_sInputRequired = sizeof (u_long);
    m_xInputState = ExpectingConnectionNameSize;
}

void CForwardingAgent::SetStateToExpectingConnectionName (unsigned int sInputRequired)
{
    m_sInputRequired = sInputRequired;
    m_xInputState = ExpectingConnectionName;
}

void CForwardingAgent::SetStateToExpectingInputChunkSize ()
{
    m_sInputRequired = sizeof (u_long);
    m_xInputState = ExpectingInputChunkSize;
}

void CForwardingAgent::SetStateToExpectingInputChunk (unsigned int sInputRequired)
{
    m_sInputRequired = sInputRequired <= sizeof (m_iInputBuffer)
	? sInputRequired
	: sizeof (m_iInputBuffer);

    m_sInputDeferred = sInputRequired - m_sInputRequired;
    m_xInputState = ExpectingInputChunk;
}

void CForwardingAgent::SetStateToExpectingResync ()
{
    SendNack ("R");

    m_sInputRequired = sizeof (unsigned int);
    m_xInputState = ExpectingResync;
}


/////////////////////////////////////////////////////////////////////////////
//  Output Generation and Processing
/////////////////////////////////////////////////////////////////////////////

void CForwardingAgent::SendReply ()
{
    CString iReplyText;
    m_pLastRequest->GetReplyForWire (iReplyText);
    Send ('r', iReplyText, iReplyText.GetLength ());
}

void CForwardingAgent::Send (char iTag, LPCSTR pData, u_long sData)
{
    CString& rOutputBuffer = m_iOutputBuffer.GetBufferForUpdate ();

    u_long sOutput = sizeof (iTag) + sizeof (sData) + sData;
    char* pOutputCursor = rOutputBuffer.GetBufferSetLength (sOutput);

    memcpy (pOutputCursor, &iTag, sizeof (iTag));	pOutputCursor += sizeof (iTag);

    u_long sDataNBO = htonl (sData);
    memcpy (pOutputCursor, &sDataNBO, sizeof (u_long));	pOutputCursor += sizeof (u_long);
    memcpy (pOutputCursor, pData, sData);		pOutputCursor += sData;

    rOutputBuffer.ReleaseBuffer (sOutput);

    DrainOutput ();
}

void CForwardingAgent::DrainOutput ()
{
    while (m_pSocket->IsWriteable () && m_iOutputBuffer.IsntEmpty ())
	m_pSocket->Send (m_iOutputBuffer);
}


/////////////////////////////////////////////////////////////////////////////
//  Event Handlers
/////////////////////////////////////////////////////////////////////////////

void CForwardingAgent::OnReply (CRequest* pRequest)
{
    if (m_pLastRequest)
	m_pLastRequest->Release ();
    m_pLastRequest = pRequest;
    if (m_pLastRequest->Succeeded ())
	SendReply ();
    else SendNack ("r");
}

void CForwardingAgent::OnReadable (CSocketAgent* pSocket)
{
    if (pSocket != m_pSocket)
	return;

#ifdef _DEBUG
    afxDump << this << "+++ OnReadable\n";
#endif

    unsigned int cBytesAcquired;
    while (
	m_pSocket->IsReadable () &&
	m_pSocket->Receive (cBytesAcquired, m_iInputBuffer + m_sInputAcquired, m_sInputRequired)
    )
    {
	m_sInputAcquired += cBytesAcquired;
	m_sInputRequired -= cBytesAcquired;

	CrankStateMachine ();
    }
}

void CForwardingAgent::OnWriteable (CSocketAgent* pSocket)
{
    if (pSocket != m_pSocket)
	return;

#ifdef _DEBUG
    afxDump << this << "+++ OnWriteable\n";
#endif

    DrainOutput ();
}

void CForwardingAgent::OnClose (CSocketAgent* pSocket)
{
    if (pSocket != m_pSocket)
	return;

#ifdef _DEBUG
    afxDump << this << "+++ OnClose\n";
#endif

    Release ();
}


/////////////////////////////////////////////////////////////////////////////
// CForwardingAgent message handlers
