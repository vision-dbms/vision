// ipframe.h : interface of the CInPlaceFrame class
//

class CInPlaceFrame : public COleIPFrameWnd
{
    DECLARE_DYNCREATE(CInPlaceFrame)
public:
    CInPlaceFrame();

// Attributes
public:

// Operations
public:

// Implementation
public:
    virtual ~CInPlaceFrame();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    virtual BOOL OnCreateControlBars(CWnd* pWndFrame, CWnd* pWndDoc);

protected:
    CToolBar    m_wndToolBar;
    COleResizeBar   m_wndResizeBar;
    COleDropTarget m_dropTarget;

// Generated message map functions
protected:
    //{{AFX_MSG(CInPlaceFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	    // NOTE - the ClassWizard will add and remove member functions here.
	    //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
