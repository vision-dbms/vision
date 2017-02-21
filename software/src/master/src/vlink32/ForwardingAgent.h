#ifndef AFX_FORWARDINGAGENT_H__CD03FA5C_0477_11D2_BE30_00A0C931D9D4__INCLUDED_
#define AFX_FORWARDINGAGENT_H__CD03FA5C_0477_11D2_BE30_00A0C931D9D4__INCLUDED_
#pragma once

/************************
 *****  Components  *****
 ************************/

#include "NetworkClient.h"

/**************************
 *****  Declarations  *****
 **************************/

class CConnection;
class CForwardingService;
class CTextBuffer;

/*************************
 *****  Definitions  *****
 *************************/

class CForwardingAgent : public CNetworkClient
{
//  Construction
protected:
    DECLARE_DYNAMIC(CForwardingAgent)

public:
    CForwardingAgent(CForwardingService* pService, SOCKET xSocket);

//  Destruction
protected:
    ~CForwardingAgent();

//  State Machine Execution
protected:
    void CrankStateMachine ();

    void ProcessConnect		();
    void ProcessDisconnect	();
    void ProcessInputChunk	();
    void ProcessRequest		();

    BOOL GetSize (unsigned int& rSize, unsigned int iMaxSize = -1);

    void SetStateToExpectingCommand		();
    void SetStateToExpectingConnectionNameSize	();
    void SetStateToExpectingConnectionName	(unsigned int sInputRequired);
    void SetStateToExpectingInputChunkSize	();
    void SetStateToExpectingInputChunk		(unsigned int sInputRequired);
    void SetStateToExpectingResync		();

//  Output Generation and Processing
protected:
    void SendAck  (LPCSTR pWhat) { Send ('+', pWhat); }
    void SendNack (LPCSTR pWhat) { Send ('-', pWhat); }
    void SendReply();

    void Send (char iTag, LPCSTR pData) { Send (iTag, pData, strlen (pData)); }

    void Send (char iTag, LPCSTR pData, u_long sData);

    void DrainOutput ();

//  Event Handlers
public:
    void OnReply	(CRequest* pRequest);

protected:
    void OnReadable	(CSocketAgent* pSocket);
    void OnWriteable	(CSocketAgent* pSocket);
    void OnClose	(CSocketAgent* pSocket);

//  State
protected:
    CForwardingService*	const	m_pService;
    CSocketAgent*		m_pSocket;
    CConnection*		m_pConnection;
    CRequest*			m_pLastRequest;
    CTextBuffer*		m_pInputBuffer;
    char			m_iInputBuffer[1024];
    unsigned int		m_sInputAcquired;
    unsigned int		m_sInputRequired;
    unsigned int		m_sInputDeferred;
    enum {
	ExpectingCommand,
	ExpectingConnectionNameSize,
	ExpectingConnectionName,
	ExpectingInputChunkSize,
	ExpectingInputChunk,
	ExpectingResync
    }				m_xInputState;
    CSocketAgent::OutputBuffer	m_iOutputBuffer;

//  Operations
public:
// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CForwardingAgent)
	    // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_VIRTUAL

// Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CForwardingAgent)
	    // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CForwardingAgent)
	    // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_DISPATCH
    DECLARE_DISPATCH_MAP()
    DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORWARDINGAGENT_H__CD03FA5C_0477_11D2_BE30_00A0C931D9D4__INCLUDED_)
