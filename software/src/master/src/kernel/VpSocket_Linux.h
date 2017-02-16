#ifndef VpSocket_Linux_Interface
#define VpSocket_Linux_Interface

#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>

#include <netinet/in.h>
#include <netinet/tcp.h>

#include <arpa/inet.h>

#include <netdb.h>

#define INVALID_SOCKET		-1

#define SocketHandleIsValid(h)	(0 <= (h))
#define SocketHandleIsInvalid(h)(0 >  (h))

#define getpeername(fd,addr,addrlen) getpeername (fd, addr, (socklen_t*)(addrlen))
#define getsockname(fd,addr,addrlen) getsockname (fd, addr, (socklen_t*)(addrlen))
#define getsockopt(fd,level,optname,optval,optlen) \
    getsockopt (fd,level,optname,optval,(socklen_t*)(optlen))

#endif
