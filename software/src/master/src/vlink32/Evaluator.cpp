//////////////////////////////////////////////////////////////////////////////
//
//  CEvaluator Implementation
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//  Interfaces
//////////////////////////////////////////////////////////////////////////////

//  System...
#include "stdafx.h"
#include "stdext.h"

//  Supporting...
#include "Agent.h"
#include "history.h"
#include "request.h"

//  Self...
#include "Evaluator.h"

/////////////////////////////////////////////////////////////////////////////
//  Globals
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//  Diagnostics
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

void CEvaluator::Dump (CDumpContext& dc) const
{
    CVLinkCmdTarget::Dump (dc);
}
#endif


/////////////////////////////////////////////////////////////////////////////
//  Message Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CEvaluator, CVLinkCmdTarget)
	//{{AFX_MSG_MAP(CEvaluator)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//  Dispatch Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_DISPATCH_MAP(CEvaluator, CVLinkCmdTarget)
	//{{AFX_DISPATCH_MAP(CEvaluator)
	DISP_PROPERTY(CEvaluator, "alwaysEvaluate", m_alwaysEvaluate, VT_BOOL)
	DISP_PROPERTY_EX(CEvaluator, "RequestText", OAGetRequestText, OASetRequestText, VT_BSTR)
	DISP_PROPERTY_EX(CEvaluator, "UnixRequestText", OAGetUnixRequestText, OASetUnixRequestText, VT_BSTR)
	DISP_FUNCTION(CEvaluator, "WaitForReply", OAWaitForReply, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CEvaluator, "CompletedWithin", OACompletedWithin, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CEvaluator, "Completed", OACompleted, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CEvaluator, "InProgress", OAInProgress, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CEvaluator, "Succeeded", OASucceeded, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CEvaluator, "Failed", OAFailed, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CEvaluator, "ReplyBuffer", OAReplyBuffer, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CEvaluator, "ReplyCursor", OAReplyCursor, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CEvaluator, "ReplyText", OAReplyText, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CEvaluator, "UnixReplyText", OAUnixReplyText, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CEvaluator, "RequestBuffer", OARequestBuffer, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CEvaluator, "RequestCursor", OARequestCursor, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CEvaluator, "Request", OARequest, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CEvaluator, "RequestHistory", OARequestHistory, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CEvaluator, "RequestCount", OARequestCount, VT_I4, VTS_NONE)
	DISP_FUNCTION(CEvaluator, "Agent", OAAgent, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CEvaluator, "Generation", OAGeneration, VT_I4, VTS_NONE)
	DISP_FUNCTION(CEvaluator, "ReplyTruncated", OAReplyTruncated, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CEvaluator, "RequestTruncated", OARequestTruncated, VT_BOOL, VTS_NONE)
	DISP_DEFVALUE(CEvaluator, "ReplyText")
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
//  Construction
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CEvaluator, CVLinkCmdTarget)

CEvaluator::CEvaluator (CAgent* pAgent, long initialHistoryLimit)
: m_pAgent(pAgent)
, m_pHistory(new CHistory(initialHistoryLimit))
{
    pAgent->Retain ();

    m_pRequest		= NULL;
    m_alwaysEvaluate	= FALSE;

//  Add this instance to the class instance list, ...
//	Instances.AddItem (this);
}

/////////////////////////////////////////////////////////////////////////////
//  Destruction
/////////////////////////////////////////////////////////////////////////////

CEvaluator::~CEvaluator()
{
//  Release component objects, ...
    m_pAgent->Release();
    m_pHistory->Release ();
    if (m_pRequest)
	m_pRequest->Release ();

//  ... and this object from its class instance list.
//	Instances.DeleteItem (this);
}


/////////////////////////////////////////////////////////////////////////////
//  Operations
/////////////////////////////////////////////////////////////////////////////

void CEvaluator::Evaluate (LPCSTR expression)
{
    if (NULL == m_pRequest || m_alwaysEvaluate || *m_pRequest != expression)
    {
	if (m_pRequest)
	    m_pRequest->Release ();
	m_pRequest = m_pAgent->Submit (expression);
	m_pHistory->Append (m_pRequest);
    }
}


/////////////////////////////////////////////////////////////////////////////
//  Handlers
/////////////////////////////////////////////////////////////////////////////

void CEvaluator::OAWaitForReply(long timeout)
{
    if (m_pRequest)
	m_pRequest->WaitForReply (timeout);
}

BOOL CEvaluator::OACompleted()
{
    return m_pRequest && m_pRequest->Completed ();
}

BOOL CEvaluator::OACompletedWithin(long timeout)
{
    OAWaitForReply (timeout);
    return OACompleted ();
}

BOOL CEvaluator::OASucceeded()
{
    return m_pRequest && m_pRequest->Succeeded ();
}

BOOL CEvaluator::OAFailed()
{
    return m_pRequest && m_pRequest->Failed ();
}

BOOL CEvaluator::OAInProgress()
{
    return m_pRequest && m_pRequest->InProgress ();
}

LPDISPATCH CEvaluator::OAReplyBuffer ()
{
    return m_pRequest ? m_pRequest->OAReplyBuffer () : NULL;
}

LPDISPATCH CEvaluator::OAReplyCursor ()
{
    return m_pRequest ? m_pRequest->OAReplyCursor () : NULL;
}

BSTR CEvaluator::OAReplyText()
{
    CString s;
    return m_pRequest ? m_pRequest->OAReplyText () : s.AllocSysString ();
}

BOOL CEvaluator::OAReplyTruncated()
{
    return m_pRequest ? m_pRequest->OAReplyTruncated() : FALSE;
}

BSTR CEvaluator::OAUnixReplyText()
{
    CString s;
    return m_pRequest ? m_pRequest->OAUnixReplyText () : s.AllocSysString ();
}

LPDISPATCH CEvaluator::OARequestBuffer ()
{
    return m_pRequest ? m_pRequest->OARequestBuffer () : NULL;
}

LPDISPATCH CEvaluator::OARequestCursor()
{
    return m_pRequest ? m_pRequest->OARequestCursor () : NULL;
}

BSTR CEvaluator::OAGetRequestText()
{
    CString s;
    return m_pRequest ? m_pRequest->OARequestText () : s.AllocSysString();
}

void CEvaluator::OASetRequestText(LPCSTR lpszNewValue)
{
    Evaluate (lpszNewValue);
}

BSTR CEvaluator::OAGetUnixRequestText()
{
    CString s;
    return m_pRequest ? m_pRequest->OAUnixRequestText () : s.AllocSysString();
}

void CEvaluator::OASetUnixRequestText(LPCSTR lpszNewValue)
{
    Evaluate (lpszNewValue);
}

BOOL CEvaluator::OARequestTruncated()
{
    return m_pRequest ? m_pRequest->OARequestTruncated() : FALSE;
}

LPDISPATCH CEvaluator::OAAgent()
{
    return m_pAgent->GetIDispatch (TRUE);
}

long CEvaluator::OAGeneration()
{
    return OARequestCount ();
}

LPDISPATCH CEvaluator::OARequestHistory()
{
    return m_pHistory->GetIDispatch (TRUE);
}

long CEvaluator::OARequestCount()
{
    return m_pHistory->Count();
}

LPDISPATCH CEvaluator::OARequest()
{
    return m_pRequest ? m_pRequest->GetIDispatch(TRUE) : NULL;
}
