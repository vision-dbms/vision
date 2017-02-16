/*****  VQueryAgent Implementation  ******/
//
//  This file can be compiled as a standalone program.  It has been compiled and tested under
//  Windows/NT using Microsoft Visual C++ (tm), and HP-UX 10.x, Solaris 5.5, and AIX 4.2 using
//  the standard C++ compiler available for those operating systems.  Assuming that this file
//  is named vquery.cpp, the following table lists the command used to compile it on the Unix
//  systems under which it has been tested:
//
//    Operating
//    System               Command
//   ----------            -------
//    AIX                  xlC -O -o vquery vquery.cpp
//    HP-UX                CC  -O -o vquery vquery.cpp
//    Solaris              CC  -O -o vquery vquery.cpp -lsocket -lnsl
// 
//  Under Windows/NT, this program can be compiled as a console application using Microsoft
//  Visual C++.  Because this program uses sockets, the wsock32.lib library must be added to
//  the object/library link option in the build settings for the project used to compile it.

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/*********************
 *****  Generic  *****
 *********************/

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <limits.h>

#include <sys/types.h>

#define True	1
#define False	0

#define ErrorExitValue	1
#define NormalExitValue	0

#define StandardTCPMSS	1460

#if defined(_WIN32)

/*************************
 *****  NT Specific  *****
 *************************/

#include <windows.h>
#include <io.h>

#define srand48(iSeed) srand(iSeed)
#define drand48() ((double)rand () / (double)(RAND_MAX + 1))

#define read		_read
#define strcasecmp	_stricmp
#define write		_write

#pragma comment(lib, "ws2_32.lib")

#else

/***************************
 *****  UNIX Specific  *****
 ***************************/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

typedef int SOCKET;

#define INVALID_SOCKET	-1
#define SOCKET_ERROR	-1

double drand48 (void);

#ifndef INADDR_NONE
#define INADDR_NONE ((u_long)0xffffffff)
#endif

#if defined(_AIX)
#include <strings.h>

typedef size_t sockaddrsize_t;
#else
typedef int sockaddrsize_t;
#endif

#endif

/******************
 *****  Self  *****
 ******************/

#include "vquery.h"


/***********************
 ***********************
 *****  Utilities  *****
 ***********************
 ***********************/

static char const* EnvironmentString (
    char const*			pVariableName,
    char const*			pDefaultValue
)
{
    char const *pVariableValue = getenv (pVariableName);
    return pVariableValue ? pVariableValue : pDefaultValue;
}

static char* Quote (char* pResult, char const* pSource)
{
    *pResult++ = '"';
    while (*pSource)
    {
	unsigned int sSpan = strcspn (pSource, "\\\"");
	strncpy (pResult, pSource, sSpan);
	pResult += sSpan;
	pSource += sSpan;
	if (*pSource)
	{
	    *pResult++ = '\\';
	    *pResult++ = *pSource++;
	}
    }
    *pResult++ = '"';

    return pResult;
}


/*******************************
 *******************************
 *****  Socket Management  *****
 *******************************
 *******************************/

/*********************
 *****  Globals  *****
 *********************/

#if defined(_WIN32)
static int WSInitialized = False;
#endif

/*********************
 *****  Startup  *****
 *********************/

static int StartSocketLayer (
    void
)
{
#if defined(_WIN32)
    WORD	wsVersion = MAKEWORD (1,1);
    WSADATA	wsData;

    if (!WSInitialized)
    {
	if (WSAStartup (wsVersion, &wsData) != 0)
	    return False;

	if (LOBYTE (wsData.wVersion) != 1 || HIBYTE (wsData.wVersion) != 1)
	{
	    WSACleanup ();
	    return False;
	}

	WSInitialized = True;
    }
#endif

    return True;
}

/**********************
 *****  Shutdown  *****
 **********************/

static int StopSocketLayer (
    void
)
{
#if defined(_WIN32)
    if (WSInitialized && WSACleanup ())
	return False;

#endif
    return True;
}


/*********************
 *********************
 *****  in_addr  *****
 *********************
 *********************/

/**************************
 *****  Construction  *****
 **************************/

static int in_addr_Construct (
    struct in_addr*		pThis, 
    char*			pHostName
)
{
    struct hostent*		pHostEntry;

    pThis->s_addr = inet_addr (pHostName);
    if (INADDR_NONE != pThis->s_addr)
	return True;

    if (0 != (pHostEntry = gethostbyname (pHostName)) && AF_INET == pHostEntry->h_addrtype)
    {
	memcpy (pThis, pHostEntry->h_addr, pHostEntry->h_length);
	return True;
    }

    return False;
}


