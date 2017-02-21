/////////////////////////////////////////////////////////////////////////////
//
//  CNetworkClient Implementation
//
/////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//  Interfaces
//////////////////////////////////////////////////////////////////////////////

//  System...
#include "stdafx.h"
#include "stdext.h"

#include <afxpriv.h>

//  Supporting...
#include "Network.h"
#include "vlinkapp.h"

//  Self...
#include "NetworkClient.h"


/////////////////////////////////////////////////////////////////////////////
//  Diagnostics
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//  Message Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CNetworkClient, CVLinkCmdTarget)
    //{{AFX_MSG_MAP(CNetworkClient)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//  Dispatch Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_DISPATCH_MAP(CNetworkClient, CVLinkCmdTarget)
    //{{AFX_DISPATCH_MAP(CNetworkClient)
    DISP_FUNCTION(CNetworkClient, "Started", OAStarted, VT_BOOL, VTS_NONE)
    DISP_FUNCTION(CNetworkClient, "Stopped", OAStopped, VT_BOOL, VTS_NONE)
    DISP_FUNCTION(CNetworkClient, "LastErrorCode", OALastErrorCode, VT_I2, VTS_NONE)
    DISP_FUNCTION(CNetworkClient, "LastErrorDescription", OALastErrorMessage, VT_BSTR, VTS_NONE)
    DISP_FUNCTION(CNetworkClient, "Failed", OAFailed, VT_BOOL, VTS_NONE)
    //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
//  Construction
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CNetworkClient, CVLinkCmdTarget)

CNetworkClient::CNetworkClient (CNetworkClient* pClient)
: m_pNetwork	(Application()->Network ())
, m_pClient	(pClient)
, m_fStopped	(FALSE)
, m_fFailed	(FALSE)
, m_xLastError	(0)
{
    if (m_pClient == NULL)
	m_pClient = this;
}


/////////////////////////////////////////////////////////////////////////////
//  Destruction
/////////////////////////////////////////////////////////////////////////////

CNetworkClient::~CNetworkClient ()
{
}


/////////////////////////////////////////////////////////////////////////////
//  Operations
/////////////////////////////////////////////////////////////////////////////

void CNetworkClient::Stop ()
{
    m_fStopped = TRUE;
}

void CNetworkClient::Fail (LPCSTR errorLocation, int errorCode)
{
    RecordError (errorLocation, errorCode);
    m_fFailed = TRUE;
    Stop ();
    if (this != m_pClient)
	m_pClient->OnAgentFailure (this);
}

void CNetworkClient::RecordError (LPCSTR errorLocation, int errorCode)
{
    m_lastErrorLocation	= errorLocation;
    m_xLastError	= errorCode;
#ifdef _DEBUG
    afxDump << this << m_lastErrorLocation << ": " << LastErrorCode () << ", " << LastErrorMessage() << "\n";
#endif
}

void CNetworkClient::OnAgentFailure (CNetworkClient *pAgent)
{
#ifdef _DEBUG
    afxDump << this << "OnAgentFailure " << pAgent->LastErrorMessage () << "\n";
#endif
    Fail (pAgent->FailureLocation (), pAgent->LastErrorCode ());
}

void CNetworkClient::OnWSAEvent (LPARAM lParam)
{
#ifdef _DEBUG
    afxDump << this << "Default OnWSAEvent Called --- Missing Override !!!\n";
#endif
}

void CNetworkClient::OnAddressData (CLocatorAgent *pAgent)
{
#ifdef _DEBUG
    afxDump << this << "Default OnAddressData Called --- Missing Override !!!\n";
#endif
}

void CNetworkClient::OnAccept (CSocketAgent *pAgent)
{
#ifdef _DEBUG
    afxDump << this << "Default OnAccept Called --- Missing Override !!!\n";
#endif
}

void CNetworkClient::OnConnect (CSocketAgent *pAgent)
{
#ifdef _DEBUG
    afxDump << this << "Default OnConnect Called --- Missing Override !!!\n";
#endif
}

void CNetworkClient::OnListen (CSocketAgent *pAgent)
{
#ifdef _DEBUG
    afxDump << this << "Default OnListen Called --- Missing Override !!!\n";
#endif
}

void CNetworkClient::OnClose (CSocketAgent *pAgent)
{
#ifdef _DEBUG
    afxDump << this << "Default OnClose Called --- Missing Override !!!\n";
#endif
}

void CNetworkClient::OnReadable (CSocketAgent *pAgent)
{
#ifdef _DEBUG
    afxDump << this << "Default OnReadable Called --- Missing Override !!!\n";
#endif
}

void CNetworkClient::OnWriteable (CSocketAgent *pAgent)
{
#ifdef _DEBUG
    afxDump << this << "Default OnWriteable Called --- Missing Override !!!\n";
#endif
}


/////////////////////////////////////////////////////////////////////////////
//  Handlers
/////////////////////////////////////////////////////////////////////////////

BOOL CNetworkClient::OAStarted()
{
    return Started ();
}

BOOL CNetworkClient::OAStopped()
{
    return Stopped ();
}

BOOL CNetworkClient::OAFailed()
{
    return Failed ();
}

short CNetworkClient::OALastErrorCode()
{
    return m_xLastError;
}

BSTR CNetworkClient::OALastErrorMessage()
{
    USES_CONVERSION;

    return ::SysAllocString(T2COLE (LastErrorMessage()));
}
