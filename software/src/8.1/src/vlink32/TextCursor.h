#ifndef CTextCursor_Interface
#define CTextCursor_Interface

/************************
*****  Components  *****
************************/

#include "vlinkcmd.h"
#include "Collection.h"
#include "TextBuffer.h"

/**************************
*****  Declarations  *****
**************************/

/*************************
*****  Definitions  *****
*************************/

class CTextCursor : public CVLinkCmdTarget
{
    DECLARE_DYNAMIC(CTextCursor)

// Globals
public:
    static CCollectionKernel	Instances;

// Properties
protected:
    CTextBuffer* const	m_pBuffer;
    unsigned long	m_position;

// Construction
public:
    CTextCursor(CTextBuffer* pBuffer, unsigned long position = 0);

// Destruction
public:
    ~CTextCursor ();

// Diagnostics
#ifdef _DEBUG
public:
    void Dump (CDumpContext& dc) const;
#endif

// Operations
public:
    CTextCursor* Clone () const {
	CTextCursor* pClone;
	TRY {
	    pClone = new CTextCursor (m_pBuffer, m_position);
	} CATCH(CMemoryException, pException) {
	    pClone = NULL;
	} END_CATCH
	return pClone;
    }

    BOOL IsAtBeginning		() const { return 0 == m_position; }
    BOOL IsntAtBeginning	() const { return 0 != m_position; }
    BOOL IsAtEnd		() const { return m_pBuffer->Length () == m_position; }
    BOOL IsntAtEnd		() const { return m_pBuffer->Length () != m_position; }

    unsigned long DistanceToEnd	()		const { return m_pBuffer->DistanceToEnd (m_position); }
    unsigned long DistanceUntil	(LPCSTR charSet)const { return m_pBuffer->DistanceUntil (m_position, charSet); }
    unsigned long DistanceWhile	(LPCSTR charSet)const { return m_pBuffer->DistanceWhile (m_position, charSet); }
    unsigned long DistanceTo	(LPCSTR pattern)const {	return m_pBuffer->DistanceTo	(m_position, pattern); }

    unsigned long GetPosition	() const { return m_position; }
    unsigned long FixedPosition	(long newPosition) const {
	unsigned long result = newPosition < 0 ? 0 : (unsigned long)newPosition;
	return result > m_pBuffer->Length() ? m_pBuffer->Length () : result;
    }
    void SetPosition 		(long newPosition) { m_position = FixedPosition (newPosition); }

    void Skip			(long	count  ) { SetPosition (m_position + count); }
    void SkipUntil		(LPCSTR charSet) { m_position += DistanceUntil	(charSet); }
    void SkipWhile		(LPCSTR charSet) { m_position += DistanceWhile	(charSet); }
    void SkipTo			(LPCSTR pattern) { m_position =+ DistanceTo		(pattern); }

// Handlers
protected:
    // Generated message map functions
    //{{AFX_MSG(CTextCursor)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CTextCursor)
    BOOL m_returnUnixStyleNewLines;
    afx_msg long OAGetPosition();
    afx_msg void OASetPosition(long nNewValue);
    afx_msg LPDISPATCH OAClone();
    afx_msg LPDISPATCH OABuffer();
    afx_msg BOOL OAIsAtBeginning();
    afx_msg BOOL OAIsntAtBeginning();
    afx_msg BOOL OAIsAtEnd();
    afx_msg BOOL OAIsntAtEnd();
    afx_msg long OADistanceUntil(LPCSTR charSet);
    afx_msg long OADistanceWhile(LPCSTR charSet);
    afx_msg long OADistanceTo(LPCSTR pattern);
    afx_msg long OADistanceToEnd();
    afx_msg void OASkip(long count);
    afx_msg void OASkipUntil(LPCSTR charSet);
    afx_msg void OASkipWhile(LPCSTR charSet);
    afx_msg void OASkipTo(LPCSTR pattern);
    afx_msg BSTR OAGet(long count);
    afx_msg BSTR OAGetUntil(LPCSTR charSet);
    afx_msg BSTR OAGetWhile(LPCSTR charSet);
    afx_msg BSTR OAGetTo(LPCSTR pattern);
    afx_msg BSTR OAGetToEnd();
    afx_msg BSTR OAGetLine();
    afx_msg BSTR OAGetCell();
    afx_msg BSTR OAPeek(long count);
    afx_msg BSTR OAPeekUntil(LPCSTR charSet);
    afx_msg BSTR OAPeekWhile(LPCSTR charSet);
    afx_msg BSTR OAPeekTo(LPCSTR pattern);
    afx_msg BSTR OAPeekToEnd();
    afx_msg BSTR OAPeekLine();
    afx_msg BSTR OAPeekCell();
    afx_msg BSTR OAText();
    //}}AFX_DISPATCH
    DECLARE_DISPATCH_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif
