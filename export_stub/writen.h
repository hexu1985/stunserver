#ifndef __unp_writen_h
#define __unp_writen_h

#ifdef USE_WINSOCK
#include <winsock2.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <errno.h>

ssize_t     writen(int, const void *, size_t);

#endif

