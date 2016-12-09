#ifndef CNetworkServer_Interface
#define CNetworkServer_Interface

/////////////////////////////////////////////////////////////////////////////
//  Components
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//  Declarations
/////////////////////////////////////////////////////////////////////////////

#include <winsock.h>

class CLocatorAgent;

/////////////////////////////////////////////////////////////////////////////
//  Messages
/////////////////////////////////////////////////////////////////////////////

#define WM_WinsockFirst		WM_AsyncHostByName

#define WM_AsyncHostByName	(WM_USER +  0)
#define WM_AsyncProtocolByName	(WM_USER +  1)
#define WM_AsyncServiceByName	(WM_USER +  2)
#define WM_AsyncSelect		(WM_USER +  3)

#define WM_WinsockLast		WM_AsyncSelect

/////////////////////////////////////////////////////////////////////////////
//  Definitions
/////////////////////////////////////////////////////////////////////////////

class CNetwork : public CWnd
{
    friend class CNetworkClient;
    friend class CXbyYAgent;
    friend class CSocketAgent;

//  Globals
public:
    enum QueryMethod {
	Asynchronous, Synchronous, Wired, Unknown
    };
    enum QueryType {
	Host, Service, Protocol
    };
    static const DWORD		g_maxTickCount;
    static const UINT		g_tickDuration;

protected:
    static int			g_startupStatus;
    static DWORD		g_instanceCount;

public:
    static LPCSTR ErrorMessage	(int errorCode = g_startupStatus ? g_startupStatus : WSAGetLastError ());
    static void AddressAsString	(struct sockaddr    const &rAddress, CString &rString);
    static void AddressAsString (struct sockaddr_in const &rAddress, CString &rString);

//  Construction
public:
    CNetwork ();

//  Destruction
    ~CNetwork ();

//  Diagnostics
#ifdef _DEBUG
public:
    void Dump (CDumpContext& dc) const;
#endif

//  Operations
public:
    CSocketAgent* newSocketAgent (
	CNetworkClient* pClient, int xAddressFamily = AF_INET, int xType = SOCK_STREAM, int xProtocol = 0
    ) {
	return newSocketAgent (pClient, socket (xAddressFamily, xType, xProtocol));
    }
    CSocketAgent* newSocketAgent (CNetworkClient* pClient, SOCKET xSocket);

    void GetHostByName		(CLocatorAgent* pClient, LPCSTR pHostName);
    void GetProtocolByName	(CLocatorAgent* pClient, LPCSTR pProtocolName);
    void GetServiceByName	(CLocatorAgent* pClient, LPCSTR pServiceName, LPCSTR pProtocolName);

    enum QueryMethod HostQueryMethod	() const { return QueryMethod (Host	); }
    enum QueryMethod ServiceQueryMethod	() const { return QueryMethod (Service	); }
    enum QueryMethod ProtocolQueryMethod() const { return QueryMethod (Protocol	); }

    enum QueryMethod QueryMethod (enum QueryType queryType) const;

    DWORD TickCount (long interval = 0) const {
	interval *= 1000 / g_tickDuration;
	return g_maxTickCount - m_tickCount > (DWORD)interval ? m_tickCount + (DWORD)interval : g_maxTickCount;
    }

    BOOL RegisterRequest	(WORD hRequest, CNetworkClient *pClient);
    void UnregisterRequest	(WORD hRequest);

    void ProcessNextNetworkEvent();

protected:
    void EnableTimeouts		();
    void DisableTimeouts	();

//  State
protected:
    CMapWordToOb		m_enabledAgents;
    DWORD			m_tickCount;
    DWORD			m_timerEnablingCount;
    UINT			m_timerId;

// Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CNetwork)
    afx_msg LRESULT OnWSAMessage(WPARAM wParam, LPARAM lParam);
    afx_msg void OnTimer	(UINT nIDEvent);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif
