#ifndef CEvaluator_Interface
#define CEvaluator_Interface

/************************
 *****  Components  *****
 ************************/

#include "vlinkcmd.h"

/**************************
 *****  Declarations  *****
 **************************/

class CAgent;
class CHistory;
class CRequest;

/*************************
 *****  Definitions  *****
 *************************/

class CEvaluator : public CVLinkCmdTarget
{
    DECLARE_DYNAMIC(CEvaluator)

// Globals
public:

// Properties
protected:
    CAgent*	const	m_pAgent;
    CHistory*	const	m_pHistory;
    CRequest*		m_pRequest;

// Construction
public:
    CEvaluator (CAgent* pAgent, long initialHistoryLimit = 0);

// Destruction
public:
    ~CEvaluator();

// Diagnostics
#ifdef _DEBUG
public:
    void Dump (CDumpContext& dc) const;
#endif

// Operations
public:
    void Evaluate (LPCSTR expression);

// Handlers
protected:
    // Generated message map functions
    //{{AFX_MSG(CEvaluator)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CEvaluator)
    BOOL m_alwaysEvaluate;
    afx_msg BSTR OAGetRequestText();
    afx_msg void OASetRequestText(LPCSTR lpszNewValue);
    afx_msg BSTR OAGetUnixRequestText();
    afx_msg void OASetUnixRequestText(LPCSTR lpszNewValue);
    afx_msg void OAWaitForReply(long timeout);
    afx_msg BOOL OACompletedWithin(long timeout);
    afx_msg BOOL OACompleted();
    afx_msg BOOL OAInProgress();
    afx_msg BOOL OASucceeded();
    afx_msg BOOL OAFailed();
    afx_msg LPDISPATCH OAReplyBuffer();
    afx_msg LPDISPATCH OAReplyCursor();
    afx_msg BSTR OAReplyText();
    afx_msg BSTR OAUnixReplyText();
    afx_msg LPDISPATCH OARequestBuffer();
    afx_msg LPDISPATCH OARequestCursor();
    afx_msg LPDISPATCH OARequest();
    afx_msg LPDISPATCH OARequestHistory();
    afx_msg long OARequestCount();
    afx_msg LPDISPATCH OAAgent();
    afx_msg long OAGeneration();
    afx_msg BOOL OAReplyTruncated();
    afx_msg BOOL OARequestTruncated();
    //}}AFX_DISPATCH
    DECLARE_DISPATCH_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif

