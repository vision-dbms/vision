/*****  VSockets Implementation  *****/

/*************************
 *****  Definitions  *****
 *************************/

/*****  General  *****/
#include "VStdLib.h"

/******  Specific  *****/
#include "VSockets.h"

#if defined(_WIN32)
#pragma comment(lib, "ws2_32.lib")

#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#endif

#ifndef INADDR_NONE
#define INADDR_NONE ((u_long)0xffffffff)
#endif

#if defined(_AIX)
typedef size_t sockaddrsize_t;
#else
typedef int sockaddrsize_t;
#endif


/**************************
 *****  VUID::Access  *****
 **************************/

char const* VUID::asString (char const *pSeparator) {
    static char iWorkBuffer[256];

    sprintf (
	iWorkBuffer,
	"%08x%s%04x%s%04x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x%s%02x",
	m_u0,
	pSeparator, m_u1,
	pSeparator, m_u2,
	pSeparator, m_u3[0],
	pSeparator, m_u3[1],
	pSeparator, m_u3[2],
	pSeparator, m_u3[3],
	pSeparator, m_u4[0],
	pSeparator, m_u4[1],
	pSeparator, m_u4[2],
	pSeparator, m_u4[3]
    );

    return iWorkBuffer;
}


/*************************************
 *****  VSockets::Construction  ******
 *************************************/

VSockets::VSockets (bool fInitializationRequired) {
#if defined(_WIN32)
    if (fInitializationRequired) {
	WSAData wsaData;

	if (WSAStartup (0x0101, &wsaData) != 0)
	    m_fInitialized = false;
	else if (0x0101 != wsaData.wVersion) {
	    WSACleanup ();
	    m_fInitialized = false;
	}
	else m_fInitialized = true;
    }
    else m_fInitialized = false;

#else
    m_fInitialized = fInitializationRequired;
#endif
}


bool VSockets::socketName (
    SOCKET xSocket, unsigned char iHost[4], unsigned short &xPort
) const
{
    struct sockaddr_in iSocketAddress;
    sockaddrsize_t sSocketAddress = sizeof (iSocketAddress);

    if (INVALID_SOCKET == xSocket || SOCKET_ERROR == getsockname (
	    xSocket, (struct sockaddr *)&iSocketAddress, &sSocketAddress
	)
    ) return false;

    memcpy (iHost, &iSocketAddress.sin_addr, sizeof (iHost));
    xPort = ntohs (iSocketAddress.sin_port);

    return true;
}

bool VSockets::peerName (
    SOCKET xSocket, unsigned char iHost[4], unsigned short &xPort
) const
{
    struct sockaddr_in iSocketAddress;
    sockaddrsize_t sSocketAddress = sizeof (iSocketAddress);

    if (INVALID_SOCKET == xSocket || SOCKET_ERROR == getpeername (
	    xSocket, (struct sockaddr *)&iSocketAddress, &sSocketAddress
	)
    ) return false;

    memcpy (iHost, &iSocketAddress.sin_addr, sizeof (iHost));
    xPort = ntohs (iSocketAddress.sin_port);

    return true;
}


/***********************************
 *****  VSockets::Destruction  *****
 ***********************************/

VSockets::~VSockets ()
{
#if defined(_WIN32)
    if (m_fInitialized)
    {
	m_fInitialized = false;

	WSACleanup ();
    }
#endif
}


/****************************************
 *****  VSockets::Remote Execution  *****
 ****************************************/

bool VSockets::rexec (
    SOCKET&		xDataSocket,
    SOCKET&		xControlSocket,
    char *		pErrorMessageBuffer,
    size_t		sErrorMessageBuffer,
    char const *	pHostName,
    char const *	pUserName,
    char const *	pPassword, 
    char const *	pCommand,
    u_short		xHostPort,
    bool		bFull
)
{
    u_short xControlServicePort = 0;
    SOCKET xControlServiceSocket;

//  If the Full implementation of rexec is requested, 
//  create a service that will accept the control connection...
    if (bFull) {
	xControlServiceSocket = serve (xControlServicePort);

	if (INVALID_SOCKET == xControlServiceSocket) {
	    strncpy (pErrorMessageBuffer, "Service Creation Failed", sErrorMessageBuffer);
	    return false;
	}
    }

//  Create the data connection...
    xDataSocket = connect (pHostName, xHostPort ? xHostPort : 512);
    if (INVALID_SOCKET == xDataSocket) {
	if (bFull)
	    close (xControlServiceSocket);
	strncpy (pErrorMessageBuffer, "Data Connection Creation Failed", sErrorMessageBuffer);
	return false;
    }

//  Send the parameters to the remote 'rexecd'...
    char iPortName[16];
    sprintf (iPortName, "%u", xControlServicePort);

    send (xDataSocket, iPortName, strlen (iPortName) + 1);
    send (xDataSocket, pUserName, strlen (pUserName) + 1);
    send (xDataSocket, pPassword, strlen (pPassword) + 1);
    send (xDataSocket, pCommand , strlen (pCommand ) + 1);

    if (bFull) {
//  Accept the control/error connection from the remote 'rexecd'...
	xControlSocket = accept (
	    xControlServiceSocket, (struct sockaddr *)0, (sockaddrsize_t *)0
	);
	close (xControlServiceSocket);
	if (INVALID_SOCKET == xControlSocket) {
	    close (xDataSocket);
	    strncpy (
		pErrorMessageBuffer, "Control Connection Creation Failed", sErrorMessageBuffer
	    );
	    return false;
	}
    }

//  Read the result byte from the remote 'rexecd'...
    char xControlByte;
    if (1 != recv (xDataSocket, &xControlByte, 1)) {
	close (xDataSocket);
	if (bFull)
	    close (xControlSocket);
	strncpy (
	    pErrorMessageBuffer, "Status Acquisition Failed", sErrorMessageBuffer
	);
	return false;
    }

//  If rexecd could not start the process, find out why, ...
    if (0 != xControlByte) {
	int sMessageChunk = recv (xDataSocket, pErrorMessageBuffer, sErrorMessageBuffer);
	while (sMessageChunk > 0) {
	    pErrorMessageBuffer += sMessageChunk;
	    sErrorMessageBuffer -= sMessageChunk;
	    sMessageChunk = recv (xDataSocket, pErrorMessageBuffer, sErrorMessageBuffer);
	}
	*pErrorMessageBuffer = '\0';

	close (xDataSocket);
	if (bFull)
	    close (xControlSocket);

	return false;
    }

//  Otherwise return successfully, ...
    return true;
}


