//////////////////////////////////////////////////////////////////////////////
//
//  CTextBuffer Implementation
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//  Interfaces
//////////////////////////////////////////////////////////////////////////////

//  System...
#include "stdafx.h"
#include "stdext.h"

//  Supporting...
#include "TextCursor.h"

//  Self...
#include "TextBuffer.h"


/////////////////////////////////////////////////////////////////////////////
//  CTextPart Implementation
/////////////////////////////////////////////////////////////////////////////

CTextPart::CTextPart (CTextBuffer* pParent, CTextPart* pNextPart, CTextPart* pPrevPart, size_t spaceRequired)
: m_pParent(pParent)
{
    m_pNextPart		= pNextPart;
    m_pPrevPart		= pPrevPart;
    m_pSpace		= new char[spaceRequired + 1];
    m_spaceAllocated	= spaceRequired;
    m_spaceConsumed	= 0;

    if (NULL == m_pSpace)
    {
	char message[80];
	sprintf (message, "CTextPart::CTextPart: 'new' failed allocating %u bytes", spaceRequired + 1);
	AfxMessageBox (message);
    }
}

unsigned long CTextPart::DistanceUntil (unsigned long offset, LPCSTR charSet) const
{
    unsigned long	totalDistance = 0;
    CTextPart*		pPart = (CTextPart *)this;

    while (pPart)
    {
	unsigned long thisDistance = strcspn (pPart->m_pSpace + offset, charSet);
	totalDistance += thisDistance;
	if (offset + thisDistance < pPart->m_spaceConsumed) return totalDistance;
	pPart	= pPart->NextPart ();
	offset	= 0;
    }
    return totalDistance;
}

unsigned long CTextPart::DistanceWhile (unsigned long offset, LPCSTR charSet) const
{
    unsigned long	totalDistance = 0;
    CTextPart*		pPart = (CTextPart *)this;

    while (pPart)
    {
	unsigned long thisDistance = strspn (pPart->m_pSpace + offset, charSet);
	totalDistance += thisDistance;
	if (offset + thisDistance < pPart->m_spaceConsumed) return totalDistance;
	pPart	= pPart->NextPart ();
	offset	= 0;
    }
    return totalDistance;
}

unsigned long CTextPart::DistanceTo (unsigned long offset, LPCSTR pattern) const
{
    if ('\0' == pattern[0])
	return 0;
    const char patternStart[] = {pattern[0], '\0'};

    unsigned long	totalDistance = 0;
    CTextPart*		pPart = (CTextPart *)this;

    while (pPart)
    {
	unsigned long thisDistance = strcspn (pPart->m_pSpace + offset, patternStart);
	offset			+= thisDistance;
	totalDistance	+= thisDistance;
	if (offset < pPart->m_spaceConsumed)
	{
	    if (pPart->Matches (offset, pattern)) return totalDistance;
	    offset++;
	    totalDistance++;
	}
	if (offset >= pPart->SpaceConsumed ())
	{
	    pPart	= pPart->NextPart ();
	    offset	= 0;
	}
    }
    return totalDistance;
}

BOOL CTextPart::Matches (unsigned long offset, LPCSTR pattern) const
{
    CTextPart*	pPart		= (CTextPart *)this;
    size_t	patternLength	= strlen (pattern);

    while (pPart && patternLength > 0)
    {
	size_t thisDistance
	    = patternLength < (size_t)(pPart->m_spaceConsumed - offset)
	    ? patternLength : (size_t)(pPart->m_spaceConsumed - offset);

	if (0 != strncmp (pPart->m_pSpace + offset, pattern, thisDistance))
	    return FALSE;

	pattern		+= thisDistance;
	patternLength	-= thisDistance;

	pPart	= pPart->NextPart ();
	offset	= 0;
    }
    return TRUE;
}

