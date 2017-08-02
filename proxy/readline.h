#ifndef __unp_readline_h
#define __unp_readline_h

#include <unistd.h>
#include <string>

ssize_t readline(int sockfd, std::string &data, const std::string &end_tag);

#ifndef MAXLINE
#define MAXLINE 1024
#endif

#endif


