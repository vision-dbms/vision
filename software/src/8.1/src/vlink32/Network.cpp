/////////////////////////////////////////////////////////////////////////////
//
//  CNetwork Implementation
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//  Interfaces
/////////////////////////////////////////////////////////////////////////////

//  System...
#include "stdafx.h"
//#include "stdext.h"

//  Supporting...
#include "netagent.h"
#include "LocatorAgent.h"
#include "SocketAgent.h"
#include "vlinkapp.h"

//  Self...
#include "Network.h"

/////////////////////////////////////////////////////////////////////////////
//  WINSOCK Utilities
/////////////////////////////////////////////////////////////////////////////

#pragma comment (lib, "ws2_32.lib")

#define MAJOR_VER 1
#define MINOR_VER 1
#define WSA_MAKEWORD(x,y) ((y)*256+(x))

/////////////////////////////////////////////////////////////////////////////
//  Globals
/////////////////////////////////////////////////////////////////////////////

const DWORD	CNetwork::g_maxTickCount  = ~(DWORD)0;
const UINT	CNetwork::g_tickDuration  = 1000;
int		CNetwork::g_startupStatus = WSASYSNOTREADY;
DWORD		CNetwork::g_instanceCount = 0;


/////////////////////////////////////////////////////////////////////////////
//  Diagnostics
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

void CNetwork::Dump (CDumpContext& dc) const
{
    dc << "a CNetwork at " << (LPVOID)this << " ";
}
#endif


/////////////////////////////////////////////////////////////////////////////
//  Message Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CNetwork, CWnd)
    //{{AFX_MSG_MAP(CNetwork)
    ON_MESSAGE (WM_AsyncHostByName	, OnWSAMessage)
    ON_MESSAGE (WM_AsyncProtocolByName	, OnWSAMessage)
    ON_MESSAGE (WM_AsyncServiceByName	, OnWSAMessage)
    ON_MESSAGE (WM_AsyncSelect		, OnWSAMessage)
    ON_WM_TIMER()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
//  Error Formatting
/////////////////////////////////////////////////////////////////////////////

LPCSTR CNetwork::ErrorMessage (int errorCode)
{
    switch (errorCode)
    {
    case WSAHOST_NOT_FOUND:
	return "Winsock: WSAHOST_NOT_FOUND";
    case WSATRY_AGAIN:
	return "Winsock: WSATRY_AGAIN";
    case WSANO_RECOVERY:
	return "Winsock: WSANO_RECOVERY";
    case WSANO_DATA:
	return "Winsock: WSANO_DATA";
    case WSAEWOULDBLOCK:
	return "Winsock: WSAEWOULDBLOCK";
    case WSAEINPROGRESS:
	return "Winsock: WSAEINPROGRESS";
    case WSAEALREADY:
	return "Winsock: WSAEALREADY";
    case WSAENOTSOCK:
	return "Winsock: WSAENOTSOCK";
    case WSAEDESTADDRREQ:
	return "Winsock: WSAEDESTADDRREQ";
    case WSAEMSGSIZE:
	return "Winsock: WSAEMSGSIZE";
    case WSAEPROTOTYPE:
	return "Winsock: WSAEPROTOTYPE";
    case WSAENOPROTOOPT:
	return "Winsock: WSAENOPROTOOPT";
    case WSAEPROTONOSUPPORT:
	return "Winsock: WSAEPROTONOSUPPORT";
    case WSAESOCKTNOSUPPORT:
	return "Winsock: WSAESOCKTNOSUPPORT";
    case WSAEOPNOTSUPP:
	return "Winsock: WSAEOPNOTSUPP";
    case WSAEPFNOSUPPORT:
	return "Winsock: WSAEPFNOSUPPORT";
    case WSAEAFNOSUPPORT:
	return "Winsock: WSAEAFNOSUPPORT";
    case WSAEADDRINUSE:
	return "Winsock: WSAEADDRINUSE";
    case WSAEADDRNOTAVAIL:
	return "Winsock: WSAEADDRNOTAVAIL";
    case WSAENETDOWN:
	return "Winsock: WSAENETDOWN";
    case WSAENETUNREACH:
	return "Winsock: WSAENETUNREACH";
    case WSAENETRESET:
	return "Winsock: WSAENETRESET";
    case WSAECONNABORTED:
	return "Winsock: WSAECONNABORTED";


    case WSAECONNRESET:
	return "Winsock: WSAECONNRESET";
    case WSAENOBUFS:
	return "Winsock: WSAENOBUFS";
    case WSAEISCONN:
	return "Winsock: WSAEISCONN";
    case WSAENOTCONN:
	return "Winsock: WSAENOTCONN";
    case WSAESHUTDOWN:
	return "Winsock: WSAESHUTDOWN";
    case WSAETOOMANYREFS:
	return "Winsock: WSAETOOMANYREFS";
    case WSAETIMEDOUT:
	return "Winsock: WSAETIMEDOUT";
    case WSAECONNREFUSED:
	return "Winsock: WSAECONNREFUSED";
    case WSAELOOP:
	return "Winsock: WSAELOOP";
    case WSAENAMETOOLONG:
	return "Winsock: WSAENAMETOOLONG";
    case WSAEHOSTDOWN:
	return "Winsock: WSAEHOSTDOWN";
    case WSAEHOSTUNREACH:
	return "Winsock: WSAEHOSTUNREACH";
    case WSAENOTEMPTY:
	return "Winsock: WSAENOTEMPTY";
    case WSAEPROCLIM:
	return "Winsock: WSAEPROCLIM";
    case WSAEUSERS:
	return "Winsock: WSAEUSERS";
    case WSAEDQUOT:
	return "Winsock: WSAEDQUOT";
    case WSAESTALE:
	return "Winsock: WSAESTALE";
    case WSAEREMOTE:
	return "Winsock: WSAEREMOTE";

    case WSASYSNOTREADY:
	return "Winsock: Socket Subsystem Not Ready";
    case WSAVERNOTSUPPORTED:
	return "Winsock: Version Not Supported";
    case WSAEINVAL:
	return "Winsock: Invalid Value";
    default:
	return "Winsock: Unrecognized Error";
    }
}