/***************************
 ***************************
 ******  sockaddr_in  ******
 ***************************
 ***************************/

/**************************
 *****  Construction  *****
 **************************/

static int sockaddr_Construct (struct sockaddr *pThis, char const *pAddressString) {
    struct sockaddr_in*		pAddress = (struct sockaddr_in *)pThis;
    char			iHostName[257];
    int				xPort;

    if (2 != sscanf (
	pAddressString, "%256[^:]:%u", iHostName, &xPort
    ) || !in_addr_Construct (&pAddress->sin_addr, iHostName)
    ) return False;

    pAddress->sin_port		= htons ((u_short)xPort);
    pAddress->sin_family	= AF_INET;

    return True;
}


/*******************************
 *******************************
 *****  VQueryAddressList  *****
 *******************************
 *******************************/

/************************
 *****  Definition  *****
 ************************/

class VQueryAddressList {
//  Construction
public:
    VQueryAddressList ();

//  Element Addition
public:
    void add (struct sockaddr* pAddress);

//  Element Enumeration
public:
    struct sockaddr* get ();

//  Element Randomization
protected:
    void randomize ();

//  State
protected:
    struct sockaddr*		m_pAddresses;
    size_t			m_cAddresses;
    u_long			m_xNextAddress;
};


/**************************
 *****  Construction  *****
 **************************/

VQueryAddressList::VQueryAddressList ()
: m_pAddresses (0)
, m_cAddresses (0)
, m_xNextAddress (0)
{
}


/******************************
 *****  Element Addition  *****
 ******************************/

void VQueryAddressList::add (struct sockaddr* pAddress)
{
    m_pAddresses = (struct sockaddr *)realloc (
	m_pAddresses, (m_cAddresses + 1) * sizeof (struct sockaddr)
    );
    if (m_pAddresses)
        m_pAddresses[m_cAddresses++] = *pAddress;
    else
	m_cAddresses = 0;
}


/*********************************
 *****  Element Enumeration  *****
 *********************************/

struct sockaddr* VQueryAddressList::get ()
{
    if (0 == m_cAddresses)
	return 0;

    if (0 == m_xNextAddress)
	randomize ();

    return m_xNextAddress < m_cAddresses
	? &m_pAddresses[m_xNextAddress++]
	: 0;
}


/***********************************
 *****  Element Randomization  *****
 ***********************************/

void VQueryAddressList::randomize ()
{
    time_t iTime;
    u_int xPass;

    if (m_cAddresses <= 1)
	return;

    time (&iTime);
    srand48 (iTime);

    for (xPass = 0; xPass < 2 * m_cAddresses; xPass++)
    {
	u_long xElement1 = (u_long)(drand48 () * m_cAddresses) % m_cAddresses;
	u_long xElement2 = (u_long)(drand48 () * m_cAddresses) % m_cAddresses;

	if (xElement1 != xElement2)
	{
	    struct sockaddr t = m_pAddresses[xElement1];
	    m_pAddresses[xElement1] = m_pAddresses[xElement2];
	    m_pAddresses[xElement2] = t;
	}
    }
}


/*************************
 *************************
 *****  VQueryAgent  *****
 *************************
 *************************/

/************************
 *****  Definition  *****
 ************************/

class VQueryAgent {
//  Construction
public:
    VQueryAgent (int argc, char* argv[], char* envp[]);

//  Configuration
public:
    unsigned int configure ();

protected:
    void registerRedirection (u_short xRedirectionPort);

    void registerRedirectionIfApplicable (
	u_short		xRedirectionPort,
	struct in_addr*	pRemoteAddress,
	u_int		cRemoteAddressBits    
    );

    void registerServer (
	struct sockaddr* pServerAddress, u_int xServerAddressList
    );

    void registerServerIfApplicable (
	struct sockaddr* pServerAddress,
	struct in_addr*	 pRemoteAddress,
	u_int		 cRemoteAddressBits    
    );

//  Redirection
public:
    int redirect (char const* pRequest);

protected:
    unsigned int recvRedirectionMessage (unsigned int fCopyingReplyToStdout);

    unsigned int sendRedirectionMessage (
	char iTag, char const* pMessage, unsigned int fCopyingReplyToStdout = False
    );

//  Connection
public:
    int connect ();

protected:
    struct sockaddr* firstAddress ();
    struct sockaddr* nextAddress ();
    struct sockaddr* anAddress ();

//  Transmission
public:
    unsigned int send (char const* pCommand);

protected:
    unsigned int send (char const* pData, unsigned int sData);

