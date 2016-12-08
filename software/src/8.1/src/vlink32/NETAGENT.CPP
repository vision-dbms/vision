/**********************************************************
 **********************************************************
 *****  CXbyYAgent and Derived Class Implementations  *****
 **********************************************************
 **********************************************************/

/////////////////////////////////////////////////////////////////////////////
//  Interfaces
/////////////////////////////////////////////////////////////////////////////

//  System...
#include "stdafx.h"
#include "stdext.h"

//  Supporting...
#include "LocatorAgent.h"

//  Self
#include "netagent.h"


/////////////////////////////////////////////////////////////////////////////
//
//  CXbyYAgent
//
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC (CXbyYAgent, CNetworkClient)

//  Construction
CXbyYAgent::CXbyYAgent (CLocatorAgent *pClient) : CNetworkAgent<CLocatorAgent> (pClient), m_hRequest(NULL)
{
}

//  Destruction
CXbyYAgent::~CXbyYAgent()
{
    if (m_hRequest)
	WSACancelAsyncRequest (m_hRequest);
}

//  Operations
void CXbyYAgent::BeginQuery ()
{
    switch (QueryMethod ())
    {
    case CNetwork::Asynchronous:
	DoAsynchronousQuery ();
	break;

    case CNetwork::Synchronous:
	DoSynchronousQuery ();
	break;

    case CNetwork::Wired:
	DoWiredQuery ();
	break;

    default:
	Fail ("Unsupported Query Method", WSAEOPNOTSUPP);
	break;
    }
}

void CXbyYAgent::EndQuery ()
{
    if (m_hRequest)
    {
	m_pNetwork->UnregisterRequest ((WORD)m_hRequest);
	m_hRequest = NULL;
    }
    Release ();
}

void CXbyYAgent::Stop ()
{
    if (m_hRequest && SOCKET_ERROR == WSACancelAsyncRequest (m_hRequest))
	RecordError ("Stop");
    CNetworkClient::Stop ();

    EndQuery ();
}

void CXbyYAgent::DoAsynchronousQuery ()
{
    DoSynchronousQuery ();
}

void CXbyYAgent::DoSynchronousQuery ()
{
    DoWiredQuery ();
}

void CXbyYAgent::DoWiredQuery ()
{
    Fail ("Wired Query Not Supported", WSAEOPNOTSUPP);
}


/////////////////////////////////////////////////////////////////////////////
//
//  CHostByNameAgent
//
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC (CHostByNameAgent, CXbyYAgent)

//  Construction
CHostByNameAgent::CHostByNameAgent (CLocatorAgent *pClient, LPCSTR host)
: CXbyYAgent (pClient), m_host (host)
{
    BeginQuery ();
}

//  Destruction
CHostByNameAgent::~CHostByNameAgent () {
}

//  Diagnostics
#ifdef _DEBUG
void CHostByNameAgent::Dump (CDumpContext& dc) const
{
    CXbyYAgent::Dump (dc);
    dc << " (" << m_host << ")";
}
#endif

//  Operations
void CHostByNameAgent::DoAsynchronousQuery ()
{
#ifdef _DEBUG
    afxDump << this << "Asynchronous Query Started\n";
#endif
    m_hRequest = WSAAsyncGetHostByName (
	m_pNetwork->m_hWnd,
	WM_AsyncHostByName,
	m_host,
	(LPSTR)m_requestBuffer,
	sizeof (m_requestBuffer)
    );
    if (m_hRequest && m_pNetwork->RegisterRequest ((WORD)m_hRequest, this))
	Retain ();
    else DoSynchronousQuery ();
}

void CHostByNameAgent::DoSynchronousQuery ()
{
#ifdef _DEBUG
    afxDump << this << "Synchronous Query Started\n";
#endif

    struct hostent *pHostEntry = gethostbyname (m_host);
    if (pHostEntry)
    {
	m_hostEntry = *pHostEntry;
	m_pClient->OnHostData (this);
    }
    else DoWiredQuery ();
}

void CHostByNameAgent::OnWSAEvent (LPARAM lParam)
{
    if (0 == WSAGETASYNCERROR (lParam))
	m_pClient->OnHostData (this);
    else DoSynchronousQuery ();

    EndQuery ();
}


/////////////////////////////////////////////////////////////////////////////
//
//  CProtocolByNameAgent
//
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC (CProtocolByNameAgent, CXbyYAgent)

//  Construction
CProtocolByNameAgent::CProtocolByNameAgent (CLocatorAgent *pClient, LPCSTR protocol)
: CXbyYAgent (pClient), m_protocol (protocol)
{
    BeginQuery ();
}

//  Destruction
CProtocolByNameAgent::~CProtocolByNameAgent ()
{
}

//  Diagnostics
#ifdef _DEBUG
void CProtocolByNameAgent::Dump (CDumpContext& dc) const
{
    CXbyYAgent::Dump (dc);
    dc << " (" << m_protocol << ")";
}
#endif

