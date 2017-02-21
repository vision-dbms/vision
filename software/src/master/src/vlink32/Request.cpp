//////////////////////////////////////////////////////////////////////////////
//
//  CRequest Implementation
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//  Interfaces
//////////////////////////////////////////////////////////////////////////////

//  System...
#include "stdafx.h"
#include "stdext.h"

//  Supporting...
#include "Connection.h"
#include "ForwardingAgent.h"
#include "TextBuffer.h"
#include "TextCursor.h"

//  Self...
#include "Request.h"

/////////////////////////////////////////////////////////////////////////////
//  Globals
/////////////////////////////////////////////////////////////////////////////

const int CRequest::g_defaultTimeout = 300;

CCollectionKernel CRequest::Instances;

/////////////////////////////////////////////////////////////////////////////
//  Diagnostics
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

void CRequest::Dump (CDumpContext& dc) const
{
    CVLinkCmdTarget::Dump (dc);
}
#endif


/////////////////////////////////////////////////////////////////////////////
//  Message Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CRequest, CVLinkCmdTarget)
    //{{AFX_MSG_MAP(CRequest)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//  Dispatch Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_DISPATCH_MAP(CRequest, CVLinkCmdTarget)
    //{{AFX_DISPATCH_MAP(CRequest)
    DISP_FUNCTION(CRequest, "WaitForReply", OAWaitForReply, VT_EMPTY, VTS_I4)
    DISP_FUNCTION(CRequest, "WaitForCompletion", OAWaitForCompletion, VT_EMPTY, VTS_I4)
    DISP_FUNCTION(CRequest, "CompletedWithin", OACompletedWithin, VT_BOOL, VTS_I4)
    DISP_FUNCTION(CRequest, "Completed", OACompleted, VT_BOOL, VTS_NONE)
    DISP_FUNCTION(CRequest, "InProgress", OAInProgress, VT_BOOL, VTS_NONE)
    DISP_FUNCTION(CRequest, "Succeeded", OASucceeded, VT_BOOL, VTS_NONE)
    DISP_FUNCTION(CRequest, "Failed", OAFailed, VT_BOOL, VTS_NONE)
    DISP_FUNCTION(CRequest, "ReplyBuffer", OAReplyBuffer, VT_DISPATCH, VTS_NONE)
    DISP_FUNCTION(CRequest, "ReplyCursor", OAReplyCursor, VT_DISPATCH, VTS_NONE)
    DISP_FUNCTION(CRequest, "ReplyText", OAReplyText, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CRequest, "UnixReplyText", OAUnixReplyText, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CRequest, "RequestBuffer", OARequestBuffer, VT_DISPATCH, VTS_NONE)
    DISP_FUNCTION(CRequest, "RequestCursor", OARequestCursor, VT_DISPATCH, VTS_NONE)
    DISP_FUNCTION(CRequest, "RequestText", OARequestText, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CRequest, "UnixRequestText", OAUnixRequestText, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CRequest, "ReplyTruncated", OAReplyTruncated, VT_BOOL, VTS_NONE)
    DISP_FUNCTION(CRequest, "RequestTruncated", OARequestTruncated, VT_BOOL, VTS_NONE)
    DISP_DEFVALUE(CRequest, "RequestText")
    //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
//  Construction
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CRequest, CVLinkCmdTarget)

CRequest::CRequest (
    CConnection*	pConnection,
    CTextBuffer*	pRequestText,
    unsigned int	nAcksExpected,
    CForwardingAgent*	pSubmittingAgent
)
: m_pConnection		(pConnection)
, m_pRequestBuffer	(pRequestText)
, m_pReplyBuffer	(new CTextBuffer ())
, m_pSubmittingAgent	(pSubmittingAgent)
, m_pQueueSuccessor	(NULL)
, m_status		(StatusInProgress)
, m_unreceivedAckCount	(nAcksExpected)
{
    if (m_pSubmittingAgent)
	m_pSubmittingAgent->Retain ();

//  DumpBuffer ("Request Text", pRequest, sRequest);

//  Add this instance to the class instance list, ...
    Instances.AddItem (this);
}

/////////////////////////////////////////////////////////////////////////////
//  Destruction
/////////////////////////////////////////////////////////////////////////////

CRequest::~CRequest()
{
//  Release component and referenced objects, ...
    if (m_pSubmittingAgent)
	m_pSubmittingAgent->Release ();
    if (m_pReplyBuffer)
	m_pReplyBuffer->Release ();
    if (m_pRequestBuffer)
	m_pRequestBuffer->Release ();

//  ... and remove it from the class instance list.
    Instances.DeleteItem (this);
}


/////////////////////////////////////////////////////////////////////////////
//  Operations
/////////////////////////////////////////////////////////////////////////////

BOOL CRequest::operator== (const CString operand) const
{
    return *m_pRequestBuffer == operand;
}

BOOL CRequest::operator== (LPCSTR operand) const
{
    return *m_pRequestBuffer == operand;
}

BOOL CRequest::operator!= (const CString operand) const
{
    return *m_pRequestBuffer != operand;
}

BOOL CRequest::operator!= (LPCSTR operand) const
{
    return *m_pRequestBuffer != operand;
}

long CRequest::Timeout () const
{
    return m_pConnection && m_pConnection->Timeout() >= 0
	? m_pConnection->Timeout() : g_defaultTimeout;
}