    static unsigned int wrighte (int fd, char const* pData, unsigned int sData);

//  Reception
public:
    void receive (unsigned int promptCount);

protected:
    unsigned int recv (char* pData, unsigned int sData);

//  Shutdown
protected:
    void closeServerSocket (unsigned int fGracefulShutdown = True);

//  Request Strings
public:
    char const* request ();

protected:
    static char const* externalRequest ();
    char const* internalRequest ();

//  Attribute and Property Strings
protected:
    char const* environment	() const;
    char const* pathInfo	() const;
    char const* queryString	() const;
    char const* remoteAddr	() const;
    char const* remoteHost	() const;
    char const* visionClass	() const;
    char const* visionExpression() const;

//  Error Reporting
public:
    void report (char const* pMessage);

//  State
protected:
    char ** const		m_pEnvironment;
    char const*			m_pClass;
    char const*			m_pExpression;
    char const*			m_pDefaultClass;
    char const*			m_pDefaultExpression;
    char const*			m_pPathInfo;
    char const*			m_pQueryString;
    char const*			m_pRequestMethod;
    char const*			m_pRemoteAddress;
    u_long			m_xRemoteAddress;
    char			m_iServiceName[257];
    VQueryAddressList		m_iServerAddressList[33];   /* 1 per possible subnet */
    int				m_fServerAddressFixed;
    struct sockaddr		m_iServerAddress;
    SOCKET			m_iServerSocket;
    u_long			m_xEnumerationCursor;
    u_short			m_xRedirectionPort;
};


/**************************
 *****  Construction  *****
 **************************/

VQueryAgent::VQueryAgent (int argc, char* argv[], char* envp[])
: m_pEnvironment	(envp)
, m_pClass		(0)
, m_pExpression		(0)
, m_pDefaultClass	(0)
, m_pDefaultExpression	(0)
, m_pPathInfo		(EnvironmentString ("PATH_INFO", argc > 1 ? argv[1] : "/"))
, m_pQueryString	(EnvironmentString ("QUERY_STRING"	, ""))
, m_pRequestMethod	(EnvironmentString ("REQUEST_METHOD"	, ""))
, m_pRemoteAddress	(EnvironmentString ("REMOTE_ADDR"	, ""))
, m_xEnumerationCursor	(33)
, m_xRedirectionPort	(0)
{
    strcpy (m_iServiceName, "");
}


/***************************
 *****  Configuration  *****
 ***************************/