//  Operations
void CProtocolByNameAgent::DoAsynchronousQuery ()
{
#ifdef _DEBUG
    afxDump << this << "Asynchronous Query Started\n";
#endif
    m_hRequest = WSAAsyncGetProtoByName (
	m_pNetwork->m_hWnd,
	WM_AsyncProtocolByName,
	m_protocol,
	(LPSTR)m_requestBuffer,
	sizeof (m_requestBuffer)
    );
    if (m_hRequest && m_pNetwork->RegisterRequest ((WORD)m_hRequest, this))
	Retain ();
    else DoSynchronousQuery ();
}

void CProtocolByNameAgent::DoSynchronousQuery ()
{
#ifdef _DEBUG
    afxDump << this << "Synchronous Query Started\n";
#endif
    struct protoent *pProtocolEntry = getprotobyname (m_protocol);
    if (pProtocolEntry)
    {
	m_protocolEntry = *pProtocolEntry;
	m_pClient->OnProtocolData (this);
    }
    else DoWiredQuery ();
}

void CProtocolByNameAgent::DoWiredQuery ()
{
#ifdef _DEBUG
    afxDump << this << "Wired Query Started\n";
#endif
    m_protocolEntry.p_proto = AfxGetApp()->GetProfileInt (
	"winsock protocol", m_protocol, m_protocol.CompareNoCase ("tcp") == 0 ? IPPROTO_TCP : 0
    );
    if (0 != m_protocolEntry.p_proto)
	m_pClient->OnProtocolData (this);
    else Fail ("Wired Query", WSAEOPNOTSUPP);
}

void CProtocolByNameAgent::OnWSAEvent (LPARAM lParam)
{
    if (0 == WSAGETASYNCERROR (lParam))
	m_pClient->OnProtocolData (this);
    else DoSynchronousQuery ();

    EndQuery ();
}


/////////////////////////////////////////////////////////////////////////////
//
//  CServiceByNameAgent
//
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC (CServiceByNameAgent, CXbyYAgent)

//  Construction
CServiceByNameAgent::CServiceByNameAgent (CLocatorAgent *pClient, LPCSTR service, LPCSTR protocol)
    : CXbyYAgent (pClient), m_service (service), m_protocol (protocol)
{
    m_protocolWasNull = NULL == protocol;
    BeginQuery ();
}

//  Destruction
CServiceByNameAgent::~CServiceByNameAgent () {
}

//  Diagnostics
#ifdef _DEBUG
void CServiceByNameAgent::Dump (CDumpContext& dc) const
{
    CXbyYAgent::Dump (dc);
    dc << " (" << m_service << "/" << m_protocol << ")";
}
#endif

//  Operations
void CServiceByNameAgent::DoAsynchronousQuery ()
{
#ifdef _DEBUG
    afxDump << this << "Asynchronous Query Started\n";
#endif
    m_hRequest = WSAAsyncGetServByName (
	m_pNetwork->m_hWnd,
	WM_AsyncServiceByName,
	m_service,
	m_protocolWasNull ? NULL : (LPCSTR)m_protocol,
	(LPSTR)m_requestBuffer,
	sizeof (m_requestBuffer)
    );
    if (m_hRequest && m_pNetwork->RegisterRequest ((WORD)m_hRequest, this))
	Retain ();
    else DoSynchronousQuery ();
}

void CServiceByNameAgent::DoSynchronousQuery ()
{
#ifdef _DEBUG
    afxDump << this << "Synchronous Query Started\n";
#endif
    struct servent *pServiceEntry = getservbyname (
	m_service, m_protocolWasNull ? NULL : (LPCSTR)m_protocol
    );
    if (pServiceEntry)
    {
	m_serviceEntry = *pServiceEntry;
	m_pClient->OnServiceData (this);
    }
    else DoWiredQuery ();
}

void CServiceByNameAgent::DoWiredQuery ()
{
#ifdef _DEBUG
    afxDump << this << "Wired Query Started\n";
#endif
    m_serviceEntry.s_port = htons (
	AfxGetApp()->GetProfileInt (
	    "winsock service", m_service, m_service.CompareNoCase ("exec") == 0 ? IPPORT_EXECSERVER : 0
	)
    );
    if (0 != m_serviceEntry.s_port)
    {
	m_serviceEntry.s_proto = m_protocolWasNull ? "tcp" : m_protocol.GetBuffer(m_protocol.GetLength());
	m_pClient->OnServiceData (this);
	if (!m_protocolWasNull)
	    m_protocol.ReleaseBuffer(-1);
    }
    else Fail ("Wired Query", WSAEOPNOTSUPP);

}

void CServiceByNameAgent::OnWSAEvent (LPARAM lParam)
{
    if (0 == WSAGETASYNCERROR (lParam))
	m_pClient->OnServiceData (this);
    else DoSynchronousQuery ();

    EndQuery ();
}