/////////////////////////////////////////////////////////////////////////////
//  Address Formatting
/////////////////////////////////////////////////////////////////////////////

void CNetwork::AddressAsString (struct sockaddr const &rAddress, CString &rString)
{
    switch (rAddress.sa_family)
    {
    case AF_INET:
	AddressAsString ((struct sockaddr_in const &)rAddress, rString);
	break;
    default:
	{
	    rString = ""; rString += (WORD)rAddress.sa_family;
	    for (unsigned int i = 0; i < sizeof (rAddress.sa_data); i++)
	    {
		rString += ":"; rString += (WORD)rAddress.sa_data[i];
	    }
	}
	break;
    }
}

void CNetwork::AddressAsString (const struct sockaddr_in &rAddress, CString &rString)
{
    switch (rAddress.sin_family)
    {
    case AF_INET:
	rString  =  ""; rString += (WORD)(rAddress.sin_addr.S_un.S_un_b.s_b1);
	rString += '.'; rString += (WORD)(rAddress.sin_addr.S_un.S_un_b.s_b2);
	rString += '.'; rString += (WORD)(rAddress.sin_addr.S_un.S_un_b.s_b3);
	rString += '.'; rString += (WORD)(rAddress.sin_addr.S_un.S_un_b.s_b4);
	rString += ':'; rString += (WORD)ntohs (rAddress.sin_port);
	break;
    default:
	AddressAsString ((struct sockaddr const &)rAddress, rString);
	break;
    }
}


/////////////////////////////////////////////////////////////////////////////
//  Construction
/////////////////////////////////////////////////////////////////////////////

CNetwork::CNetwork()
{
    if (0 == g_instanceCount)
    {
	WSADATA wsaData;
	g_startupStatus = WSAStartup (WSA_MAKEWORD (MAJOR_VER, MINOR_VER), &wsaData);
#ifdef _DEBUG
	afxDump
	    << "     " << (LPCSTR)wsaData.szDescription << "\n"
	    << "     " << wsaData.wVersion		<< "\n"
	    << "     " << wsaData.wHighVersion		<< "\n"
	    << "     " << (LPCSTR)wsaData.szSystemStatus<< "\n"
	    << "     " << wsaData.iMaxSockets		<< "\n"
	    << "     " << wsaData.iMaxUdpDg		<< "\n"
	;
#endif
    }
    g_instanceCount++;

    m_tickCount			= 0;
    m_timerEnablingCount	= 0;
    m_timerId			= 0;
}

/////////////////////////////////////////////////////////////////////////////
//  Destruction
/////////////////////////////////////////////////////////////////////////////

CNetwork::~CNetwork()
{
    POSITION position = m_enabledAgents.GetStartPosition ();
    while (position)
    {
	WORD hRequest;
	union {
	    CObject 		*pObject;
	    CNetworkClient	*pClient;
	};
	m_enabledAgents.GetNextAssoc (position, hRequest, pObject);
	pClient->Fail ("Network Shutdown", WSAESHUTDOWN);
    }
    m_enabledAgents.RemoveAll ();

    if (0 == --g_instanceCount && 0 != g_startupStatus)
	WSACleanup ();
}


/////////////////////////////////////////////////////////////////////////////
//  Operations
/////////////////////////////////////////////////////////////////////////////

CSocketAgent* CNetwork::newSocketAgent (CNetworkClient* pClient, SOCKET xSocket) {
    return new CSocketAgent (pClient, xSocket);
}

