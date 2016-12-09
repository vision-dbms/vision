#ifndef CLocatorAgent_Interface
#define CLocatorAgent_Interface

/************************
 *****  Components  *****
 ************************/

#include "NetworkClient.h"

/**************************
 *****  Declarations  *****
 **************************/

class CHostByNameAgent;
class CServiceByNameAgent;
class CProtocolByNameAgent;


/*************************
 *****  Definitions  *****
 *************************/

class CLocatorAgent : public CNetworkClient
{
    DECLARE_DYNAMIC (CLocatorAgent)

//  Friends
    friend CHostByNameAgent;
    friend CServiceByNameAgent;
    friend CProtocolByNameAgent;

//  Construction
public:
    CLocatorAgent (CNetworkClient *pClient);

//  Destruction
protected:
    ~CLocatorAgent();

//  Operations
public:
    SOCKADDR_IN const&	address	() const { return m_address; }
    int			protocol() const { return m_protocol; }

    void SetHost	(LPCSTR hostname, BOOL dottedAddressPossible = TRUE);

    void SetPort	(LPCSTR service, LPCSTR protocol = "");
    void SetPort	(int port, BOOL hostByteOrder = TRUE);

    void SetProtocol	(LPCSTR protocol);
    void SetProtocol	(int protocol);

protected:
    virtual void OnHostData	(CHostByNameAgent	*pAgent);
    virtual void OnServiceData	(CServiceByNameAgent	*pAgent);
    virtual void OnProtocolData	(CProtocolByNameAgent	*pAgent);

    void ReportAddressIfComplete();

//  State
protected:
    unsigned int	m_hostIsValid		: 1,
			m_portIsValid		: 1,
			m_protocolIsValid	: 1;
    SOCKADDR_IN		m_address;
    int			m_protocol;
};



#endif
