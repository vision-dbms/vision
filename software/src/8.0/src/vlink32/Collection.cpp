//////////////////////////////////////////////////////////////////////////////
//
//  CCollection Implementation
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//  Interfaces
//////////////////////////////////////////////////////////////////////////////

//  System
#include "stdafx.h"
#include "stdext.h"

#include <afxpriv.h>

//  Supporting

//  Self
#include "Collection.h"


/////////////////////////////////////////////////////////////////////////////
//  Diagnostics
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

////////////////
//  CCollectionKernel
////////////////

void CCollectionKernel::Dump (CDumpContext& dc) const
{
    CObject::Dump (dc);
}

////////////////
//  CCollection
////////////////

void CCollection::Dump (CDumpContext& dc) const
{
    CVLinkCmdTarget::Dump (dc);
}
#endif


/////////////////////////////////////////////////////////////////////////////
//  Maps
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCollection, CVLinkCmdTarget)
    //{{AFX_MSG_MAP(CCollection)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CCollection, CVLinkCmdTarget)
    //{{AFX_DISPATCH_MAP(CCollection)
    DISP_FUNCTION(CCollection, "Count", OACount, VT_I4, VTS_NONE)
    DISP_FUNCTION(CCollection, "Item", OAItem, VT_DISPATCH, VTS_VARIANT)
    DISP_FUNCTION_ID(CCollection, "_NewEnum", DISPID_NEWENUM, OA_NewEnum, VT_DISPATCH, VTS_NONE)
    DISP_DEFVALUE(CCollection, "Item")
    //}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP (CCollection, CVLinkCmdTarget)
    INTERFACE_PART (CCollection, IID_IEnumVARIANT, EnumVARIANT)
END_INTERFACE_MAP()


/////////////////////////////////////////////////////////////////////////////
//  Construction
/////////////////////////////////////////////////////////////////////////////

////////////////
//  CCollectionKernel
////////////////

IMPLEMENT_DYNAMIC(CCollectionKernel, CObject)

CCollectionKernel::CCollectionKernel ()
{
    m_count		= 0;
    m_pOldestItem	=
    m_pNewestItem	= NULL;
    m_pCachedItem	= NULL;
    m_pNewestEnumerator	= NULL;
}

////////////////
//  CCollection
////////////////

IMPLEMENT_DYNAMIC(CCollection, CVLinkCmdTarget)

CCollection::CCollection (CCollectionKernel* pKernel, CVLinkCmdTarget** ppNextElement) : m_pKernel(pKernel)
{
    m_pPrevEnumerator	= pKernel->m_pNewestEnumerator;
    m_pNextEnumerator	= NULL;
    m_ppNextElement	= ppNextElement;
}


/////////////////////////////////////////////////////////////////////////////
//  Destruction
/////////////////////////////////////////////////////////////////////////////

////////////////
//  CCollectionKernel
////////////////

CCollectionKernel::~CCollectionKernel ()
{
}

////////////////
//  CCollection
////////////////

CCollection::~CCollection()
{
    if (m_pPrevEnumerator)
	m_pPrevEnumerator->m_pNextEnumerator = m_pNextEnumerator;
    if (m_pNextEnumerator)
	m_pNextEnumerator->m_pPrevEnumerator = m_pPrevEnumerator;
    else m_pKernel->m_pNewestEnumerator = m_pPrevEnumerator;
}


/////////////////////////////////////////////////////////////////////////////
//  Operations
/////////////////////////////////////////////////////////////////////////////

////////////////
//  CCollectionKernel
////////////////

CVLinkCmdTarget* CCollectionKernel::Item (VARIANT FAR& index)
{
    VARIANT		convertedIndex;
    CVLinkCmdTarget*	pItem;

    VariantInit (&convertedIndex);

    pItem = NOERROR == VariantChangeType (&convertedIndex, &index, 0, VT_I4)
    ? Item (V_I4	(&convertedIndex))
    : NOERROR == VariantChangeType (&convertedIndex, &index, 0, VT_BSTR)
    ? Item (V_BSTR	(&convertedIndex))
    : NULL;

    VariantClear (&convertedIndex);

    return pItem;
}