void CNetwork::GetHostByName (CLocatorAgent* pClient, LPCSTR pHostName)
{
    CHostByNameAgent* pAgent = new CHostByNameAgent (pClient, pHostName);
    pAgent->Release ();
}

void CNetwork::GetProtocolByName (CLocatorAgent* pClient, LPCSTR pProtocolName)
{
    CProtocolByNameAgent* pAgent = new CProtocolByNameAgent (pClient, pProtocolName);
    pAgent->Release ();
}

void CNetwork::GetServiceByName (CLocatorAgent* pClient, LPCSTR pServiceName, LPCSTR pProtocolName)
{
    CServiceByNameAgent* pAgent = new CServiceByNameAgent (pClient, pServiceName, pProtocolName);
    pAgent->Release ();
}


enum CNetwork::QueryMethod CNetwork::QueryMethod (enum QueryType queryType) const
{
    LPCSTR key;

    switch (queryType)
    {
    case CNetwork::Host:
	key = "Host Resolution";
	break;
    case CNetwork::Service:
	key = "Service Resolution";
	break;
    case CNetwork::Protocol:
	key = "Protocol Resolution";
	break;
    default:
	key = "Resolution";
	break;
    }
    CString value = AfxGetApp()->GetProfileString ("winsock", key, "Asynchronous");
    return 0 == value.CompareNoCase ("Asynchronous")
	? CNetwork::Asynchronous
	: 0 == value.CompareNoCase ("Synchronous")
	? CNetwork::Synchronous
	: 0 == value.CompareNoCase ("Wired")
	? CNetwork::Wired
	: CNetwork::Unknown;
}

BOOL CNetwork::RegisterRequest (WORD hRequest, CNetworkClient *pClient)
{
    BOOL result;

    TRY {
	m_enabledAgents[hRequest] = pClient;
	result = TRUE;
    } CATCH (CMemoryException, pException) {
	WSASetLastError (WSAEFAULT);
	result = FALSE;
    } END_CATCH

#ifdef _DEBUG
    afxDump
	<< this
	<< "RegisterRequest: "
	<< hRequest
	<< (result ? " registered for " : " NOT REGISTERED for ")
	<< pClient
	<< "\n";
#endif

    return result;
}

void CNetwork::UnregisterRequest (WORD hRequest)
{
#ifdef _DEBUG
    afxDump << this << "UnregisterRequest:" << hRequest << "\n";
#endif
    
    m_enabledAgents.RemoveKey (hRequest);
}

void CNetwork::ProcessNextNetworkEvent ()
{
    MSG msg;

    if (m_hWnd && GetMessage (&msg, m_hWnd, 0, 0))	// WM_WinsockFirst, WM_WinsockLast))
    {
	switch (msg.message)
	{
	case WM_TIMER:
	    OnTimer (msg.wParam);
	    break;
	case WM_AsyncHostByName:
	case WM_AsyncProtocolByName:
	case WM_AsyncServiceByName:
	case WM_AsyncSelect:
	    OnWSAMessage (msg.wParam, msg.lParam);
	    break;
	default:
#ifdef _DEBUG
	    afxDump
		<< this
		<< ">>> ProcessNextNetworkEvent: Unexpected message "
		<< msg.message
		<< ", wParam = " << msg.wParam
		<< ", lParam = " << msg.lParam
		<< "\n"
	    ;
#endif
	    break;
	}
    }
}

void CNetwork::EnableTimeouts ()
{
    if (++m_timerEnablingCount > 0 && 0 == m_timerId && m_hWnd)
    {
	m_timerId = SetTimer (1000 /* Random Timer Id Number */, g_tickDuration, NULL);
#ifdef _DEBUG
	if (m_timerId)
	    afxDump << this << " EnableTimeouts: Timer enabled, TickCount = " << m_tickCount << ", TickDuration = " << (long)g_tickDuration << "ms\n";
	else
	    afxDump << this << " EnableTimeouts: Timer not started!!!\n";
#endif
    }
}

void CNetwork::DisableTimeouts ()
{
    if (m_timerEnablingCount > 0 && --m_timerEnablingCount == 0 && 0 != m_timerId && m_hWnd)
    {
	KillTimer (m_timerId);
	m_timerId = 0;
#ifdef _DEBUG
	afxDump << this << " DisableTimeouts: Timer disabled, TickCount = " << m_tickCount << "\n";
#endif
    }
}


/////////////////////////////////////////////////////////////////////////////
//  Handlers
/////////////////////////////////////////////////////////////////////////////

LRESULT CNetwork::OnWSAMessage (WPARAM wParam, LPARAM lParam)
{
    union {
	CObject		*pObject;
	CNetworkClient	*pClient;
    };

    if (m_enabledAgents.Lookup (wParam, pObject))
	pClient->OnWSAEvent (lParam);

    return 0;
}

void CNetwork::OnTimer(UINT nIDEvent)
{
    m_tickCount++;
    CWnd::OnTimer(nIDEvent);
}