unsigned int VQueryAgent::configure () {
/*****  Initialize the internally known fields, ...  *****/
    if (0 == (m_pPathInfo = strchr (m_pPathInfo, '/'))
    ||	1 != sscanf (++m_pPathInfo, "%256[^/]", m_iServiceName)
    )
    {
	m_pPathInfo = "/";
	strcpy (m_iServiceName, "");
	report (
	    "Your request does not contain the name or address of a Vision server."
	);
	return False;
    }

    if (0 == (m_pPathInfo = strchr (m_pPathInfo, '/')))
	m_pPathInfo = "/";

    m_xRemoteAddress = inet_addr (m_pRemoteAddress);

/****** ... open and parse the configuration file, ...  *****/
    char const* pConfigFileName = EnvironmentString (
	"VQueryConfig", "/vision/lib/vquery.cfg"
    );
    FILE* pConfigFile = fopen (pConfigFileName, "r");

    unsigned int hostConnectionsAllowed = True;	/* should be False */

    if (pConfigFile) {
	unsigned int xLine = 1;
	char iBuffer[4096];

	while (fgets (iBuffer, sizeof (iBuffer), pConfigFile)) {
	    char const* const pWhitespaceBreakSet = " \t\r\n";
	    char const* const pRestOfLineBreakSet = "\r\n";
	    enum {
		ParseState_Error,
		ParseState_ExpectingTag,
		ParseState_ExpectingDefaultAttributeName,
		ParseState_ExpectingDefaultClassName,
		ParseState_ExpectingDefaultExpression,
		ParseState_ExpectingHostConnectionsAllowedEOL,
		ParseState_ExpectingClassServiceName,
		ParseState_ExpectingClassName,
		ParseState_ExpectingExpressionServiceName,
		ParseState_ExpectingExpression,
		ParseState_ExpectingRedirectServiceName,
		ParseState_ExpectingRedirectPort,
		ParseState_ExpectingRedirectRemoteAddress,
		ParseState_ExpectingRedirectRemoteAddressBits,
		ParseState_ExpectingRedirectEOL,
		ParseState_ExpectingServiceName,
		ParseState_ExpectingServerAddress,
		ParseState_ExpectingRemoteAddress,
		ParseState_ExpectingRemoteAddressBits,
		ParseState_ExpectingServiceEOL
	    } xState = ParseState_ExpectingTag;
	    struct sockaddr iServerAddress;
	    struct in_addr  iRemoteAddress;
	    u_int	    cRemoteAddressBits;
	    u_short	    xRedirectionPort;

	    char const* pBreakSet = pWhitespaceBreakSet;
	    for (
		char *pToken = strtok (iBuffer, pBreakSet);
		pToken && ParseState_Error != xState;
		pToken = strtok (0, pBreakSet)
	    )
	    {
		switch (xState) {
		case ParseState_ExpectingTag:
		    xState = 0 == strcasecmp (pToken, "server")
		    ? ParseState_ExpectingServiceName
		    : 0 == strcasecmp (pToken, "default")
		    ? ParseState_ExpectingDefaultAttributeName
		    : 0 == strcasecmp (pToken, "class")
		    ? ParseState_ExpectingClassServiceName
		    : 0 == strcasecmp (pToken, "expression")
		    ? ParseState_ExpectingExpressionServiceName
		    : 0 == strcasecmp (pToken, "redirect")
		    ? ParseState_ExpectingRedirectServiceName
		    : ParseState_Error;
		    break;

		case ParseState_ExpectingDefaultAttributeName:
		    xState = 0 == strcasecmp (pToken, "class")
		    ? ParseState_ExpectingDefaultClassName
		    : 0 == strcasecmp (pToken, "expression")
		    ? ParseState_ExpectingDefaultExpression
		    : 0 == strcasecmp (pToken, "hostConnectionsAllowed")
		    ? ParseState_ExpectingHostConnectionsAllowedEOL
		    : ParseState_Error;
		    break;

		case ParseState_ExpectingDefaultClassName:
		    pToken += strspn (pToken, " \t");
		    if (strlen (pToken) > 0)
		    {
			free ((void*)m_pDefaultClass);
			m_pDefaultClass = strdup (pToken);
		    }
		    xState = ParseState_ExpectingTag;
		    break;

		case ParseState_ExpectingDefaultExpression:
		    pToken += strspn (pToken, " \t");
		    if (strlen (pToken) > 0)
		    {
			free ((void*)m_pDefaultExpression);
			m_pDefaultExpression = strdup (pToken);
		    }
		    xState = ParseState_ExpectingTag;
		    break;

		case ParseState_ExpectingClassServiceName:
		    xState = 0 == strcasecmp (pToken, m_iServiceName)
		    ? ParseState_ExpectingClassName
		    : ParseState_Error;
		    break;

		case ParseState_ExpectingClassName:
		    pToken += strspn (pToken, " \t");
		    if (strlen (pToken) > 0)
		    {
			free ((void*)m_pClass);
			m_pClass = strdup (pToken);
		    }
		    xState = ParseState_ExpectingTag;
		    break;

		case ParseState_ExpectingExpressionServiceName:
		    xState = 0 == strcasecmp (pToken, m_iServiceName)
		    ? ParseState_ExpectingExpression
		    : ParseState_Error;
		    break;

		case ParseState_ExpectingExpression:
		    pToken += strspn (pToken, " \t");
		    if (strlen (pToken) > 0)
		    {
			free ((void*)m_pExpression);
			m_pExpression = strdup (pToken);
		    }
		    xState = ParseState_ExpectingTag;
		    break;

		case ParseState_ExpectingRedirectServiceName:
		    xState = 0 == strcasecmp (pToken, m_iServiceName)
		    ? ParseState_ExpectingRedirectPort
		    : ParseState_Error;
		    break;

		case ParseState_ExpectingRedirectPort:
		    xState = 1 == sscanf (pToken, "%hu", &xRedirectionPort)
		    ? ParseState_ExpectingRedirectRemoteAddress
		    : ParseState_Error;
		    break;

		case ParseState_ExpectingRedirectRemoteAddress:
		    xState = in_addr_Construct (&iRemoteAddress, pToken)
		    ? ParseState_ExpectingRedirectRemoteAddressBits
		    : ParseState_Error;
		    break;
		
		case ParseState_ExpectingRedirectRemoteAddressBits:
		    xState = 1 == sscanf (
			pToken, "%u", &cRemoteAddressBits
		    ) && cRemoteAddressBits < 33
		    ? ParseState_ExpectingRedirectEOL
		    : ParseState_Error;
		    break;

		case ParseState_ExpectingServiceName:
		    xState = 0 == strcasecmp (pToken, m_iServiceName)
		    ? ParseState_ExpectingServerAddress
		    : ParseState_Error;
		    break;

		case ParseState_ExpectingServerAddress:
		    xState = sockaddr_Construct (&iServerAddress, pToken)
		    ? ParseState_ExpectingRemoteAddress
		    : ParseState_Error;
		    break;

		case ParseState_ExpectingRemoteAddress:
		    xState = in_addr_Construct (&iRemoteAddress, pToken)
		    ? ParseState_ExpectingRemoteAddressBits
		    : ParseState_Error;
		    break;

		case ParseState_ExpectingRemoteAddressBits:
		    xState = 1 == sscanf (
			pToken, "%u", &cRemoteAddressBits
		    ) && cRemoteAddressBits < 33
		    ? ParseState_ExpectingServiceEOL
		    : ParseState_Error;
		    break;

		default:
		    xState = ParseState_Error;
		    break;
		}

		switch (xState) {
		case ParseState_ExpectingClassName:
		case ParseState_ExpectingExpression:
		case ParseState_ExpectingDefaultClassName:
		case ParseState_ExpectingDefaultExpression:
		case ParseState_ExpectingHostConnectionsAllowedEOL:
		    pBreakSet = pRestOfLineBreakSet;
		    break;
		default:
		    pBreakSet = pWhitespaceBreakSet;
		    break;
		}
	    }

	    switch (xState) {
	    case ParseState_ExpectingHostConnectionsAllowedEOL:
		hostConnectionsAllowed = True;
		break;

	    case ParseState_ExpectingRedirectRemoteAddress:
		registerRedirection (xRedirectionPort);
		break;

	    case ParseState_ExpectingRedirectRemoteAddressBits:
		registerRedirectionIfApplicable (
		    xRedirectionPort, &iRemoteAddress, 32
		);
		break;

	    case ParseState_ExpectingRedirectEOL:
		registerRedirectionIfApplicable (
		    xRedirectionPort, &iRemoteAddress, cRemoteAddressBits
		);
		break;

	    case ParseState_ExpectingRemoteAddress:
		registerServer (&iServerAddress, 0);
		break;

	    case ParseState_ExpectingRemoteAddressBits:
		registerServerIfApplicable (
		    &iServerAddress, &iRemoteAddress, 32
		);
		break;

	    case ParseState_ExpectingServiceEOL:
		registerServerIfApplicable (
		    &iServerAddress, &iRemoteAddress, cRemoteAddressBits
		);
		break;

	    case ParseState_Error:
		break;

	    default:
		break;
	    }
	}

	fclose (pConfigFile);
    }

    m_fServerAddressFixed = hostConnectionsAllowed && sockaddr_Construct (
	&m_iServerAddress, m_iServiceName
    );

/*****  ... and return.  *****/
    return True;
}


