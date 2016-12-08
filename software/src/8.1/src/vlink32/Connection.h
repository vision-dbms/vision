#ifndef CConnection_Interface
#define CConnection_Interface

/************************
 *****  Components  *****
 ************************/

#include "NetworkClient.h"

#include "Collection.h"
#include "ConnectionController.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "SocketAgent.h"

class CForwardingAgent;
class CForwardingService;
class CHistory;
class CRequest;
class CTextBuffer;

/*************************
 *****  Definitions  *****
 *************************/

class CConnection : public CNetworkClient
{
    friend CForwardingService;
    friend CRequest;

    DECLARE_DYNAMIC(CConnection)

//  Globals
public:
    static const long		g_defaultTimeout;
    static const CString 	g_launchDirective;

    static CCollectionKernel	Instances;

//  Construction
public:
    CConnection (CConnectionData& rConnectionData);

//  Destruction
public:
    ~CConnection ();

//  Diagnostics
#ifdef _DEBUG
public:
    void Dump (CDumpContext& dc) const;
#endif

// Lookup
public:
    static CConnection *GetConnection (
	LPCSTR hostname, LPCSTR username, LPCSTR password, LPCSTR database, short objectSpace
    );
    static CConnection *GetConnection (CConnectionData& connectionData);

//  Access
public:
    static CConnection *NewestInstance	() { return (CConnection*)Instances.NewestItem(); }
    static CConnection *OldestInstance	() { return (CConnection*)Instances.OldestItem(); }

    CConnection *NextNewestInstance	() const { return (CConnection*)m_pNextNewestItem; }
    CConnection *NextOldestInstance	() const { return (CConnection*)m_pNextOldestItem; }

    CString const&	ForwardingId	() const { return m_iController.m_forwardingId; }
    LPCSTR		Database	() const { return m_iController.m_database; }
    LPCSTR		Executable	() const { return m_iController.m_executable; }
    LPCSTR		Hostname 	() const { return m_iController.m_hostname; }
    short		ObjectSpace	() const { return m_iController.m_osNumber; }
    LPCSTR		Options		() const { return m_iController.m_options; }
    long 		Timeout		() const {
	return m_iController.m_timeout < 0 ? g_defaultTimeout : m_iController.m_timeout;
    }
    LPCSTR		Username 	() const { return m_iController.m_username; }

    BOOL	ReplyBufferIsEmpty	() const { return m_pReplyCursor >= m_pReplyLimit; }
    BOOL	ReplyBufferIsntEmpty	() const { return m_pReplyCursor <  m_pReplyLimit; }

    BOOL	IsReadable		() const { return ReplyBufferIsntEmpty () || m_pDSocket->IsReadable (); }
    BOOL	IsWriteable		() const { return m_pDSocket->IsWriteable (); }

    BOOL	IsUp			() { return OAIsUp		(); }
    BOOL	IsDown			() { return OAIsDown		(); }
    BOOL	IsStable		() { return OAIsStable		(); }
    BOOL	IsUnstable		() { return OAIsUnstable	(); }

//  Operations
public:
    void	WaitForConnection	(long timeout) { OAWaitForConnection (timeout); }
    BOOL	IsUpWithin		(long timeout) { return OAIsUpWithin (timeout); }

    void 	Stop			();

    CRequest* Submit (CString& rRequestString, unsigned int nAcksExpected = 0);
    CRequest* Submit (LPCSTR pRequestText, unsigned int sRequestText);
    CRequest* Submit (LPCSTR pRequestText);
    CRequest* Submit (
	CTextBuffer*		pRequestText,
	unsigned int		nAcksExpected    = 0,
	CForwardingAgent*	pSubmittingAgent = 0
    );

protected:
    void SendRequests		();
    void SetSender		(CRequest *pRequest);

    void ReceiveReplies		();
    void NextReceiver		();

    void OnAddressData		(CLocatorAgent *pAgent);

    void OnConnect		(CSocketAgent *pAgent);
    void OnClose		(CSocketAgent *pAgent);
    void OnReadable		(CSocketAgent *pAgent);
    void OnWriteable		(CSocketAgent *pAgent);

//  State
protected:
    CConnectionController	m_iController;
    CString const		m_iPrompt;
    CForwardingService*		m_pForwardingService;
    CSocketAgent*		m_pDSocket;
    CHistory*	 		m_pRequestHistory;
    CRequest*			m_pConnectionRequest;
    CRequest*			m_pReceiver;		// also the request queue head
    CRequest*			m_pSender;
    CRequest*			m_pTail;
    CSocketAgent::OutputBuffer	m_iOutputBuffer;
    LPCSTR			m_pReplyCursor;
    LPCSTR			m_pReplyLimit;
    char			m_iReplyBuffer[31 * 1024];

// Maps
protected:
    // Generated message map functions
    //{{AFX_MSG(CConnection)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

//	DECLARE_OLECREATE(CConnection)

protected:
    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CConnection)
    afx_msg long OAGetTimeout();
    afx_msg void OASetTimeout(long nNewValue);
    afx_msg BSTR OAHostname();
    afx_msg BSTR OAUsername();
    afx_msg BSTR OAPassword();
    afx_msg BSTR OADatabase();
    afx_msg short OAObjectSpace(); 
    afx_msg BSTR OAOptions();
    afx_msg BSTR OAExecutable();
    afx_msg BSTR OACommand();
    afx_msg BSTR OADescription();
    afx_msg BSTR OALocalAddress ();
    afx_msg BSTR OARemoteAddress();
    afx_msg void OAWaitForCompletion(long timeout);
    afx_msg BOOL OACompletedWithin(long timeout);
    afx_msg void OAWaitForConnection(long timeout);
    afx_msg BOOL OAIsUpWithin(long timeout);
    afx_msg BOOL OAIsUp();
    afx_msg BOOL OAIsDown();
    afx_msg BOOL OAIsStable();
    afx_msg BOOL OAIsUnstable();
    afx_msg LPDISPATCH OASubmit(LPCSTR request);
    afx_msg long OARequestCount();
    afx_msg LPDISPATCH OARequestHistory();
    afx_msg BOOL OAStoreTemplate(LPCSTR storedTemplateName);
    afx_msg LPDISPATCH OACreateAgent();
    //}}AFX_DISPATCH
    DECLARE_DISPATCH_MAP()
};

#endif