void CTextPart::DosSubstring (CString& rResult, unsigned long offset, unsigned long count) const
{
// Get the substring with Unix style newlines...
    UnixSubstring (rResult, offset, count);
    if (rResult.IsEmpty ())
	return;

// ... determine the number of lines in the result, ...
    unsigned int	lcount = 0;
    LPCSTR		ucursor = rResult;
    while (ucursor = strchr (ucursor, '\n'))
    {
	lcount++;
	ucursor++;
    }

// ... return the Unix substring if no newlines need to be converted...
    if (0 == lcount)
	return;

// ... allocate and check the Dos result string, ...
    unsigned int	ulength = rResult.GetLength ();
    TCHAR*		dstring = new TCHAR [ulength + lcount + 1];
    if (NULL == dstring)
    {
	rResult.Empty ();
	return;
    }

    ucursor = rResult;
    char*		dcursor = dstring;
    const LPCSTR	ulimit = ucursor + ulength;
    while (ucursor < ulimit)
    {
	char* pNL = strchr (ucursor, '\n');
	if (pNL)
	{
	    size_t copyLength = (size_t)(pNL - ucursor);
	    strncpy (dcursor, ucursor, copyLength);
	    ucursor += copyLength + 1;
	    dcursor += copyLength;

	    strncpy (dcursor, "\r\n", 2);
	    dcursor += 2;
	}
	else
	{
	    size_t copyLength = (size_t)(ulimit - ucursor);
	    strncpy (dcursor, ucursor, copyLength);
	    dcursor += copyLength;
	    ucursor += copyLength;
	}
    }
    *dcursor = '\0';

    rResult = dstring;
    delete[] dstring;
}

void CTextPart::UnixSubstring (CString& rResult, unsigned long offset, unsigned long count) const
{
    unsigned int	icount	= count;
    unsigned int	ioffset	= (unsigned int)offset;
    unsigned int	tcount	= 0;
    TCHAR*		result	= rResult.GetBufferSetLength (icount + 1);

    if (NULL == result)
	return;

    for (CTextPart const* pPart = this; pPart && icount > tcount; pPart = pPart->NextPart ())
    {
	size_t bytesAvailable
	    = pPart->m_spaceConsumed - ioffset < icount - tcount
	    ? pPart->m_spaceConsumed - ioffset : icount - tcount;
	memcpy ((char *)result + tcount, pPart->m_pSpace + ioffset, bytesAvailable);
	tcount	+= bytesAvailable;
	ioffset	 = 0;
    }
    if (tcount > 0 && result[tcount - 1])
    {
	result[tcount] = '\0';
	rResult.ReleaseBuffer ();
    }
    else rResult.ReleaseBuffer (tcount);
}

void CTextPart::Append (LPCSTR &string, size_t &stringLength)
{
    if (NULL == this)
	return;

    size_t copyLength
	= stringLength < m_spaceAllocated - m_spaceConsumed
	? stringLength : m_spaceAllocated - m_spaceConsumed;

    memcpy (m_pSpace + m_spaceConsumed, string, copyLength);
    m_spaceConsumed += copyLength;
    m_pSpace[m_spaceConsumed] = '\0';

    string	 += copyLength;
    stringLength -= copyLength;
}

size_t CTextPart::Trim (BOOL removeLastNewLine)
{
    size_t amountTrimmed = 0;

    if (NULL == this)
	return amountTrimmed;

    if (removeLastNewLine && m_spaceConsumed > 0 && '\n' == m_pSpace[m_spaceConsumed - 1])
    {
	m_spaceConsumed--;
	amountTrimmed++;
    }

    if (m_spaceAllocated > m_spaceConsumed)
    {
	char* pNewSpace = new char[m_spaceConsumed + 1];
	memcpy (pNewSpace, m_pSpace, m_spaceConsumed);
	pNewSpace[m_spaceConsumed] = '\0';

	delete[] m_pSpace;

	m_pSpace = pNewSpace;
	m_spaceAllocated = m_spaceConsumed;
    }

    return amountTrimmed;
}


/////////////////////////////////////////////////////////////////////////////
//  Globals
/////////////////////////////////////////////////////////////////////////////

CCollectionKernel CTextBuffer::Instances;

