#ifndef VpSocket_Solaris_2x_Interface
#define VpSocket_Solaris_2x_Interface

#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>

#include <netinet/in.h>
#include <netinet/tcp.h>

#include <arpa/inet.h>

#include <netdb.h>
#include <stropts.h>

#define INVALID_SOCKET		-1

#define SocketHandleIsValid(h)	(0 <= (h))
#define SocketHandleIsInvalid(h)(0 >  (h))

#ifndef	INADDR_NONE
#define	INADDR_NONE		((u_long)0xffffffff)	/* -1 return */
#endif

#endif
