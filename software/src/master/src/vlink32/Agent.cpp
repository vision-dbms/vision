//////////////////////////////////////////////////////////////////////////////
//
//  CAgent Implementation
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
#include "ConnectionTemplate.h"
#include "Evaluator.h"
#include "history.h"
#include "request.h"
#include "vlinkapp.h"

//  Self...
#include "Agent.h"

/////////////////////////////////////////////////////////////////////////////
//  Globals
/////////////////////////////////////////////////////////////////////////////

CCollectionKernel CAgent::Instances;


/////////////////////////////////////////////////////////////////////////////
//  Diagnostics
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
//  Message Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CAgent, CVLinkCmdTarget)
	//{{AFX_MSG_MAP(CAgent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//  Dispatch Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_DISPATCH_MAP(CAgent, CVLinkCmdTarget)
	//{{AFX_DISPATCH_MAP(CAgent)
	DISP_FUNCTION(CAgent, "CreateConnectionTemplate", OACreateConnectionTemplate, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CAgent, "CreateConnection", OACreateConnection, VT_DISPATCH, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_I2)
	DISP_FUNCTION(CAgent, "CreateDefaultConnection", OACreateDefaultConnection, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CAgent, "CreateStoredConnection", OACreateStoredConnection, VT_DISPATCH, VTS_BSTR)
	DISP_FUNCTION(CAgent, "GetConnection", OAGetConnection, VT_DISPATCH, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_I2)
	DISP_FUNCTION(CAgent, "GetDefaultConnection", OAGetDefaultConnection, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CAgent, "GetStoredConnection", OAGetStoredConnection, VT_DISPATCH, VTS_BSTR)
	DISP_FUNCTION(CAgent, "IsUpWithin", OAIsUpWithin, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CAgent, "IsUp", OAIsUp, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CAgent, "IsDown", OAIsDown, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CAgent, "Submit", OASubmit, VT_DISPATCH, VTS_BSTR)
	DISP_FUNCTION(CAgent, "Connection", OAConnection, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CAgent, "DefaultConnectTimeout", OADefaultConnectTimeout, VT_I4, VTS_NONE)
	DISP_FUNCTION(CAgent, "DefaultRequestTimeout", OADefaultRequestTimeout, VT_I4, VTS_NONE)
	DISP_FUNCTION(CAgent, "RequestCount", OARequestCount, VT_I4, VTS_NONE)
	DISP_FUNCTION(CAgent, "RequestHistory", OARequestHistory, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CAgent, "WaitForConnection", WaitForConnection, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CAgent, "Started", OAStarted, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CAgent, "Stopped", OAStopped, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CAgent, "CreateEvaluator", OACreateEvaluator, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CAgent, "GetEvaluator", OAGetEvaluator, VT_DISPATCH, VTS_BSTR)
	DISP_FUNCTION(CAgent, "GetNamedObject", OAGetNamedObject, VT_DISPATCH, VTS_BSTR)
	DISP_FUNCTION(CAgent, "CompletedWithin", OACompletedWithin, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CAgent, "Failed", OAFailed, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CAgent, "QuoteString", OAQuoteString, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION(CAgent, "QuoteSymbol", OAQuoteSymbol, VT_BSTR, VTS_BSTR)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

/////////////////////////////////////////////////////////////////////////////
//  OLE UUID Data
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_OLECREATE(CAgent, "VLink.Agent", 0x36b2702, 0x8a92, 0x101b, 0xb9, 0xa0, 0x2, 0x60, 0x8c, 0xac, 0xd3, 0x7a)


/////////////////////////////////////////////////////////////////////////////
//  Construction
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CAgent, CVLinkCmdTarget)

CAgent::CAgent(CConnection* pConnection)
: m_pConnection		(pConnection)
, m_pRequestHistory	(new CHistory ())
{
//  Initialize the agent, ...
    if (m_pConnection)
	m_pConnection->Retain ();

//  ... and add it to its class instance list:
    Instances.AddItem (this);
}

/////////////////////////////////////////////////////////////////////////////
//  Destruction
/////////////////////////////////////////////////////////////////////////////

CAgent::~CAgent()
{
#ifdef _DEBUG
    afxDump << this << "~CAgent\n";
#endif

//  Release component objects, ...
    if (m_pConnection)
	m_pConnection->Release ();
    m_pRequestHistory->Release ();

    CString	key;
    CObject*	value;
    POSITION	pos = m_nameMap.GetStartPosition ();
    while (pos)
    {
	m_nameMap.GetNextAssoc (pos, key, value);
	((CVLinkCmdTarget*)value)->Release ();
    }

//  ... and remove this instance from the class instance list...
    Instances.DeleteItem (this);
}


/////////////////////////////////////////////////////////////////////////////
//  Operations
/////////////////////////////////////////////////////////////////////////////

CConnection* CAgent::CreateConnection (LPCSTR hostname, LPCSTR username, LPCSTR password, LPCSTR database, short objectSpace)
{
    CConnectionData iConnectionData (hostname, username, password, database, objectSpace);
    return SetConnection (new CConnection (iConnectionData));
}

CConnection* CAgent::CreateConnection (LPCSTR storedTemplateName)
{
    CConnectionData connectionData;
    return SetConnection (
	connectionData.Load (storedTemplateName) ? new CConnection (connectionData) : NULL
    );
}

CConnection* CAgent::CreateConnection (CConnectionData& connectionData)
{
    return SetConnection (new CConnection (connectionData));
}

CConnection* CAgent::GetConnection (LPCSTR hostname, LPCSTR username, LPCSTR password, LPCSTR database, short objectSpace)
{
    return SetConnection (CConnection::GetConnection (hostname, username, password, database, objectSpace));
}

