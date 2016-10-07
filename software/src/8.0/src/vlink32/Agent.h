#ifndef CAgent_Interface
#define CAgent_Interface

/************************
 *****  Components  *****
 ************************/

#include "vlinkcmd.h"
#include "Collection.h"

/**************************
 *****  Declarations  *****
 **************************/

class CConnectionTemplate;
class CConnectionData;
class CConnection;
class CHistory;
class CRequest;

/*************************
 *****  Definitions  *****
 *************************/

class CAgent : public CVLinkCmdTarget
{
    DECLARE_DYNCREATE(CAgent)

// Globals
public:
    static CCollectionKernel	Instances;

// Properties
protected:
    CConnection*		m_pConnection;
    CHistory*			m_pRequestHistory;
    CMapStringToOb		m_nameMap;

// Construction
public:
    CAgent (CConnection *pConnection = NULL);

// Destruction
public:
    ~CAgent ();

// Diagnostics

// Operations
public:
    CHistory* RequestHistory () const { return m_pRequestHistory; }

    CConnectionTemplate* CreateConnectionTemplate ();

    CConnection* CreateConnection (
	LPCSTR hostname, LPCSTR username, LPCSTR password, LPCSTR database, short objectSpace
    );
    CConnection* CreateConnection (
	LPCSTR standardConnectionName
    );
    CConnection* CreateConnection (
	CConnectionData& connectionData
    );

    CConnection* GetConnection (
	LPCSTR hostname, LPCSTR username, LPCSTR password, LPCSTR database, short objectSpace
    );
    CConnection* GetConnection (
	LPCSTR standardConnectionName
    );
    CConnection* GetConnection (
	CConnectionData& connectionData
    );

    BOOL IsUpWithin (long timeout) { return OAIsUpWithin (timeout); }

    CConnection* SetConnection (CConnection* pConnection);

    CRequest* Submit (CString& request) { return Submit (request, request.GetLength ()); }
    CRequest* Submit (LPCSTR   request) { return Submit (request, strlen (request)); }
    CRequest* Submit (LPCSTR   request, int requestLength);

// Handlers
protected:
	// Generated message map functions
	//{{AFX_MSG(CAgent)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	DECLARE_OLECREATE(CAgent)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CAgent)
	afx_msg LPDISPATCH OACreateConnectionTemplate();
	afx_msg LPDISPATCH OACreateConnection(LPCSTR hostname, LPCSTR username, LPCSTR password, LPCSTR database, short objectSpace);
	afx_msg LPDISPATCH OACreateDefaultConnection();
	afx_msg LPDISPATCH OACreateStoredConnection(LPCSTR standardConnectionName);
	afx_msg LPDISPATCH OAGetConnection(LPCSTR hostname, LPCSTR username, LPCSTR password, LPCSTR database, short objectSpace);
	afx_msg LPDISPATCH OAGetDefaultConnection();
	afx_msg LPDISPATCH OAGetStoredConnection(LPCSTR standardConnectionName);
	afx_msg BOOL OAIsUpWithin(long timeout);
	afx_msg BOOL OAIsUp();
	afx_msg BOOL OAIsDown();
	afx_msg LPDISPATCH OASubmit(LPCSTR request);
	afx_msg LPDISPATCH OAConnection();
	afx_msg long OADefaultConnectTimeout();
	afx_msg long OADefaultRequestTimeout();
	afx_msg long OARequestCount();
	afx_msg LPDISPATCH OARequestHistory();
	afx_msg void WaitForConnection(long timeout);
	afx_msg BOOL OAStarted();
	afx_msg BOOL OAStopped();
	afx_msg LPDISPATCH OACreateEvaluator();
	afx_msg LPDISPATCH OAGetEvaluator(LPCSTR name);
	afx_msg LPDISPATCH OAGetNamedObject(LPCSTR name);
	afx_msg BOOL OACompletedWithin(long timeout);
	afx_msg BOOL OAFailed();
	afx_msg BSTR OAQuoteString(LPCSTR string);
	afx_msg BSTR OAQuoteSymbol(LPCSTR string);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
};

#endif
