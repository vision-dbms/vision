#ifndef CHElement_Interface
#define CHElement_Interface

/************************
 *****  Components  *****
 ************************/

#include "vlinkcmd.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class CHElement : public CVLinkCmdTarget
{
    friend class CHistory;

    DECLARE_DYNAMIC(CHElement)

// Globals
public:

// Properties
protected:
    CVLinkCmdTarget* const	m_pObject;
    CHElement*				m_pPredecessor;
    CHElement*				m_pSuccessor;
    unsigned long			m_position;

// Construction
public:
    CHElement (CVLinkCmdTarget* pObject);

// Destruction
public:
    ~CHElement();

// Diagnostics
#ifdef _DEBUG
public:
    void Dump (CDumpContext& dc) const;
#endif

// Operations
public:
    void	GetSynopsis	(CString& synopsis) const { m_pObject->GetSynopsis (synopsis); }

    BOOL	IsNewest	() const { return NULL == m_pSuccessor; }
    BOOL	IsOldest	() const { return NULL == m_pPredecessor; }

    void	Link		(CHElement** ppNewestElement, CHElement** ppOldestElement, unsigned int &counter);
    CHElement*	Unlink		(CHElement** ppNewestElement, CHElement** ppOldestElement);

// Handlers
protected:
    // Generated message map functions
    //{{AFX_MSG(CHElement)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CHElement)
    afx_msg LPDISPATCH OAObject();
    afx_msg LPDISPATCH OAPredecessor();
    afx_msg LPDISPATCH OASuccessor();
    afx_msg long OAPosition();
    afx_msg BOOL OAIsNewest();
    afx_msg BOOL OAIsOldest();
    //}}AFX_DISPATCH
    DECLARE_DISPATCH_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif

