// srvritem.h : interface of the CVLinkSrvrItem class
//

class CVLinkSrvrItem : public COleServerItem
{
	DECLARE_DYNAMIC(CVLinkSrvrItem)

// Constructors
public:
	CVLinkSrvrItem(CVLinkDocument* pContainerDoc);

// Attributes
	CVLinkDocument* GetDocument() const
		{ return (CVLinkDocument*)COleServerItem::GetDocument(); }

// Implementation
public:
	~CVLinkSrvrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual BOOL OnDraw(CDC* pDC, CSize& rSize);
	virtual BOOL OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize);

protected:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
};

/////////////////////////////////////////////////////////////////////////////
