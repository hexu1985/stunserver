#ifndef __unp_readline_h
#define __unp_readline_h

#ifdef USE_WINSOCK
#include <winsock2.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <string>

ssize_t readline(int sockfd, std::string &data, const std::string &end_tag);

#ifndef MAXLINE
#define MAXLINE 1024
#endif

#endif


