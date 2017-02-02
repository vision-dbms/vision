// cntritem.h : interface of the CVLinkCntrItem class
//

class CVLinkDocument;
class CVLinkView;

class CVLinkCntrItem : public COleClientItem
{
    DECLARE_SERIAL(CVLinkCntrItem)

// Construction
public:
    CVLinkCntrItem(CVLinkDocument* pContainer = NULL);
	    // Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE.
	    //  IMPLEMENT_SERIALIZE requires the class have a constructor with
	    //  zero arguments.  Normally, OLE items are constructed with a
	    //  non-NULL document pointer.

// Destruction
    ~CVLinkCntrItem();

// Debugging
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

// Attributes
public:
    CVLinkDocument* GetDocument() { return (CVLinkDocument*)COleClientItem::GetDocument(); }
    CVLinkView  * GetActiveView() { return (CVLinkView*)COleClientItem::GetActiveView(); }
    CRect m_rect;

// Operations
public:
    void InvalidateItem ();
    void UpdateExtentFromServer ();

// Implementation
public:
    virtual void Serialize(CArchive& ar);
    virtual void OnGetItemPosition(CRect& rPosition);
    virtual void OnDeactivateUI(BOOL bUndoable);

protected:
    virtual void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam);
    virtual BOOL OnChangeItemPosition(const CRect& rectPos);
    virtual BOOL CanActivate();
};

/////////////////////////////////////////////////////////////////////////////
