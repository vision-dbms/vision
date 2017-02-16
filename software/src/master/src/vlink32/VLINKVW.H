// vlinkvw.h : interface of the CVLinkView class
//
/////////////////////////////////////////////////////////////////////////////

class CVLinkCntrItem;

class CVLinkView : public CScrollView
{
//  Construction
protected: // create from serialization only
	CVLinkView();
	DECLARE_DYNCREATE(CVLinkView)

//  Initialization
protected:
	virtual void OnInitialUpdate();
	virtual void OnUpdate (CView *pSender, LPARAM lHint, CObject *pHint);

// Attributes
public:
	CVLinkDocument* GetDocument();
	// m_pSelection holds the selection to the current CVLinkCntrItem.
	// For many applications, such a member variable isn't adequate to
	//  represent a selection, such as a multiple selection or a selection
	//  of objects that are not CVLinkCntrItem objects.  This selection
	//  mechanism is provided just to help you get started.

	// TODO: replace this selection mechanism with one appropriate to your app.
	CVLinkCntrItem* m_pSelection;

// Operations
public:
	CVLinkCntrItem* PickItem (
		CPoint					point
	);
	
	void SetSelection (
		CVLinkCntrItem			*pItem
	);

	void SetupTracker (
		CVLinkCntrItem *		pItem,
		CRectTracker *			pTracker
	);

// Implementation
public:
	virtual ~CVLinkView();
	virtual void OnDraw(
		CDC	*					pDC
	);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(
		CDumpContext&			dc
	) const;
#endif

protected:

	// Printing support
	virtual BOOL OnPreparePrinting (
		CPrintInfo*				pInfo
	);
	virtual void OnBeginPrinting (
		CDC*					pDC,
		CPrintInfo*				pInfo
	);
	virtual void OnEndPrinting (
		CDC*					pDC,
		CPrintInfo*				pInfo
	);

	// OLE Container support
	virtual BOOL IsSelected (
		const CObject* pDocItem
	) const;

// Generated message map functions
protected:
	//{{AFX_MSG(CVLinkView)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInsertObject();
	afx_msg void OnCancelEdit();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in vlinkvw.cpp
inline CVLinkDocument* CVLinkView::GetDocument(void)
{
	return (CVLinkDocument*)m_pDocument;
}
#endif

/////////////////////////////////////////////////////////////////////////////
