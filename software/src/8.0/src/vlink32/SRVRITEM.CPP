/////////////////////////////////////////////////////////////////////////////
//
//  CVLinkSrvrItem Implementation
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//  Interfaces
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdext.h"

#include "vlinkapp.h"

#include "vlinkdoc.h"
#include "srvritem.h"
#include "cntritem.h"


/////////////////////////////////////////////////////////////////////////////
//  Diagnostics
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

void CVLinkSrvrItem::AssertValid() const
{
    COleServerItem::AssertValid();
}

void CVLinkSrvrItem::Dump(CDumpContext& dc) const
{
    COleServerItem::Dump(dc);
}
#endif


/////////////////////////////////////////////////////////////////////////////
//  Construction
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CVLinkSrvrItem, COleServerItem)

CVLinkSrvrItem::CVLinkSrvrItem(CVLinkDocument* pContainerDoc)
    : COleServerItem(pContainerDoc, TRUE)
{
    // TODO: add one-time construction code here
    //  (eg, adding additional clipboard formats to the item's data source)
}

/////////////////////////////////////////////////////////////////////////////
//  Destruction
/////////////////////////////////////////////////////////////////////////////

CVLinkSrvrItem::~CVLinkSrvrItem()
{
    // TODO: add cleanup code here
}

/////////////////////////////////////////////////////////////////////////////
//  Serialization
/////////////////////////////////////////////////////////////////////////////

void CVLinkSrvrItem::Serialize(CArchive& ar)
{
    // CVLinkSrvrItem::Serialize will be called by the framework if
    //  the item is copied to the clipboard.  This can happen automatically
    //  through the OLE callback OnGetClipboardData.  A good default for
    //  the embedded item is simply to delegate to the document's Serialize
    //  function.  If you support links, then you will want to serialize
    //  just a portion of the document.

    if (!IsLinkedItem())
    {
	CVLinkDocument* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->Serialize(ar);
    }
}


/////////////////////////////////////////////////////////////////////////////
//  Operations
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//  Handlers
/////////////////////////////////////////////////////////////////////////////

BOOL CVLinkSrvrItem::OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize)
{
    // Most applications, like this one, only handle drawing the content
    //  aspect of the item.  If you wish to support other aspects, such
    //  as DVASPECT_THUMBNAIL (by overriding OnDrawEx), then this
    //  implementation of OnGetExtent should be modified to handle the
    //  additional aspect(s).

    if (dwDrawAspect != DVASPECT_CONTENT)
	return COleServerItem::OnGetExtent(dwDrawAspect, rSize);

    // CVLinkSrvrItem::OnGetExtent is called to get the extent in
    //  HIMETRIC units of the entire item.  The default implementation
    //  here simply returns a hard-coded number of units.

    CVLinkDocument* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    // TODO: replace this arbitrary size

    rSize = CSize(3000, 3000);   // 3000 x 3000 HIMETRIC units

    return TRUE;
}

BOOL CVLinkSrvrItem::OnDraw(CDC* pDC, CSize& rSize)
{
    CVLinkDocument* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    // TODO: set mapping mode and extent
    //  (The extent is usually the same as the size returned from OnGetExtent)
    pDC->SetMapMode(MM_ANISOTROPIC);
    pDC->SetWindowOrg(0,0);
    pDC->SetWindowExt(3000, 3000);

    // TODO: add drawing code here.  Optionally, fill in the HIMETRIC extent.
    //  All drawing takes place in the metafile device context (pDC).

    // TODO: also draw embedded CVLinkCntrItem objects.

    // The following code draws the first item at an arbitrary position.

    // TODO: remove this code when your real drawing code is complete

    POSITION pos = pDoc->GetStartPosition();
    CVLinkCntrItem* pItem = (CVLinkCntrItem*)pDoc->GetNextClientItem(pos);
    if (pItem)
	pItem->Draw(pDC, CRect(10, 10, 1010, 1010));
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
