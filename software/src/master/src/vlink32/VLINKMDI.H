// vlinkmdi.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVLinkMDIChild frame with splitter

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CVLinkMDIChild : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CVLinkMDIChild)
protected:
	CVLinkMDIChild();			// protected constructor used by dynamic creation

// Attributes
protected:
	CSplitterWnd	m_wndSplitter;
public:

// Operations
public:

// Implementation
public:
	virtual ~CVLinkMDIChild();
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

	// Generated message map functions
	//{{AFX_MSG(CVLinkMDIChild)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CVLinkMDIChild)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
};

/////////////////////////////////////////////////////////////////////////////
