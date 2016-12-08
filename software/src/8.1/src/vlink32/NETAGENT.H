#ifndef CNetworkAgent_Interface
#define CNetworkAgent_Interface

/************************
 *****  Components  *****
 ************************/

#include "NetworkClient.h"

/**************************
 *****  Declarations  *****
 **************************/

class CLocatorAgent;

/*************************
 *****  Definitions  *****
 *************************/

/////////////////////////////////////////////////////////////////////////////
//  CXbyYAgent
/////////////////////////////////////////////////////////////////////////////

class CXbyYAgent : public CNetworkAgent<CLocatorAgent>
{
    DECLARE_DYNAMIC (CXbyYAgent)

//  Construction
protected:
    CXbyYAgent (CLocatorAgent *pClient);

//  Destruction
public:
    ~CXbyYAgent ();

//  Operations
protected:
    void BeginQuery ();
    void EndQuery   ();

    void Stop	    ();

protected:
    virtual enum CNetwork::QueryMethod QueryMethod () = 0;

    virtual void DoAsynchronousQuery	();
    virtual void DoSynchronousQuery	();
    virtual void DoWiredQuery		();

//  State
protected:
    HANDLE m_hRequest;
};


/////////////////////////////////////////////////////////////////////////////
//  CHostByNameAgent
/////////////////////////////////////////////////////////////////////////////

class CHostByNameAgent : public CXbyYAgent
{
    DECLARE_DYNAMIC (CHostByNameAgent)

//  Construction
public:
    CHostByNameAgent (CLocatorAgent *pClient, LPCSTR host);

//  Destruction
protected:
    ~CHostByNameAgent ();

//  Diagnostics
#ifdef _DEBUG
public:
    void Dump (CDumpContext& dc) const;
#endif

//  Access
public:
    struct hostent const &host () const { return m_hostEntry; }

//  Operations
protected:
    virtual enum CNetwork::QueryMethod QueryMethod () { return m_pNetwork->HostQueryMethod (); }

    virtual void DoAsynchronousQuery	();
    virtual void DoSynchronousQuery	();
    virtual void OnWSAEvent		(LPARAM lParam);

//  State
protected:
    CString		m_host;
    union {
	struct hostent	m_hostEntry;
	BYTE		m_requestBuffer[MAXGETHOSTSTRUCT];
    };
};


/////////////////////////////////////////////////////////////////////////////
//  CProtocolByNameAgent
/////////////////////////////////////////////////////////////////////////////

class CProtocolByNameAgent : public CXbyYAgent
{
    DECLARE_DYNAMIC (CProtocolByNameAgent)

//  Construction
public:
    CProtocolByNameAgent (CLocatorAgent *pClient, LPCSTR protocol);

//  Destruction
protected:
    ~CProtocolByNameAgent ();

//  Diagnostics
#ifdef _DEBUG
public:
    void Dump (CDumpContext& dc) const;
#endif

//  Access
public:
    struct protoent const &protocol () const { return m_protocolEntry; }

//  Operations
protected:
    virtual enum CNetwork::QueryMethod QueryMethod () { return m_pNetwork->ProtocolQueryMethod (); }

    virtual void DoAsynchronousQuery	();
    virtual void DoSynchronousQuery	();
    virtual void DoWiredQuery		();
    virtual void OnWSAEvent		(LPARAM lParam);

//  State
protected:
    CString		m_protocol;
    union {
	struct protoent	m_protocolEntry;
	BYTE		m_requestBuffer[MAXGETHOSTSTRUCT];
    };
};


/////////////////////////////////////////////////////////////////////////////
//  CServiceByNameAgent
/////////////////////////////////////////////////////////////////////////////

class CServiceByNameAgent : public CXbyYAgent
{
    DECLARE_DYNAMIC (CServiceByNameAgent)

//  Construction
public:
    CServiceByNameAgent (CLocatorAgent *pClient, LPCSTR service, LPCSTR protocol);

//  Destruction
protected:
    ~CServiceByNameAgent ();

//  Diagnostics
#ifdef _DEBUG
public:
    void Dump (CDumpContext& dc) const;
#endif

//  Access
public:
    struct servent const &service () const { return m_serviceEntry; }

//  Operations
protected:
    virtual enum CNetwork::QueryMethod QueryMethod () { return m_pNetwork->ServiceQueryMethod (); }

    virtual void DoAsynchronousQuery	();
    virtual void DoSynchronousQuery	();
    virtual void DoWiredQuery		();
    virtual void OnWSAEvent		(LPARAM lParam);

//  State
protected:
    CString		m_service;
    CString		m_protocol;
    BOOL		m_protocolWasNull;
    union {
	struct servent	m_serviceEntry;
	BYTE		m_requestBuffer[MAXGETHOSTSTRUCT];
    };
};


#endif
