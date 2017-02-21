#ifndef CConnectionTemplate_Interface
#define CConnectionTemplate_Interface

/************************
 *****  Components  *****
 ************************/

#include "vlinkcmd.h"
#include "Collection.h"
#include "ConnectionData.h"

/**************************
 *****  Declarations  *****
 **************************/

class CAgent;
class CConnection;

/*************************
 *****  Definitions  *****
 *************************/

class CConnectionTemplate : public CVLinkCmdTarget
{
    DECLARE_DYNAMIC(CConnectionTemplate)

// Globals
public:
    static CCollectionKernel	Instances;

// Properties
protected:
    CAgent* const		m_pAgent;
    CConnectionData		m_templateData;
	
// Construction
public:
    CConnectionTemplate (CAgent* pAgent);

// Destruction
public:
    ~CConnectionTemplate ();

// Diagnostics
#ifdef _DEBUG
public:
    void Dump (CDumpContext& dc) const;
#endif

// Operations
public:
    CConnection* CreateConnection ();
    CConnection* GetConnection ();

// Handlers
protected:
    // Generated message map functions
    //{{AFX_MSG(CConnectionTemplate)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

//	DECLARE_OLECREATE (CConnectionTemplate);

    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CConnectionTemplate)
    afx_msg BSTR OAGetDatabase();
    afx_msg void OASetDatabase(LPCSTR lpszNewValue);
    afx_msg BSTR OAGetExecutable();
    afx_msg void OASetExecutable(LPCSTR lpszNewValue);
    afx_msg BSTR OAGetHostname();
    afx_msg void OASetHostname(LPCSTR lpszNewValue);
    afx_msg short OAGetObjectSpace();
    afx_msg void OASetObjectSpace(short nNewValue);
    afx_msg BSTR OAGetOptions();
    afx_msg void OASetOptions(LPCSTR lpszNewValue);
    afx_msg BSTR OAGetPassword();
    afx_msg void OASetPassword(LPCSTR lpszNewValue);
    afx_msg long OAGetTimeout();
    afx_msg void OASetTimeout(long nNewValue);
    afx_msg BSTR OAGetUsername();
    afx_msg void OASetUsername(LPCSTR lpszNewValue);
    afx_msg BSTR OACommand();
    afx_msg BSTR OADescription();
    afx_msg LPDISPATCH OAAgent();
    afx_msg LPDISPATCH OACreateConnection();
    afx_msg LPDISPATCH OAGetConnection();
    afx_msg BOOL OALoadDefaultTemplate();
    afx_msg BOOL OALoadTemplate(LPCSTR templateName);
    afx_msg BOOL OAStoreDefaultTemplate();
    afx_msg BOOL OAStoreTemplate(LPCSTR templateName);
    //}}AFX_DISPATCH
    DECLARE_DISPATCH_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif
