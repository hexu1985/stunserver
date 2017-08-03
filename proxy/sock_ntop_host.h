#ifndef	__unp_sock_ntop_host_h
#define	__unp_sock_ntop_host_h

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <string.h>
#include <stdio.h>

/* Define to 1 if the system supports IPv6 */
#define IPV6    1

#ifdef  __cplusplus
extern "C" {
#endif

char *Sock_ntop_host(const struct sockaddr *, socklen_t);

#ifdef  __cplusplus
}   // extern "C"
#endif

#endif

