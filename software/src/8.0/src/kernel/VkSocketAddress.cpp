/*****  Vision Kernel Socket Address Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "VkSocketAddress.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VkStatus.h"
#include "VkUUID.h"
#include "V_VString.h"


/************************
 ************************
 *****  'hostname'  *****
 ************************
 ************************/

char const *VkSocketAddress::hostname (char *pBuffer, size_t sBuffer) {
    return
#if defined(sun)
	sysinfo (SI_HOSTNAME, pBuffer, sBuffer) < 0
#else
	gethostname (pBuffer, sBuffer) < 0
#endif
	? "" : pBuffer;
}

bool VkSocketAddress::getIPAddress (
    VString const &rHostname, VString &rAddress
) {
    if (rHostname.isntEmpty ()) {
	struct hostent *pHostData;
	pHostData = gethostbyname ((char*) rHostname.content ());
	
	in_addr localip;
	strncpy ((char*)&localip, pHostData->h_addr_list[0], pHostData->h_length);
	rAddress.setTo (inet_ntoa (localip));
	return true;
    }
    return false;
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

bool VkSocketAddress::constructINetAddress (
    char const*			pAddressString,
    bool			fBindOnly,
    int				xType,
    int				Unused(xProtocol),
    VkStatus*			pStatusReturn
) {
    struct sockaddr_in *	psockaddr_in = &m_iAddress.inInternetFormat;
    char			hostName[1024];
    int				serviceOffset;
    char			serviceName[64];
    int				servicePort;
    struct hostent		*hostent;
    struct servent		*servent;

/*****  Initialize the address size, ...  *****/
    m_size = sizeof (*psockaddr_in);

/*****  ... parse the host name, ...  *****/
    serviceOffset = 0;
    STD_sscanf (pAddressString, " %1024[^ \t:] : %n", hostName, &serviceOffset);
    if (0 == serviceOffset) {
	char iBuffer[256];
	strcpy (hostName, hostname (iBuffer, sizeof (iBuffer)));
    }
    pAddressString += serviceOffset;

    if (fBindOnly)
	psockaddr_in->sin_addr.s_addr = INADDR_ANY;
    else if (
	INADDR_NONE != (psockaddr_in->sin_addr.s_addr = inet_addr (hostName)));
    else if (
	IsntNil (hostent = gethostbyname (hostName))
    )	memcpy (&psockaddr_in->sin_addr, hostent->h_addr, hostent->h_length);
    else {
	pStatusReturn->MakeFailureStatus ();
	return false;
    }

/*****  ... and service address, ... *****/
    if (1 == STD_sscanf (pAddressString, " %d", &servicePort))
	psockaddr_in->sin_port = htons ((u_short)servicePort);
    else if (
	0 == STD_sscanf (pAddressString, " %64s", serviceName)
    )	psockaddr_in->sin_port = htons (0);
    else if (
	IsntNil (
	    servent = getservbyname (
		serviceName,
		SOCK_STREAM == xType ? "tcp" :
		SOCK_DGRAM  == xType ? "udp" : ""
	    )
	)
    )	psockaddr_in->sin_port = (unsigned short)servent->s_port;
    else {
	pStatusReturn->MakeFailureStatus ();
	return false;
    }

/*****  ... and return the socket address if all was successful:  *****/
    return true;
}


bool VkSocketAddress::constructUnixAddress (
    char const*			pAddress,
    bool			fBindOnly,
    int				Unused(xType),
    int				Unused(xProtocol),
    VkStatus*			pStatusReturn
) {
    size_t sAddress = strcspn (pAddress, " \t\n");

    char*	pResultPoint = m_iAddress.inUnixFormat.sun_path;
    char const*	pResultLimit = pResultPoint + sizeof (m_iAddress.inUnixFormat.sun_path) - 1;

    if (pResultPoint + sAddress > pResultLimit) {
	pStatusReturn->MakeFailureStatus ();
	return false;
    }

    strncpy (pResultPoint, pAddress, sAddress);
    pResultPoint += sAddress;

    if (fBindOnly && (sAddress < 1 || '/' == pAddress[sAddress - 1])) {
	VString iSuffix;

	VkUUIDGenerate iUUID;
	iUUID.GetString (iSuffix);
	size_t sSuffix = iSuffix.length ();
	if (pResultPoint + sSuffix > pResultLimit) {
	    pStatusReturn->MakeFailureStatus ();
	    return false;
	}
	strcpy (pResultPoint, iSuffix);
	pResultPoint += sSuffix;
    }

    pResultPoint[0] = '\0';

    m_size = sizeof (m_iAddress.inUnixFormat.sun_family) + (
	pResultPoint - m_iAddress.inUnixFormat.sun_path
    );

    return true;
}


bool VkSocketAddress::constructGenericAddress (
    char const*			pAddressString,
    bool			Unused(fBindOnly),
    int				Unused(xType),
    int				Unused(xProtocol),
    VkStatus*			pStatusReturn
) {
    size_t limit = sizeof (m_iAddress) - sizeof (
	m_iAddress.inOpenVisionFormat.sa_family
    );
    size_t index = 0;
    u_long nextByte;
    int	   nextOffset;

    while (
	1 == STD_sscanf (
	    pAddressString, index > 0 ? " . %u%n" : "%u%n", &nextByte, &nextOffset
	)
    ) if (index < limit) {
	m_iAddress.inOpenVisionFormat.sa_data[index++] = (char)nextByte;
	pAddressString += nextOffset;
    }
    else {
	pStatusReturn->MakeFailureStatus ();
	return false;
    }

    m_size = index + sizeof (m_iAddress.inOpenVisionFormat.sa_family);

    return true;
}


bool VkSocketAddress::construct (
    char const*			pProtocolString,
    char const*			pAddressString,
    bool			fBindOnly,
    int	*			pTypeReturn,
    int *			pProtocolReturn,
    VkStatus*			pStatusReturn
) {
/*****  Parse the protocol string, ...  *****/
    *pProtocolReturn		= 0;
    bool bNotAWildFamily	= pProtocolString[strspn (pProtocolString, " \t")] != '-';
    bool familyNameSpecified	= false;
    bool typeNameSpecified	= false;

/*----------------------------------------------------------------------------------*
 *  The extra validity test associated with 'bNotAWildFamily' works around a bug in
 *  the OpenVMS C RTL which incorrectly accepts a solitary '-' (as in '-/...') as a
 *  valid input to a %u format.  This is wrong on two counts: '-' by itself isn't a
 *  a valid number and unsigned numbers are never negative!
 *----------------------------------------------------------------------------------*/
    int xFamily; char familyName[64], typeName[64];
    if (bNotAWildFamily && STD_sscanf (
	    pProtocolString, "%u/%u/%u", &xFamily, pTypeReturn, pProtocolReturn
	) >= 2
    );
    else if (
	bNotAWildFamily && STD_sscanf (
	    pProtocolString, "%u/%64[^/]/%u", &xFamily, typeName, pProtocolReturn
	) >= 2
    ) typeNameSpecified = true;
    else if (
	STD_sscanf (
	    pProtocolString, "%64[^/]/%u/%u", familyName, pTypeReturn, pProtocolReturn
	) >= 2
    ) familyNameSpecified = true;
    else if (
	STD_sscanf (
	    pProtocolString, "%64[^/]/%64[^/]/%u", familyName, typeName, pProtocolReturn
	)
    ) familyNameSpecified = typeNameSpecified = true;
    else {
	pStatusReturn->MakeFailureStatus ();
	return false;
    }

/*****  ...  convert symbolic names if necessary, ...  *****/
    if (!familyNameSpecified);
    else if (strcasecmp (familyName, "inet") == 0)
	xFamily = AF_INET;
    else if (strcasecmp (familyName, "unix") == 0)
	xFamily = AF_UNIX;
    else if (strcmp (familyName, "-") == 0) 
	xFamily = strchr (pAddressString, '/') ? AF_UNIX : AF_INET;
    else {
	pStatusReturn->MakeFailureStatus ();
	return false;
    }

    if (!typeNameSpecified);
    else if (0 == strcasecmp (typeName, "stream"))
	*pTypeReturn = SOCK_STREAM;
    else if (0 == strcasecmp (typeName, "dgram"))
	*pTypeReturn = SOCK_DGRAM;
    else {
	pStatusReturn->MakeFailureStatus ();
	return false;
    }

/*****  ... and build the address:  *****/
    m_iAddress.inGenericFormat.sa_family = (unsigned short)xFamily;
    switch (xFamily) {
    case AF_INET:
	return constructINetAddress (
	    pAddressString, fBindOnly, *pTypeReturn, *pProtocolReturn, pStatusReturn
	);
    case AF_UNIX:
	return constructUnixAddress (
	    pAddressString, fBindOnly, *pTypeReturn, *pProtocolReturn, pStatusReturn
	);
    default:
	return constructGenericAddress (
	    pAddressString, fBindOnly, *pTypeReturn, *pProtocolReturn, pStatusReturn
	);
    }
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

/***************************************
 *****  Classification Predicates  ***** 
 ***************************************/

bool VkSocketAddress::isMulticast () const {
    unsigned char const iNetOctet = reinterpret_cast<unsigned char const*>(&m_iAddress.inInternetFormat.sin_addr.s_addr)[0];
    return familyIsINet () && iNetOctet >= 224 && iNetOctet < 240;
}

bool VkSocketAddress::specifiesPort () const {
    return familyIsINet () && m_iAddress.inInternetFormat.sin_port != 0;
}

/***************************************
 *****  GetSocketName/GetPeerName  *****
 ***************************************/

int VkSocketAddress::GetSocketName (SOCKET hSocket, VkStatus *pStatusReturn) {
    m_size = sizeof (m_iAddress);
    if (getsockname (hSocket, &m_iAddress.inGenericFormat, &m_size) < 0) {
	pStatusReturn->MakeErrorStatus ();
	return -1;
    }
    else if (m_size > sizeof (m_iAddress)) {
	pStatusReturn->MakeFailureStatus ();
	return -1;
    }
    return 0;
}

int VkSocketAddress::GetPeerName (SOCKET hSocket, VkStatus *pStatusReturn) {
    m_size = sizeof (m_iAddress);
    if (getpeername (hSocket, &m_iAddress.inGenericFormat, &m_size) < 0) {
	pStatusReturn->MakeErrorStatus ();
	return -1;
    }
    else if (m_size > sizeof (m_iAddress)) {
	pStatusReturn->MakeFailureStatus ();
	return -1;
    }
    return 0;
}


/***********************
 ***********************
 *****  Formating  *****
 ***********************
 ***********************/

int VkSocketAddress::Format (char **ppString, size_t *psString) const {
    socklen_t addressSize = m_size;
    u_long hostNumber;
    size_t sString;
    char * pString;
    char * pCursor;

/*****  Initialize the return parameters, ...  *****/
    *ppString = NilOf (char *);
    *psString = 0;

/*****  ... format the socket based on its address family, ...  *****/
    switch (m_iAddress.inGenericFormat.sa_family) {
    case AF_INET:
	if (addressSize != sizeof (m_iAddress.inInternetFormat))
	    return -1;

	hostNumber = m_iAddress.inInternetFormat.sin_addr.s_addr;
	if (INADDR_ANY == hostNumber) {
	    char iBuffer[256];
	    char const *pHostname = hostname (iBuffer, sizeof (iBuffer));
	    sString = strlen (pHostname) + 12;
	    if (IsNil (pString = (char *)allocate (sString)))
		return -1;
	    sprintf (pString, "%s:%u", pHostname, ntohs (m_iAddress.inInternetFormat.sin_port));
	}
	else {
	    sString = 30;
	    if (IsNil (pString = (char *)allocate (sString)))
		return -1;
	    sprintf (
		pString,
		"%u.%u.%u.%u:%u",
		((unsigned char *)&hostNumber)[0],
		((unsigned char *)&hostNumber)[1],
		((unsigned char *)&hostNumber)[2],
		((unsigned char *)&hostNumber)[3],
		ntohs (m_iAddress.inInternetFormat.sin_port)
	    );
	}
	break;

    case AF_UNIX:
	if (addressSize < sizeof (m_iAddress.inUnixFormat.sun_family)
	||  addressSize > sizeof (m_iAddress.inUnixFormat)
	)   return -1;

	addressSize -= sizeof (m_iAddress.inUnixFormat.sun_family);

	sString = addressSize;
	if (IsNil (pString = (char *)allocate (sString + 1)))
	    return -1;

	strncpy (pString, m_iAddress.inUnixFormat.sun_path, sString);
	pString[sString] = '\0';
	break;

    default:
	if (addressSize < sizeof (m_iAddress.inGenericFormat.sa_family) || addressSize > sizeof (m_iAddress))
	    return -1;

	addressSize -= sizeof (m_iAddress.inGenericFormat.sa_family);

	sString = 4 * addressSize;
	if (IsNil (pString = pCursor = (char *)allocate (sString + 1)))
	    return -1;

	pString[0] = '\0';
	for (socklen_t addressIndex = 0; addressIndex < addressSize; addressIndex++) {
	    sprintf (
		pCursor,
		addressIndex > 0 ? ".%u" : "%u",
		(unsigned char)m_iAddress.inGenericFormat.sa_data[addressIndex]
	    );
	    pCursor += strlen (pCursor);
	}
	break;
    }

/*****  ... update the return parameters, ...  *****/
    *ppString = pString;

/*****  ... deleting trailing '\0's to prevent down-stream problems, ...  *****/
    *psString = strlen (pString);

/*****  ... and return.  *****/
    return 0;
}

bool VkSocketAddress::getName (VString &rString) const {
    char *pName; size_t sName;
    if (Format (&pName, &sName) < 0)
	return false;

    rString.setTo (pName, sName);
    deallocate (pName);

    return true;
}
