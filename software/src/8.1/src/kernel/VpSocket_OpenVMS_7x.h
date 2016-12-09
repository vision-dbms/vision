#ifndef VpSocket_Interface
#define VpSocket_Interface

#include <sys/time.h>
#include <sys/socket.h>
#include <sys/wait.h>

#include <netinet/in.h>
#include <netinet/tcp.h>

#include <arpa/inet.h>

#include <netdb.h>
#include <stropts.h>

#define INVALID_SOCKET		-1

#define SocketHandleIsValid(h)	(0 <= (h))
#define SocketHandleIsInvalid(h)(0 >  (h))

#define getpeername(fd,addr,addrlen) getpeername (fd, addr, (size_t*)(addrlen))
#define getsockname(fd,addr,addrlen) getsockname (fd, addr, (size_t*)(addrlen))

/*
 *  Socket length type...
 */
#if defined(_DECC_V4_SOURCE)
typedef int socklen_t;
#else
typedef size_t socklen_t;
#endif

/*
 *  UNIX domain socket address type...
 */
struct sockaddr_un {
    short sun_family;
    char sun_path[92];
};


#endif
