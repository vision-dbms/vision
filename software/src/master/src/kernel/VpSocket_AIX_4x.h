#ifndef VpSocket_AIX_4x_Interface
#define VpSocket_AIX_4x_Interface

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

#ifdef __cplusplus

#define getpeername(fd,addr,addrlen) getpeername (fd, addr, (size_t*)(addrlen))
#define getsockname(fd,addr,addrlen) getsockname (fd, addr, (size_t*)(addrlen))

#endif

#endif
