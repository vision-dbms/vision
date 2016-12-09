//////////////////////////////////////////////////////////////////////////////
//
//  CTextCursor Implementation
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//  Interfaces
//////////////////////////////////////////////////////////////////////////////

//  System...
#include "stdafx.h"
#include "stdext.h"

//  Supporting...
#include "TextBuffer.h"

//  Self...
#include "TextCursor.h"

/////////////////////////////////////////////////////////////////////////////
//  Globals
/////////////////////////////////////////////////////////////////////////////

CCollectionKernel CTextCursor::Instances;

/////////////////////////////////////////////////////////////////////////////
//  Diagnostics
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

void CTextCursor::Dump (CDumpContext& dc) const
{
    CVLinkCmdTarget::Dump (dc);
}
#endif


/////////////////////////////////////////////////////////////////////////////
//  Message Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CTextCursor, CVLinkCmdTarget)
	//{{AFX_MSG_MAP(CTextCursor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//  Dispatch Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_DISPATCH_MAP(CTextCursor, CVLinkCmdTarget)
	//{{AFX_DISPATCH_MAP(CTextCursor)
	DISP_PROPERTY(CTextCursor, "ReturnUnixStyleNewLines", m_returnUnixStyleNewLines, VT_BOOL)
	DISP_PROPERTY_EX(CTextCursor, "Position", OAGetPosition, OASetPosition, VT_I4)
	DISP_FUNCTION(CTextCursor, "Clone", OAClone, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CTextCursor, "Buffer", OABuffer, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CTextCursor, "IsAtBeginning", OAIsAtBeginning, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CTextCursor, "IsntAtBeginning", OAIsntAtBeginning, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CTextCursor, "IsAtEnd", OAIsAtEnd, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CTextCursor, "IsntAtEnd", OAIsntAtEnd, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CTextCursor, "DistanceUntil", OADistanceUntil, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CTextCursor, "DistanceWhile", OADistanceWhile, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CTextCursor, "DistanceTo", OADistanceTo, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CTextCursor, "DistanceToEnd", OADistanceToEnd, VT_I4, VTS_NONE)
	DISP_FUNCTION(CTextCursor, "Skip", OASkip, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CTextCursor, "SkipUntil", OASkipUntil, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CTextCursor, "SkipWhile", OASkipWhile, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CTextCursor, "SkipTo", OASkipTo, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CTextCursor, "Get", OAGet, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CTextCursor, "GetUntil", OAGetUntil, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION(CTextCursor, "GetWhile", OAGetWhile, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION(CTextCursor, "GetTo", OAGetTo, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION(CTextCursor, "GetToEnd", OAGetToEnd, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CTextCursor, "GetLine", OAGetLine, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CTextCursor, "GetCell", OAGetCell, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CTextCursor, "Peek", OAPeek, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CTextCursor, "PeekUntil", OAPeekUntil, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION(CTextCursor, "PeekWhile", OAPeekWhile, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION(CTextCursor, "PeekTo", OAPeekTo, VT_BSTR, VTS_BSTR)
	DISP_FUNCTION(CTextCursor, "PeekToEnd", OAPeekToEnd, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CTextCursor, "PeekLine", OAPeekLine, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CTextCursor, "PeekCell", OAPeekCell, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CTextCursor, "Text", OAText, VT_BSTR, VTS_NONE)
	DISP_DEFVALUE(CTextCursor, "Text")
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
//  Construction
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CTextCursor, CVLinkCmdTarget)

CTextCursor::CTextCursor (
    CTextBuffer* pBuffer, unsigned long position
) : m_pBuffer(pBuffer), m_position(FixedPosition (position)), m_returnUnixStyleNewLines (FALSE)
{
//  Initialize the instance, ...
    m_pBuffer->Retain ();

//  ... add it to the class instance list, ...
    Instances.AddItem (this);

//  ... and return.
}

/////////////////////////////////////////////////////////////////////////////
//  Destruction
/////////////////////////////////////////////////////////////////////////////

CTextCursor::~CTextCursor()
{
//  Release the instance structures, ...
    m_pBuffer->Release ();

//  ... and remove this instance from the class instance list.
    Instances.DeleteItem (this);
}


/////////////////////////////////////////////////////////////////////////////
//  Operations
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//  Handlers
/////////////////////////////////////////////////////////////////////////////

long CTextCursor::OAGetPosition()
{
    return m_position + 1;
}

void CTextCursor::OASetPosition(long nNewValue)
{
    SetPosition (nNewValue - 1);
}

LPDISPATCH CTextCursor::OAClone()
{
    CTextCursor* pClone = Clone ();
    return pClone ? pClone->GetIDispatch(FALSE) : NULL;
}

LPDISPATCH CTextCursor::OABuffer ()
{
    return m_pBuffer->GetIDispatch(TRUE);
}

BOOL CTextCursor::OAIsAtBeginning()
{
    return IsAtBeginning ();
}

BOOL CTextCursor::OAIsntAtBeginning()
{
    return IsntAtBeginning ();
}

BOOL CTextCursor::OAIsAtEnd()
{
    return IsAtEnd ();
}

BOOL CTextCursor::OAIsntAtEnd()
{
    return IsntAtEnd ();
}

long CTextCursor::OADistanceToEnd ()
{
    return DistanceToEnd ();
}

long CTextCursor::OADistanceUntil(LPCSTR charSet)
{
    return DistanceUntil (charSet);
}

long CTextCursor::OADistanceWhile(LPCSTR charSet)
{
    return DistanceWhile (charSet);
}

long CTextCursor::OADistanceTo(LPCSTR pattern)
{
    return DistanceTo (pattern);
}

void CTextCursor::OASkip(long count)
{
    Skip (count);
}

void CTextCursor::OASkipUntil (LPCSTR charSet)
{
    SkipUntil (charSet);
}

void CTextCursor::OASkipWhile (LPCSTR charSet)
{
    SkipWhile (charSet);
}

void CTextCursor::OASkipTo (LPCSTR pattern)
{
    SkipTo (pattern);
}

BSTR CTextCursor::OAGet(long count)
{
    CString iResult;
    if (m_returnUnixStyleNewLines)
	m_pBuffer->UnixSubstring (iResult, m_position, count);
    else m_pBuffer->DosSubstring (iResult, m_position, count);
    SetPosition (m_position + count);
    return iResult.AllocSysString ();
}

BSTR CTextCursor::OAGetUntil(LPCSTR charSet)
{
    return OAGet (DistanceUntil (charSet));
}

BSTR CTextCursor::OAGetWhile(LPCSTR charSet)
{
    return OAGet (DistanceWhile (charSet));
}

BSTR CTextCursor::OAGetTo (LPCSTR pattern)
{
    return OAGet (DistanceTo (pattern));
}

BSTR CTextCursor::OAGetToEnd ()
{
    return OAGet (DistanceToEnd ());
}

BSTR CTextCursor::OAGetLine()
{
    BSTR result = OAGetUntil ("\n");
    SkipWhile ("\n");
    return result;
}

BSTR CTextCursor::OAGetCell()
{
    BSTR result = OAGetUntil ("\377");
    SkipWhile ("\377");
    return result;
}

BSTR CTextCursor::OAPeek(long count)
{
    return m_pBuffer->OAGet (m_position, count);
}

BSTR CTextCursor::OAPeekUntil(LPCSTR charSet)
{
    return OAPeek (DistanceUntil (charSet));
}

BSTR CTextCursor::OAPeekWhile(LPCSTR charSet)
{
    return OAPeek (DistanceWhile (charSet));
}

BSTR CTextCursor::OAPeekTo (LPCSTR pattern)
{
    return OAPeek (DistanceTo (pattern));
}

BSTR CTextCursor::OAPeekToEnd ()
{
    return OAPeek (DistanceToEnd ());
}

BSTR CTextCursor::OAPeekLine()
{
    return OAPeekUntil ("\n");
}

BSTR CTextCursor::OAPeekCell()
{
    return OAPeekUntil ("\377");
}

BSTR CTextCursor::OAText()
{
    return OAGetToEnd ();
}
