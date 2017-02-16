#ifndef CSocketAgent_Interface
#define CSocketAgent_Interface

/************************
 *****  Components  *****
 ************************/

#include "NetworkClient.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class CSocketAgent : public CNetworkClient
{
    DECLARE_DYNAMIC (CSocketAgent)

//  Output Buffer Nested Type
public:
    class OutputBuffer {
	friend CSocketAgent;

    //  Construction
    public:
	OutputBuffer () : m_iCursor (0) {
	}

    //  Access
    public:
	BOOL IsEmpty	() const { return m_iCursor >= (unsigned int)m_iBuffer.GetLength (); }
	BOOL IsntEmpty	() const { return m_iCursor <  (unsigned int)m_iBuffer.GetLength (); }

    //  Update
    public:
	CString& GetBufferForUpdate () {
	    ResetIfEmpty ();
	    return m_iBuffer;
	}

	void ResetIfEmpty () {
	    if (IsEmpty ())
	    {
		m_iCursor = 0;
		m_iBuffer.Empty ();
	    }
	}

    //  State
    protected:
	CString		m_iBuffer;
	unsigned int	m_iCursor;
    };

//  Construction
public:
    CSocketAgent (CNetworkClient *pClient, SOCKET xSocket);

//  Destruction
public:
    ~CSocketAgent ();
    
//  Operations
public:
    void Stop ();

protected:
    void OnWSAEvent (LPARAM lParam);

public:
    SOCKET Accept (struct sockaddr* pRemoteAddress = 0, int* pRemoteAddressLength = 0);

    void Bind (unsigned short iPortNumber);
    void Bind (struct sockaddr_in const *pAddress);
    void Bind (struct sockaddr    const *pAddress);

    void Connect (struct sockaddr_in const *pAddress);
    void Connect (struct sockaddr    const *pAddress);

    void Listen (int backlog = SOMAXCONN);

    void Send (OutputBuffer& rBuffer) {
	Send (rBuffer.m_iCursor, rBuffer.m_iBuffer);
	rBuffer.ResetIfEmpty ();
    }

    void Send (unsigned int &rOffset, CString const& pData) { Send (rOffset, pData, pData.GetLength ()); }
    void Send (unsigned int &rOffset, char const*    pData) { Send (rOffset, pData, strlen (pData)); }
    void Send (unsigned int &rOffset, char const*    pData, unsigned int length);

    BOOL Receive (unsigned int& rBytesTransferred, char* pBuffer, unsigned int sBuffer);

    BOOL ReceivedByteCount	(int *bytesReceived);

    BOOL LocalAddress (CString &s);
    BOOL LocalAddress (struct sockaddr *address);

    BOOL RemoteAddress (CString &s);
    BOOL RemoteAddress (struct sockaddr *address);

    BOOL IsReadable	() const { return !m_recvBlocked; };
    BOOL IsWriteable	() const { return !m_sendBlocked; };

//  State
protected:
    SOCKET	m_xSocket;
    BOOL	m_recvBlocked;
    BOOL	m_sendBlocked;
};


#endif