CVLinkCmdTarget* CCollectionKernel::Item (long index)
{
    unsigned long uindex = (unsigned long)index;

#ifdef _DEBUG
    {
	afxDump << this << "Item[" << index << "/" << m_count << "]\n";
	unsigned long xItem;
	CVLinkCmdTarget *pItem;
	for (
	    xItem = 1, pItem = m_pOldestItem;
	    pItem;
	    xItem++, pItem = pItem->NextOldestItem ()
	) afxDump
	    << xItem
	    << ":" << (LPVOID)pItem
	    << " NextOldest = " << (LPVOID)pItem->NextOldestItem ()
	    << " NextNewest = " << (LPVOID)pItem->NextNewestItem ()
	    << "\n";
	for (
	    xItem = m_count, pItem = m_pNewestItem;
	    pItem;
	    xItem--, pItem = pItem->NextNewestItem ()
	) afxDump
	    << xItem
	    << ":" << (LPVOID)pItem
	    << " NextNewest = " << (LPVOID)pItem->NextNewestItem ()
	    << " NextOldest = " << (LPVOID)pItem->NextOldestItem ()
	    << "\n";
    }
#endif

//  Check and convert 'index'...
    if (index < 1 || uindex > m_count)
	return NULL;

//  ... set the cached item if it is null, ...
    if (NULL == m_pCachedItem)
    {
	m_pCachedItem = m_pOldestItem;
	m_cachedIndex = 1;
    }

//  ... guess oldest ==> newest, ..
    CVLinkCmdTarget*			pOriginItem	= m_pOldestItem;
    unsigned long			originIndex	= 1;
    unsigned long			distance	= uindex - 1;
    enum {FromOldest, FromNewest}	direction	= FromOldest;

//  ... check newest ==> oldest, ...
    if (distance > m_count - uindex)
    {
	pOriginItem	= m_pNewestItem;
	originIndex	= m_count;
	distance	= m_count - uindex;
	direction	= FromNewest;
    }

//  ... check cachedItem ==> newest, ...
    if (uindex >= m_cachedIndex && distance > uindex - m_cachedIndex)
    {
	pOriginItem	= m_pCachedItem;
	originIndex	= m_cachedIndex;
	distance	= uindex - m_cachedIndex;
	direction	= FromOldest;
    }

//  ... check cachedItem ==> oldest, ...
    if (m_cachedIndex >= uindex && distance > m_cachedIndex - uindex)
    {
	pOriginItem	= m_pCachedItem;
	originIndex	= m_cachedIndex;
	distance	= m_cachedIndex - uindex;
	direction	= FromNewest;
    }

//  ... update the cached item, ...
    m_pCachedItem = pOriginItem;
    m_cachedIndex = originIndex;

//  ... do the search, ...
    switch (direction)
    {
    case FromOldest:
	while (distance-- > 0)
	{
	    m_pCachedItem = m_pCachedItem->NextOldestItem ();
	    m_cachedIndex++;
	}
	break;
    case FromNewest:
	while (distance-- > 0)
	{
	    m_pCachedItem = m_pCachedItem->NextNewestItem ();
	    m_cachedIndex--;
	}
	break;
    }

//  ... and return the result:

    return m_pCachedItem;
}


CVLinkCmdTarget* CCollectionKernel::Item (BSTR index)
{
    USES_CONVERSION;
    return Item (OLE2CT (index));
}

CVLinkCmdTarget* CCollectionKernel::Item (LPCSTR index)
{
    CObject* pItem;
    return m_nameMap.Lookup (index, pItem) ? (CVLinkCmdTarget*)pItem : NULL;
}


void CCollectionKernel::AddItem (CVLinkCmdTarget* pItem)
{
    ASSERT (NULL == pItem->m_pNextNewestItem && NULL == pItem->m_pNextOldestItem);

//  Link the item, ...
    if (NULL == m_pNewestItem)
	m_pNewestItem = m_pOldestItem = pItem;
    else
    {
	pItem->m_pNextNewestItem = m_pNewestItem;
	m_pNewestItem->m_pNextOldestItem = pItem;
	m_pNewestItem = pItem;
    }

//  ... increment the element count, ...
    m_count++;

//  ... and add it to the naming dictionary.
    TRY {
	CString uid;
	pItem->GetUID (uid);
	m_nameMap.SetAt (uid, pItem);
    } CATCH (CMemoryException, pException) {
    } END_CATCH
}

void CCollectionKernel::DeleteItem (CVLinkCmdTarget* pItem)
{
//  Remove the element from the naming dictionary, ...
    TRY {
	CString uid;
	pItem->GetUID (uid);
	m_nameMap.RemoveKey (uid);
    } CATCH (CMemoryException, pException) {
    } END_CATCH

//  ... clear the cached item, ...
    m_pCachedItem = NULL;

//  ... update the next item pointer in all enumerators for this collection...
    CCollection* pEnumerator;
    for (
	pEnumerator = m_pNewestEnumerator; pEnumerator; pEnumerator = pEnumerator->m_pPrevEnumerator
    ) pEnumerator->Fix (pItem);

//  ... unlink the item, ...
    if (pItem->m_pNextNewestItem)
	pItem->m_pNextNewestItem->m_pNextOldestItem = pItem->m_pNextOldestItem;
    else m_pOldestItem = pItem->m_pNextOldestItem;
    if (pItem->m_pNextOldestItem)
	pItem->m_pNextOldestItem->m_pNextNewestItem = pItem->m_pNextNewestItem;
    else m_pNewestItem = pItem->m_pNextNewestItem;

    pItem->m_pNextNewestItem =
    pItem->m_pNextOldestItem = NULL;

//  ... and decrement the element count.
    m_count--;
}


CCollection* CCollectionKernel::Collection ()
{
    CCollection* pCollection;
    TRY {
	pCollection = new CCollection (this, &m_pOldestItem);
    } CATCH (CMemoryException, pException) {
	pCollection = NULL;
    } END_CATCH
    return pCollection;
}

