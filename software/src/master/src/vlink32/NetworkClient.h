#ifndef CNetworkClient_Interface
#define CNetworkClient_Interface

/************************
 *****  Components  *****
 ************************/

#include "vlinkcmd.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Network.h"

class CHostByNameAgent;
class CLocatorAgent;
class CProtocolByNameAgent;
class CServiceByNameAgent;
class CSocketAgent;

/*************************
 *****  Definitions  *****
 *************************/

class CNetworkClient : public CVLinkCmdTarget
{
    friend CNetwork;
    friend CLocatorAgent;
    friend CHostByNameAgent;
    friend CProtocolByNameAgent;
    friend CServiceByNameAgent;
    friend CSocketAgent;

    DECLARE_DYNAMIC (CNetworkClient)

//  Construction
protected:
    CNetworkClient (CNetworkClient *pClient = NULL);

//  Destruction
protected:
    ~CNetworkClient ();

//  Diagnostics

//  Access
public:
    CNetwork*	Network		() const { return m_pNetwork; }

    DWORD	MaxTickCount	()			const { return m_pNetwork->g_maxTickCount; }
    DWORD	TickCount	(long timeout = 0)	const { return m_pNetwork->TickCount(timeout); }

    BOOL	Started		() const { return TRUE; }
    BOOL	Stopped		() const { return m_fStopped; }
    BOOL	Failed		() const { return m_fFailed; }

    LPCSTR	FailureLocation	() const { return m_lastErrorLocation; }
    LPCSTR	LastErrorMessage() const { return m_pNetwork->ErrorMessage (m_xLastError); }
    int		LastErrorCode	() const { return m_xLastError; }

//  Operations
public:
    void ProcessNextNetworkEvent() { m_pNetwork->ProcessNextNetworkEvent (); }

protected:
    virtual void Stop		();
    virtual void Fail		(LPCSTR location, int errorCode = WSAGetLastError ());

protected:
    virtual void OnAgentFailure	(CNetworkClient *pAgent);

    virtual void OnWSAEvent	(LPARAM lParam);

    virtual void OnAddressData	(CLocatorAgent *pAgent);

    virtual void OnAccept	(CSocketAgent *pAgent);
    virtual void OnConnect	(CSocketAgent *pAgent);
    virtual void OnListen	(CSocketAgent *pAgent);
    virtual void OnClose	(CSocketAgent *pAgent);
    virtual void OnReadable	(CSocketAgent *pAgent);
    virtual void OnWriteable	(CSocketAgent *pAgent);

    void RecordError		(LPCSTR errorLocation, int errorCode = WSAGetLastError ());

    void EnableTimeouts		() { m_pNetwork->EnableTimeouts  (); }
    void DisableTimeouts	() { m_pNetwork->DisableTimeouts (); }

//  State
protected:
    CNetwork* const		m_pNetwork;
    CNetworkClient*		m_pClient;
    BOOL			m_fStopped;
    BOOL			m_fFailed;
    BOOL			m_fIndependent;
    int				m_xLastError;
    CString			m_lastErrorLocation;

protected:
    // Generated message map functions
    //{{AFX_MSG(CNetworkClient)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CNetworkClient)
    afx_msg BOOL OAStarted();
    afx_msg BOOL OAStopped();
    afx_msg short OALastErrorCode();
    afx_msg BSTR OALastErrorMessage();
    afx_msg BOOL OAFailed();
    //}}AFX_DISPATCH
    DECLARE_DISPATCH_MAP()
};


template <class ClientBaseClass> class CNetworkAgent : public CNetworkClient
{
//  Construction
protected:
    CNetworkAgent (ClientBaseClass* pClient) : CNetworkClient (), m_pClient (pClient) {
	m_pClient->Retain ();
    }

//  Destruction
protected:
    ~CNetworkAgent () {
	m_pClient->Release ();
    }

//  State
protected:
    ClientBaseClass* const m_pClient;
};

#endif