CConnection* CAgent::GetConnection (LPCSTR storedTemplateName)
{
    CConnectionData connectionData;
    return SetConnection (
	connectionData.Load (storedTemplateName) ? CConnection::GetConnection (connectionData) : NULL
    );
}

CConnection* CAgent::GetConnection (CConnectionData& connectionData)
{
    return SetConnection (CConnection::GetConnection (connectionData));
}

CConnection* CAgent::SetConnection (CConnection* pConnection)
{
    if (m_pConnection)
	m_pConnection->Release ();
    m_pConnection = pConnection;
    return m_pConnection;
}

CRequest* CAgent::Submit (LPCSTR request, int requestLength)
{
    if (NULL == m_pConnection)
	return NULL;

    CRequest* pRequest = m_pConnection->Submit (request, requestLength);
    m_pRequestHistory->Append (pRequest);
    return pRequest;
}


/////////////////////////////////////////////////////////////////////////////
//  Handlers
/////////////////////////////////////////////////////////////////////////////

LPDISPATCH CAgent::OACreateConnectionTemplate()
{
    CConnectionTemplate *pConnectionTemplate = new CConnectionTemplate (this);
    return pConnectionTemplate ? pConnectionTemplate->GetIDispatch (FALSE) : NULL;
}

LPDISPATCH CAgent::OACreateConnection(LPCSTR hostname, LPCSTR username, LPCSTR password, LPCSTR database, short objectSpace)
{
    CreateConnection (hostname, username, password, database, objectSpace);
    return OAConnection ();
}

LPDISPATCH CAgent::OACreateDefaultConnection()
{
    return OACreateStoredConnection (NULL);
}

LPDISPATCH CAgent::OACreateStoredConnection(LPCSTR storedTemplateName)
{
    CreateConnection (storedTemplateName);
    return OAConnection ();
}

LPDISPATCH CAgent::OAGetConnection(LPCSTR hostname, LPCSTR username, LPCSTR password, LPCSTR database, short objectSpace)
{
    GetConnection (hostname, username, password, database, objectSpace);
    return OAConnection ();
}

LPDISPATCH CAgent::OAGetDefaultConnection()
{
    return OAGetStoredConnection (NULL);
}

LPDISPATCH CAgent::OAGetStoredConnection(LPCSTR storedTemplateName)
{
    GetConnection (storedTemplateName);
    return OAConnection ();
}

void CAgent::WaitForConnection(long timeout)
{
    if (m_pConnection)
	m_pConnection->WaitForConnection (timeout);
}

BOOL CAgent::OAIsUpWithin(long timeout)
{
    return m_pConnection && m_pConnection->IsUpWithin(timeout);
}

BOOL CAgent::OAIsUp()
{
    return m_pConnection && m_pConnection->IsUp ();
}

BOOL CAgent::OAIsDown()
{
    return NULL == m_pConnection || m_pConnection->IsDown ();
}

LPDISPATCH CAgent::OASubmit(LPCSTR request)
{
    CRequest *pRequest = Submit (request);
    return pRequest ? pRequest->GetIDispatch (FALSE) : NULL;
}

LPDISPATCH CAgent::OAConnection()
{
    return m_pConnection ? m_pConnection->GetIDispatch (TRUE) : NULL;
}

long CAgent::OADefaultConnectTimeout()
{
    return CConnection::g_defaultTimeout;
}

long CAgent::OADefaultRequestTimeout()
{
    return CRequest::g_defaultTimeout;
}

long CAgent::OARequestCount()
{
    return m_pRequestHistory->Count();
}

LPDISPATCH CAgent::OARequestHistory()
{
    return m_pRequestHistory->GetIDispatch(TRUE);
}

BOOL CAgent::OAStarted()
{
    return m_pConnection && m_pConnection->Started();
}

BOOL CAgent::OAStopped()
{
    return m_pConnection && m_pConnection->Stopped();
}

BOOL CAgent::OAFailed()
{
    return OAStopped ();
}

LPDISPATCH CAgent::OACreateEvaluator()
{
    CEvaluator* pEvaluator = new CEvaluator (this);
    return pEvaluator->GetIDispatch (FALSE);
}

LPDISPATCH CAgent::OAGetEvaluator(LPCSTR name)
{
    union {
	CObject*	pObject;
	CEvaluator*	pEvaluator;
    };
    if (!m_nameMap.Lookup(name, pObject))
	pEvaluator = NULL;
    else if (!pObject->IsKindOf (RUNTIME_CLASS(CEvaluator)))
    {
	m_nameMap.RemoveKey (name);
	((CVLinkCmdTarget*)pObject)->Release ();
	pEvaluator = NULL;
    }
    if (NULL == pEvaluator)
    {
	pEvaluator = new CEvaluator (this);
	m_nameMap[name] = pEvaluator;
    }
    return pEvaluator->GetIDispatch (TRUE);
}

LPDISPATCH CAgent::OAGetNamedObject(LPCSTR name)
{
    CObject* pObject;
    return m_nameMap.Lookup(name, pObject) ? ((CVLinkCmdTarget*)pObject)->GetIDispatch(TRUE) : NULL;
}

BOOL CAgent::OACompletedWithin(long timeout)
{
    return OAIsUpWithin (timeout);
}

BSTR CAgent::OAQuoteString(LPCSTR string)
{
    return Application()->OAQuoteString (string);
}

BSTR CAgent::OAQuoteSymbol(LPCSTR string)
{
    return Application()->OAQuoteSymbol (string);
}