LPDISPATCH CCollectionKernel::OACollection ()
{
    CCollection* pCollection = Collection ();
    return pCollection ? pCollection->GetIDispatch(FALSE) : NULL;
}


////////////////
//  CCollection
////////////////

void CCollection::Fix (CVLinkCmdTarget *pDeletedItem)
{
    if (&pDeletedItem->m_pNextOldestItem == m_ppNextElement) 
	m_ppNextElement = pDeletedItem->m_pNextNewestItem
	    ? &pDeletedItem->m_pNextNewestItem->m_pNextOldestItem
	    : &m_pKernel->m_pOldestItem;
}

HRESULT CCollection::Next (unsigned long celt, VARIANT FAR* rgvar, unsigned long FAR* pceltFetched)
{
    unsigned long elementIndex;
    for (elementIndex = 0; elementIndex < celt && *m_ppNextElement; elementIndex++)
    {
	CVLinkCmdTarget* pElement = *m_ppNextElement;
	VariantInit	(&rgvar[elementIndex]);
	V_VT		(&rgvar[elementIndex]) = VT_DISPATCH;
	V_DISPATCH	(&rgvar[elementIndex]) = pElement->GetIDispatch(TRUE);
	m_ppNextElement = &pElement->m_pNextOldestItem;
    }
    *pceltFetched = elementIndex;
    while (elementIndex < celt)
    {
	VariantInit (&rgvar[elementIndex]);
	elementIndex++;
    }
    return celt > *pceltFetched ? ResultFromScode (S_FALSE) : NOERROR;
}

HRESULT CCollection::Skip (unsigned long celt)
{
    while (celt > 0 && *m_ppNextElement)
    {
	celt--;
	m_ppNextElement = &(*m_ppNextElement)->m_pNextOldestItem;
    }
    return celt > 0 ? ResultFromScode (S_FALSE) : NOERROR;
}

HRESULT CCollection::Reset ()
{
    m_ppNextElement = &m_pKernel->m_pOldestItem;
    return NOERROR;
}

HRESULT CCollection::Clone (IEnumVARIANT FAR* FAR* ppenum)
{
    CCollection* pCollection;
    TRY {
	pCollection = new CCollection (m_pKernel, m_ppNextElement);
    } CATCH (CMemoryException, pException) {
	pCollection = NULL;
    } END_CATCH
    return NULL == pCollection
	? ResultFromScode(E_OUTOFMEMORY)
	: pCollection->GetIDispatch(FALSE)->QueryInterface (IID_IEnumVARIANT, (void FAR* FAR*)ppenum);
}


////////////////
//  CCollection::XEnumVARIANT
////////////////

STDMETHODIMP_(ULONG) CCollection::XEnumVARIANT::AddRef()
{
    METHOD_PROLOGUE (CCollection, EnumVARIANT)
    return pThis->ExternalAddRef ();
}

STDMETHODIMP_(ULONG) CCollection::XEnumVARIANT::Release()
{
    METHOD_PROLOGUE (CCollection, EnumVARIANT)
    return pThis->ExternalRelease ();
}

STDMETHODIMP CCollection::XEnumVARIANT::QueryInterface (REFIID iid, void FAR* FAR* ppvObj)
{
    METHOD_PROLOGUE (CCollection, EnumVARIANT)
    return (HRESULT)pThis->ExternalQueryInterface (&iid, ppvObj);
}

STDMETHODIMP CCollection::XEnumVARIANT::Next	(
    unsigned long celt, VARIANT FAR* rgvar, unsigned long FAR* pceltFetched
)
{
    METHOD_PROLOGUE (CCollection, EnumVARIANT)
    return pThis->Next (celt, rgvar, pceltFetched);
}

STDMETHODIMP CCollection::XEnumVARIANT::Skip	(unsigned long celt)
{
    METHOD_PROLOGUE (CCollection, EnumVARIANT)
    return pThis->Skip (celt);
}

STDMETHODIMP CCollection::XEnumVARIANT::Reset	()
{
    METHOD_PROLOGUE (CCollection, EnumVARIANT)
    return pThis->Reset ();
}

STDMETHODIMP CCollection::XEnumVARIANT::Clone	(IEnumVARIANT FAR* FAR* ppenum)
{
    METHOD_PROLOGUE (CCollection, EnumVARIANT)
    return pThis->Clone (ppenum);
}


/////////////////////////////////////////////////////////////////////////////
//  Handlers
/////////////////////////////////////////////////////////////////////////////

long CCollection::OACount()
{
    return m_pKernel->Count ();
}

LPDISPATCH CCollection::OAItem(const VARIANT FAR& index)
{
    CVLinkCmdTarget* pItem = Item ((VARIANT FAR&)index);
    return pItem ? pItem->GetIDispatch (TRUE) : NULL;
}

LPDISPATCH CCollection::OA_NewEnum()
{
    return GetIDispatch (TRUE);
}