/////////////////////////////////////////////////////////////////////////////
//  Diagnostics
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

void CTextBuffer::Dump (CDumpContext& dc) const
{
    CVLinkCmdTarget::Dump (dc);
}
#endif


/////////////////////////////////////////////////////////////////////////////
//  Message Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CTextBuffer, CVLinkCmdTarget)
	//{{AFX_MSG_MAP(CTextBuffer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//  Dispatch Map
/////////////////////////////////////////////////////////////////////////////

BEGIN_DISPATCH_MAP(CTextBuffer, CVLinkCmdTarget)
	//{{AFX_DISPATCH_MAP(CTextBuffer)
	DISP_PROPERTY(CTextBuffer, "ReturnUnixStyleNewLines", m_returnUnixStyleNewLines, VT_BOOL)
	DISP_FUNCTION(CTextBuffer, "Length", OALength, VT_I4, VTS_NONE)
	DISP_FUNCTION(CTextBuffer, "Text", OAText, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CTextBuffer, "UnixText", OAUnixText, VT_BSTR, VTS_NONE)
	DISP_FUNCTION(CTextBuffer, "Cursor", OACursor, VT_DISPATCH, VTS_NONE)
	DISP_FUNCTION(CTextBuffer, "DistanceUntil", OADistanceUntil, VT_I4, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CTextBuffer, "DistanceWhile", OADistanceWhile, VT_I4, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CTextBuffer, "DistanceTo", OADistanceTo, VT_I4, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CTextBuffer, "DistanceToEnd", OADistanceToEnd, VT_I4, VTS_I4)
	DISP_FUNCTION(CTextBuffer, "Get", OAGet, VT_BSTR, VTS_I4 VTS_I4)
	DISP_FUNCTION(CTextBuffer, "GetUntil", OAGetUntil, VT_BSTR, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CTextBuffer, "GetWhile", OAGetWhile, VT_BSTR, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CTextBuffer, "GetTo", OAGetTo, VT_BSTR, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CTextBuffer, "GetToEnd", OAGetToEnd, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CTextBuffer, "GetLine", OAGetLine, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CTextBuffer, "GetCell", OAGetCell, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CTextBuffer, "Left", OALeft, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CTextBuffer, "Right", OARight, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CTextBuffer, "Mid", OAMid, VT_BSTR, VTS_I4 VTS_I4)
	DISP_FUNCTION(CTextBuffer, "Truncated", OATruncated, VT_BOOL, VTS_NONE)
	DISP_DEFVALUE(CTextBuffer, "Text")
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
//  Construction
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CTextBuffer, CVLinkCmdTarget)

void CTextBuffer::Initialize (LPCSTR srcString, size_t srcLength, BOOL moreToCome)
{
//  Initialize the instance, ...
    m_pFirstPart		=
    m_pLastPart			= NULL;
    m_size			= 0;
    m_returnUnixStyleNewLines	= FALSE;

    if (srcString)
    {
	AppendLines (srcString, srcLength, moreToCome);
	Trim (FALSE);
    }

//  ... add it to the class instance list, ...
    Instances.AddItem (this);

//  ... and return.
}

/////////////////////////////////////////////////////////////////////////////
//  Destruction
/////////////////////////////////////////////////////////////////////////////

CTextBuffer::~CTextBuffer()
{
//  Release the instance structures, ...
    CTextPart* pPart = m_pFirstPart;

    m_pFirstPart = m_pLastPart = NULL;
    m_size = 0;

    while (pPart)
    {
	CTextPart* pNextPart = pPart->NextPart ();
	delete pPart;
	pPart = pNextPart;
    }

//  ... and remove this instance from the class instance list.
    Instances.DeleteItem (this);
}


/////////////////////////////////////////////////////////////////////////////
//  Operations
/////////////////////////////////////////////////////////////////////////////

////////////////
//  CTextBuffer::operator == (LPCSTR)
////////////////

