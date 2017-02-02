#ifndef VpSocket_HPUX_10x_Interface
#define VpSocket_HPUX_10x_Interface

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

#if __cplusplus < 199707L
#define select(n,pr,pw,pe,pt)	select (n, (int*)(pr), (int*)(pw), (int*)(pe), (pt))
#endif

#endif
