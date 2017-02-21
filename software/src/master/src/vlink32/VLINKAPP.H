#ifndef CApplication_Interface
#define CApplication_Interface

/************************
 *****  Components  *****
 ************************/

#include "Network.h"

/**************************
 *****  Declarations  *****
 **************************/

class CAgent;
class CConnection;
class CForwardingService;
class CVLinkCmdTarget;


/*************************
 *****  Definitions  *****
 *************************/

class CApplication : public CWinApp
{
    friend CAgent;
    friend CConnection;
    friend CForwardingService;
    friend CVLinkCmdTarget;

    DECLARE_DYNAMIC (CApplication)

//  Globals
    static const DWORD	g_iIdleThreshold;
    static const DWORD	g_iUIDBase;
    static DWORD	g_iUIDSequence;

//  Construction
public:
    CApplication ();

//  Destruction
public:
    ~CApplication();

//  Diagnostics
#ifdef _DEBUG
    void Dump (CDumpContext& dc) const;
#endif

//  Access
public:
    void GetClassName	(CString& s) const;
    void GetUID		(CString& s) const;

    CConnection* LookupConnection (LPCSTR pConnectionId) const;

    CNetwork*	Network () { return &m_iNetwork; }

//  Update
protected:
    void CreateOLEDocumentTemplate (
	REFCLSID	clsid,
	CRuntimeClass *	pDocClass,
	CRuntimeClass *	pViewClass,
	CRuntimeClass *	pFrameClass,
	CRuntimeClass *	pIPFrameClass,
	UINT		nIDResource,
	UINT		nIDSrvrIP,
	UINT		nIDSrvrEmbedded,
	UINT		nIDCntrIP
    );

    void Detach (CForwardingService* pForwardingService);

    void Register (CConnection* pConnection);

    BOOL ShowMainWindow	(int showCmd);

//  Overrides
protected:
    virtual BOOL InitInstance	(void);
    virtual int	 ExitInstance	(void);
    virtual BOOL OnIdle		(long idleSlot);

//  State
protected:
    CObList		m_templateServers;
    BOOL		m_guiInitialized;
    DWORD		m_iIdleThreshold;
    CNetwork		m_iNetwork;
    CMapStringToPtr	m_iNamedObjectMap;
    CForwardingService*	m_pForwardingService;

//  Handlers
protected:
    // Generated message map functions
    //{{AFX_MSG(CApplication)
    afx_msg void OnAppAbout();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    //{{AFX_DISPATCH(CApplication)
    afx_msg float GetTop();
    afx_msg void SetTop(float newValue);
    afx_msg float GetLeft();
    afx_msg void SetLeft(float newValue);
    afx_msg float GetWidth();
    afx_msg void SetWidth(float newValue);
    afx_msg float GetHeight();
    afx_msg void SetHeight(float newValue);
    afx_msg BOOL GetVisible();
    afx_msg void SetVisible(BOOL bNewValue);
    afx_msg BSTR OAGetDefaultFilePath();
    afx_msg void OASetDefaultFilePath(LPCTSTR lpszNewValue);
    afx_msg BSTR OAClassName();
    afx_msg BSTR OAName();
    afx_msg BSTR OAUID();
    afx_msg LPDISPATCH OAApplication();
    afx_msg LPDISPATCH OAParent();
    afx_msg BSTR OAFullName();
    afx_msg BSTR OAPath();
    afx_msg LPDISPATCH OAAgents();
    afx_msg LPDISPATCH OAConnections();
    afx_msg LPDISPATCH OAConnectionTemplates();
    afx_msg LPDISPATCH OARequests();
    afx_msg BOOL OAShowMainWindow(short showCmd);
    afx_msg LPDISPATCH OACreateConnection(LPCSTR hostname, LPCSTR username, LPCSTR password, LPCSTR database, short objectSpace);
    afx_msg LPDISPATCH OACreateStoredConnection(LPCSTR storedTemplateName);
    afx_msg LPDISPATCH OAGetConnection(LPCSTR hostname, LPCSTR username, LPCSTR password, LPCSTR database, short objectSpace);
    afx_msg LPDISPATCH OAGetStoredConnection(LPCSTR storedTemplateName);
    afx_msg long OADefaultConnectTimeout();
    afx_msg long OADefaultRequestTimeout();
    afx_msg LPDISPATCH OACreateDefaultConnection();
    afx_msg LPDISPATCH OAGetDefaultConnection();
    afx_msg LPDISPATCH OACreateAgent();
    afx_msg LPDISPATCH OASavedTemplateNames();
    afx_msg long OAGetProfileLong(LPCSTR sectionName, LPCSTR entryName, long defaultValue);
    afx_msg short OAGetProfileShort(LPCSTR sectionName, LPCSTR entryName, short defaultValue);
    afx_msg BSTR OAGetProfileString(LPCSTR sectionName, LPCSTR entryName, LPCSTR defaultValue);
    afx_msg BOOL OAWriteProfileLong(LPCSTR sectionName, LPCSTR entryName, long newValue);
    afx_msg BOOL OAWriteProfileShort(LPCSTR sectionName, LPCSTR entryName, short newValue);
    afx_msg BOOL OAWriteProfileString(LPCSTR sectionName, LPCSTR entryName, LPCSTR value);
    afx_msg BSTR OAQuoteString(LPCSTR string);
    afx_msg BSTR OAQuoteSymbol(LPCSTR sourceString);
    afx_msg long OATextLimit();
    afx_msg void OARemoveNamedObject(LPCSTR name);
    afx_msg LPDISPATCH OAGetNamedObject(LPCSTR name);
    afx_msg void OASetNamedObject(LPCSTR name, LPDISPATCH newValue);
    //}}AFX_DISPATCH
    DECLARE_DISPATCH_MAP()
};


/****************************
 *****  Global Globals  *****
 ****************************/

extern CApplication theApp;


#endif
