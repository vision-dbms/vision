#ifndef CTextBuffer_Interface
#define CTextBuffer_Interface

/************************
 *****  Components  *****
 ************************/

#include "vlinkcmd.h"
#include "Collection.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

//////////////////////////
// CTextPart
//////////////////////////
class CTextPart	: CObject
{
    friend class CTextBuffer;
    friend class CTextCursor;

//  Construction
protected:
    CTextPart (
	CTextBuffer* pParent, CTextPart* pNextPart, CTextPart* pPrevPart, size_t spaceRequired
    );

//  Destruction
protected:
    ~CTextPart () { delete[] m_pSpace; }

//  Operations
protected:
    CTextPart*	NextPart	() const { return m_pNextPart; }
    CTextPart*	PrevPart	() const { return m_pPrevPart; }
    char*	Space		() const { return m_pSpace; }
    size_t	SpaceAllocated	() const { return m_spaceAllocated; }
    size_t	SpaceConsumed	() const { return m_spaceConsumed; }
    BOOL	IsEmpty		() const { return m_spaceConsumed == 0; }
    BOOL	IsFull		() const { return m_spaceConsumed >= m_spaceAllocated; }

    operator	!=		(LPCSTR operand) const {
	    return strncmp (m_pSpace, operand, m_spaceConsumed) != 0;
    }

    unsigned long	DistanceUntil	(unsigned long offset, LPCSTR charSet) const;
    unsigned long	DistanceWhile	(unsigned long offset, LPCSTR charSet) const;
    unsigned long	DistanceTo	(unsigned long offset, LPCSTR pattern) const;

    BOOL		Matches		(unsigned long offset, LPCSTR pattern) const;

    void		DosSubstring	(CString& rResult, unsigned long offset, unsigned long count) const;
    void		UnixSubstring	(CString& rResult, unsigned long offset, unsigned long count) const;

    void		Append		(LPCSTR &string, size_t &stringLength);
    size_t		Trim		(BOOL removeLastNewLine);

//  State
protected:
    CTextBuffer* const	m_pParent;
    CTextPart*		m_pNextPart;
    CTextPart*		m_pPrevPart;
    char*		m_pSpace;
    size_t		m_spaceAllocated;
    size_t		m_spaceConsumed;
};


//////////////////////////
// CTextBuffer
//////////////////////////

class CTextBuffer : public CVLinkCmdTarget
{
    friend class CTextCursor;
    friend class CTextPart;
    friend class CRequest;

    DECLARE_DYNAMIC(CTextBuffer)

//  Globals
public:
    static CCollectionKernel	Instances;

//  Construction
public:
    CTextBuffer (const CString& srcString, BOOL moreToCome = FALSE) {
	Initialize (srcString, srcString.GetLength (), moreToCome);
    }
    CTextBuffer (LPCSTR srcString, BOOL moreToCome = FALSE) {
	Initialize (srcString, strlen (srcString), moreToCome);
    }
    CTextBuffer (LPCSTR srcString, size_t srcLength, BOOL moreToCome = FALSE) {
	Initialize (srcString, srcLength, moreToCome);
    }
    CTextBuffer () {
	Initialize (NULL, 0, FALSE);
    }

protected:
    void Initialize (LPCSTR srcString, size_t srcLength, BOOL moreToCome);

//  Destruction
public:
    ~CTextBuffer();

//  Diagnostics
#ifdef _DEBUG
public:
    void Dump (CDumpContext& dc) const;
#endif

//  Operations
public:
    operator !=			(LPCSTR	operand) const { return !(*this == operand); }
    operator ==			(LPCSTR	operand) const;

    unsigned long Length	() const { return m_size; }
    CTextCursor* Cursor		();

    CTextPart* TextPartAtOffset	(unsigned long& offset) const;

    unsigned long DistanceUntil	(unsigned long offset, LPCSTR charSet) const;
    unsigned long DistanceWhile	(unsigned long offset, LPCSTR charSet) const;
    unsigned long DistanceTo	(unsigned long offset, LPCSTR pattern) const;
    unsigned long DistanceToEnd	(unsigned long offset) const {return m_size - FixedOffset (offset); }

    void DosSubstring		(CString& rResult, unsigned long offset, unsigned long count) const;
    void UnixSubstring		(CString& rResult, unsigned long offset, unsigned long count) const;

    void DosText		(CString& rResult) const;
    void UnixText		(CString& rResult) const;

    void GetSynopsis		(CString &synopsis) const;

    void AppendLines		(CString&	string) { AppendLines (string, (size_t)string.GetLength()); }
    void AppendLines		(LPCSTR		string) { AppendLines (string, strlen (string)); }
    void AppendLines		(LPCSTR		string, size_t stringLength, BOOL moreToCome = TRUE);

    void AppendBytes		(CString& 	string) { AppendBytes (string, (size_t)string.GetLength()); }
    void AppendBytes		(LPCSTR		string) { AppendBytes (string, strlen (string)); }
    void AppendBytes		(LPCSTR		string, size_t stringLength, BOOL moreToCome = TRUE);

    void Trim			(BOOL removeLastNewLine);

protected:
    void NormalizeRange (unsigned long &offset, unsigned long &count) const;

    unsigned long FixedOffset (unsigned long offset) const {
	    return offset > Length () ? Length () : offset;
    }

//  State
protected:
    CTextPart*		m_pFirstPart;
    CTextPart*		m_pLastPart;
    unsigned long	m_size;

//  Handlers
protected:
    // Generated message map functions
    //{{AFX_MSG(CTextBuffer)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CTextBuffer)
    BOOL m_returnUnixStyleNewLines;
    afx_msg long OALength();
    afx_msg BSTR OAText();
    afx_msg BSTR OAUnixText();
    afx_msg LPDISPATCH OACursor();
    afx_msg long OADistanceUntil(long offset, LPCSTR charSet);
    afx_msg long OADistanceWhile(long offset, LPCSTR charSet);
    afx_msg long OADistanceTo(long offset, LPCSTR pattern);
    afx_msg long OADistanceToEnd(long offset);
    afx_msg BSTR OAGet(long offset, long count);
    afx_msg BSTR OAGetUntil(long offset, LPCSTR charSet);
    afx_msg BSTR OAGetWhile(long offset, LPCSTR charSet);
    afx_msg BSTR OAGetTo(long offset, LPCSTR pattern);
    afx_msg BSTR OAGetToEnd(long offset);
    afx_msg BSTR OAGetLine(long offset);
    afx_msg BSTR OAGetCell(long offset);
    afx_msg BSTR OALeft(long count);
    afx_msg BSTR OARight(long count);
    afx_msg BSTR OAMid(long origin, long count);
    afx_msg BOOL OATruncated();
    //}}AFX_DISPATCH
    DECLARE_DISPATCH_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif
