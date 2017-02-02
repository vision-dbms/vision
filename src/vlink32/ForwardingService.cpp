/////////////////////////////////////////////////////////////////////////////
//
//  CForwardingService Implementation
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//  Interfaces
/////////////////////////////////////////////////////////////////////////////

//  System
#include "stdafx.h"

//  Supporting
#include "Connection.h"
#include "ForwardingAgent.h"
#include "SocketAgent.h"
#include "vlinkapp.h"

//  Self
#include "ForwardingService.h"


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

BEGIN_MESSAGE_MAP(CForwardingService, CNetworkClient)
    //{{AFX_MSG_MAP(CForwardingService)
	    // NOTE - the ClassWizard will add and remove mapping macros here.
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
//  Dispatch Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_DISPATCH_MAP(CForwardingService, CNetworkClient)
    //{{AFX_DISPATCH_MAP(CForwardingService)
	    // NOTE - the ClassWizard will add and remove mapping macros here.
    //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IForwardingService to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {C9C33B45-0459-11D2-BE30-00A0C931D9D4}
static const IID IID_IForwardingService =
{ 0xc9c33b45, 0x459, 0x11d2, { 0xbe, 0x30, 0x0, 0xa0, 0xc9, 0x31, 0xd9, 0xd4 } };

BEGIN_INTERFACE_MAP(CForwardingService, CNetworkClient)
    INTERFACE_PART(CForwardingService, IID_IForwardingService, Dispatch)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
//  Construction, Destruction, and Initialization
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CForwardingService, CNetworkClient)

CForwardingService::CForwardingService(unsigned short iPortNumber)
: CNetworkClient (), m_iPortNumber (iPortNumber), m_pSocket (0)
{
    m_pSocket = m_pNetwork->newSocketAgent (this);
    m_pSocket->Bind	(m_iPortNumber);
    m_pSocket->Listen	();
}

CForwardingService::~CForwardingService()
{
    Application()->Detach (this);

    if (m_pSocket)
	m_pSocket->Release ();
}


/////////////////////////////////////////////////////////////////////////////
//  Access
/////////////////////////////////////////////////////////////////////////////

CConnection* CForwardingService::Get (LPCSTR pConnectionId)
{
    union {
	CObject*	pOldObject;
	CConnection*	pOldConnection;
    };
    return m_iConnectionRegistry.Lookup (pConnectionId, pOldObject)
	? pOldConnection : NULL;
}


/////////////////////////////////////////////////////////////////////////////
//  Update
/////////////////////////////////////////////////////////////////////////////

void CForwardingService::Attach (CConnection* pConnection)
{
    union {
	CObject*	pOldObject;
	CConnection*	pOldConnection;
    };
    if (!m_iConnectionRegistry.Lookup (pConnection->ForwardingId(), pOldObject))
    {
	Retain ();
	pConnection->m_pForwardingService = this;
	m_iConnectionRegistry[pConnection->ForwardingId()] = pConnection;
    }
}

void CForwardingService::Detach (CConnection* pConnection)
{
    union {
	CObject*	pOldObject;
	CConnection*	pOldConnection;
    };
    if (m_iConnectionRegistry.Lookup (
	    pConnection->ForwardingId(), pOldObject
	) && pConnection == pOldConnection
    )
    {
	m_iConnectionRegistry.RemoveKey (pConnection->ForwardingId ());
	pConnection->m_pForwardingService = NULL;
	Release ();
    }
}


/////////////////////////////////////////////////////////////////////////////
//  Event Handlers
/////////////////////////////////////////////////////////////////////////////

void CForwardingService::OnAccept (CSocketAgent* pSocket)
{
    if (pSocket != m_pSocket)
	return;

#ifdef _DEBUG
    afxDump << this << "+++ OnAccept\n";
#endif

    SOCKET xSocket = m_pSocket->Accept ();
    if (INVALID_SOCKET != xSocket)
    {
	new CForwardingAgent (this, xSocket);
    }
}


/////////////////////////////////////////////////////////////////////////////
// CForwardingService message handlers
