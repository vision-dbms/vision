//////////////////////////////////////////////////////////////////////////////
//
//  CSocketAgent Implementation
//
//////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//  Interfaces
/////////////////////////////////////////////////////////////////////////////

//  System...
#include "stdafx.h"
#include "stdext.h"

//  Supporting...
#include "Network.h"

//  Self
#include "SocketAgent.h"


/////////////////////////////////////////////////////////////////////////////
//  Construction
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC (CSocketAgent, CNetworkClient)

CSocketAgent::CSocketAgent (CNetworkClient *pClient, SOCKET xSocket)
: CNetworkClient (pClient), m_recvBlocked (TRUE), m_sendBlocked (TRUE), m_xSocket (xSocket)
{
    if (SOCKET_ERROR == m_xSocket)
	Fail ("Construction");
    else if (
	SOCKET_ERROR == WSAAsyncSelect (
	    m_xSocket,
	    m_pNetwork->m_hWnd,
	    WM_AsyncSelect,
	    FD_READ | FD_WRITE | FD_ACCEPT | FD_CONNECT | FD_CLOSE
	) || !m_pNetwork->RegisterRequest ((WORD)m_xSocket, this)
    ) Fail ("Start");
}


/////////////////////////////////////////////////////////////////////////////
//  Destruction
/////////////////////////////////////////////////////////////////////////////

CSocketAgent::~CSocketAgent ()
{
    Stop ();

    if (INVALID_SOCKET != m_xSocket)
	closesocket (m_xSocket);
}


/////////////////////////////////////////////////////////////////////////////
//  Operations
/////////////////////////////////////////////////////////////////////////////

SOCKET CSocketAgent::Accept (struct sockaddr* pRemoteAddress, int* pRemoteAddressLength)
{
    return accept (m_xSocket, pRemoteAddress, pRemoteAddressLength);
}

void CSocketAgent::Bind (struct sockaddr_in const *pAddress)
{
    Bind ((sockaddr*)pAddress);
}

void CSocketAgent::Bind (unsigned short iPortNumber)
{
    struct sockaddr_in iAddress;
    iAddress.sin_family			= AF_INET;
    iAddress.sin_port			= htons (iPortNumber);
    iAddress.sin_addr.S_un.S_addr	= INADDR_ANY;
    Bind (&iAddress);
}
void CSocketAgent::Bind (struct sockaddr const *pAddress)
{
    if (SOCKET_ERROR == bind (m_xSocket, pAddress, sizeof (*pAddress)))
	RecordError ("Bind");
}

void CSocketAgent::Connect (struct sockaddr_in const *pAddress)
{
    Connect ((sockaddr*)pAddress);
}

void CSocketAgent::Connect (struct sockaddr const *pAddress)
{
    if (SOCKET_ERROR == connect (
	    m_xSocket, pAddress, sizeof (*pAddress)
	) && WSAEWOULDBLOCK != WSAGetLastError ()
    ) RecordError ("Connect");
}

void CSocketAgent::Listen (int backlog)
{
    if (SOCKET_ERROR == listen (
	    m_xSocket, backlog
	) && WSAEWOULDBLOCK != WSAGetLastError ()
    ) RecordError ("Listen");
}

BOOL CSocketAgent::LocalAddress (CString &s)
{
    struct sockaddr name;
    if (LocalAddress (&name))
    {
	m_pNetwork->AddressAsString (name, s);
	return TRUE;
    }
    return FALSE;
}

BOOL CSocketAgent::LocalAddress	(struct sockaddr *name)
{
    int namelen = sizeof (*name);
    if (SOCKET_ERROR == getsockname (m_xSocket, name, &namelen))
    {
	RecordError ("Local Address Query");
	return FALSE;
    }
    return TRUE;
}

BOOL CSocketAgent::RemoteAddress (CString &s)
{
    struct sockaddr name;
    if (RemoteAddress (&name))
    {
	m_pNetwork->AddressAsString (name, s);
	return TRUE;
    }
    return FALSE;
}

