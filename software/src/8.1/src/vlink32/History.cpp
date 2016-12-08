//////////////////////////////////////////////////////////////////////////////
//
//  CHistory Implementation
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//  Interfaces
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdext.h"

#include "helement.h"

#include "history.h"

/////////////////////////////////////////////////////////////////////////////
//  Globals
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//  Diagnostics
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

void CHistory::Dump (CDumpContext& dc) const
{
    CVLinkCmdTarget::Dump (dc);
}
#endif


/////////////////////////////////////////////////////////////////////////////
//  Message Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CHistory, CVLinkCmdTarget)
	//{{AFX_MSG_MAP(CHistory)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//  Dispatch Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_DISPATCH_MAP(CHistory, CVLinkCmdTarget)
	//{{AFX_DISPATCH_MAP(CHistory)
	DISP_PROPERTY_EX(CHistory, "Limit", OAGetLimit, OASetLimit, VT_I4)
	DISP_FUNCTION(CHistory, "Count", OACount, VT_I4, VTS_NONE)
	DISP_FUNCTION(CHistory, "Size", OASize, VT_I4, VTS_NONE)
	DISP_FUNCTION(CHistory, "Newest", OANewest, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CHistory, "Oldest", OAOldest, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CHistory, "NthNewest", OANthNewest, VT_DISPATCH, VTS_I4)
	DISP_FUNCTION(CHistory, "NthOldest", OANthOldest, VT_DISPATCH, VTS_I4)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
//  Construction
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CHistory, CVLinkCmdTarget)

CHistory::CHistory (long initialLimit)
{
    m_count 	= 0;
    m_limit	= initialLimit;
    m_size	= 0;
    m_pNewest	=
    m_pOldest	= NULL;

//  Add this instance to the class instance list, ...
//  Instances.AddItem (this);
}

/////////////////////////////////////////////////////////////////////////////
//  Destruction
/////////////////////////////////////////////////////////////////////////////

CHistory::~CHistory()
{
//  Release component objects, ...
    while (m_pNewest)
    {
	CHElement* pElement = m_pNewest;
	pElement->Unlink (&m_pNewest, &m_pOldest);
	pElement->Release ();
    }

//  ... and this object from its class instance list.
//	Instances.DeleteItem (this);
}


/////////////////////////////////////////////////////////////////////////////
//  Operations
/////////////////////////////////////////////////////////////////////////////

void CHistory::GetSynopsis (CString& synopsis) const
{
    CHElement*	pElement;
    for (pElement = m_pNewest; pElement; pElement = pElement->m_pPredecessor)
    {
	CString spart;
	pElement->GetSynopsis (spart);
	synopsis += spart;
	synopsis += "\r\n";
    }
}

CHElement* CHistory::NthNewest(unsigned int index) const
{
    if (index + 1 > m_size)
	return NULL;

    CHElement* pElement;
    for (pElement = m_pNewest; index-- > 0; pElement = pElement->m_pPredecessor);
    return pElement;
}

CHElement* CHistory::NthOldest(unsigned int index) const
{
    if (index + 1 > m_size)
	return NULL;

    CHElement* pElement;
    for (pElement = m_pOldest; index-- > 0; pElement = pElement->m_pSuccessor);
    return pElement;
}

BOOL CHistory::SetLimit(unsigned int limit)
{
    m_limit = limit;
    return TRUE;
}

BOOL CHistory::Append(CVLinkCmdTarget* pObject)
{
    while (m_size >= m_limit && m_size > 0)
    {
	CHElement* pElement = m_pOldest;
	pElement->Unlink (&m_pNewest, &m_pOldest);
	pElement->Release ();
	m_size--;
    }
    
    BOOL result = TRUE;
    TRY {
	if (m_limit > 0)
	{
	    CHElement* pElement = new CHElement (pObject);
	    pElement->Link (&m_pNewest, &m_pOldest, m_count);
	    m_size++;
	}
	else m_count++;
    } CATCH (CMemoryException, pException) {
	result = FALSE;
    } END_CATCH

    return result;
}


/////////////////////////////////////////////////////////////////////////////
//  Handlers
/////////////////////////////////////////////////////////////////////////////

long CHistory::OAGetLimit()
{
    return m_limit;
}

void CHistory::OASetLimit(long nNewValue)
{
    SetLimit (nNewValue);
}

long CHistory::OACount()
{
    return m_count;
}

long CHistory::OASize()
{
    return m_size;
}

LPDISPATCH CHistory::OANewest()
{
    return m_pNewest ? m_pNewest->GetIDispatch(TRUE) : NULL;
}

LPDISPATCH CHistory::OAOldest()
{
    return m_pOldest ? m_pOldest->GetIDispatch(TRUE) : NULL;
}

LPDISPATCH CHistory::OANthNewest(long index)
{
    CHElement* pElement = NthNewest ((unsigned long)index);
    return pElement ? pElement->GetIDispatch(TRUE) : NULL;
}

LPDISPATCH CHistory::OANthOldest(long index)
{
    CHElement* pElement = NthOldest ((unsigned long)index);
    return pElement ? pElement->GetIDispatch(TRUE) : NULL;
}
