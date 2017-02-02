#ifndef VkSocketAddress_Interface
#define VkSocketAddress_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca.h"

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VpSocket.h"

class VkStatus;
class VString;


/*************************
 *****  Definitions  *****
 *************************/

struct sockaddr_ov {
    unsigned short sa_family;		/* address family */
    char           sa_data[256];	/* address data */
};

/**
 * Descriptor for socket addresses of various types.
 */
class Vca_API VkSocketAddress : public VTransient {
    DECLARE_FAMILY_MEMBERS (VkSocketAddress, VTransient);

//  Hostname
public:
    static char const *hostname (char *pBuffer, size_t sBuffer);
    static bool getIPAddress (VString const &rHostName, VString &rIPAddress);

//  Construction
public:
    VkSocketAddress (sockaddr const &rAddress, socklen_t sAddress = sizeof (sockaddr)) {
	setTo (rAddress, sAddress);
    }
    VkSocketAddress (sockaddr_in const &rAddress) {
	setTo (rAddress);
    }
    VkSocketAddress (VkSocketAddress const &rOther) {
	setTo (rOther);
    }
    VkSocketAddress () {
	m_iAddress.inGenericFormat.sa_family = AF_UNSPEC;
    }

    bool construct (
	char const	*pProtocolString,
	char const	*pAddressString,
	bool		 fBindOnly,
	int		*pTypeReturn,
	int		*pProtocolReturn,
	VkStatus	*pStatusReturn
    );

protected:
    bool constructINetAddress (
	char const	*pAddressString,
	bool		 fBindOnly,
	int		 xType,
	int		 xProtocol,
	VkStatus	*pStatusReturn
    );
    bool constructUnixAddress (
	char const	*pAddressString,
	bool		 fBindOnly,
	int		 xType,
	int		 xProtocol,
	VkStatus	*pStatusReturn
    );
    bool constructGenericAddress (
	char const	*pAddressString,
	bool		 fBindOnly,
	int		 xType,
	int		 xProtocol,
	VkStatus	*pStatusReturn
    );

//  Access
public:
    int Format (char **ppString, size_t *psString) const;

    unsigned short family () const {
	return m_iAddress.inGenericFormat.sa_family;
    }
    bool familyIs () const {
	return family () != AF_UNSPEC;
    }
    bool familyIsINet () const {
	return family () == AF_INET;
    }
    bool familyIsUnix () const {
	return family () == AF_UNIX;
    }

    bool getAddress (struct sockaddr &rAddress) const {
	if (familyIs ()) {
	    rAddress = m_iAddress.inGenericFormat;
	    return true;
	}
	return false;
    }
    bool getAddress (struct sockaddr_in &rAddress) const {
	if (familyIsINet ()) {
	    rAddress = m_iAddress.inInternetFormat;
	    return true;
	}
	return false;
    }
    bool getAddress (struct sockaddr_un &rAddress) const {
	if (familyIsUnix ()) {
	    rAddress = m_iAddress.inUnixFormat;
	    return true;
	}
	return false;
    }
    bool getName (VString &rName) const;

    bool isMulticast () const;

    bool specifiesPort () const;

    char const *unixPathName () const {
	return familyIsUnix () ? m_iAddress.inUnixFormat.sun_path : 0;
    }

//  Update
public:
    void setTo (sockaddr const &rAddress, socklen_t sAddress = sizeof (sockaddr)) {
	m_size = sAddress;
	if (m_size > sizeof (m_iAddress))
	    m_size = sizeof (m_iAddress);
	memcpy (&m_iAddress.inGenericFormat, &rAddress, m_size);
    }
    void setTo (sockaddr_in const &rAddress) {
	m_size = sizeof (sockaddr_in);
	memcpy (&m_iAddress.inInternetFormat, &rAddress, m_size);
    }
    void setTo (ThisClass const &rOther) {
	m_size = rOther.m_size;
	m_iAddress = rOther.m_iAddress;
    }

//  Use
public:
    int GetSocketName (SOCKET hSocket, VkStatus *pStatusReturn);
    int GetPeerName (SOCKET hSocket, VkStatus *pStatusReturn);

    int bind (SOCKET hSocket) const {
	return ::bind (hSocket, &m_iAddress.inGenericFormat, m_size);
    }
    int connect (SOCKET hSocket) const {
	return ::connect (hSocket, &m_iAddress.inGenericFormat, m_size);
    }

//  State
protected:
    /** Size of this socket's address. */
    socklen_t			m_size;
    /** The address of this socket. */
    union address_t {
	struct sockaddr		inGenericFormat;
	struct sockaddr_in	inInternetFormat;
	struct sockaddr_un	inUnixFormat;
	struct sockaddr_ov	inOpenVisionFormat;
    }				m_iAddress;
};


#endif