/*********************************
 *****  Server Registration  *****
 *********************************/

void VQueryAgent::registerRedirection (u_short xRedirectionPort)
{
    m_xRedirectionPort = xRedirectionPort;
}

void VQueryAgent::registerRedirectionIfApplicable (
    u_short		xRedirectionPort,
    struct in_addr*	pRemoteAddress,
    u_int		cRemoteAddressBits    
)
{
    if ((ntohl (m_xRemoteAddress) & (
	    cRemoteAddressBits < 32 ? ~((u_long)UINT_MAX >> cRemoteAddressBits) : (u_long)UINT_MAX
	)) == (ntohl (pRemoteAddress->s_addr) & (
	    cRemoteAddressBits < 32 ? ~((u_long)UINT_MAX >> cRemoteAddressBits) : (u_long)UINT_MAX
	))
    ) registerRedirection (xRedirectionPort);
}


/*********************************
 *****  Server Registration  *****
 *********************************/

void VQueryAgent::registerServer (
    struct sockaddr*		pServerAddress,
    u_int			xServerAddressList
)
{
    m_iServerAddressList[xServerAddressList].add (pServerAddress);
}

void VQueryAgent::registerServerIfApplicable (
    struct sockaddr*		pServerAddress,
    struct in_addr*		pRemoteAddress,
    u_int			cRemoteAddressBits    
)
{
    if ((ntohl (m_xRemoteAddress) & (
	    cRemoteAddressBits < 32 ? ~((u_long)UINT_MAX >> cRemoteAddressBits) : (u_long)UINT_MAX
	)) == (ntohl (pRemoteAddress->s_addr) & (
	    cRemoteAddressBits < 32 ? ~((u_long)UINT_MAX >> cRemoteAddressBits) : (u_long)UINT_MAX
	))
    ) registerServer (pServerAddress, cRemoteAddressBits);
}


/*************************
 *****  Redirection  *****
 *************************/