BOOL CSocketAgent::RemoteAddress (struct sockaddr *name)
{
    int namelen = sizeof (*name);
    if (SOCKET_ERROR == getpeername (m_xSocket, name, &namelen))
    {
	RecordError ("Remote Address Query");
	return FALSE;
    }
    return TRUE;
}

void CSocketAgent::Send (unsigned int &rOffset, char const* pData, unsigned int sData)
{
    int bytesMoved = m_sendBlocked ? 0 : send (m_xSocket, pData + rOffset, sData - rOffset, 0);
    if (SOCKET_ERROR != bytesMoved)
	rOffset += (unsigned int)bytesMoved;
    else if (WSAEWOULDBLOCK == WSAGetLastError ())
	m_sendBlocked = TRUE;
    else RecordError ("Send");
}

BOOL CSocketAgent::Receive (unsigned int& rBytesTransferred, char* pBuffer, unsigned int sBuffer)
{
    rBytesTransferred = m_recvBlocked ? 0 : recv (m_xSocket, pBuffer, sBuffer, 0);
    if (SOCKET_ERROR != rBytesTransferred)
    {
//-------------------------------------------------------------------------
// Why did I do this test???
	if (0 == rBytesTransferred)
	{
	    m_recvBlocked = TRUE;
	    RecordError ("Receive Zero Bytes", WSAENOTCONN);
	    return FALSE;
	}
//-------------------------------------------------------------------------
//	DumpBuffer ("Data Received", pBuffer, rBytesTransferred);

	return TRUE;
    }

    if (WSAEWOULDBLOCK == WSAGetLastError ())
    {
	m_recvBlocked = TRUE;
	rBytesTransferred = 0;
	return TRUE;
    }

    RecordError ("Receive");
    return FALSE;
}

BOOL CSocketAgent::ReceivedByteCount (int *byteCount)
{
    unsigned long bytesReceivedL;

    if (SOCKET_ERROR == ioctlsocket (m_xSocket, FIONREAD, &bytesReceivedL))
    {
	RecordError ("Receive Byte Count Query");
	*byteCount = 0;
	return FALSE;
    }
    *byteCount = bytesReceivedL > (unsigned long)0x00007fff ? 0x7fff : (int)bytesReceivedL;
    return TRUE;
}

void CSocketAgent::Stop ()
{
    if (m_fStopped)
	return;

    if (INVALID_SOCKET != m_xSocket)
    {
	if (SOCKET_ERROR == WSAAsyncSelect (m_xSocket, m_pNetwork->m_hWnd, 0, 0))
	    RecordError ("Stop");
	m_pNetwork->UnregisterRequest ((WORD)m_xSocket);
    }
    CNetworkClient::Stop ();
}

void CSocketAgent::OnWSAEvent (LPARAM lParam)
{
    if (0 != WSAGETSELECTERROR (lParam))
    {
	Fail ("OnWSAEvent", WSAGETSELECTERROR (lParam));
	return;
    }

    switch (WSAGETSELECTEVENT (lParam))
    {
    case FD_ACCEPT:
	m_pClient->OnAccept (this);
	break;
    case FD_CONNECT:
	m_pClient->OnConnect (this);
	break;
    case FD_CLOSE:
	m_recvBlocked =
	m_sendBlocked = TRUE;
	m_pClient->OnClose (this);
	break;
    case FD_READ:
	m_recvBlocked = FALSE;
// Disable read notification messages to avoid flooding the Windows message queue...
	WSAAsyncSelect (
	    m_xSocket,
	    m_pNetwork->m_hWnd,
	    WM_AsyncSelect,
	    FD_WRITE | FD_ACCEPT | FD_CONNECT | FD_CLOSE
	);
	m_pClient->OnReadable (this);
// ... and re-enable them once all available data has been read.
	WSAAsyncSelect (
	    m_xSocket,
	    m_pNetwork->m_hWnd,
	    WM_AsyncSelect,
	    FD_READ | FD_WRITE | FD_ACCEPT | FD_CONNECT | FD_CLOSE
	);
	break;
    case FD_WRITE:
	m_sendBlocked = FALSE;
	m_pClient->OnWriteable (this);
	break;
    default:
	break;
    }
}
