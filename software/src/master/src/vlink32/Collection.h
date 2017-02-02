#ifndef CCollection_Interface
#define CCollection_Interface

#include "vlinkcmd.h"

////////////////
//  Collection Kernel
////////////////

class CCollectionKernel : public CObject
{
    friend class CCollection;

    DECLARE_DYNAMIC(CCollectionKernel)

// Properties
protected:
    unsigned long		m_count;
    CVLinkCmdTarget*		m_pOldestItem;
    CVLinkCmdTarget*		m_pNewestItem;
    CMapStringToOb		m_nameMap;
    CCollection*		m_pNewestEnumerator;
    CVLinkCmdTarget*		m_pCachedItem;
    unsigned long		m_cachedIndex;

// Construction
public:
    CCollectionKernel ();

// Destruction
public:
    ~CCollectionKernel ();

// Diagnostics
#ifdef _DEBUG
public:
    void Dump (CDumpContext& dc) const;
#endif

// Operations
public:
    unsigned long	Count		() const { return m_count; }
    CVLinkCmdTarget*	OldestItem	() const { return m_pOldestItem; }
    CVLinkCmdTarget*	NewestItem	() const { return m_pNewestItem; }

    CVLinkCmdTarget*	Item		(VARIANT FAR& index);
    CVLinkCmdTarget*	Item		(long index);
    CVLinkCmdTarget*	Item		(BSTR index);
    CVLinkCmdTarget*	Item		(LPCSTR index);

    void		AddItem		(CVLinkCmdTarget *pItem);
    void 		DeleteItem	(CVLinkCmdTarget *pItem);
    
    CCollection*	Collection	();
    LPDISPATCH		OACollection	();
};

////////////////
//  Collection
////////////////

class CCollection : public CVLinkCmdTarget
{
    friend class CCollectionKernel;

    DECLARE_DYNAMIC(CCollection)

// Properties
protected:
    CCollectionKernel* const	m_pKernel;
    CCollection*		m_pPrevEnumerator;
    CCollection*		m_pNextEnumerator;
    CVLinkCmdTarget**		m_ppNextElement;

// Construction
protected:
    CCollection (CCollectionKernel *pKernel, CVLinkCmdTarget** ppNextElement);

// Destruction
public:
    ~CCollection ();

// Diagnostics
#ifdef _DEBUG
public:
    void Dump (CDumpContext& dc) const;
#endif

// Operations
public:
    unsigned long	Count		() const { return m_pKernel->Count	(); }
    CVLinkCmdTarget*	OldestItem	() const { return m_pKernel->OldestItem	(); }
    CVLinkCmdTarget*	NewestItem	() const { return m_pKernel->NewestItem	(); }

    CVLinkCmdTarget*	Item		(VARIANT FAR&	index) const { return m_pKernel->Item (index); }
    CVLinkCmdTarget*	Item		(long		index) const { return m_pKernel->Item (index); }
    CVLinkCmdTarget*	Item		(LPCSTR		index) const { return m_pKernel->Item (index); }

protected:
    void		Fix		(CVLinkCmdTarget* pDeletedItem);

    HRESULT 		Next		(unsigned long celt, VARIANT FAR* rgvar, unsigned long FAR* pceltFetched);
    HRESULT 		Skip		(unsigned long celt);
    HRESULT 		Reset		();
    HRESULT 		Clone		(IEnumVARIANT FAR* FAR* ppenum);

// Handlers
protected:
	// Generated message map functions
	//{{AFX_MSG(CCollection)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CCollection)
	afx_msg long OACount();
	afx_msg LPDISPATCH OA_NewEnum();
	afx_msg LPDISPATCH OAItem(const VARIANT FAR& index);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART(EnumVARIANT, IEnumVARIANT)
		STDMETHOD(Next)	(unsigned long celt, VARIANT FAR* rgvar, unsigned long FAR* pceltFetched);
		STDMETHOD(Skip)	(unsigned long celt);
		STDMETHOD(Reset)();
		STDMETHOD(Clone)(IEnumVARIANT FAR* FAR* ppenum);
	END_INTERFACE_PART(EnumVARIANT)
};

/////////////////////////////////////////////////////////////////////////////

#endif
