#ifndef __unp_writen_h
#define __unp_writen_h

#include <unistd.h>
#include <errno.h>

ssize_t     writen(int, const void *, size_t);

#endif

