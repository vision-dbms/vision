//////////////////////////////////////////////////////////////////////////////
//
//  CLocatorAgent Implementation
//
//////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//  Interfaces
/////////////////////////////////////////////////////////////////////////////

//  System...
#include "stdafx.h"
#include "stdext.h"

//  Supporting...
#include "Netagent.h"

//  Self
#include "LocatorAgent.h"


/////////////////////////////////////////////////////////////////////////////
//
//  Construction
//
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC (CLocatorAgent, CNetworkClient)

CLocatorAgent::CLocatorAgent (CNetworkClient *pClient)
: CNetworkClient    (pClient)
, m_hostIsValid	    (FALSE)
, m_portIsValid	    (FALSE)
, m_protocolIsValid (FALSE)
{
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destruction
//
/////////////////////////////////////////////////////////////////////////////

CLocatorAgent::~CLocatorAgent() {
}


/////////////////////////////////////////////////////////////////////////////
//
//  Operations and Overrides
//
/////////////////////////////////////////////////////////////////////////////

void CLocatorAgent::SetHost (LPCSTR hostname, BOOL dottedAddressPossible)
{
#ifdef _DEBUG
    afxDump << this << "SetHost: " << hostname << ", " << dottedAddressPossible << "\n";
#endif

    if (dottedAddressPossible && INADDR_NONE != (m_address.sin_addr.s_addr = inet_addr (hostname)))
    {
	m_address.sin_family = AF_INET;
	m_hostIsValid = TRUE;
	return;
    }

    m_hostIsValid = FALSE;
    m_pNetwork->GetHostByName (this, hostname);
}

void CLocatorAgent::SetPort (LPCSTR service, LPCSTR protocol)
{
#ifdef _DEBUG
    afxDump << this << "SetPort: " << service << ", " << protocol << "\n";
#endif

    if (protocol)
	SetProtocol (protocol);
    else m_protocolIsValid = FALSE;

    m_portIsValid = FALSE;
    m_pNetwork->GetServiceByName (this, service, protocol);
}

void CLocatorAgent::SetPort (int port, BOOL hostByteOrder)
{
#ifdef _DEBUG
    afxDump << this << "SetPort: " << port << ", " << hostByteOrder << "\n";
#endif

    m_address.sin_port	= hostByteOrder ? htons (port) : port;
    m_portIsValid	= TRUE;
}

void CLocatorAgent::SetProtocol (LPCSTR protocol)
{
#ifdef _DEBUG
    afxDump << this << "SetProtocol: " << protocol << "\n";
#endif

    m_protocolIsValid = FALSE;
    m_pNetwork->GetProtocolByName (this, protocol);
}

void CLocatorAgent::SetProtocol (int protocol)
{
#ifdef _DEBUG
    afxDump << this << "SetProtocol: " << protocol << "\n";
#endif

    m_protocol		= protocol;
    m_protocolIsValid	= TRUE;
}


void CLocatorAgent::OnHostData (CHostByNameAgent *pAgent)
{
    struct hostent const &rHost = pAgent->host ();
    m_address.sin_family = rHost.h_addrtype;
    memcpy (&m_address.sin_addr, rHost.h_addr, rHost.h_length);
    m_hostIsValid = TRUE;

#ifdef _DEBUG
    CString s;
    m_pNetwork->AddressAsString (m_address, s);
    afxDump << this << "OnHostData: " << m_address.sin_family << ", " << s << "\n";
#endif

    ReportAddressIfComplete ();
}

void CLocatorAgent::OnServiceData (CServiceByNameAgent *pAgent)
{
    struct servent const &rServiceEntry = pAgent->service ();
#ifdef _DEBUG
    afxDump
	<< this
	<< "OnServiceData: "
	<< (NULL == rServiceEntry.s_proto ? "***NULL***" : (LPVOID)(rServiceEntry.s_proto))
	<< ", "
	<< rServiceEntry.s_port
	<< ", "
	<< (WORD)(rServiceEntry.s_port)
	<< "\n";
#endif
    if (!m_protocolIsValid)
	SetProtocol (rServiceEntry.s_proto);
    SetPort (rServiceEntry.s_port, FALSE);

    ReportAddressIfComplete ();
}

void CLocatorAgent::OnProtocolData (CProtocolByNameAgent *pAgent)
{
    struct protoent const &rProtocolEntry = pAgent->protocol ();
#ifdef _DEBUG
    afxDump << this << "OnProtocolData: " << rProtocolEntry.p_proto << "\n";
#endif
    SetProtocol (rProtocolEntry.p_proto);

    ReportAddressIfComplete ();
}

void CLocatorAgent::ReportAddressIfComplete ()
{
    if (m_hostIsValid && m_portIsValid && m_protocolIsValid)
	m_pClient->OnAddressData (this);
}
