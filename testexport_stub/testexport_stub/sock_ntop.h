#ifndef	__unp_sock_ntop_h
#define	__unp_sock_ntop_h

#if defined WIN32 || defined _WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#endif
#include <string.h>
#include <stdio.h>

/* Define to 1 if the system supports IPv6 */
#define IPV6    1

#ifdef  __cplusplus
extern "C" {
#endif

char *Sock_ntop(const struct sockaddr *, socklen_t);

#ifdef  __cplusplus
}   // extern "C"
#endif

#endif