CTextBuffer::operator == (LPCSTR operand) const
{
    for (CTextPart* pPart = m_pFirstPart; pPart; pPart = pPart->NextPart ())
    {
	if (*pPart != operand)
	    return FALSE;
	operand += pPart->SpaceConsumed ();
    }
    return '\0' == *operand;
}

////////////////
//  CTextBuffer::Cursor
////////////////

CTextCursor* CTextBuffer::Cursor ()
{
    CTextCursor* pCursor;
    TRY {
	pCursor = new CTextCursor (this);
    } CATCH (CMemoryException, pException) {
	pCursor = NULL;
    } END_CATCH
    return pCursor;
}

////////////////
//  CTextBuffer::TextPartAtOffset
////////////////

CTextPart* CTextBuffer::TextPartAtOffset (unsigned long& offset) const
{
    offset = FixedOffset (offset);
    for (
	CTextPart* pPart = m_pFirstPart;
	pPart && offset >= pPart->SpaceConsumed ();
	offset -= pPart->SpaceConsumed (), pPart = pPart->NextPart ()
    );

    return pPart;
}


////////////////
//  CTextBuffer::Distance...
////////////////

unsigned long CTextBuffer::DistanceUntil (unsigned long offset, LPCSTR charSet) const
{
    CTextPart *pPart = TextPartAtOffset (offset);
    return pPart->DistanceUntil (offset, charSet);
}

unsigned long CTextBuffer::DistanceWhile (unsigned long offset, LPCSTR charSet) const
{
    CTextPart *pPart = TextPartAtOffset (offset);
    return pPart->DistanceWhile (offset, charSet);
}

unsigned long CTextBuffer::DistanceTo	(unsigned long offset, LPCSTR pattern) const
{
    CTextPart *pPart = TextPartAtOffset (offset);
    return pPart->DistanceTo (offset, pattern);
}

void CTextBuffer::DosSubstring (CString& rResult, unsigned long offset, unsigned long count) const
{
    NormalizeRange (offset, count);
    CTextPart *pPart = TextPartAtOffset (offset);
    pPart->DosSubstring (rResult, offset, count);
}

void CTextBuffer::UnixSubstring (CString& rResult, unsigned long offset, unsigned long count) const
{
    NormalizeRange (offset, count);
    CTextPart *pPart = TextPartAtOffset (offset);
    pPart->UnixSubstring (rResult, offset, count);
}

void CTextBuffer::DosText (CString& rResult) const
{
    DosSubstring (rResult, 0, Length ());
}

void CTextBuffer::UnixText (CString& rResult) const
{
    UnixSubstring (rResult, 0, Length ());
}

void CTextBuffer::NormalizeRange (unsigned long &offset, unsigned long &count) const
{
    unsigned long endPoint1 = FixedOffset ((unsigned long)offset);
    unsigned long endPoint2 = FixedOffset ((unsigned long)(endPoint1 + count));

    if (endPoint1 <= endPoint2)
    {
	offset	= endPoint1;
	count	= endPoint2 - endPoint1;
    }
    else
    {
	offset	= endPoint2;
	count	= endPoint1 - endPoint2;
    }
}

void CTextBuffer::GetSynopsis (CString &synopsis) const {
    DosSubstring (synopsis, 0, DistanceUntil (0, "\n"));
}


////////////
//  CTextBuffer::Append...
////////////

void CTextBuffer::AppendLines (LPCSTR string, size_t stringLength, BOOL moreToCome)
{
    size_t copyLength;
    size_t skipLength;
    while (stringLength > 0)
    {
	LPCSTR pCR = (LPCSTR)memchr (string, '\r', stringLength);
	if (pCR)
	{
	    copyLength = (size_t)(pCR - string);
	    skipLength = copyLength + 1;
	}
	else copyLength = skipLength = stringLength;

	stringLength -= skipLength;
	AppendBytes (string, copyLength, moreToCome || stringLength > 0);
	string += skipLength;
    }
}