void CRequest::GetSynopsis (CString& synopsis) const
{
    m_pRequestBuffer->GetSynopsis (synopsis);
}

void CRequest::GetRequestForWire (CString& rRequestText)
{
    m_pRequestBuffer->UnixText (rRequestText);

    if (m_unreceivedAckCount > 0)
	return;	// ... only true for the 'rexec' connection request.

    if (rRequestText.GetLength () > 0 && rRequestText[rRequestText.GetLength () - 1] != '\n')
	    rRequestText += '\n';

    LPCSTR requestCursor = rRequestText;
    while (requestCursor = strchr (requestCursor, '\n'))
    {
	m_unreceivedAckCount++;
	requestCursor++;
    }
    m_unreceivedAckCount++;

    rRequestText += CConnection::g_launchDirective;
}

void CRequest::GetReplyForWire (CString& rReplyText)
{
    m_pReplyBuffer->UnixText (rReplyText);
}

void CRequest::GetReplyFromWire (LPCSTR replyText, size_t replyLength, unsigned long ackCount)
{
#ifdef _DEBUG
    char data[40], *suffix;
    if (replyLength > sizeof (data) - 1)
    {
	strncpy (data, replyText, sizeof(data) - 1);
	data[sizeof(data) - 1] = '\0';
	suffix = "...";
    }
    else
    {
	strncpy (data, replyText, replyLength);
	data[replyLength] = '\0';
	suffix = "";
    }
    afxDump
    << this
    << " GetReplyFromWire: Reply Length = "
    << replyLength
    << ", Unreceived Ack Count = "
    << m_unreceivedAckCount
    << ", Ack Count = "
    << ackCount
    << ", Data = <"
#if 0
    << data
    << suffix
#else
    << replyText
#endif
    << ">\n";
    DumpBuffer ("+++ Reply Content: ", data, strlen(data));
#endif
    m_unreceivedAckCount = m_unreceivedAckCount > ackCount ? m_unreceivedAckCount - ackCount : 0;
    m_pReplyBuffer->AppendLines (replyText, replyLength, m_unreceivedAckCount > 0);
}

void CRequest::Succeed ()
{
    m_pReplyBuffer->Trim (TRUE);

    m_status = StatusSucceeded;
    ReleaseConnection ();
    NotifyAgent ();

#ifdef _DEBUG
    afxDump << this << " Completed Successfully !!!\n";
#endif
}

void CRequest::Fail ()
{
    m_status = StatusFailed;
    ReleaseConnection ();
    NotifyAgent ();

#ifdef _DEBUG
    afxDump << this << " Failed !!!\n";
#endif
}

void CRequest::NotifyAgent ()
{
    if (m_pSubmittingAgent)
    {
	CForwardingAgent* pAgent = m_pSubmittingAgent;
	m_pSubmittingAgent = NULL;

	pAgent->OnReply (this);

	pAgent->Release ();
    }
}

void CRequest::ReleaseConnection ()
{
    m_pConnection = NULL;
}


/////////////////////////////////////////////////////////////////////////////
//  Handlers
/////////////////////////////////////////////////////////////////////////////

void CRequest::OAWaitForReply(long timeout)
{
    DWORD loopCount = 0;
    if (InProgress ())
    {
	DWORD tickCount = m_pConnection->TickCount (timeout < 0 ? Timeout () : timeout);
	do {
	    m_pConnection->ProcessNextNetworkEvent ();
	} while (InProgress () && tickCount > m_pConnection->TickCount ());
    }

#ifdef _DEBUG
    afxDump << "\n+++++\n" << this << "WaitForReply Completed In " << loopCount << (1 != loopCount ? " Passes\n" : "Pass\n");
#endif
}

void CRequest::OAWaitForCompletion(long timeout)
{
    OAWaitForReply (timeout);
}

BOOL CRequest::OACompletedWithin(long timeout)
{
    WaitForReply (timeout);
    return Completed ();
}

BOOL CRequest::OACompleted()
{
    return Completed ();
}

BOOL CRequest::OASucceeded()
{
    return Succeeded ();
}

BOOL CRequest::OAFailed()
{
    return Failed ();
}

BOOL CRequest::OAInProgress()
{
    return InProgress ();
}

LPDISPATCH CRequest::OAReplyBuffer ()
{
    return m_pReplyBuffer->GetIDispatch (TRUE);
}

LPDISPATCH CRequest::OAReplyCursor ()
{
    return m_pReplyBuffer->OACursor ();
}

BSTR CRequest::OAReplyText()
{
    return m_pReplyBuffer->OAText ();
}

BSTR CRequest::OAUnixReplyText()
{
    return m_pReplyBuffer->OAUnixText ();
}

LPDISPATCH CRequest::OARequestBuffer ()
{
    return m_pRequestBuffer->GetIDispatch (TRUE);
}

LPDISPATCH CRequest::OARequestCursor()
{
    return m_pRequestBuffer->OACursor ();
}

BSTR CRequest::OARequestText()
{
    return m_pRequestBuffer->OAText ();
}

BSTR CRequest::OAUnixRequestText()
{
    return m_pRequestBuffer->OAUnixText ();
}

BOOL CRequest::OAReplyTruncated()
{
    return FALSE;
}

BOOL CRequest::OARequestTruncated()
{
    return FALSE;
}