int VQueryAgent::redirect (char const* pRequest)
{
    if (0 == m_xRedirectionPort)
	return False;

    struct sockaddr_in iRedirectionAddress;
    iRedirectionAddress.sin_addr.s_addr	= m_xRemoteAddress;
    iRedirectionAddress.sin_port	= htons ((u_short)m_xRedirectionPort);
    iRedirectionAddress.sin_family	= AF_INET;

    m_iServerSocket = socket (AF_INET, SOCK_STREAM, 0);
    if (m_iServerSocket < 0)
	return False;

    if (::connect (
	    m_iServerSocket, (struct sockaddr*)&iRedirectionAddress, sizeof (iRedirectionAddress)
	) < 0
    )
    {
	closeServerSocket (False);
	return False;
    }

    if (!sendRedirectionMessage ('c', m_iServiceName))
    {
	closeServerSocket ();
	return False;
    }

    if (!sendRedirectionMessage ('<', pRequest) || !sendRedirectionMessage ('r', "", True))
	report ("An error occurred redirecting this request to your personal session.");

    closeServerSocket ();
    return True;
}


unsigned int VQueryAgent::recvRedirectionMessage (unsigned int fCopyingReplyToStdout)
{
    char iTag;
    if (0 != recv (&iTag, sizeof (iTag)) || '-' == iTag)
	return False;

    unsigned int sData;
    if (0 != recv ((char *)&sData, sizeof (sData)))
	return False;

    sData = ntohl (sData);
    unsigned int sResidue = 0;
    unsigned int sTransfer = 0;
    char iBuffer[2048];
    while (
	sData > 0 && 0 == (
	    sResidue = recv (
		iBuffer,
		sTransfer = sData < sizeof (iBuffer) ? sData : sizeof (iBuffer)
	    )
	)
    )
    {
	if (fCopyingReplyToStdout)
	    wrighte (fileno (stdout), iBuffer, sTransfer);
	sData -= sTransfer;
    }

    return 0 == sResidue;
}


unsigned int VQueryAgent::sendRedirectionMessage (
    char iTag, char const* pMessage, unsigned int fCopyingReplyToStdout
)
{
    unsigned int sMessage = strlen (pMessage);
    u_long sMessageNBO = ntohl (sMessage);
    return 0 == send (&iTag, sizeof (iTag))
	&& 0 == send ((char const*)&sMessageNBO, sizeof (sMessageNBO))
	&& 0 == send (pMessage, sMessage)
	&& recvRedirectionMessage (fCopyingReplyToStdout);
}


/************************
 *****  Connection  *****
 ************************/

int VQueryAgent::connect () {
    for (
	struct sockaddr *pServerAddress = firstAddress ();
	pServerAddress;
	pServerAddress = nextAddress ()
    )
    {
	m_iServerSocket = socket (AF_INET, SOCK_STREAM, 0);

	if (m_iServerSocket < 0)
	{
	    report (
		"The socket used to connect to that server could not be created."
	    );
	    return False;
	}

	if (::connect (
		m_iServerSocket, pServerAddress, sizeof (*pServerAddress)
	    ) >= 0
	)
	{
	    m_iServerAddress = *pServerAddress;
	    return True;
	}

	closeServerSocket (False);
    }

    report ("The server could not be contacted.");

    return False;
}


/*********************************
 *****  Address Enumeration  *****
 *********************************/

struct sockaddr* VQueryAgent::firstAddress () {
    if (m_fServerAddressFixed)
	return &m_iServerAddress;

    m_xEnumerationCursor = 32;

    return anAddress ();
}

struct sockaddr* VQueryAgent::nextAddress () {
    if (m_fServerAddressFixed)
	return 0;

    return anAddress ();
}

struct sockaddr* VQueryAgent::anAddress () {
    while (m_xEnumerationCursor < 33) {
	struct sockaddr* pAddress = m_iServerAddressList[
	    m_xEnumerationCursor
	].get ();
	if (pAddress)
	    return pAddress;
	m_xEnumerationCursor--;
    }

    return 0;
}


/**************************
 *****  Transmission  *****
 **************************/

unsigned int VQueryAgent::send (char const* pCommand)
{
    unsigned int promptCount = 1;
    char const *pCursor;
    char const *nCursor;

    for (
	pCursor = pCommand;
	0 != (nCursor = strchr (pCursor, '\n'));
	pCursor = nCursor + 1
    ) promptCount++;
    
    send (pCommand, strlen (pCommand));

    return promptCount;
}

unsigned int VQueryAgent::send (char const* pData, unsigned int sData)
{
    int sTransfer;
    while (sData > 0 && (sTransfer = ::send (m_iServerSocket, pData, sData, 0)) > 0)
    {
	pData += sTransfer;
	sData -= sTransfer;
    }
    return sData;
}