/*******************************
 *****  VSockets::Connect  *****
 *******************************/

SOCKET VSockets::connect (
    char const *pHostName, u_short xHostPort
)
{
    u_long iHostIPAddress = inet_addr (pHostName);
    if (INADDR_NONE == iHostIPAddress)
    {
	struct hostent *pHostEntry = gethostbyname (pHostName);
	if (pHostEntry && AF_INET == pHostEntry->h_addrtype)
	    iHostIPAddress = *(u_long *)pHostEntry->h_addr;
	else return INVALID_SOCKET;
    }

    struct sockaddr_in iSocketAddress;
    iSocketAddress.sin_family = AF_INET;
    iSocketAddress.sin_port = htons (xHostPort);
#if defined(_WIN32)
    iSocketAddress.sin_addr.S_un.S_addr = iHostIPAddress;
#else
    iSocketAddress.sin_addr.s_addr = iHostIPAddress;
#endif

    SOCKET xSocket = socket (AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == xSocket)
	return INVALID_SOCKET;

    if (SOCKET_ERROR == ::connect (
	    xSocket, (struct sockaddr *)&iSocketAddress, sizeof (iSocketAddress)
	)
    )
    {
	close (xSocket);
	return INVALID_SOCKET;
    }

    return xSocket;
}


/*****************************
 *****  VSockets::Serve  *****
 *****************************/

SOCKET VSockets::serve (u_short& xPort, int sBacklog)
{
    struct sockaddr_in iSocketAddress;
    iSocketAddress.sin_family = AF_INET;
    iSocketAddress.sin_port = htons (xPort);
#if defined(_WIN32)
    iSocketAddress.sin_addr.S_un.S_addr = INADDR_ANY;
#else
    iSocketAddress.sin_addr.s_addr = INADDR_ANY;
#endif

    SOCKET xSocket = socket (AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == xSocket)
	return INVALID_SOCKET;

    sockaddrsize_t sSocketAddress = sizeof (iSocketAddress);
    if (SOCKET_ERROR == bind (
	    xSocket, (struct sockaddr *)&iSocketAddress, sSocketAddress
	) || SOCKET_ERROR == getsockname (
	    xSocket, (struct sockaddr *)&iSocketAddress, &sSocketAddress
	) || SOCKET_ERROR == listen (xSocket, sBacklog)
    )
    {
	close (xSocket);
	return INVALID_SOCKET;
    }

    xPort = ntohs (iSocketAddress.sin_port);

    return xSocket;
}


/***************************
 *****  VSockets::I/O  *****
 ***************************/

int VSockets::send (SOCKET xSocket, char const *pData, size_t sData)
{
    char const *pDataRemaining = pData;
    size_t	sDataRemaining = sData;
    do {
	int sChunkSent = ::send (
	    xSocket, pDataRemaining, sDataRemaining, 0
	);
	if (sChunkSent > 0)
	{
	    pDataRemaining += sChunkSent;
	    sDataRemaining -= sChunkSent;
	}
	else break;
    } while (sDataRemaining > 0);

    return (int)(sData - sDataRemaining);
}

int VSockets::recv (SOCKET xSocket, char *pBuffer, size_t sBuffer)
{
    return ::recv (xSocket, pBuffer, sBuffer, 0);
}


/****************************************
 *****  VSockets::Close & Shutdown  *****
 ****************************************/

int VSockets::endReception (SOCKET xSocket) {
    return shutdown (xSocket, 0);
}

int VSockets::endTransmission (SOCKET xSocket) {
    return shutdown (xSocket, 1);
}

int VSockets::close (SOCKET xSocket)
{
#if defined(_WIN32)
    return ::closesocket (xSocket);
#else
    return ::close (xSocket);
#endif
}
