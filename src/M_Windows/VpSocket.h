#ifndef VpSocket_Interface
#define VpSocket_Interface

#define SocketHandleIsValid(h)	(INVALID_SOCKET != (SOCKET)(h))
#define SocketHandleIsInvalid(h)(INVALID_SOCKET == (SOCKET)(h))

/*
 *  Socket length type...
 */
typedef int socklen_t;

/*
 * UNIX domain socket address type...
 */
struct sockaddr_un {
	short	sun_family;		/* AF_UNIX */
	char	sun_path[92];		/* path name (gag) */
};

#endif
