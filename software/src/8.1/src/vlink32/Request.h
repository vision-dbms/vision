#ifndef CRequest_Interface
#define CRequest_Interface

/************************
 *****  Components  *****
 ************************/

#include "vlinkcmd.h"
#include "Collection.h"

/**************************
 *****  Declarations  *****
 **************************/

class CForwardingAgent;
class CTextBuffer;

/*************************
 *****  Definitions  *****
 *************************/

class CRequest : public CVLinkCmdTarget
{
    friend class CConnection;
    friend class CEvaluator;

    DECLARE_DYNAMIC(CRequest)

//  Globals
public:
    static const int		g_defaultTimeout;
    static CCollectionKernel	Instances;

//  Construction
protected:
    CRequest (
	CConnection*		pConnection,
	CTextBuffer*		pRequestText,
	unsigned int		nAcksExpected,
	CForwardingAgent*	pSubmittingAgent
    );

//  Destruction
public:
    ~CRequest();

//  Diagnostics
#ifdef _DEBUG
public:
    void Dump (CDumpContext& dc) const;
#endif

//  Operations
public:
    BOOL operator== (const CString	operand) const;
    BOOL operator== (LPCSTR		operand) const;
    BOOL operator!= (const CString	operand) const;
    BOOL operator!= (LPCSTR		operand) const;

    void GetSynopsis		(CString& synopsis) const;

    CRequest* QueueSuccessor	() const { return m_pQueueSuccessor; }

    BOOL ReplyIsComplete	() const { return 0 == m_unreceivedAckCount; }
    BOOL ReplyIsntComplete	() const { return 0 <  m_unreceivedAckCount; }

    long Timeout		() const;
    void WaitForReply		(long timeout) { OAWaitForReply (timeout); }
    BOOL CompletedWithin	(long timeout) { return OACompletedWithin (timeout); }

    BOOL Completed		() const { return !InProgress (); }
    BOOL InProgress		() const { return m_status == StatusInProgress; }
    BOOL Succeeded		() const { return m_status == StatusSucceeded; }
    BOOL Failed			() const { return m_status == StatusFailed; }

protected:
    void SetQueueSuccessor	(CRequest *pSuccessor) { m_pQueueSuccessor = pSuccessor; }

    void GetRequestForWire	(CString &rRequestText);

public:
    void GetReplyForWire	(CString &rReplyString);

protected:
    void GetReplyFromWire	(LPCSTR replyText, size_t replyLength, unsigned long ackCount = 0);

    void Succeed		();
    void Fail			();

    void ReleaseConnection	();
    void NotifyAgent		();

//  State
protected:
    CTextBuffer* const		m_pReplyBuffer;
    CTextBuffer* const		m_pRequestBuffer;
    CConnection*		m_pConnection;
    CForwardingAgent*		m_pSubmittingAgent;
    CRequest*			m_pQueueSuccessor;
    enum {
	StatusInProgress,
	StatusSucceeded,
	StatusFailed
    }				m_status;
    unsigned int		m_unreceivedAckCount;

// Handlers
protected:
    // Generated message map functions
    //{{AFX_MSG(CRequest)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CRequest)
    afx_msg void OAWaitForReply(long timeout);
    afx_msg void OAWaitForCompletion(long timeout);
    afx_msg BOOL OACompletedWithin(long timeout);
    afx_msg BOOL OACompleted();
    afx_msg BOOL OAInProgress();
    afx_msg BOOL OASucceeded();
    afx_msg BOOL OAFailed();
    afx_msg LPDISPATCH OAReplyBuffer();
    afx_msg LPDISPATCH OAReplyCursor();
    afx_msg BSTR OAReplyText();
    afx_msg BSTR OAUnixReplyText();
    afx_msg LPDISPATCH OARequestBuffer();
    afx_msg LPDISPATCH OARequestCursor();
    afx_msg BSTR OARequestText();
    afx_msg BSTR OAUnixRequestText();
    afx_msg BOOL OAReplyTruncated();
    afx_msg BOOL OARequestTruncated();
    //}}AFX_DISPATCH
    DECLARE_DISPATCH_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif
