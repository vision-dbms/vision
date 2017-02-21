/////////////////////////////////////////////////////////////////////////////
// vlinkdoc.h : CVLinkDocument Interface
/////////////////////////////////////////////////////////////////////////////

//  External References
class CVLinkSrvrItem;

//  Update Hints
#define HINT_UPDATE_WINDOW	0
#define HINT_UPDATE_ITEM	1

class CVLinkDocument : public COleServerDoc
{
// Globals
public:
	static const CLSID BASED_CODE guid;

// Construction
protected: // create from serialization only
	CVLinkDocument (void);
	DECLARE_DYNCREATE (CVLinkDocument)

// Attributes
protected:

public:
	CVLinkSrvrItem* GetEmbeddedItem (void)
	{
		return (CVLinkSrvrItem*)COleServerDoc::GetEmbeddedItem();
	}

protected:	
	CSize m_size;
	
public:
	CSize GetSize (void) { return m_size; }

// Operations
public:

// Implementation
public:
	virtual ~CVLinkDocument (void);
	virtual void Serialize (CArchive& ar);
	virtual COleServerItem* OnGetEmbeddedItem (void);
#ifdef _DEBUG
	virtual void AssertValid (void) const;
	virtual void Dump (CDumpContext& dc) const;
#endif

protected:
	virtual BOOL OnNewDocument (void);

// Generated message map functions
protected:
	//{{AFX_MSG(CVLinkDocument)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CVLinkDocument)
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
};

/////////////////////////////////////////////////////////////////////////////