unsigned int VQueryAgent::wrighte (int fd, char const* pData, unsigned int sData)
{
    int sTransfer;
    while (sData > 0 && (sTransfer = ::write (fd, pData, sData)) > 0)
    {
	pData += sTransfer;
	sData -= sTransfer;
    }
    return sData;
}


/***********************
 *****  Reception  *****
 ***********************/

void VQueryAgent::receive (unsigned int promptCount) {
    static char const*	pVisionPrompt = "V> ";
    static size_t const	sVisionPrompt = strlen (pVisionPrompt);

    char	buffer[StandardTCPMSS * 16];

    char*	pRecvRegion = buffer;
    int		sRecvRegion = sizeof (buffer) - 1;

    int		sRecv;

    while (promptCount > 0 && (sRecv = ::recv (m_iServerSocket, pRecvRegion, sRecvRegion, 0)) > 0) {
	pRecvRegion[sRecv] = '\0';

	char const* pScanRegion = buffer;
	char const* pNextPrompt;
	while ((pNextPrompt = strstr (pScanRegion, pVisionPrompt)) != 0 && promptCount > 1) {
	    pScanRegion = pNextPrompt + sVisionPrompt;
	    promptCount--;
	}

	if (pNextPrompt) {
	//  ... prompt count must be 1 at this point...
	    wrighte (fileno (stdout), pScanRegion, pNextPrompt - pScanRegion);
	    promptCount--;
	}
	else if (promptCount > 1) {
	    size_t sScanRegion = strlen (pScanRegion);
	    memmove (buffer, pScanRegion, sScanRegion);

	    pRecvRegion = buffer + sScanRegion;
	    sRecvRegion = sizeof (buffer) - sScanRegion - 1;
	}
	else {
	    size_t sScanRegion = strlen (pScanRegion);
	    unsigned int sPromptPrefix = sScanRegion < sVisionPrompt ? sScanRegion : sVisionPrompt - 1;
	    char const*  pPromptPrefix = pScanRegion + sScanRegion - sPromptPrefix;
	    while (sPromptPrefix > 0 && strncmp (pVisionPrompt, pPromptPrefix, sPromptPrefix) != 0) {
		pPromptPrefix++;
		sPromptPrefix--;
	    }
	    wrighte (fileno (stdout), pScanRegion, sScanRegion - sPromptPrefix);
	    memmove (buffer, pPromptPrefix, sPromptPrefix);

	    pRecvRegion = buffer + sPromptPrefix;
	    sRecvRegion = sizeof (buffer) - sPromptPrefix - 1;
	}
    }
    closeServerSocket ();
}


unsigned int VQueryAgent::recv (char* pData, unsigned int sData)
{
    int sTransfer;
    while (sData > 0 && (sTransfer = ::recv (m_iServerSocket, pData, sData, 0)) > 0)
    {
	pData += sTransfer;
	sData -= sTransfer;
    }
    return sData;
}


/**********************
 *****  Shutdown  *****
 **********************/

void VQueryAgent::closeServerSocket (unsigned int fGracefulShutdown) {
    if (fGracefulShutdown) {
	shutdown (m_iServerSocket, 1);
	char iBuffer[256];
	while (::recv (m_iServerSocket, iBuffer, sizeof (iBuffer), 0) > 0);
    }

#if defined(_WIN32)
    closesocket (m_iServerSocket);
#else
    close (m_iServerSocket);
#endif
    m_iServerSocket = INVALID_SOCKET;
}


/*****************************
 *****  Request Strings  *****
 *****************************/

char const* VQueryAgent::request ()
{
    return strcmp (m_pPathInfo, "/-") ? internalRequest () : externalRequest ();
}

char const* VQueryAgent::externalRequest ()
{
    size_t	usize	= 0;
    size_t	asize	= 4096;
    char *	buffer	= (char*)malloc (asize);
    size_t	bcount;

    while (bcount = read (fileno (stdin), buffer + usize, asize - usize - 1))
    {
	if ((size_t)-1 == bcount)
	    return "";

	usize += bcount;
	if (asize <= usize + 1 && 0 == (
		buffer = (char*)realloc (buffer, asize += 4096)
	    )
	) return "";
    }
    buffer[usize] = '\0';

    return buffer;
}


