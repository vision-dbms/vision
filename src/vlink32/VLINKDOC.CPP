/////////////////////////////////////////////////////////////////////////////
//
//  CVLinkDocument Implementation
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
#include "srvritem.h"


/////////////////////////////////////////////////////////////////////////////
//  Diagnostics
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

void CVLinkDocument::AssertValid(void) const
{
	COleServerDoc::AssertValid();
}

void CVLinkDocument::Dump(CDumpContext& dc) const
{
	COleServerDoc::Dump(dc);
}
#endif


/////////////////////////////////////////////////////////////////////////////
//  Message Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CVLinkDocument, COleServerDoc)
	//{{AFX_MSG_MAP(CVLinkDocument)
	//}}AFX_MSG_MAP
	// Enable default OLE container implementation
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, COleServerDoc::OnUpdatePasteMenu)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_LINK, COleServerDoc::OnUpdatePasteLinkMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, COleServerDoc::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, COleServerDoc::OnEditLinks)
	ON_UPDATE_COMMAND_UI(ID_OLE_VERB_FIRST, COleServerDoc::OnUpdateObjectVerbMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_CONVERT, COleServerDoc::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_OLE_EDIT_CONVERT, COleServerDoc::OnEditConvert)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//  Dispatch Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_DISPATCH_MAP(CVLinkDocument, COleServerDoc)
	//{{AFX_DISPATCH_MAP(CVLinkDocument)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

/////////////////////////////////////////////////////////////////////////////
//  OLE UUID Data
/////////////////////////////////////////////////////////////////////////////

const CLSID BASED_CODE CVLinkDocument::guid = {
	0xfd65d2a0, 0x6c06, 0x101b, { 0xb9, 0xa0, 0x2, 0x60, 0x8c, 0xac, 0xd3, 0x7a }
};


/////////////////////////////////////////////////////////////////////////////
//  Construction
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CVLinkDocument, COleServerDoc)

CVLinkDocument::CVLinkDocument(void)
{
	// For most containers, using compound files is a good idea.
	EnableCompoundFile();

	EnableAutomation();

	// TODO: add one-time construction code here
	m_size = CSize (32000, 32000);

	AfxOleLockApp();
}

/////////////////////////////////////////////////////////////////////////////
//  Destruction
/////////////////////////////////////////////////////////////////////////////

CVLinkDocument::~CVLinkDocument(void)
{
	AfxOleUnlockApp();
}

BOOL CVLinkDocument::OnNewDocument(void)
{
	if (!COleServerDoc::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//  Serialization
/////////////////////////////////////////////////////////////////////////////

void CVLinkDocument::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}

	// Calling the base class COleServerDoc enables serialization
	//  of the container document's COleClientItem objects.
	COleServerDoc::Serialize(ar);
}


/////////////////////////////////////////////////////////////////////////////
//  Handlers
/////////////////////////////////////////////////////////////////////////////

COleServerItem* CVLinkDocument::OnGetEmbeddedItem(void)
{
	// OnGetEmbeddedItem is called by the framework to get the COleServerItem
	//  that is associated with the document.  It is only called when necessary.

	CVLinkSrvrItem* pItem = new CVLinkSrvrItem(this);
	ASSERT_VALID(pItem);
	return pItem;
}
