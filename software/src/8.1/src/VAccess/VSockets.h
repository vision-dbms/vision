/*****  VSockets Definition (Win32)  *****/
#ifndef VSockets_Definition
#define VSockets_Definition

/********************************
 *****  Socket Definitions  *****
 ********************************/

#if !defined(_WIN32)
typedef int SOCKET;

#define INVALID_SOCKET	-1
#define SOCKET_ERROR	-1

#endif


/**********************************
 *****  UID Class Definition  *****
 **********************************/

class VUID {
/*****  Access  *****/
public:
    char const* asString (char const *pSeparator = ".");

/*****  State  *****/
public:
    unsigned int	m_u0;
    unsigned short	m_u1;
    unsigned short	m_u2;
    unsigned char	m_u3[4];
    unsigned char	m_u4[4];
};


/*********************************************
 *****  Socket Manager Class Definition  *****
 *********************************************/

class VSockets {
/*****  Construction/Destruction  *****/
public:
    VSockets (bool fInitializationRequired);
    ~VSockets ();

/*****  Query  *****/
public:
    int initialized () {
	return m_fInitialized;
    }

    bool socketName (
	SOCKET xSocket, unsigned char iHost[4], unsigned short &xPort
    ) const;
    bool peerName (
	SOCKET xSocket, unsigned char iHost[4], unsigned short &xPort
    ) const;

/*****  Execute  *****/
public:
    bool rexec (
	SOCKET&		xDataSocket,
	SOCKET&		xControlSocket,
	char *		pErrorMessageBuffer,
	size_t		sErrorMessageBuffer,
	char const *	pHostName,
	char const *	pUserName,
	char const *	pPassword, 
	char const *	pCommand,
	u_short		xHostPort = 512,
	bool		fFull = FALSE
    );

/*****  Connect  *****/
public:
    SOCKET connect (
	char const *pHostName, unsigned short xHostPort
    );

/*****  Serve  *****/
public:
    SOCKET serve (u_short& xPort, int sBacklog = 1);

/*****  I/O  *****/
public:
    int send (SOCKET xSocket, char const *pData, size_t sData);
    int recv (SOCKET xSocket, char *pBuffer, size_t sBuffer);

/*****  Shutdown  *****/
public:
    int endReception	(SOCKET xSocket);
    int endTransmission (SOCKET xSocket);
    int close		(SOCKET xSocket);

/*****  State  *****/
protected:
    bool m_fInitialized;
};

#endif
