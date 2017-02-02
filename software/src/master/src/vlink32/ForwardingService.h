#ifndef AFX_FORWARDINGSERVICE_H__C9C33B46_0459_11D2_BE30_00A0C931D9D4__INCLUDED_
#define AFX_FORWARDINGSERVICE_H__C9C33B46_0459_11D2_BE30_00A0C931D9D4__INCLUDED_
#pragma once 

/************************
 *****  Components  *****
 ************************/

#include "NetworkClient.h"

/**************************
 *****  Declarations  *****
 **************************/

class CApplication;
class CConnection;

/*************************
 *****  Definitions  *****
 *************************/

class CForwardingService : public CNetworkClient
{
    friend CApplication;
    friend CConnection;

//  Construction
protected:
    DECLARE_DYNAMIC(CForwardingService)

    CForwardingService(unsigned short iPortNumber);

//  Destruction
protected:
    ~CForwardingService();

//  Access
public:
    CConnection* Get (LPCSTR pConnectionId);

//  Update
protected:
    void Attach (CConnection* pConnection);
    void Detach (CConnection* pConnection);

//  Control
public:

//  Event Handlers
protected:
    void OnAccept (CSocketAgent* pSocket);

//  State
protected:
    unsigned short const	m_iPortNumber;
    CSocketAgent*		m_pSocket;
    CMapStringToOb		m_iConnectionRegistry;

// Operations
public:
// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CForwardingService)
	    // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_VIRTUAL

// Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CForwardingService)
	    // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
    // Generated OLE dispatch map functions
    //{{AFX_DISPATCH(CForwardingService)
	    // NOTE - the ClassWizard will add and remove member functions here.
    //}}AFX_DISPATCH
    DECLARE_DISPATCH_MAP()
    DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORWARDINGSERVICE_H__C9C33B46_0459_11D2_BE30_00A0C931D9D4__INCLUDED_)
