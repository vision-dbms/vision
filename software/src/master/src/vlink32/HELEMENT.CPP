//////////////////////////////////////////////////////////////////////////////
//
//  CHElement Implementation
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//  Interfaces
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdext.h"

#include "helement.h"

/////////////////////////////////////////////////////////////////////////////
//  Globals
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//  Diagnostics
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

void CHElement::Dump (CDumpContext& dc) const
{
    CVLinkCmdTarget::Dump (dc);
}
#endif


/////////////////////////////////////////////////////////////////////////////
//  Message Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CHElement, CVLinkCmdTarget)
	//{{AFX_MSG_MAP(CHElement)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//  Dispatch Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_DISPATCH_MAP(CHElement, CVLinkCmdTarget)
	//{{AFX_DISPATCH_MAP(CHElement)
	DISP_FUNCTION(CHElement, "Object", OAObject, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CHElement, "Predecessor", OAPredecessor, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CHElement, "Successor", OASuccessor, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CHElement, "Position", OAPosition, VT_I4, VTS_NONE)
	DISP_FUNCTION(CHElement, "IsNewest", OAIsNewest, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CHElement, "IsOldest", OAIsOldest, VT_BOOL, VTS_NONE)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
//  Construction
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CHElement, CVLinkCmdTarget)

CHElement::CHElement (CVLinkCmdTarget* pObject) : m_pObject (pObject)
{
    if (m_pObject)
	m_pObject->Retain ();
    m_pPredecessor = m_pSuccessor = NULL;

//  Add this instance to the class instance list, ...
//	Instances.AddItem (this);
}

/////////////////////////////////////////////////////////////////////////////
//  Destruction
/////////////////////////////////////////////////////////////////////////////

CHElement::~CHElement()
{
//  Release component objects, ...
    if (m_pObject)
	m_pObject->Release ();
    if (m_pPredecessor)
	m_pPredecessor->Release ();

//  ... and this object from its class instance list.
//  Instances.DeleteItem (this);
}


/////////////////////////////////////////////////////////////////////////////
//  Operations
/////////////////////////////////////////////////////////////////////////////

void CHElement::Link (CHElement** ppNewestElement, CHElement** ppOldestElement, unsigned int &counter)
{
    ASSERT (NULL == m_pPredecessor && NULL == m_pSuccessor);

    m_pPredecessor	= *ppNewestElement;
    m_position		= ++counter;
    if (m_pPredecessor)
	m_pPredecessor->m_pSuccessor = this;
    else *ppOldestElement = this;
    *ppNewestElement = this;
}

CHElement* CHElement::Unlink(CHElement** ppNewestElement, CHElement** ppOldestElement)
{
    if (m_pPredecessor)
	m_pPredecessor->m_pSuccessor = m_pSuccessor;
    else *ppOldestElement = m_pSuccessor;

    if (m_pSuccessor)
	m_pSuccessor->m_pPredecessor = m_pPredecessor;
    else *ppNewestElement = m_pPredecessor;

    m_pPredecessor = m_pSuccessor = NULL;

    return this;
}


/////////////////////////////////////////////////////////////////////////////
//  Handlers
/////////////////////////////////////////////////////////////////////////////

LPDISPATCH CHElement::OAPredecessor ()
{
    return m_pPredecessor ? m_pPredecessor->GetIDispatch(TRUE) : NULL;
}

LPDISPATCH CHElement::OASuccessor ()
{
    return m_pSuccessor ? m_pSuccessor->GetIDispatch(TRUE) : NULL;
}

LPDISPATCH CHElement::OAObject ()
{
    return m_pObject ? m_pObject->GetIDispatch(TRUE) : NULL;
}

long CHElement::OAPosition()
{
    return m_position;
}

BOOL CHElement::OAIsNewest()
{
    return IsNewest();
}

BOOL CHElement::OAIsOldest()
{
    return IsOldest();
}
