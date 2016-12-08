#ifndef CHistory_Interface
#define CHistory_Interface

/************************
 *****  Components  *****
 ************************/

#include "vlinkcmd.h"

/**************************
 *****  Declarations  *****
 **************************/

class CHElement;

/*************************
 *****  Definitions  *****
 *************************/

class CHistory : public CVLinkCmdTarget
{
    DECLARE_DYNAMIC(CHistory)

// Globals
public:

// Properties
protected:
    unsigned int		m_count;
    unsigned int		m_limit;
    unsigned int		m_size;
    CHElement*			m_pOldest;
    CHElement*			m_pNewest;
    CHElement*			m_pCurrent;

// Construction
public:
    CHistory (long initialHistoryLimit = 0);

// Destruction
public:
    ~CHistory();

// Diagnostics
#ifdef _DEBUG
public:
    void Dump (CDumpContext& dc) const;
#endif

// Operations
public:
    void 		GetSynopsis	(CString& synopsis) const;

    unsigned int	Count		() const { return m_count; }
    unsigned int	Limit		() const { return m_limit; }
    unsigned int	Size		() const { return m_size ; }

    CHElement*		Newest		() const { return m_pNewest; }
    CHElement*		Oldest		() const { return m_pOldest; }

    CHElement*		NthNewest	(unsigned int index) const;
    CHElement*		NthOldest	(unsigned int index) const;

    BOOL		SetLimit	(unsigned int limit);
    BOOL		Append		(CVLinkCmdTarget* pObject);

// Handlers
protected:
    // Generated message map functions
    //{{AFX_MSG(CHistory)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CHistory)
    afx_msg long OAGetLimit();
    afx_msg void OASetLimit(long nNewValue);
    afx_msg long OACount();
    afx_msg long OASize();
    afx_msg LPDISPATCH OANewest();
    afx_msg LPDISPATCH OAOldest();
    afx_msg LPDISPATCH OANthNewest(long index);
    afx_msg LPDISPATCH OANthOldest(long index);
    //}}AFX_DISPATCH
    DECLARE_DISPATCH_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif
