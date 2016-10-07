/////////////////////////////////////////////////////////////////////////////
// vlinkvw.cpp : implementation of the CVLinkView class
/////////////////////////////////////////////////////////////////////////////

//  System...
#include "stdafx.h"
#include "stdext.h"

#include "resource.h"

//  Supporting...
#include "vlinkdoc.h"
#include "cntritem.h"

//  Self...
#include "vlinkvw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVLinkView
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CVLinkView, CScrollView)

BEGIN_MESSAGE_MAP(CVLinkView, CScrollView)
    //{{AFX_MSG_MAP(CVLinkView)
    ON_WM_SETFOCUS()
    ON_WM_SIZE()
    ON_COMMAND(ID_OLE_INSERT_NEW, OnInsertObject)
    ON_COMMAND(ID_CANCEL_EDIT, OnCancelEdit)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONDBLCLK()
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
    ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
    ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
    ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
    //}}AFX_MSG_MAP
    // Standard printing commands
    ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVLinkView construction/destruction
/////////////////////////////////////////////////////////////////////////////

CVLinkView::CVLinkView(void)
{
    // TODO: add construction code here
}

CVLinkView::~CVLinkView(void)
{
}

/////////////////////////////////////////////////////////////////////////////
//  OnDraw
/////////////////////////////////////////////////////////////////////////////

void CVLinkView::OnDraw(CDC* pDC)
{
    CVLinkDocument* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    // TODO: add draw code for native data here
    // TODO: also draw all OLE items in the document

    POSITION pos = pDoc->GetStartPosition();
    while (pos)
    {
	    // Draw the item
	CVLinkCntrItem *pItem = (CVLinkCntrItem *)pDoc->GetNextItem (pos);
	pItem->Draw (pDC, pItem->m_rect);

	// Draw the tracker
	CRectTracker tracker;
	SetupTracker (pItem, &tracker);
	tracker.Draw (pDC);
    }
}

void CVLinkView::OnInitialUpdate(void)
{
    CScrollView::OnInitialUpdate();

    SetScrollSizes (MM_TEXT, GetDocument()->GetSize());

    // TODO: remove this code when final selection model code is written
    m_pSelection = NULL;    // initialize selection
}

void CVLinkView::OnUpdate (CView *pSender, LPARAM lHint, CObject *pHint)
{
    switch (lHint)
    {
    case HINT_UPDATE_WINDOW:
	Invalidate ();
	break;
    case HINT_UPDATE_ITEM:
	{
		CRectTracker tracker;
		SetupTracker ((CVLinkCntrItem *)pHint, &tracker);
		CRect rect;
		tracker.GetTrueRect (rect);
		InvalidateRect (rect);
	}
	break;
    }
}

/////////////////////////////////////////////////////////////////////////////
// CVLinkView printing
/////////////////////////////////////////////////////////////////////////////

BOOL CVLinkView::OnPreparePrinting(CPrintInfo* pInfo)
{
    // default preparation
    return DoPreparePrinting(pInfo);
}

void CVLinkView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    // TODO: add extra initialization before printing
}

void CVLinkView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    // TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
//  Item Selection Support
/////////////////////////////////////////////////////////////////////////////
                      
CVLinkCntrItem *CVLinkView::PickItem (CPoint point)
{
    CVLinkDocument *pDoc	= GetDocument ();
    CVLinkCntrItem *pItemHit	= NULL;
    POSITION pos = pDoc->GetStartPosition ();
    while (pos)
    {
	CVLinkCntrItem *pItem = (CVLinkCntrItem *)pDoc->GetNextItem (pos);
	if (pItem->m_rect.PtInRect (point))
	    pItemHit = pItem;
    }
    return pItemHit;
}

void CVLinkView::SetSelection (CVLinkCntrItem *pItem)
{
    if (NULL == pItem || pItem != m_pSelection)
    {
	//  Close the current active item, ...
	COleClientItem *pActiveItem = GetDocument()->GetInPlaceActiveItem (this);
	if (pActiveItem && pItem != pActiveItem)
	    pActiveItem->Close ();
	
	//  ... and switch to the new item.
	if (m_pSelection != pItem)
	{
	    if (m_pSelection)
		OnUpdate (NULL, HINT_UPDATE_ITEM, m_pSelection);
	    m_pSelection = pItem;
	    if (m_pSelection)
		OnUpdate (NULL, HINT_UPDATE_ITEM, m_pSelection);
	}
    }
}

void CVLinkView::SetupTracker (CVLinkCntrItem *pItem, CRectTracker *pTracker)
{
    pTracker->m_rect = pItem->m_rect;
    if (pItem == m_pSelection)
	pTracker->m_nStyle |= CRectTracker::resizeInside;

    switch (pItem->GetType ())
    {
    case OT_LINK:
	pTracker->m_nStyle |= CRectTracker::dottedLine;
	break;
    default:
	pTracker->m_nStyle |= CRectTracker::solidLine;
	break;
    }

    switch (pItem->GetItemState ())
    {
    case COleClientItem::openState:
    case COleClientItem::activeUIState:
	pTracker->m_nStyle |= CRectTracker::hatchInside;
	break;
    default:
	break;
    }
}

/////////////////////////////////////////////////////////////////////////////
// OLE Client support and commands
/////////////////////////////////////////////////////////////////////////////

BOOL CVLinkView::IsSelected(const CObject* pDocItem) const
{
    // The implementation below is adequate if your selection consists of
    //  only CVLinkCntrItem objects.  To handle different selection
    //  mechanisms, the implementation here should be replaced.

    // TODO: implement this function that tests for a selected OLE client item

    return pDocItem == m_pSelection;
}

