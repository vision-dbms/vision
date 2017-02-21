/////////////////////////////////////////////////////////////////////////////
//
//  CVLinkCntrItem Implementation
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//  Interfaces
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdext.h"

#include "vlinkapp.h"

#include "vlinkdoc.h"
#include "cntritem.h"


/////////////////////////////////////////////////////////////////////////////
//  Diagnostics
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

void CVLinkCntrItem::AssertValid() const
{
	COleClientItem::AssertValid();
}

void CVLinkCntrItem::Dump(CDumpContext& dc) const
{
	COleClientItem::Dump(dc);
}
#endif


/////////////////////////////////////////////////////////////////////////////
//  Construction
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CVLinkCntrItem, COleClientItem, 0)

CVLinkCntrItem::CVLinkCntrItem(CVLinkDocument* pContainer)
	: COleClientItem(pContainer)
{
	m_rect.SetRect (10, 10, 50, 50);
}

/////////////////////////////////////////////////////////////////////////////
//  Destruction
/////////////////////////////////////////////////////////////////////////////

CVLinkCntrItem::~CVLinkCntrItem()
{
	// TODO: add cleanup code here
}


/////////////////////////////////////////////////////////////////////////////
//  Operations
/////////////////////////////////////////////////////////////////////////////

void CVLinkCntrItem::InvalidateItem ()
{
	GetDocument()->UpdateAllViews (NULL, HINT_UPDATE_ITEM, this);
}

void CVLinkCntrItem::UpdateExtentFromServer ()
{
	CSize size;
	if (GetExtent (&size))
	{
		CClientDC dc(NULL);
		dc.HIMETRICtoDP(&size);
		
		if (size != m_rect.Size ())
		{
			InvalidateItem ();
			m_rect.bottom = m_rect.top + size.cy;
			m_rect.right = m_rect.left + size.cx;
			InvalidateItem ();
			
			GetDocument()->SetModifiedFlag();
		}
	}
}

void CVLinkCntrItem::OnChange(OLE_NOTIFICATION nCode, DWORD dwParam)
{
	ASSERT_VALID(this);

	COleClientItem::OnChange(nCode, dwParam);

	// When an item is being edited (either in-place or fully open)
	//  it sends OnChange notifications for changes in the state of the
	//  item or visual appearance of its content.

	switch (nCode)
	{
	case OLE_CHANGED:
		InvalidateItem ();
		UpdateExtentFromServer ();
		break;
	case OLE_CHANGED_STATE:
	case OLE_CHANGED_ASPECT:
		InvalidateItem ();
		break;
	}
}

BOOL CVLinkCntrItem::OnChangeItemPosition(const CRect& rectPos)
{
	ASSERT_VALID(this);

	// During in-place activation CVLinkCntrItem::OnChangeItemPosition
	//  is called by the server to change the position on of the in-place
	//  window.  Usually, this is a result of the data in the server
	//  document changing such that the extent has changed or as a result
	//  of in-place resizing.
	//
	// The default here is to call the base class, which will call
	//  COleClientItem::SetItemRects to move the item
	//  to the new position.

	if (!COleClientItem::OnChangeItemPosition(rectPos))
		return FALSE;

	m_rect = rectPos;
	InvalidateItem ();
	m_rect = rectPos;
	InvalidateItem ();

	GetDocument()->SetModifiedFlag();

	return TRUE;
}

void CVLinkCntrItem::OnGetItemPosition(CRect& rPosition)
{
	ASSERT_VALID(this);

	// During in-place activation, CVLinkCntrItem::OnGetItemPosition
	//  will be called to determine the location of this item.  The default
	//  implementation created from AppWizard simply returns a hard-coded
	//  rectangle.  Usually, this rectangle would reflect the current
	//  position of the item relative to the view used for activation.
	//  You can obtain the view by calling CVLinkCntrItem::GetActiveView.

	rPosition = m_rect;
}

void CVLinkCntrItem::OnDeactivateUI(BOOL bUndoable)
{
	COleClientItem::OnDeactivateUI(bUndoable);

	// Close an in-place active item whenever it removes the user
	//  interface.  The action here should match as closely as possible
	//  to the handling of the escape key in the view.

	Deactivate();   // nothing fancy here -- just deactivate the object
}

void CVLinkCntrItem::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);

	// Call base class first to read in COleClientItem data.
	// Since this sets up the m_pDocument pointer returned from
	//  CVLinkCntrItem::GetDocument, it is a good idea to call
	//  the base class Serialize first.
	COleClientItem::Serialize(ar);

	// now store/retrieve data specific to CVLinkCntrItem
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

BOOL CVLinkCntrItem::CanActivate()
{
	// Editing in-place while the server itself is being edited in-place
	//  does not work and is not supported.  So, disable in-place
	//  activation in this case.
	CVLinkDocument* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(COleServerDoc)));
	if (pDoc->IsInPlaceActive())
		return FALSE;

	// otherwise, rely on default behavior
	return COleClientItem::CanActivate();
}

/////////////////////////////////////////////////////////////////////////////