char const* VQueryAgent::internalRequest ()
{
    static char	iResultBuffer[65535];
    char* pResult = iResultBuffer;

    char const* pSource = visionExpression ();

    while (*pSource)
    {
	unsigned int sScan = strcspn (pSource, "$");
	strncpy (pResult, pSource, sScan);
	pResult += sScan;
	pSource += sScan;
	if (*pSource)
	{
	    *pResult++ = ' ';

	    unsigned int anInternalSymbol = '$' == *++pSource;
	    if (anInternalSymbol)
		++pSource;
	    unsigned int sSymbol = strspn (
		pSource, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_"
	    );
	    sSymbol += strspn (
		pSource + sSymbol, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789"
	    );
	    if (sSymbol > 0)
	    {
		strncpy (pResult, pSource, sSymbol);
		pResult[sSymbol] = '\0';
		pSource += sSymbol;

		char const* pSymbol = pResult;

		if (!anInternalSymbol)
		    pResult = Quote (pResult, EnvironmentString (pSymbol, ""));
		else if (0 == strcasecmp (pSymbol, "class"))
		    pResult += sprintf (pResult, "%s", visionClass ());
		else if (0 == strcasecmp (pSymbol, "path"))
		    pResult = Quote (pResult, pathInfo ());
		else if (0 == strcasecmp (pSymbol, "query"))
		    pResult = Quote (pResult, queryString ());
		else if (0 == strcasecmp (pSymbol, "environment"))
		    pResult = Quote (pResult, environment ());
		else
		    pResult = Quote (pResult, "");
	    }
	    else pResult = Quote (pResult, "");
	}
    }

    strcpy (pResult, "\n?g\n");

    return iResultBuffer;
}


/********************************************
 *****  Attribute And Property Strings  *****
 ********************************************/

char const* VQueryAgent::environment () const
{
    static char iBuffer[65545];
    iBuffer[0] = '\0';

    char* pResult = iBuffer;
    unsigned int first = True;
    for (char** ppBinding = m_pEnvironment; *ppBinding; ppBinding++)
    {
	pResult += sprintf (pResult, first ? "%s" : "\n%s", *ppBinding);
	first = False;
    }

    return iBuffer; 
}

char const* VQueryAgent::pathInfo () const {
    return m_pPathInfo;
}

char const* VQueryAgent::queryString () const {
    return strcasecmp (m_pRequestMethod, "get" ) == 0
	? m_pQueryString
	: strcasecmp (m_pRequestMethod, "post") == 0
	? externalRequest ()
	: "";
}

char const* VQueryAgent::remoteAddr () const {
    return m_pRemoteAddress;
}

char const* VQueryAgent::remoteHost () const {
    return EnvironmentString ("REMOTE_HOST", "");
}

char const* VQueryAgent::visionClass () const {
    return m_pClass
	? m_pClass
	: m_pDefaultClass
	? m_pDefaultClass
	: "Interface HtmlAccess";
}

char const* VQueryAgent::visionExpression () const {
    return m_pExpression
	? m_pExpression
	: m_pDefaultExpression
	? m_pDefaultExpression
	: "$$class get: $$path usingQuery: $$query for: $REMOTE_HOST at: $REMOTE_ADDR";
}


/*****************************
 *****  Error Reporting  *****
 *****************************/

void VQueryAgent::report (char const* pMessage)
{
    printf ("Content-Type: text/html\n\n");
    printf ("<HTML>\n");
    printf ("<HEAD> <TITLE> VQuery Error </TITLE> </HEAD>\n");
    printf ("<BODY>\n");
    printf ("<H2> VQuery Error </H2>\n");
    printf ("<HR>\n");
//    printf ("<STRONG>\n");
    printf ("Your request to access", m_pPathInfo);
    printf (strlen (m_iServiceName) > 0 ? " the '%s'" : " a", m_iServiceName);
    printf (" Vision server failed.\n%s\n", pMessage);
//    printf ("</STRONG>\n");
    printf ("<HR>\n");
    printf ("</BODY>\n");
    printf ("</HTML>\n");
}


/**************************
 **************************
 *****  Main Program  *****
 **************************
 **************************/

int main (
    int				argc,
    char			*argv[],
    char			*envp[]
)
{
    VQueryAgent iVQueryAgent (argc, argv, envp);

    if (!StartSocketLayer ())
    {
	iVQueryAgent.report (
	    "The operating system does not support the version of the socket library required by this program."
	);
	return ErrorExitValue;
    }

    int iExitCode = ErrorExitValue;
    if (iVQueryAgent.configure ()) {
	char const* pRequest = iVQueryAgent.request ();
	if (iVQueryAgent.redirect (pRequest))
	    iExitCode = NormalExitValue;
	else if (iVQueryAgent.connect ()) {
	    iVQueryAgent.receive (iVQueryAgent.send (pRequest));
	    iExitCode = NormalExitValue;
	}
    }

    StopSocketLayer ();

    return iExitCode;
}