void CVLinkView::OnInsertObject()
{
    // Invoke the standard Insert Object dialog box to obtain information
    //  for new CVLinkCntrItem object.
    COleInsertDialog dlg;
    if (dlg.DoModal() != IDOK)
	return;

    BeginWaitCursor();

    CVLinkCntrItem* pItem = NULL;
    TRY {
	// Create new item connected to this document.
	CVLinkDocument* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pItem = new CVLinkCntrItem(pDoc);
	ASSERT_VALID(pItem);

	// Initialize the item from the dialog data.
	if (!dlg.CreateItem(pItem))
	    AfxThrowMemoryException();  // any exception will do
	ASSERT_VALID(pItem);

	pItem->UpdateLink();
	pItem->UpdateExtentFromServer ();

	// If item created from class list (not from file) then launch
	//  the server to edit the item.
	if (dlg.GetSelectionType() == COleInsertDialog::createNewItem)
	    pItem->DoVerb(OLEIVERB_SHOW, this);

	ASSERT_VALID(pItem);

	// As an arbitrary user interface design, this sets the selection
	//  to the last item inserted.

	SetSelection (pItem);
	pItem->InvalidateItem ();
    } CATCH(CException, e) {
	if (pItem)
	{
	    ASSERT_VALID(pItem);
	    pItem->Delete();
	}
	AfxMessageBox(IDP_FAILED_TO_CREATE);
    } END_CATCH

    EndWaitCursor();
}

// The following command handler provides the standard keyboard
//  user interface to cancel an in-place editing session.
void CVLinkView::OnCancelEdit()
{
    // Close any in-place active item on this view.
    COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
    if (pActiveItem)
    {
	pActiveItem->Close();
    }
    ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
}

// Special handling of OnSetFocus and OnSize are required for a container
//  when an object is being edited in-place.
void CVLinkView::OnSetFocus(CWnd* pOldWnd)
{
    COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
    if (pActiveItem && pActiveItem->GetItemState() == COleClientItem::activeUIState)
    {
	// need to set focus to this item if it is in the same view
	CWnd* pWnd = pActiveItem->GetInPlaceWindow();
	if (pWnd)
	{
	    pWnd->SetFocus();   // don't call the base class
	    return;
	}
    }

    CScrollView::OnSetFocus(pOldWnd);
}

void CVLinkView::OnSize(UINT nType, int cx, int cy)
{
    CScrollView::OnSize(nType, cx, cy);
    COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
    if (pActiveItem)
	pActiveItem->SetItemRects();
}

/////////////////////////////////////////////////////////////////////////////
// CVLinkView diagnostics

#ifdef _DEBUG
void CVLinkView::AssertValid() const
{
    CScrollView::AssertValid();
}

void CVLinkView::Dump(CDumpContext& dc) const
{
    CScrollView::Dump(dc);
}

CVLinkDocument* CVLinkView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVLinkDocument)));
    return (CVLinkDocument*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVLinkView message handlers

void CVLinkView::OnLButtonDown(UINT nFlags, CPoint point)
{
    CVLinkCntrItem *pItemHit = PickItem (point);
    SetSelection (pItemHit);

    if (pItemHit)
    {
	CRectTracker tracker;
	SetupTracker (pItemHit, &tracker);
	UpdateWindow ();
	if (tracker.Track (this, point))
	{
	    pItemHit->InvalidateItem ();
	    pItemHit->m_rect = tracker.m_rect;
	    pItemHit->InvalidateItem ();

	    GetDocument()->SetModifiedFlag ();
	}
    }
    
    CScrollView::OnLButtonDown(nFlags, point);
}

void CVLinkView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    OnLButtonDown (nFlags, point);
    
    if (m_pSelection) m_pSelection->DoVerb (
	GetKeyState (VK_CONTROL) < 0 ? OLEIVERB_OPEN : OLEIVERB_PRIMARY, this
    );

    CScrollView::OnLButtonDblClk(nFlags, point);
}

void CVLinkView::OnEditCopy()
{
    if (m_pSelection)
	m_pSelection->CopyToClipboard ();
}

void CVLinkView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
    pCmdUI->Enable (m_pSelection != NULL);
}

void CVLinkView::OnEditPaste()
{
    CVLinkCntrItem *pItem = NULL;

    TRY {
	//  Create a new item, ...
	CVLinkDocument *pDoc = GetDocument ();
	ASSERT_VALID (pDoc);
	pItem = new CVLinkCntrItem (pDoc);
	ASSERT_VALID (pItem);
	
	//  ... initialize it from the clipboard, ...
	if (!pItem->CreateFromClipboard ())
	    AfxThrowUserException ();
	ASSERT_VALID (pItem);
	
	// ... update its size, ...
	pItem->UpdateExtentFromServer ();
	
	// ... and select it.
	SetSelection (pItem);
	pItem->InvalidateItem ();
    } CATCH (CException, e) {
	if (pItem)
	{
	    ASSERT_VALID(pItem);
	    pItem->Delete ();
	}
	AfxMessageBox (IDP_FAILED_TO_CREATE);
    } END_CATCH	
}

void CVLinkView::OnEditClear()
{
    if (m_pSelection)
    {
	m_pSelection->Delete ();
	m_pSelection = NULL;
	GetDocument()->UpdateAllViews (NULL);
    }
}

void CVLinkView::OnUpdateEditClear(CCmdUI* pCmdUI)
{
    pCmdUI->Enable (m_pSelection != NULL);
}