void CTextBuffer::AppendBytes (LPCSTR string, size_t stringLength, BOOL moreToCome)
{
    m_size += stringLength;
    m_pLastPart->Append (string, stringLength);
    while (stringLength > 0)
    {
	if (m_pLastPart)
	{
	    m_pLastPart->m_pNextPart = new CTextPart (this, NULL, m_pLastPart, moreToCome ? 8192 : stringLength);
	    m_pLastPart = m_pLastPart->NextPart ();
	}
	else m_pFirstPart = m_pLastPart = new CTextPart (this, NULL, m_pLastPart, moreToCome ? 8192 : stringLength);

	m_pLastPart->Append (string, stringLength);
    }
}


////////////
//  CTextBuffer::Trim
////////////

void CTextBuffer::Trim (BOOL removeLastNewLine)
{
    m_size -= m_pLastPart->Trim (removeLastNewLine);
    if (m_pLastPart && m_pLastPart->IsEmpty ())
    {
	CTextPart* pOldLastPart = m_pLastPart;

	m_pLastPart = m_pLastPart->m_pPrevPart;
	if (m_pLastPart)
	    m_pLastPart->m_pNextPart = NULL;
	else m_pFirstPart = NULL;

	delete pOldLastPart;
    }
}


/////////////////////////////////////////////////////////////////////////////
//  Handlers
/////////////////////////////////////////////////////////////////////////////

long CTextBuffer::OALength()
{
    return Length ();
}

BSTR CTextBuffer::OAText ()
{
    CString iResult;
    DosSubstring (iResult, 0, Length ());
    return iResult.AllocSysString ();
}

BSTR CTextBuffer::OAUnixText ()
{
    CString iResult;
    UnixSubstring (iResult, 0, Length ());
    return iResult.AllocSysString ();
}

LPDISPATCH CTextBuffer::OACursor ()
{
    CTextCursor* pCursor = Cursor ();
    return pCursor ? pCursor->GetIDispatch (FALSE) : NULL;
}

long CTextBuffer::OADistanceToEnd (long offset)
{
    return DistanceToEnd ((unsigned long)offset);
}

long CTextBuffer::OADistanceUntil(long offset, LPCSTR charSet)
{
    return DistanceUntil ((unsigned long)offset, charSet);
}

long CTextBuffer::OADistanceWhile(long offset, LPCSTR charSet)
{
    return DistanceWhile ((unsigned long)offset, charSet);
}

long CTextBuffer::OADistanceTo(long offset, LPCSTR pattern)
{
    return DistanceTo ((unsigned long)offset, pattern);
}

BSTR CTextBuffer::OAGet(long offset, long count)
{
    CString iResult;
    if (m_returnUnixStyleNewLines)
	UnixSubstring (iResult, offset, count);
    else DosSubstring (iResult, offset, count);
    return iResult.AllocSysString ();
}

BSTR CTextBuffer::OAGetUntil(long offset, LPCSTR charSet)
{
    return OAGet (offset, DistanceUntil ((unsigned long)offset, charSet));
}

BSTR CTextBuffer::OAGetWhile(long offset, LPCSTR charSet)
{
    return OAGet (offset, DistanceWhile ((unsigned long)offset, charSet));
}

BSTR CTextBuffer::OAGetTo (long offset, LPCSTR pattern)
{
    return OAGet (offset, DistanceTo ((unsigned long)offset, pattern));
}

BSTR CTextBuffer::OAGetToEnd (long offset)
{
    return OAGet (offset, DistanceToEnd ((unsigned long)offset));
}

BSTR CTextBuffer::OAGetLine (long offset)
{
    return OAGetUntil (offset, "\n");
}

BSTR CTextBuffer::OAGetCell (long offset)
{
    return OAGetUntil (offset, "\377");
}

BSTR CTextBuffer::OALeft(long count)
{
    return OAGet (0, count);
}

BSTR CTextBuffer::OARight(long count)
{
    return OAGet (Length () - count, count);
}

BSTR CTextBuffer::OAMid(long offset, long count)
{
    return OAGet (offset, count);
}

BOOL CTextBuffer::OATruncated()
{
    return FALSE;
}
